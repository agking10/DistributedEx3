CC=gcc
CXX=g++

CFLAGS = -g -c -Wall -pedantic
CPPFLAGS = -std=c++17
INCLUDE_FLAGS = -I. -I/include
SP_LIBRARY = libspread-core.a libspread-util.a

MCAST_OBJ = 

all: test mcast


mcast: $(MCAST_OBJ)
		$(CXX) -o mcast $(MCAST_OBJ) -ldl $(SP_LIBRARY)

clean:
	rm *.o
	rm mcast

%.o:    %.c
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) $*.c

%.o:	%.cpp
	$(CXX)  $(CFLAGS) $(INCLUDE_FLAGS) $*.cpp

%.o:	%.hpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(INCLUDE_FLAGS) $*.hpp
