/*
command.h

The 'Command' object will take in a string with the format "<Checksum> <Time> <Major_Command> <Optional Arguments>",
and checks if that command is valid (has enough parameters, has a good checksum, etc).

Created by David Benepe, Last updated on 10/24/2017

Example usage:
----------------------------------------------------------------------------------------------------

Command cmd("a71630f9 09:30.1234 SENDTEXT Hello World!"); // Command object takes in a string

if(cmd.isValid()) { // Checks if the command is safe to use.
	string major_command = cmd.getMajorCommand();
	if(major_command == "SENDTEXT") {
		string text = cmd.getArgument();
		string time_stamp = cmd.getTimeStamp();
		uint32_t checksum = cmd.getChecksum(); // Not necessary, but it's there if needed.

		// ... do stuff here
	}
} else {
	cerr << "Command not valid: " << cmd.getNotValidReason() << endl;
}

----------------------------------------------------------------------------------------------------
*/

#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <zlib.h>
#include "boost_libs.h"
using namespace std;

class Command {
public:
	Command(string command_string);
	~Command();
	bool isValid();
	uint32_t getChecksum();
	string getMajorCommand();
	string getTimeStamp();
	string getArgument();
	string getNotValidReason();

private:
	bool checkValidChecksum();
	
	string original;
	uint32_t crc; // checksum
	string time_stamp;
	string major_command;
	string argument;
	bool is_valid;
	string not_valid_reason;
};
