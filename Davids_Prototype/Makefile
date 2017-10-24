CXXFLAGS= -Wall -g -O0 -std=c++11 
LDLIBS =  -lboost_system -lpthread 

EXECUTABLES = chat_gui chat_server

.PHONY: all chat_server chat_gui

all: ${EXECUTABLES}

chat_server: chat_message.hpp chat_server.cpp

chat_gui: 
	g++ -std=c++11 `fltk-config --cxxflags` chat_client.cpp  `fltk-config --ldflags` -o chat_gui -lboost_system -lpthread

clean:
	rm -f ${EXECUTABLES}
