#pragma once
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "../boost_libs.h"
#include "chat_message.hpp"
using namespace std;

typedef deque<chat_message> chat_message_queue;

class chat_participant
{
public:
  virtual ~chat_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
  int chat_id = -1;
};

typedef shared_ptr<chat_participant> chat_participant_ptr;
