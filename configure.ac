AC_INIT(parse-embedded, 1.0.5, damiank@fb.com)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(common/simplejson.c)

# AC_CANONICAL_HOST is needed to access $host_os
AC_CANONICAL_HOST

os_dir=unix

case "${host_os}" in
        cygwin*|mingw*)
            os_dir=mingw
            ;;
esac

AM_CONDITIONAL([WINDOWS], [test "$os_dir" = "mingw"])
AM_CONDITIONAL([UNIX], [test "$os_dir" = "unix"])


AM_COND_IF([UNIX],
           AC_CONFIG_SRCDIR(unix/src/parse.c),
           AC_CONFIG_SRCDIR(mingw/src/parse.c)
           )

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

AM_COND_IF([UNIX],
           AC_CHECK_LIB(uuid, uuid_generate_random)
           )

AM_COND_IF([UNIX],
           AC_CHECK_HEADERS(uuid/uuid.h)
           )

AC_CACHE_CHECK([whether getopt has optreset support],
        ac_cv_have_getopt_optreset, [
            AC_LINK_IFELSE([AC_LANG_PROGRAM([[
#include <getopt.h>
            ]], [[ extern int optreset; optreset = 0; ]])],[ ac_cv_have_getopt_optreset="yes" ],[ ac_cv_have_getopt_optreset="no" 
        ])
])

if test "x$ac_cv_have_getopt_optreset" = "xyes" ; then
        AC_DEFINE(HAVE_OPTRESET, 1, [Define if optreset exists])
fi

AC_ARG_ENABLE([yun],
[  --enable-yun    Enable Arduino Yun support],
[case "${enableval}" in
  yes) yun=true ;;
  no)  yun=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-yun]) ;;
esac],[yun=false])
AM_CONDITIONAL([YUN_SUPPORT], [test x$yun = xtrue])

AC_CONFIG_FILES([Makefile common/Makefile])

AM_COND_IF([WINDOWS],
           AC_CONFIG_FILES([mingw/Makefile mingw/src/Makefile]),
           AC_CONFIG_FILES([unix/Makefile unix/src/Makefile])
           )

AM_COND_IF([YUN_SUPPORT],
           [AC_CONFIG_FILES([${os_dir}/yun/Makefile])])

AC_OUTPUT

