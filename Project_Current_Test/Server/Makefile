EXECUTABLES = server

SOURCE_FILES = command.cpp server_main.cpp

.PHONY: all server

all: ${EXECUTABLES}

server: 
	g++ -Wall -g -O0 -std=c++11 $(SOURCE_FILES) -o server -lboost_system -lpthread -lz

clean:
	rm -f ${EXECUTABLES}
