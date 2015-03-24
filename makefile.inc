#**********************************************************************************
#
# Common rules for building the Embedded C SDK libraries and samples.
#
#**********************************************************************************


#
# The project directory.
#
PROJECT_ROOT:=$(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST)))))


#
# The Parse Embedded C SDK directory.
#
PARSE_SDK_ROOT:=$(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))


#
# Where to find header files that do not live in the source directory.
#
IPATH+=$(PARSE_SDK_ROOT)/include
IPATH+=$(PARSE_SDK_ROOT)/common


#
# Where to find source files that do not live in this directory.
#
VPATH+=$(PARSE_SDK_ROOT)/common
