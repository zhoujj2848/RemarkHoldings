# 
# demo_b Make file
#

GPP=g++
CFLAGS=-Wall -g
LDFLAGS=-lpthread

PROGRAM = demo_b

SOURCES  = main.cpp \
	

HEADERS  = $(filter %.h, $(SOURCES))
CPPFILES   = $(filter %.cpp, $(SOURCES))
OBJS     = $(patsubst %.cpp, %.o, $(CPPFILES))

.PHONY: clean install

all: $(PROGRAM)

demo_b: $(OBJS)
	$(GPP) $^ -o $@ $(LDFLAGS)
	
$(OBJS):  $(HEADERS)
%.o: %.cpp
	$(GPP) $(LDFLAGS) $(CFLAGS) -c $< -o $@
	
clean:
	-rm -fv $(PROGRAM) $(OBJS)
