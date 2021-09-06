#
# This makefile builds an executable file for
# offline intertial sdk running under Linux
#
GAMP_DIR=.
TARGET_DIR=.

#get all cpp files in specified directories
SOURCE = $(wildcard $(GAMP_DIR)/*.cpp) \

#create obj file name for each cpp file
OBJS = $(patsubst %.cpp,%.o,$(SOURCE))

INC=-I$(GAMP_DIR)
CFLAGS= -O3 -std=c++11 -o

CC=/usr/bin/g++
RM=/bin/rm
TARGET=run_GOOD

build_cmd: $(OBJS)
	$(CC) $(INC) $^ -o $(TARGET_DIR)/$(TARGET)

%.o:%.cpp
	$(CC) $(INC) -c $(CFLAGS) $@ $<

clean_obj:
	$(RM) -f $(GAMP_DIR)/*.o

clean_exe:
	$(RM) -f $(GAMP_DIR)/$(TARGET)
