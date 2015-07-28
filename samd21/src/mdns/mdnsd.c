/*
 * tinysvcmdns - a tiny MDNS implementation for publishing services
 * Copyright (C) 2011 Darell Tan
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "socket/include/socket.h"
//#include <sys/ioctl.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <net/if.h>
//#include <syslog.h>
//#include <netdb.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//#include <unistd.h>
#include <assert.h>

/*
 * Define a proper IP socket level if not already done.
 * Required to compile on OS X
 */
#ifndef SOL_IP
#define SOL_IP IPPROTO_IP
#endif

#include "mdns.h"
#include "mdnsd.h"
#include "nm_fluffy.h"

#define DEFAULT_HOST_IP	"192.168.0.15"
extern uint8_t gIpAddressFromAp[36];

#define MDNS_ADDR "255.255.255.255"
//#define MDNS_ADDR 0xE00000FB	/* 224.0.0.251 */
#define MDNS_PORT 5353
#define DEFAULT_HTTP_PORT	80

#define PACKET_SIZE 512//65536

#define SERVICES_DNS_SD_NLABEL \
		((uint8_t *) "\x09_services\x07_dns-sd\x04_udp\x05local")

struct mdnsd {
	int sockfd;
	int stop_flag;

	struct rr_group *group;
	struct rr_list *announce;
	struct rr_list *services;
	uint8_t *hostname;
};

struct mdns_service {
	struct rr_list *entries;
};

struct sockaddr_in localInterface;
struct sockaddr_in groupSock;

static struct mdnsd mdnsSvr;
static void* mdnsRespPacket[PACKET_SIZE];
static uint16_t gRespPacketLen = 0;

/////////////////////////////////


static void log_message(int loglevel, char *fmt_str, ...) {
	va_list ap;
	char buf[2048];

	va_start(ap, fmt_str);
	vsnprintf(buf, 2047, fmt_str, ap);
	va_end(ap);
	buf[2047] = 0;

	fprintf(stderr, "%s\n", buf);
}

static int create_recv_sock() {
    /* Create a datagram socket on which to send. */
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0)
    {
          perror("Opening datagram socket error");
            exit(1);
    }
    else
          printf("Opening the datagram socket...OK.\r\n");
     
    /* Initialize the group sockaddr structure with a */
    /* group address of 225.1.1.1 and port 5555. */
    memset((char *) &groupSock, 0, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = nmi_inet_addr(MDNS_ADDR);
    groupSock.sin_port = _htons(MDNS_PORT);
       
    /* Set local interface for outbound multicast datagrams. */
    /* The IP address specified must be associated with a local, */
    /* multicast capable interface. */
	/*
    localInterface.sin_addr.s_addr = nmi_inet_addr(DEFAULT_HOST_IP);
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
    {
		perror("Setting local interface error");
        //exit(1);
    }
    else
		printf("Setting the local interface...OK\n");
	*/	  
	return sd;
}

static ssize_t send_packet(int fd, const void *data, size_t len) {
	/*static struct sockaddr_in toaddr;
	if (toaddr.sin_family != AF_INET) {
		memset(&toaddr, 0, sizeof(struct sockaddr_in));
		toaddr.sin_family = AF_INET;
		toaddr.sin_port = _htons(MDNS_PORT);
		toaddr.sin_addr.s_addr = nmi_inet_addr(MDNS_ADDR);
	}
	
	return sendto(fd, data, len, 0, (struct sockaddr *) &toaddr, sizeof(struct sockaddr_in));
	*/
	
	return sendto(fd, data, len, 0, (struct sockaddr *) &groupSock, sizeof(struct sockaddr_in));
}


// populate the specified list which matches the RR name and type
// type can be RR_ANY, which populates all entries EXCEPT RR_NSEC
static int populate_answers(struct mdnsd *svr, struct rr_list **rr_head, uint8_t *name, enum rr_type type) {
	int num_ans = 0;

	// check if we have the records
	struct rr_group *ans_grp = rr_group_find(svr->group, name);
	if (ans_grp == NULL) {
		return num_ans;
	}

	// decide which records should go into answers
	struct rr_list *n = ans_grp->rr;
	for (; n; n = n->next) {
		// exclude NSEC for RR_ANY
		if (type == RR_ANY && n->e->type == RR_NSEC)
			continue;

		if ((type == n->e->type || type == RR_ANY) && cmp_nlabel(name, n->e->name) == 0) {
			num_ans += rr_list_append(rr_head, n->e);
		}
	}

	return num_ans;
}

