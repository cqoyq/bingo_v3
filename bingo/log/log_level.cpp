/*
 * log_level.cpp
 *
 *  Created on: 2016-11-8
 *      Author: root
 */

#include "log_level.h"
using namespace bingo::log;

string bingo::log::level_to_string(log_level level){
	string out;
	switch(level){
	case LOG_LEVEL_OFF:
			out.append("off");
			break;
	case LOG_LEVEL_FATAL:
			out.append("fatal");
			break;
	case LOG_LEVEL_ERROR:
			out.append("error");
			break;
	case LOG_LEVEL_INFO:
			out.append("info");
			break;
	case LOG_LEVEL_DEBUG:
			out.append("debug");
			break;
	case LOG_LEVEL_ALL:
			out.append("all");
			break;
	default:
		out.append("unknow");
	}

	return out;
}

log_level bingo::log::string_to_level(string& s){
	if(s.compare("off") == 0){
		return LOG_LEVEL_OFF;

	}else if(s.compare("fatal") == 0){
		return LOG_LEVEL_FATAL;

	}else if(s.compare("error") == 0){
		return LOG_LEVEL_ERROR;

	}else if(s.compare("info") == 0){
		return LOG_LEVEL_INFO;

	}else if(s.compare("debug") == 0){
		return LOG_LEVEL_DEBUG;

	}else if(s.compare("all") == 0){
		return LOG_LEVEL_ALL;

	}else
		return LOG_LEVEL_UNKNOWN;
}
