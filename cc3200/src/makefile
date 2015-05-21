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
  ${OBJDIR}/certificate_file.o \
  ${OBJDIR}/client_state.o \
  ${OBJDIR}/debug.o \
  ${OBJDIR}/socket.o \
  ${OBJDIR}/utils.o \
  \
  ${OBJDIR}/certificate.o \
  ${OBJDIR}/http.o \
  ${OBJDIR}/installation.o \
  ${OBJDIR}/parse_impl.o \
  ${OBJDIR}/push.o \
  ${OBJDIR}/request.o \
  ${OBJDIR}/session.o \
  \
  ${OBJDIR}/simplejson.o \


#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${OBJDIR}/*.d} __dummy__
endif
