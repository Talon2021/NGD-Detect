# Compiler settings - Can change to clang++ if required
CXX := aarch64-buildroot-linux-gnu-g++

DESTDIR := /home/share/nfs/ngd-detect/
# Compiler flags
CXXFLAGS := -Wall -g -I./include
CXXFLAGS += -I../middleware/src/common
CXXFLAGS += -I../middleware/src/include
CXXFLAGS += -I../middleware/include
CXXFLAGS += -I../middleware/src/MSG
CXXFLAGS += -I../middleware/inc

CXXFLAGS += -I../moudle/include/minini
CXXFLAGS += -I../moudle/include/mahony
CXXFLAGS += -I../moudle/include 
CXXFLAGS += -I../moudle/mqtt


LDFLAGS := -L../middleware/lib 
LDFLAGS += -L../middleware/lib/network
LDFLAGS += -L../middleware/lib/algdetect
LDFLAGS += -L../middleware/lib/opencv -Wl,-rpath-link,../middleware/lib/opencv
LDFLAGS += -L../middleware/lib/opencv
LDFLAGS += -L../middleware/lib/gpiod 
LDFLAGS += -L../middleware/lib/rk_lib  -Wl,-rpath-link,../middleware/lib/rk_lib
LDFLAGS += -L../middleware/lib/rk_lib
#test

# Libraries to link against
LIBS := -lMidLayer -lmahony -lminini -lip -lmqtt -lpthread 

LIBS += -ljpsdk -lgpiod 

#network
LIBS += -lnetwork -lcurl -lz -ldl

#alg detect
LIBS += -lrknnrt -lrga

#rkmedia
#LIBS += -lliveMedia -lgroupsock -lBasicUsageEnvironment -lUsageEnvironment -leasymedia -lRKAP_3A -lthird_media -lrockface -lsqlite3 -lrockx -lVSC
#LIBS += -lasound -lRKAP_ANR -lRKAP_Common
#LIBS += -lOpenVX -lGAL -lArchModelSw -lNNArchPerf
#LIBS += -ldrm -lrockchip_mpp -lrockchip_vpu -lrknn_runtime -lrknn_api  -lv4l2 -lv4lconvert  -lmd_share -lrkaiq -lrga
#LIBS += -lod_share

#opencv
LIBS += -lopencv_dnn \
        -lopencv_highgui \
        -lopencv_ml \
        -lopencv_objdetect \
        -lopencv_stitching \
        -lopencv_calib3d \
        -lopencv_features2d \
        -lopencv_flann \
        -lopencv_photo \
        -lopencv_video \
        -lopencv_videoio \
        -lopencv_imgcodecs \
        -lopencv_imgproc \
        -lopencv_core \



#： Target executable name
TARGET := my_test

# Automatically find all CPP files in the current directory
SRCS := $(wildcard *.cpp)
# Replace .cpp with .o for all source files
OBJS := $(SRCS:.cpp=.o)
# Automatically find all header files
DEPS := $(wildcard *.h)

# Default target
all: $(TARGET)

# Rule to link the program
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(LIBS) -o $@  
	cp $@ $(DESTDIR) -rf
# Rule to compile the program
%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

# Phony target for clean-up
.PHONY: clean

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f $(DESTDIR)/$(TARGET)
# Phony target for debug
.PHONY: debug

debug:
	@echo "Source files: $(SRCS)"
	@echo "Object files: $(OBJS)"
	@echo "Dependency files: $(DEPS)"