#include <iostream>
#include "command.h"
#include "chat/chat_server.hpp"
using namespace std;

int main(int argc, char** argv){
	//Command cmd("a71630f9 09:30.1234 SENDTEXT Hello World!");
	try
	  {
	    if (argc < 2)
	    {
	      cerr << "Usage: chat_server <port> [<port> ...]\n";
	      return 1;
	    }

	    boost::asio::io_service io_service;

	    list<chat_server> servers;
	    for (int i = 1; i < argc; ++i)
	    {
	      tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
	      servers.emplace_back(io_service, endpoint);
	    }

	    io_service.run();
	  }
	  catch (exception& e)
	  {
	    cerr << "Exception: " << e.what() << "\n";
	  }

	  return 0;
}
