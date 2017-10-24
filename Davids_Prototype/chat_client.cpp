/*
chat_client.cpp
 ~~~~~~~~~~~~~~~

Copyright (c) 2003-2013 Christopher M. Kohlhoff (chris at kohlhoff dot com)

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


chat_gui.cpp
 ~~~~~~~~~~~~~~~

Code modified by David Benepe on October 17th 2017

This is a simple example on how to use FLTK with the boost client/server code.

*/
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Display.H>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <mutex>
#include <boost/asio.hpp>
#include "chat_message.hpp"
using namespace std;
using boost::asio::ip::tcp;

/* This mutex is here to keep the shared pointers between the FLTK thread and the chat_client thread from causing any weird behavior. */
mutex mtx;

//---------------------------------------------------------------------------------------------------------------------------------------------------------//

typedef deque<chat_message> chat_message_queue;

class chat_client
{
public:
  Fl_Text_Buffer* text_buffer;
  string output_text_string;

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
	    mtx.lock();
	    	// Append the message from the server into the text buffer, 
	    	output_text_string.append(string(read_msg_.body(), read_msg_.body_length()));
	    	output_text_string.append("\n");

	    	// Update text buffer pointer
	    	text_buffer->text(output_text_string.c_str());

            	do_read_header();
	    mtx.unlock();
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

//---------------------------------------------------------------------------------------------------------------------------------------------------------//
 
class SimpleWindow : public Fl_Window{
 
   public:
      SimpleWindow(int w, int h, const char* title );
      ~SimpleWindow();
      Fl_Button* send;
      Fl_Input* inp;
      Fl_Text_Display* text_output;
      Fl_Text_Buffer* text_buffer;

      chat_client* c;
      thread* t;
   
   private:
      static void cb_send(Fl_Widget*, void*);
      inline void cb_send_i();
 
      static void cb_quit(Fl_Widget*, void*);
      inline void cb_quit_i();
};

//----------------------------------------------------


SimpleWindow::SimpleWindow(int w, int h, const char* title):Fl_Window(w,h,title){

   begin();
      callback(cb_quit, this);

      // Init chat output
      text_buffer = new Fl_Text_Buffer();
      text_output = new Fl_Text_Display(10, 10,w - 20, h - 50);
      text_output->buffer(text_buffer);
      text_output->wrap_mode(Fl_Text_Display::WRAP_AT_BOUNDS, 0); // Enable word wrap in output window

      // Init message input & send button
      inp = new Fl_Input(10, 265, 335, 25);
      send = new Fl_Button(350, 265, 40, 25, "Send");
      send->callback(cb_send, this);

   end();
   show();
}

//----------------------------------------------------

SimpleWindow::~SimpleWindow(){}

//----------------------------------------------------

void SimpleWindow::cb_send(Fl_Widget* , void* v) {
    ( (SimpleWindow*)v )->cb_send_i();
}

void SimpleWindow::cb_send_i() {
      	char* msg_text = (char*)inp->value();
	
    	 // Send message to server if message  box is not empty.
	if(strlen(msg_text) > 0) {
		mtx.lock();
      			chat_message msg;
      			msg.body_length(strlen(msg_text));
      			memcpy(msg.body(), msg_text, msg.body_length());
      			msg.encode_header();
      			c->write(msg);
		mtx.unlock();
	}

 	// Clear message box
	inp->value("");
}

//----------------------------------------------------

void SimpleWindow::cb_quit(Fl_Widget* , void* v) {
    ( (SimpleWindow*)v )->cb_quit_i();
}

void SimpleWindow::cb_quit_i() {
     mtx.lock();
     	// Close the chat client and boost thread when the window closes.
    	c->close();
    	t->join();
    mtx.unlock();

    hide();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------//
 
int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      cerr << "Usage: gui_client <host> <port>\n";
      return 1;
    }

    // Create GUI window
    SimpleWindow win(400,300,"FLTK chat client");

    // Startup boost stuff
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    auto endpoint_iterator = resolver.resolve({ argv[1], argv[2] });
    chat_client c(io_service, endpoint_iterator);
    thread t([&io_service](){ io_service.run(); });

    // Link pointers between FLTK and boost
    c.text_buffer = win.text_buffer;
    win.c = &c;
    win.t = &t;

   return Fl::run();
  }
  catch (exception& e)
  {
    cerr << "Exception: " << e.what() << "\n";
  }

}
