CC=gcc
CXX=g++

ifndef window
window = 75
endif

CFLAGS = -g -c -Wall -pedantic -DPACKET_BURST_SIZE=$(window)
CPPFLAGS = -std=c++17
SP_LIBRARY=/home/cs417/exercises/ex3/libspread-core.a /home/cs417/exercises/ex3/libspread-util.a
MCAST_OBJ = machine.o mcast.o


all: mcast

mcast: $(MCAST_OBJ)
		$(CXX) -o mcast $(MCAST_OBJ) -ldl $(SP_LIBRARY)

clean:
	rm *.o
	rm mcast

%.o:    %.c
	$(CC) $(CFLAGS) $*.c

%.o:	%.cpp
	$(CXX)  $(CFLAGS) $*.cpp

%.o:	%.hpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) $*.hpp