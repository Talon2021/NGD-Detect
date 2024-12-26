# Include paths
include ../profile
ARCH := arm
CROSS_COMPILE_PREFIX := aarch64-buildroot-linux-gnu-
CROSS_COMPILE_PATH := /opt/rk3588_host/host/bin

CC := $(CROSS_COMPILE_PREFIX)gcc
CXX := $(CROSS_COMPILE_PREFIX)g++
STRIP := $(CROSS_COMPILE_PREFIX)strip
RM := rm -rf
INSTALL := install -cv -m 777
CFLAGS += -fPIC -shared 
CFLAGS += -Wall -std=c++17 -fcommon
RFLAGS += $(CFLAGS) -Os -DNDEBUG
DFLAGS += $(CFLAGS) -W -g 
TESTFLAGS += -O3 -DNDEBUG -Wall
LFLAGS += -lpthread #-ldbus-1

LIBDIR := ../../lib

# Include directories
CFLAGS += -I../../../moudle/include/minini
CFLAGS += -I../../../moudle/include/mahony
CFLAGS += -I../../../moudle/mqtt
CFLAGS += -I../../../moudle/ip
CFLAGS += -I../../../moudle/include
CFLAGS += -I../../inc
CFLAGS += -I../include -I../common
CFLAGS += -I../NET
CFLAGS += -I../MSG
CFLAGS += -I../../include

# Conditional flags
ifeq ($(ALG_SDK),yes)
    CFLAGS += -DALG_SDK
endif

ifeq ($(HTTP_UPLOAD),yes)
    CFLAGS += -DHTTP_UPLOAD
endif

ifeq ($(RTSP_PREVIEW),yes)
    CFLAGS += -DRTSP_PREVIEW
endif

ifeq ($(ONVIF_PRO),yes)
    CFLAGS += -DONVIF_PRO
endif

ifeq ($(NET_WLAN),yes)
    CFLAGS += -DNET_WLAN
endif

ifeq ($(NET_ETHERNET),yes)
    CFLAGS += -DNET_ETHERNET
endif

ifeq ($(PROCESS_CTRL),yes)
    CFLAGS += -DPROCESS_CTRL
endif

ifeq ($(RK3588_HGD_DETECT),yes)
    CFLAGS += -DRK3588_HGD_DETECT
endif

ifeq ($(RK3588_NGD_DETECT),yes)
    CFLAGS += -DRK3588_NGD_DETECT
endif

# Library directories
LDFLAGS += -L./lib -L../../lib

# Libraries
LDLIBS += -lstdc++ -lm -ldl -shared -fPIC -lpthread
LDLIBS += -DWITH_DBG_STRDUP 

# Compiler flags
CFLAGS += -Wall -Werror=jump-misses-init -Wno-unused-function -fPIC -Wno-packed-bitfield-compat
CFLAGS += -Wall -Werror=implicit-function-declaration -Werror=implicit-int -Werror=return-type
CFLAGS += -fpermissive

# Build flags
RFLAGS = -O3 $(CFLAGS) -g
DFLAGS = -g $(CFLAGS)

# Target
TARGET := libMidLayer.so

# Build directories
RELEASE_DIR_SRC := Release
RELEASE_DIR := $(RELEASE_DIR_SRC)/Release
DEBUG_DIR := Debug

# Source directories
SRC_DIRS := ../ ../AVL ../MPU ../common ../LOG ../MSG ../NET

# Find all C and CPP source files
CFILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
CPPFILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Object files
OBJS := $(patsubst %.c,%.o, $(CFILES))
CPPOBJS := $(patsubst %.cpp,%.o, $(CPPFILES))

# Target paths
RELEASE_TARGET := $(RELEASE_DIR)/$(TARGET)

%/.:
	@mkdir -m 777 -p $*

all: release

.PHONY: clean

release: $(RELEASE_DIR)/. $(RELEASE_TARGET)

$(RELEASE_TARGET): $(addprefix $(RELEASE_DIR)/,$(OBJS)) $(addprefix $(RELEASE_DIR)/,$(CPPOBJS))
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	@md5sum $@
	$(INSTALL) $@ ${LIBDIR}
	cp $@ /home/share/nfs/ngd-detect/ -rf

$(RELEASE_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo " [CC] $<"
	@$(CC) $(RFLAGS) -c $< -o $@

$(RELEASE_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo " [CPP] $<"
	@$(CXX) $(RFLAGS) -c $< -o $@

clean:
	@rm -rvf $(RELEASE_DIR_SRC)