// given a list of RRs, look up related records and add them
static void add_related_rr(struct mdnsd *svr, struct rr_list *list, struct mdns_pkt *reply) {
	for (; list; list = list->next) {
		struct rr_entry *ans = list->e;

		switch (ans->type) {
			case RR_PTR:
				// target host A, AAAA records
				reply->num_add_rr += populate_answers(svr, &reply->rr_add, 
										MDNS_RR_GET_PTR_NAME(ans), RR_ANY);
				break;

			case RR_SRV:
				// target host A, AAAA records
				reply->num_add_rr += populate_answers(svr, &reply->rr_add, 
										ans->data.SRV.target, RR_ANY);

				// perhaps TXT records of the same name?
				// if we use RR_ANY, we risk pulling in the same RR_SRV
				reply->num_add_rr += populate_answers(svr, &reply->rr_add, 
										ans->name, RR_TXT);
				break;

			case RR_A:
			case RR_AAAA:
				reply->num_add_rr += populate_answers(svr, &reply->rr_add, 
										ans->name, RR_NSEC);
				break;

			default:
				// nothing to add
				break;
		}
	}
}

// processes the incoming MDNS packet
// returns >0 if processed, 0 otherwise
static int process_mdns_pkt(struct mdnsd *svr, struct mdns_pkt *pkt, struct mdns_pkt *reply) {
	int i;

	assert(pkt != NULL);

	// is it standard query?
	if ((pkt->flags & MDNS_FLAG_RESP) == 0 && 
			MDNS_FLAG_GET_OPCODE(pkt->flags) == 0) {
		mdns_init_reply(reply, pkt->id);

		DEBUG_PRINTF("flags = %04x, qn = %d, ans = %d, add = %d\n", 
						pkt->flags,
						pkt->num_qn,
						pkt->num_ans_rr,
						pkt->num_add_rr);

		// loop through questions
		struct rr_list *qnl = pkt->rr_qn;
		for (i = 0; i < pkt->num_qn; i++, qnl = qnl->next) {
			struct rr_entry *qn = qnl->e;
			int num_ans_added = 0;

			char *namestr = nlabel_to_str(qn->name);
			DEBUG_PRINTF("qn #%d: type %s (%02x) %s - ", i, rr_get_type_name(qn->type), qn->type, namestr);
			free(namestr);

			// check if it's a unicast query - we ignore those
			if (qn->unicast_query) {
				DEBUG_PRINTF("skipping unicast query\n");
				continue;
			}

			num_ans_added = populate_answers(svr, &reply->rr_ans, qn->name, qn->type);
			reply->num_ans_rr += num_ans_added;

			DEBUG_PRINTF("added %d answers\n", num_ans_added);
		}

		// remove our replies if they were already in their answers
		struct rr_list *ans = NULL, *prev_ans = NULL;
		for (ans = reply->rr_ans; ans; ) {
			struct rr_list *next_ans = ans->next;
			struct rr_entry *known_ans = rr_entry_match(pkt->rr_ans, ans->e);

			// discard answers that have at least half of the actual TTL
			if (known_ans != NULL && known_ans->ttl >= ans->e->ttl / 2) {
				char *namestr = nlabel_to_str(ans->e->name);
				DEBUG_PRINTF("removing answer for %s\n", namestr);
				free(namestr);

				// check if list item is head
				if (prev_ans == NULL)
					reply->rr_ans = ans->next;
				else
					prev_ans->next = ans->next;
				free(ans);

				ans = prev_ans;

				// adjust answer count
				reply->num_ans_rr--;
			}

			prev_ans = ans;
			ans = next_ans;
		}


		// see if we can match additional records for answers
		add_related_rr(svr, reply->rr_ans, reply);

		// additional records for additional records
		add_related_rr(svr, reply->rr_add, reply);

		DEBUG_PRINTF("\n");

		return reply->num_ans_rr;
	}

	return 0;
}

// creates an announce packet given the type name PTR
static void announce_srv(struct mdnsd *svr, struct mdns_pkt *reply, uint8_t *name) {
	mdns_init_reply(reply, 0);

	reply->num_ans_rr += populate_answers(svr, &reply->rr_ans, name, RR_PTR);
	
	// remember to add the services dns-sd PTR too
	reply->num_ans_rr += populate_answers(svr, &reply->rr_ans,
	SERVICES_DNS_SD_NLABEL, RR_PTR);

	// see if we can match additional records for answers
	add_related_rr(svr, reply->rr_ans, reply);

	// additional records for additional records
	add_related_rr(svr, reply->rr_add, reply);
}

