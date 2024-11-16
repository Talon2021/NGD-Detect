include ../profile
#include ./porfile
ARCH:=arm
CROSS_COMPILE_PREFIX=arm-linux-gnueabihf-
CROSS_COMPILE_PATH=/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin

LIBDIR=../../lib

CC=$(CROSS_COMPILE_PREFIX)gcc
CXX=$(CROSS_COMPILE_PREFIX)g++
STRIP=$(CROSS_COMPILE_PREFIX)strip
RM=rm -rf
INSTALL=install -cv -m 777
CFLAGS+= -fPIC -shared 
CFLAGS+= -Wall 
RFLAGS+= $(CFLAGS) -Os -DNDEBUG
DFLAGS+= $(CFLAGS) -W -g
TESTFLAGS+= -O3 -DNDEBUG -Wall
LFLAGS+= -lpthread #-ldbus-1

ifeq ($(JPMPP_SDK),yes)
CFLAGS += -DJPMPP_SDK
endif

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

#moudle include
INCPATH+=-I../../../moudle/include/minini
INCPATH+=-I../../../moudle/include/mahony
INCPATH+=-I../../../moudle/include
INCPATH+=-I../../../moudle/ip

#network
INCPATH+=-I../../include/network

#mid
INCPATH+=-I../../inc
INCPATH+=-I../include -I../common -I../NET
LIBPATH+= -L../../lib

#alg include
INCPATH+=-I../../include/algdetect

INCPATH+=-I../../include

MPATH=$(shell pwd)
LIBNAME=$(notdir $(MPATH))
BIN=lib$(LIBNAME).so

DEBUG_DIR=DEBUG-$(ARCH)
RELEASE_DIR=RELEASE-$(ARCH)
TEST_DIR=test

DEBUG_TARGET=$(DEBUG_DIR)/$(BIN)
RELEASE_TARGET=$(RELEASE_DIR)/$(BIN)
TARGETS=$(DEBUG_TARGET) $(RELEASE_TARGET)

SRC_DIRS := $(shell find . -maxdepth 3 -type d )
CPPFILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJECTS=$(patsubst %.c, %.o, $(wildcard *.c))
OBJECTS+= $(patsubst %.cpp,%.o, $(CPPFILES))

.PHONY:debug relase clean build test install

release:$(RELEASE_DIR)/. ${LIBDIR}/. $(PROTO_TARGET) $(RELEASE_TARGET)
debug:$(DEBUG_DIR)/. ${LIBDIR}/. $(PROTO_TARGET) $(DEBUG_TARGET)

clean:
	$(RM) ${OBJECTS} ${TARGETS}
	$(RM) RELEASE-* DEBUG-*
	$(RM) ../../lib/$(BIN) -rf
	$(RM) $(LIBDIR)/$(BIN) -rf
build:
%/.:
	mkdir -m 777 -p $*
	cd $(MPATH)/$* && mkdir -m 777 -p $(SRC_DIRS)
test:
	$(CXX) $(TEST_DIR)/*.cpp -o $(TEST_DIR)/$(LIBNAME)_test $(TESTFLAGS) $(INCPATH) $(LIBPATH) $(LFLAGS) -l$(LIBNAME)
	$(INSTALL) $(TEST_DIR)/$(LIBNAME)_test $(CROSS_COMPILE_PATH)/usr/bin

$(DEBUG_TARGET):$(addprefix $(DEBUG_DIR)/,$(OBJECTS))
	$(CXX) -o $@ $^ $(LFLAGS) $(LIBPATH) $(DFLAGS)
	$(INSTALL) $@ ${LIBDIR} 
	
$(RELEASE_TARGET):$(addprefix $(RELEASE_DIR)/,$(OBJECTS))
	$(CXX) -o $@ $^ $(LFLAGS) $(LIBPATH) $(RFLAGS)
	$(STRIP) $@
	$(INSTALL) $@ ${LIBDIR}
# cp $@ ../../lib -rf
	cp $@ /home/share/nfs/ngd-detect/ -rf
#	$(INSTALL) $@ ${FS_LIBDIR}
	
$(DEBUG_DIR)/%.o:%.c
	$(CC) -c $^ -o $@ $(DFLAGS) $(INCPATH)

$(RELEASE_DIR)/%.o:%.c
	$(CC) -c $^ -o $@ $(RFLAGS) $(INCPATH)

$(DEBUG_DIR)/%.o:%.cpp
	$(CXX) -c $^ -o $@ $(DFLAGS) $(INCPATH)

$(RELEASE_DIR)/%.o:%.cpp
	$(CXX) -c $^ -o $@ $(RFLAGS) $(INCPATH)