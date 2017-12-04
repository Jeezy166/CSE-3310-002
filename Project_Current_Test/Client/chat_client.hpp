/*
chat_client.hpp
 ~~~~~~~~~~~~~~~

Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

*/
#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Display.H>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <locale>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include "chat_message.hpp"

using namespace std;
using boost::asio::ip::tcp;

class SimpleWindow;

extern SimpleWindow* win_ptr;
extern void recieveMessageToGui(SimpleWindow* win, string msg);
extern mutex mtx;

//---------------------------------------------------------------------------------------------------------------------------------------------------------//

typedef deque<chat_message> chat_message_queue;

class chat_client
{
public:
  Fl_Text_Buffer* text_buffer;
  string output_text_string;
  string last_cmd;

  chat_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator)
    : io_service_(io_service),
      socket_(io_service)
  {
    do_connect(endpoint_iterator);
  }

  void write(const chat_message& msg)
  {
    io_service_.post(
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void close()
  {
    io_service_.post([this]() { socket_.close(); });
  }

private:
  void do_connect(tcp::resolver::iterator endpoint_iterator)
  {
    boost::asio::async_connect(socket_, endpoint_iterator,
        [this](boost::system::error_code ec, tcp::resolver::iterator)
        {
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this](boost::system::error_code ec, size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](boost::system::error_code ec, size_t /*length*/)
        {
          if (!ec)
          {
	    //mtx.lock();
		cout << "Message recieved! " << read_msg_.body() << endl;
		string msg = string(read_msg_.body(), read_msg_.body_length());
		//if(last_cmd == )
		if(msg.substr(0,5) == "TEXT ")
		{
	    		// Append the message from the server into the text buffer 
	    		output_text_string.append(msg.substr(5));
	    		output_text_string.append("\n");
			
	    		// Update text buffer pointer
	    		text_buffer->text(output_text_string.c_str());
			recieveMessageToGui(win_ptr, msg);
            		do_read_header();
		} else {
			recieveMessageToGui(win_ptr, msg);
            		do_read_header();
		}
	    //mtx.unlock();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write()
  {
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](boost::system::error_code ec, size_t /*length*/)
        {
          if (!ec)
          {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
  }

private:
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
  
  
};


