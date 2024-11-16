ARCH:=arm
CROSS_COMPILE_PREFIX=arm-linux-gnueabihf-
CROSS_COMPILE_PATH=/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin

LIBDIR=../../middleware/lib

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

INCPATH+=-I../include/minini
INCPATH+=-I../include/mahony

LIBPATH+= -L../
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