// main loop to receive, process and send out MDNS replies
// also handles MDNS service announces
static void main_loop(struct mdnsd *svr) {
	void *pkt_buffer = malloc(PACKET_SIZE);

	struct mdns_pkt *mdns_reply = malloc(sizeof(struct mdns_pkt));
	memset(mdns_reply, 0, sizeof(struct mdns_pkt));

    printf("main_loop start\r\n");

	while (! svr->stop_flag) {
			struct rr_entry *ann_e = NULL;
			char *namestr = NULL;

			// extract from head of list
			if (svr->announce)
				ann_e = rr_entry_match(&svr->announce, svr->announce->e);

			if (! ann_e)
				break;

			namestr = nlabel_to_str(ann_e->name);
			//DEBUG_PRINTF("sending announce for %s\r\n", namestr);
			printf("sending announce for %s\r\n", namestr);
			free(namestr);

			announce_srv(svr, mdns_reply, ann_e->name);

			if (mdns_reply->num_ans_rr > 0) {
				size_t replylen = mdns_encode_pkt(mdns_reply, pkt_buffer, PACKET_SIZE);
				send_packet(svr->sockfd, pkt_buffer, replylen);
			}
			
			nm_bsp_sleep(1000);
	}

    printf("main loop end\r\n");

	// main thread terminating. send out "goodbye packets" for services
	mdns_init_reply(mdns_reply, 0);

	struct rr_list *svc_le = svr->services;
	for (; svc_le; svc_le = svc_le->next) {
		// set TTL to zero
		svc_le->e->ttl = 0;
		mdns_reply->num_ans_rr += rr_list_append(&mdns_reply->rr_ans, svc_le->e);
	}

	// send out packet
	if (mdns_reply->num_ans_rr > 0) {
		size_t replylen = mdns_encode_pkt(mdns_reply, pkt_buffer, PACKET_SIZE);
		send_packet(svr->sockfd, pkt_buffer, replylen);
	}

	// destroy packet
	mdns_init_reply(mdns_reply, 0);
	free(mdns_reply);

	free(pkt_buffer);

	svr->stop_flag = 2;
}

static void process_resp_mdns_pkt(struct mdnsd *svr)
{
	struct rr_entry *ann_e = NULL;
	char *namestr = NULL;
	struct mdns_pkt mdns_reply = {0,};
	
	memset(mdnsRespPacket, 0, sizeof(PACKET_SIZE));

	printf("process_resp_mdns_pkt\r\n");

	// extract from head of list
	if (svr->announce)
		ann_e = rr_entry_match(&svr->announce, svr->announce->e);

	if (! ann_e)
		return;

	namestr = nlabel_to_str(ann_e->name);
	//DEBUG_PRINTF("sending announce for %s\r\n", namestr);
	printf("sending announce for %s\r\n", namestr);
	free(namestr);

	announce_srv(svr, &mdns_reply, ann_e->name);

	if (mdns_reply.num_ans_rr > 0) {
		gRespPacketLen = (uint16_t)mdns_encode_pkt(&mdns_reply, mdnsRespPacket, PACKET_SIZE);
	}
}

/////////////////////////////////////////////////////


void mdnsd_set_hostname(struct mdnsd *svr, const char *hostname, uint32_t ip) {
	struct rr_entry *a_e = NULL,
					*nsec_e = NULL;

	// currently can't be called twice
	// dont ask me what happens if the IP changes
	assert(svr->hostname == NULL);

	a_e = rr_create_a(create_nlabel(hostname), ip);

	nsec_e = rr_create(create_nlabel(hostname), RR_NSEC);
	rr_set_nsec(nsec_e, RR_A);

	svr->hostname = create_nlabel(hostname);
	rr_group_add(&svr->group, a_e);
	rr_group_add(&svr->group, nsec_e);
}

void mdnsd_add_rr(struct mdnsd *svr, struct rr_entry *rr) {
	rr_group_add(&svr->group, rr);
}

