AC_INIT(parse-embedded, 1.0.0, damiank@fb.com)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(common/simplejson.c)
AC_CONFIG_SRCDIR(unix/src/parse.c)
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

LT_INIT(disable-static)
AC_SUBST([LIBTOOL_DEPS])

AC_SUBST(INTI_CFLAGS)
AC_SUBST(INTI_LIBS)

AC_PROG_CC
AC_SUBST(CC)

AC_CHECK_LIB(curl, curl_easy_perform)
AC_CHECK_HEADERS(curl/curl.h)
AC_CHECK_LIB(uuid, uuid_generate_random)
AC_CHECK_HEADERS(uuid/uuid.h)

AC_OUTPUT(Makefile unix/Makefile unix/src/Makefile common/Makefile)

