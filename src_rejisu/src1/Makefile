ifdef CPU
ROOTDIR    ?= /home/girish/ati-stream-sdk-v2.2-lnx64
LIB  :=  -L$(ROOTDIR)/common/lib
INCLUDES  := -I$(ROOTDIR)/include
CFLAGS := -DCPU
else
ifdef ATI
LIB  := -L$(ATISTREAMSDKROOT)/lib/x86_64/
INCLUDES  := -I$(ATISTREAMSDKROOT)/include
else
ifdef FERMI
ROOTDIR    ?= /panfs/panasas1/users/girishr@ufl.edu/NVIDIA_CUDA_SDK_3.2RC/OpenCL
LIB  := -L/usr/lib64
INCLUDES  := -I$(ROOTDIR)/common/inc
else
ROOTDIR    ?= /opt/cuda-sdk/OpenCL/common/
LIB  := -L/usr/lib64/
INCLUDES  := -I$(ROOTDIR)/common/inc
endif
endif
endif

ifdef DE
CFLAGS += -ggdb3 -DDEBUG
endif

CXXWARN_FLAGS := \
        -W -Wall \
        -Wimplicit \
        -Wswitch \
        -Wformat \
        -Wchar-subscripts \
        -Wparentheses \
        -Wmultichar \
        -Wtrigraphs \
        -Wpointer-arith \
        -Wcast-align \
        -Wreturn-type \
        -Wno-unused-function \
    $(SPACE)

LIB += -lOpenCL
INCLUDES += -I../../include/
CPPFILES = assoc.cpp  clutil.cpp  fileread.cpp  main.cpp primes.cpp
OBJFILES := $(patsubst %.cpp,%.o, $(CPPFILES))
HFILES := $(wildcard *.h)

assoc: $(OBJFILES)
	g++ $(CXXFLAGS) $(CFLAGS)  -o $@ $(OBJFILES)  $(INCLUDES) $(LIB)

%.o: %.cpp $(HFILES)
	g++ $(CXXFLAGS) $(CFLAGS)  -c -o $@ $< $(INCLUDES)

all:classoc

clean:
	rm -rf *.o assoc

