/*
 * file_entity.h
 *
 * Created: 2014-09-17 오후 5:59:01
 *  Author: WRG_D234
 */ 

#include "asf.h"
#include "http_entity.h"


#ifndef FILE_ENTITY_H_
#define FILE_ENTITY_H_


static const char* parse_server_entity_get_contents_type(void *priv_data)
{
	return "application/json";
}

int parse_server_entity_get_contents_length(void *priv_data)
{
	if( priv_data )
		return strlen(priv_data);

	return 0;
}

static int parse_server_entity_read(void *priv_data, char *buffer, uint32_t size, uint32_t written)
{
	int length = strlen(priv_data) -written;
	int min = (size > length) ? length : size;
	memcpy(buffer, priv_data + written, min);
	return min;
}

static void parse_server_entity_close(void *priv_data)
{
	if(priv_data)
	{
		free(priv_data);
		priv_data = NULL;
	}
}

static void parse_server_entity_get(struct http_entity *entity, char* json_data)
{
	if( json_data )
	{
		int json_data_length = strlen(json_data);
		entity->priv_data = (void*)malloc(json_data_length + 1);
		memset( entity->priv_data, 0, json_data_length + 1 );
		memcpy(entity->priv_data, json_data, strlen(json_data));
	}
	else
	{
		entity->priv_data = 0;
	}
	entity->get_contents_type = parse_server_entity_get_contents_type;
	entity->get_contents_length = parse_server_entity_get_contents_length;
	entity->is_chunked = 0;
	entity->read = parse_server_entity_read;
	entity->close = parse_server_entity_close;
}


#endif /* FILE_ENTITY_H_ */

