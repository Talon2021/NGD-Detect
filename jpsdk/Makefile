
CROSS_COMPILE = aarch64-buildroot-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
STRIP = $(CROSS_COMPILE)strip

DESTDIR := /home/share/nfs/ngd-detect/
CFLAGS += -I./inc


CFLAGS += -I./include/gpio
CFLAGS += -I./include/detect
CFLAGS += -I./include/rknn
CFLAGS += -I./include/librga
CFLAGS += -I./include/opencv

CFLAGS += -I./src/common
CFLAGS += -I./src/hal
CFLAGS += -I./src/logic
CFLAGS += -I./src/detect


LDFLAGS += -L./lib
LDLIBS += -lstdc++ -lm -ldl -shared -fPIC -lpthread
LDLIBS += -DWITH_DBG_STRDUP 

# Mgvl1 lib
LDFLAGS += -finput-charset=gbk

CFLAGS += -Wall -Werror=jump-misses-init -Wno-unused-function -fPIC -Wno-packed-bitfield-compat
CFLAGS += -Wall -Werror=implicit-function-declaration -Werror=implicit-int  -Werror=return-type
CFLAGS += -fpermissive
#CFLAGS += -fcommon

RFLAGS = -O3 $(CFLAGS) -g
# RFLAGS = $(CFLAGS) -g
DFLAGS = -g $(CFLAGS)

TARGET := libjpsdk.so

SRC_DIRS := . src/common src/hal src/logic src/detect

CFILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS_NOTSORT = $(patsubst %.c,%.o, $(CFILES))
CPPFILES = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
CPPOBJS_NOTSORT = $(patsubst %.cpp,%.o, $(CPPFILES))

OBJS = $(sort $(OBJS_NOTSORT))
CPPOBJS = $(sort $(CPPOBJS_NOTSORT))

RELEASE_DIR = Release
DEBUG_DIR = Debug

RELEASE_TARGET = $(RELEASE_DIR)/$(TARGET)
DEBUG_TARGET = $(DEBUG_DIR)/$(TARGET)

%/.:
	mkdir -m 777 -p $*

all: release
.PHONY : clean
release: $(RELEASE_DIR)/. $(RELEASE_DIR)/src/. $(RELEASE_DIR)/src/common/. $(RELEASE_DIR)/src/hal/. $(RELEASE_DIR)/src/logic/. $(RELEASE_DIR)/src/detect/. $(RELEASE_TARGET)
debug: $(DEBUG_DIR)/. $(DEBUG_DIR)/src/. $(DEBUG_DIR)/src/common/.  $(DEBUG_DIR)/src/hal/. $(DEBUG_DIR)/src/logic/. $(DEBUG_DIR)/src/detect/. $(DEBUG_TARGET)

$(RELEASE_TARGET):$(addprefix $(RELEASE_DIR)/,$(OBJS)) $(addprefix $(RELEASE_DIR)/,$(CPPOBJS))
	$(CC) -o $@  $^  $(LDFLAGS) $(LDLIBS)
#$(STRIP) $@
	@cp -v $(RELEASE_TARGET) ./
	@md5sum ./$(TARGET)
	cp $@ ../middleware/lib/ -rf
	cp $@ $(DESTDIR) -rf

$(DEBUG_TARGET):$(addprefix $(DEBUG_DIR)/,$(OBJS)) $(addprefix $(DEBUG_DIR)/,$(CPPOBJS))
	$(CC) -o $@  $^  $(LDFLAGS) $(LDLIBS)

$(DEBUG_DIR)/%.o:%.c
	@echo " [CC] $<"
	@$(CC) $(DFLAGS) -c $^ -o $@

$(DEBUG_DIR)/%.o: %.cpp
	@echo " [CPP] $<"
	@$(CPP) $(DFLAGS) -c $< -o $@

$(RELEASE_DIR)/%.o:%.c
	@echo " [CC] $<"
	@$(CC) $(RFLAGS) -c $^ -o $@

$(RELEASE_DIR)/%.o: %.cpp
	@echo " [CPP] $<"
	@$(CPP) $(RFLAGS) -c $< -o $@

clean:
	@rm -rvf $(RELEASE_DIR) $(DEBUG_DIR)
