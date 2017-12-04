/*
chat_participant.hpp




*/

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
	chat_participant() {
		boost::uuids::uuid new_uuid = boost::uuids::random_generator()();
		//cout << hex << new_uuid << dec << endl;
    		memcpy ( &uuid, &new_uuid, sizeof (uuid) );
		nickname = boost::lexical_cast<string>(uuid);
	}
	virtual ~chat_participant() {}
	virtual void deliver(const chat_message& msg) = 0;	
	uint64_t getUUID(){
		return uuid;
	}
	void setNickname(string newNick) {
		nickname = newNick;
	}
	string getNickname() {
		return nickname;
	}
	uint64_t getCurrentChatRoomID()
	{
		return current_room;
	}
	void setCurrentChatRoom(uint64_t roomID)
	{
		current_room = roomID;
	}
private:
	uint64_t uuid = 0;
	string nickname;
	uint64_t current_room = -1;
};

typedef shared_ptr<chat_participant> chat_participant_ptr;
