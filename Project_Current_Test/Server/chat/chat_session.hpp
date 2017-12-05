/*
chat_session.hpp



*/

#pragma once
#include "chat_room.hpp"

class chat_server;


class chat_session
  : public chat_participant,
    public enable_shared_from_this<chat_session>
{
public:
  chat_session(tcp::socket socket, vector<chat_room>& rooms)
    : socket_(move(socket)),
      rooms_(rooms)
  {
  }

  void start()
  {
    if(rooms_.size() < 1)
    {
    	rooms_.push_back(chat_room("Lobby", 0));
    	rooms_.push_back(chat_room("Chatroom A"));
    }
    participant_join_room(shared_from_this(), rooms_[0]);
    do_read_header();
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

private:
  void addNewChatroom(string room_name)
  {
	cout << "Adding new chatroom!" << endl;
    	rooms_.push_back(chat_room(room_name));
	cout << "New chatroom! " << room_name << ", UUID = " << rooms_.at(rooms_.size()-1).getUUID() << endl;
  }
  
  void deleteChatroom(string room_name)
  {
  	cout << "Deleting chatroom!" << endl;
	int iterator = 0;
    int size = rooms_.size();
	for(int i = 1; i< size; i++)
	{
	   string name = rooms_[i].getName();
		if((room_name == name)){
			iterator = i;
		}
	}
	cout << "Chatroom deleted! " << endl;
	rooms_.erase(rooms_.begin()+iterator);
	
  }

  chat_room& getRoomFromUUID(uint64_t UUID)
  {
	for(chat_room& room : rooms_)
	{
		if(room.getUUID() == UUID)
			return room;
	}
	return rooms_[0];
  }

  void participant_join_room(std::shared_ptr<chat_session> participant, chat_room& room)
  {
	uint64_t room_id = participant->getCurrentChatRoomID();
	if(room_id == (uint64_t)-1)
	{
		room.join(participant);
		participant->setCurrentChatRoom(room.getUUID());
		tellAllChatUsersToUpdateTheirUserLists(room);
	}
  }

  void participant_leave_room(std::shared_ptr<chat_session> participant)
  {
	uint64_t room_id = participant->getCurrentChatRoomID();
	if(room_id != (uint64_t)-1)
	{
		chat_room& room = getRoomFromUUID(room_id);
		room.leave(participant);
		tellAllChatUsersToUpdateTheirUserLists(room);
		participant->setCurrentChatRoom(-1);
	}
  }

  void switch_chat_rooms(std::shared_ptr<chat_session> participant, chat_room& next_room)
  {
	uint64_t room_id = participant->getCurrentChatRoomID();
	if(room_id != (uint64_t)-1)
	{	
		chat_room& room = getRoomFromUUID(room_id);
		room.leave(participant);
		tellAllChatUsersToUpdateTheirUserLists(room);
		next_room.join(participant);
		participant->setCurrentChatRoom(next_room.getUUID());
		tellAllChatUsersToUpdateTheirUserLists(next_room);
	}
  }

  void tellAllChatUsersToUpdateTheirUserLists(chat_room& room)
  {
	uint64_t roomID = room.getUUID();
	if(roomID == 0 || roomID == (uint64_t)-1) 
		return;
	string users_str = room.getListOfNicknames();
	if(users_str.length() > 0){
		string msg_str = string("USERS ") + users_str;
		read_msg_.body_length(msg_str.length());
		strcpy(read_msg_.body(), msg_str.c_str());
		read_msg_.encode_header();
		room.deliver(read_msg_);
	}
  }

  void tellAllChatUsersToUpdateTheirChatrooms()
  {
	for(chat_room& room : rooms_){
		uint64_t roomID = room.getUUID();
		if(roomID == (uint64_t)-1) return;
		string chat_rooms_str = listChatRooms();
		string users_str = room.getListOfNicknames();
		if(chat_rooms_str.length() > 0 && users_str.length() > 0){
			string pre_str = string("CHATROOMS ");
			read_msg_.body_length(chat_rooms_str.length() + pre_str.length());
			strcpy(read_msg_.body(), chat_rooms_str.c_str());
			prepend(read_msg_.body(), pre_str.c_str());
			read_msg_.encode_header();
			room.deliver(read_msg_);
		}
	}
  }

  string listChatRooms()
  {
	string out;
	for(size_t i = 1; i < rooms_.size(); i++)
	{
		out += rooms_[i].getName();
		if(i < rooms_.size() - 1)
			out += ",";
	}
	return out;
  }

  void do_read_header()
  {
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        [this, self](boost::system::error_code ec, size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            //room_.leave(shared_from_this());
	    participant_leave_room(shared_from_this());
          }
        });
  }

  void prepend(char* s, const char* t)
  {
      size_t len = strlen(t);
      size_t i;
  
      memmove(s + len, s, strlen(s) + 1);
  
      for (i = 0; i < len; ++i)
      {
          s[i] = t[i];
      }
  }

  bool checkIfNicknameAvaliable(string nickname) 
  {
	for(chat_room& room : rooms_) 
	{
		if(room.isNicknameTaken(nickname))
			return false;
	}
	return true;
  }

  bool checkIfRoomNameIsAvaliable(string nickname) 
  {
	for(chat_room& room : rooms_) 
	{
		if(room.getName() == nickname)
			return false;
	}
	return true;
  }

  void do_read_body()
  {
    auto self(shared_from_this());
    boost::asio::async_read(socket_,
        boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this, self](boost::system::error_code ec, size_t /*length*/)
        {
          if (!ec)
          {
		uint64_t room_id = shared_from_this()->getCurrentChatRoomID();
		if(room_id != (uint64_t)-1)
		{
			chat_room& room_ = getRoomFromUUID(room_id);
			//cout << "Room UUID = " << room_id << endl;
			cout << "cmd = " << string(read_msg_.body(), read_msg_.body_length()) << endl;
			Command cmd(string(read_msg_.body(), read_msg_.body_length()));
		    	if(cmd.isValid()) { // Checks if the command is safe to use.
				string major_command = cmd.getMajorCommand();
				//cout << "Command = " << major_command << endl;
				if(major_command == "SENDTEXT") {
					string text = cmd.getArgument();
					string time_stamp = cmd.getTimeStamp();
					//cout << "Text = " << text << endl;
					string client_nickname = string("TEXT ["+shared_from_this()->getNickname()+"] ");
					read_msg_.body_length(text.length()+client_nickname.length());
					strcpy(read_msg_.body(), text.c_str());
					prepend(read_msg_.body(), client_nickname.c_str());
					read_msg_.encode_header();
					room_.deliver(read_msg_);
				} else if (major_command == "REQNICK") {
					string new_nickname = cmd.getArgument();
					int counter = 1;
					while(!checkIfNicknameAvaliable(new_nickname)) {
						counter++;
						new_nickname = cmd.getArgument() + to_string(counter);
					}
					shared_from_this()->setNickname(new_nickname);
					string nickstr = string("NICKNAME ") + new_nickname;
					read_msg_.body_length(nickstr.length());
					strcpy(read_msg_.body(), nickstr.c_str());
					read_msg_.encode_header();
					room_.deliver_to_participant(read_msg_, shared_from_this());
					tellAllChatUsersToUpdateTheirUserLists(room_);
				} else if (major_command == "NAMECHATROOM") {
					string room_name = cmd.getArgument();
					int counter = 1;
					while(!checkIfRoomNameIsAvaliable(room_name)) {
						counter++;
						room_name = cmd.getArgument() + to_string(counter);
					}
					addNewChatroom(room_name);
					tellAllChatUsersToUpdateTheirChatrooms();
				} else if (major_command == "JOINCHATROOM") {
					string room_name = cmd.getArgument();
					for(chat_room& room : rooms_)
					{
						if(room.getName() == room_name) {
							switch_chat_rooms(shared_from_this(), room);
							break;
						}
					}
				} else if (major_command == "REQCHATROOMS") {
					string chat_rooms_str = listChatRooms();
					if(chat_rooms_str.length() > 0){
						string pre_str = string("CHATROOMS ");
						read_msg_.body_length(chat_rooms_str.length() + pre_str.length());
						strcpy(read_msg_.body(), chat_rooms_str.c_str());
						prepend(read_msg_.body(), pre_str.c_str());
						read_msg_.encode_header();
						room_.deliver_to_participant(read_msg_, shared_from_this());
					}
				} else if (major_command == "REQUSERS") {
					tellAllChatUsersToUpdateTheirUserLists(room_);
				} else if (major_command == "DELETE"){
					string room_name = cmd.getArgument();
					deleteChatroom(room_name);
					tellAllChatUsersToUpdateTheirChatrooms();
				}
				do_read_header();
			} else {
				cerr << "Command not valid: " << cmd.getNotValidReason() << endl;
			}

		}
          }
          else
          {
            //room_.leave(shared_from_this());
	    participant_leave_room(shared_from_this());
          }
        });
  }

  void do_write()
  {
    auto self(shared_from_this());
    boost::asio::async_write(socket_,
        boost::asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this, self](boost::system::error_code ec, size_t /*length*/)
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
	    participant_leave_room(shared_from_this());
            //room_.leave(shared_from_this());
          }
        });
  }

  tcp::socket socket_;
  vector<chat_room>& rooms_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};
