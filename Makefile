
CXX=/usr/bin/g++

CFLAGS = -g3 -o0 -c -std=c++11
LFLAGS = -o

LIBS = -lpthread -lboost_filesystem -lboost_system -lboost_date_time -lsctp

SOURCES = data_sender.cpp

OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = data_sender

PREFIX = .

all: clean $(EXECUTABLE) start_test

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LFLAGS) $(EXECUTABLE) $(OBJECTS) $(LIBS)

.cpp.o:
	$(CXX) $(CFLAGS) $< -o $@

clean: 
	rm -f $(EXECUTABLE) *.o

start_test:	
	test -f $(PREFIX)/$(EXECUTABLE) && $(PREFIX)/$(EXECUTABLE)
