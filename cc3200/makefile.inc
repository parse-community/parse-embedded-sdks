#**********************************************************************************
#
# Common rules for building the CC3200 libraries and samples.
#
#**********************************************************************************


#
# The CC3200 directory.
#
CC3200_SDK_ROOT:=${HOME}/devtools/cc3200-sdk


#
# The Parse Embedded C SDK for CC3200 directory.
#
PARSE_CC3200_SDK_ROOT:=${PARSE_SDK_ROOT}/cc3200


#
# Include the common make definitions.
#
include ${CC3200_SDK_ROOT}/tools/gcc_scripts/makedefs


#
# Additional Compiler Flags
#
CFLAGS+=-DPART_CC3200 -DSL_FULL -DSL_PLATFORM_MULTI_THREADED -DUSE_FREERTOS
CFLAGS+=-Wall


#
# Where to find header files that do not live in the source directory.
#
IPATH+=${CC3200_SDK_ROOT}/simplelink/include
IPATH+=${CC3200_SDK_ROOT}/driverlib
IPATH+=${CC3200_SDK_ROOT}/oslib
IPATH+=${CC3200_SDK_ROOT}/inc


#
# Turn verbose output on or off
#
VERBOSE=1