struct mdns_service *mdnsd_register_svc(struct mdnsd *svr, const char *instance_name, 
		const char *type, uint16_t port, const char *hostname, const char *txt[]) {
	struct rr_entry *txt_e = NULL, 
					*srv_e = NULL, 
					*ptr_e = NULL,
					*bptr_e = NULL;
	uint8_t *target;
	uint8_t *inst_nlabel, *type_nlabel, *nlabel;
	struct mdns_service *service = malloc(sizeof(struct mdns_service));
	memset(service, 0, sizeof(struct mdns_service));

	// combine service name
	type_nlabel = create_nlabel(type);
	inst_nlabel = create_nlabel(instance_name);
	nlabel = join_nlabel(inst_nlabel, type_nlabel);

	// create TXT record
	if (txt && *txt) {
		txt_e = rr_create(dup_nlabel(nlabel), RR_TXT);
		rr_list_append(&service->entries, txt_e);

		// add TXTs
		for (; *txt; txt++) 
			rr_add_txt(txt_e, *txt);
	}

	// create SRV record
	assert(hostname || svr->hostname);	// either one as target
	target = hostname ? 
				create_nlabel(hostname) : 
				dup_nlabel(svr->hostname);

	srv_e = rr_create_srv(dup_nlabel(nlabel), port, target);
	rr_list_append(&service->entries, srv_e);

	// create PTR record for type
	ptr_e = rr_create_ptr(type_nlabel, srv_e);

	// create services PTR record for type
	// this enables the type to show up as a "service"
	bptr_e = rr_create_ptr(dup_nlabel(SERVICES_DNS_SD_NLABEL), ptr_e);

	// modify lists here

	if (txt_e)
		rr_group_add(&svr->group, txt_e);
	rr_group_add(&svr->group, srv_e);
	rr_group_add(&svr->group, ptr_e);
	rr_group_add(&svr->group, bptr_e);

	// append PTR entry to announce list
	rr_list_append(&svr->announce, ptr_e);
	rr_list_append(&svr->services, ptr_e);

	// don't free type_nlabel - it's with the PTR record
	free(nlabel);
	free(inst_nlabel);

	return service;
}

void mdns_service_destroy(struct mdns_service *srv) {
	assert(srv != NULL);
	rr_list_destroy(srv->entries, 0);
	free(srv);
}

void mdnsd_initialize(struct mdnsd *svr, uint8_t* devName) {
	struct rr_entry *a2_e = NULL;
	char instanceName[64] = {0,};
	char hostName[64] = {0,};

	sprintf(instanceName, "%s", devName);

	sprintf(hostName, "%s.local", instanceName);
	printf("mDNS Host Name : %s\r\n", hostName);

	mdnsd_set_hostname(svr, hostName, nmi_inet_addr(gIpAddressFromAp));
	
	a2_e = rr_create_a(create_nlabel(hostName), nmi_inet_addr(gIpAddressFromAp));
	mdnsd_add_rr(svr, a2_e);

	const char *txt[] = {
		"", 
		NULL
	};
	
	mdnsd_register_svc(svr, instanceName, "_http._tcp.local", DEFAULT_HTTP_PORT, NULL, txt);
}

uint8_t mdnsd_start(uint8_t* devName)
{
	printf("mdnsd_start\r\n");
	
	memset(&mdnsSvr, 0, sizeof(struct mdnsd));

	mdnsSvr.sockfd = create_recv_sock();
	if (mdnsSvr.sockfd < 0) {
		printf("unable to create recv socket\r\n");
		return 0;
	}
	
	mdnsd_initialize(&mdnsSvr, devName);
	
	process_resp_mdns_pkt(&mdnsSvr);
	
	return 1;
}

void mdnsd_send_response(void)
{
	if (mdnsSvr.sockfd >= 0 && gRespPacketLen)
	{
		send_packet(mdnsSvr.sockfd, mdnsRespPacket, gRespPacketLen);
	}
	else
	{
		printf("Error, mDNS Data(%d)\r\n", gRespPacketLen);
	}
	
}


void mdnsd_stop(void)
{
	printf("mdnsd_stop\r\n");
	
	if (mdnsSvr.sockfd >= 0)
	{
		close(mdnsSvr.sockfd);
	}
	
	rr_group_destroy(mdnsSvr.group);
	rr_list_destroy(mdnsSvr.announce, 0);
	rr_list_destroy(mdnsSvr.services, 0);

	if (mdnsSvr.hostname)
		free(mdnsSvr.hostname);
}

