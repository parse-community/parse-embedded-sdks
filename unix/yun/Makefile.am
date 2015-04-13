AM_LDFLAGS = -L$(top_srcdir)/unix/src/.libs -lparse
AM_CFLAGS = -I$(top_srcdir)/include

bin_PROGRAMS = parse_request parse_push
parse_request_SOURCES = parse_request.c yun.c client.c utils.c
parse_push_SOURCES = parse_push.c yun.c client.c utils.c

