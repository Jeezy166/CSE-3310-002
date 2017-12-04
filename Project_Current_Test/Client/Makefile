CXXFLAGS= -Wall -g -O0 -std=c++11 
LDLIBS =  -lboost_system -lpthread 

EXECUTABLES = chat_gui

.PHONY: all chat_gui

all: ${EXECUTABLES}

chat_gui: 
	g++ -std=c++11 `fltk-config --cxxflags` FLTK_GUI.cpp client_main.cpp  `fltk-config --ldflags` -o chat_gui -lboost_system -lboost_date_time -lpthread -lz

clean:
	rm -f ${EXECUTABLES}
