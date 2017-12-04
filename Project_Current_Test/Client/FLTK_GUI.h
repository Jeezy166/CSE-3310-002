#pragma once
#include <zlib.h>
#include "chat_client.hpp"

class SimpleWindow : public Fl_Window{
 
   public:
      SimpleWindow(int w, int h, const char* title );
      ~SimpleWindow();
      bool ShowUsernameRequest(string message);
      bool ShowAddNewChatroomRequest();
      void UpdateChatroomButtons();
      Fl_Button* send;
      Fl_Input* inp;
      Fl_Text_Display* text_output;
      Fl_Text_Buffer* text_buffer;
      Fl_Group* chatroom_select_buttons;
      Fl_Group* users_list;
      Fl_Button* new_chat_but;
      Fl_Button* but;

      int timer_countdown = 0;

      void chatClientCallback(string msg);

      chat_client* c;
      thread* t;
   
   private:
      Fl_Box* nickLabel;
      vector<string> chat_room_strings;
      vector<string> users;
      string nick_name = "";
      string current_chatroom_name = "";

      bool ShowInputDialog(string message, string title, Fl_Callback* callback);
      void SendServerMessage(string major_command, string argument);
      void updateChatRooms(string room_list);
      void updateUsers(string user_list);

      static void timer_update_cb(void* win);

      static void joinCR_callback_cb(Fl_Widget* w, void* v);
      void joinCR_callback_cb_i(Fl_Widget* w);
      void updateButtonStates();

      static void ShowUsernameRequest_cb(Fl_Widget* w, void* v);
      static void ShowAddNewChatroomRequest_cb(Fl_Widget* w, void* v);
	
      uint32_t CalculateChecksum(string content);
      static void cb_username_request(Fl_Widget*, void*);
	
      inline void cb_username_request_i(const char*, Fl_Window*);
      static void cb_chatroom_request(Fl_Widget*, void*);
      inline void cb_chatroom_request_i(const char*, Fl_Window*);

      static void cb_send(Fl_Widget*, void*);
      inline void cb_send_i();
      void test_nicknames();
      void test_chatrooms();
 
      static void cb_quit(Fl_Widget*, void*);
      inline void cb_quit_i();
};
