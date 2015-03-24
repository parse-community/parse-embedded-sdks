#**********************************************************************************
#
# Rules for building the Blink sample application.
#
#**********************************************************************************


-include ../../../makefile.inc
-include ../../makefile.inc
-include ../makefile.inc


#
# Where to find source files that do not live in this directory.
#
VPATH+=${CC3200_SDK_ROOT}/example/common
VPATH+=${CC3200_SDK_ROOT}/drivers
VPATH+=${CC3200_SDK_ROOT}/common


#
# Where to find header files that do not live in the source directory.
#
IPATH+=${CC3200_SDK_ROOT}/example/common
IPATH+=${CC3200_SDK_ROOT}/third_party/FreeRTOS


#
# The final target file name
#
TARGET=${BINDIR}/blink.axf


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
# Rules for building the blink example.
#
${TARGET}: \
  ${OBJDIR}/blink.o \
  ${OBJDIR}/board.o \
  ${OBJDIR}/button.o \
  ${OBJDIR}/config.o \
  ${OBJDIR}/json.o \
  ${OBJDIR}/led.o \
  ${OBJDIR}/main.o \
  ${OBJDIR}/network.o \
  ${OBJDIR}/pinmux.o \
  ${OBJDIR}/time.o \
  ${OBJDIR}/urlencode.o \
  \
  ${OBJDIR}/uart_if.o \
  ${OBJDIR}/button_if.o \
  ${OBJDIR}/gpio_if.o \
  ${OBJDIR}/i2c_if.o \
  ${OBJDIR}/startup_${COMPILER}.o \
  \
  ${PARSE_CC3200_SDK_ROOT}/src/${BINDIR}/libparse.a \
  ${CC3200_SDK_ROOT}/simplelink/${COMPILER}/${BINDIR}/libsimplelink.a \
  ${CC3200_SDK_ROOT}/driverlib/${COMPILER}/${BINDIR}/libdriver.a \
  ${CC3200_SDK_ROOT}/oslib/${COMPILER}/${BINDIR}/FreeRTOS.a \

SCATTERgcc_blink=blink.ld

ENTRY_blink=ResetISR


#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
