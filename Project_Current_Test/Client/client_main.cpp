#include "FLTK_GUI.h"

mutex mtx;
SimpleWindow* win_ptr;

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
    SimpleWindow win(1000,600,"UberChat Client");
    win_ptr = &win;

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


    c.output_text_string = "Welcome to Uberchat!\nSelect a chat room by clicking one of the buttons on the left side!";
    c.text_buffer->text(c.output_text_string.c_str());

    win.UpdateChatroomButtons();
    
    while(!win.ShowUsernameRequest("Set your nickname."));

   return Fl::run();
  }
  catch (exception& e)
  {
    cerr << "Exception: " << e.what() << "\n";
  }

}
