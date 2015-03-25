#**********************************************************************************
#
# Rules for building the ParseSDK library.
#
#**********************************************************************************


-include ../../makefile.inc
-include ../makefile.inc


#
# The final target file name
#
TARGET=${BINDIR}/libparse.a


#
# The default rule, which causes the driver library to be built.
#
all: ${OBJDIR} ${BINDIR}
all: ${TARGET}


#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${OBJDIR} ${wildcard *~}
	@rm -rf ${BINDIR} ${wildcard *~}


#
# The rule to create the target directories.
#
${OBJDIR}:
	@mkdir -p ${OBJDIR}

${BINDIR}:
	@mkdir -p ${BINDIR}


#
# Rules for building the Simplelink library with os support.
#
${TARGET}: \
  ${OBJDIR}/http.o \
  ${OBJDIR}/installation.o \
  ${OBJDIR}/parse_impl.o \
  ${OBJDIR}/push.o \
  ${OBJDIR}/request.o \
  ${OBJDIR}/session.o \
  ${OBJDIR}/simplejson.o \
  \
  ${OBJDIR}/cc3200_certificate.o \
  ${OBJDIR}/cc3200_client_state.o \
  ${OBJDIR}/cc3200_debug.o \
  ${OBJDIR}/cc3200_socket.o \
  ${OBJDIR}/cc3200_utils.o \


#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${OBJDIR}/*.d} __dummy__
endif
