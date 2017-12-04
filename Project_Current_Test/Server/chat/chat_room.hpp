/*
chat_room.hpp



*/

#pragma once
#include "chat_participant.hpp"
using namespace std;

class chat_room
{
public:
  chat_room(string name)
  {
	room_name = name;

	// chat room uuid -1 means undefined, and uuid 0 is reserved for lobby.
	while(uuid == (uint64_t)-1 || uuid == 0)
	{
		boost::uuids::uuid new_uuid = boost::uuids::random_generator()();
		memcpy ( &uuid, &new_uuid, sizeof (uuid) );
	}
  }

  chat_room(string name, uint64_t custom_uuid)
  {
	room_name = name;
	uuid = custom_uuid;
  }

  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(chat_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const chat_message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      if(participant != NULL)
        participant->deliver(msg);
  }

  void deliver_to_participant(const chat_message& msg, chat_participant_ptr participant)
  {
	participant->deliver(msg);
  }

  uint64_t getUUID()
  {
	return uuid;
  }

  string getName()
  {
	return room_name;
  }

  bool isNicknameTaken(string nick) 
  {
	for(chat_participant_ptr user : participants_) 
	{
		if(user->getNickname() == nick)
			return true;
	}
	return false;
  }

  string getListOfNicknames()
  {
	string ret = "";
	set<string> nicknames;
	for(chat_participant_ptr user : participants_) 
		nicknames.insert(user->getNickname());
	for(string nick : nicknames)
		ret += nick + ",";
	return ret.substr(0, ret.length() - 1);
  }

private:
  set<chat_participant_ptr> participants_;
  string room_name = "Untitled";
  uint64_t uuid = -1;
  //int num_connections = 0;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};
