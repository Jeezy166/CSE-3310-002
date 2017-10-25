#pragma once
#include "chat_participant.hpp"
using namespace std;

class chat_room
{
public:
  void join(chat_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
    participant->chat_id = num_connections;
    num_connections++;
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
      participant->deliver(msg);
  }

private:
  set<chat_participant_ptr> participants_;
  int num_connections = 0;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};
