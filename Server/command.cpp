#include "command.h"
#include <iostream>
using namespace std;

Command::Command(string command_string) {
	original = command_string;
	
	int num_tokens = count(original.begin(), original.end(), ' ');
	if(num_tokens < 3) {
		is_valid = false;
		not_valid_reason = "Invalid format: Too few arguments";
	} else {
		boost::iterator_range<string::iterator> find_arg = boost::find_nth(original, " ", 2);
		if(find_arg.begin() != original.end())
			argument = string(find_arg.begin() + 1, original.end());
		string token_string = string(original.begin(), find_arg.begin());
		vector<string> tokens;

		boost::split(tokens, token_string, boost::is_any_of(" "));

		time_stamp = tokens[1];
		major_command = tokens[2];
		crc = (uint32_t)stoll(tokens[0], 0, 16);

		if(checkValidChecksum()) {
			is_valid = true;
		} else {
			is_valid = false;
			not_valid_reason = "Bad Checksum";
		}
	}
}
Command::~Command(){}

bool Command::isValid(){
	return is_valid;
}

string Command::getMajorCommand(){
	return major_command;
}

string Command::getTimeStamp(){
	return time_stamp;
}

string Command::getArgument(){
	return argument;
}

string Command::getNotValidReason(){
	if(!is_valid)
		return not_valid_reason;
	else
		return "This command is valid";
}

uint32_t Command::getChecksum(){
	return crc;
}

bool Command::checkValidChecksum(){
	string content(boost::find_nth(original, " ", 1).begin() + 1, original.end());
	uint32_t temp_crc = crc32(0L, Z_NULL, 0);
	temp_crc = crc32(temp_crc, (const unsigned char*) content.c_str(), content.length());
	return temp_crc == crc;
}



