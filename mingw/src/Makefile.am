LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

lib_LTLIBRARIES=libparse.la
libparse_la_SOURCES=parse.c parse_time.c parse_uuid.c parse_log.c parse_os.c
include_HEADERS=$(top_srcdir)/include/parse.h
libparse_la_CFLAGS=-fPIC -I$(top_srcdir)/common -I$(top_srcdir)/include -I$(top_srcdir)
libparse_la_CPPFLAGS=-fPIC
libparse_la_CXXFLAGS=-fPIC
libparse_la_LDFLAGS= -no-undefined -shared -fPIC 
libparse_la_LIBADD=$(top_srcdir)/common/libcommon.la

SUBDIRS=../../common
