/*
 * log_level.h
 *
 *  Created on: 2016-11-8
 *      Author: root
 */

#ifndef BINGO_LOG_LOG_LEVEL_H_
#define BINGO_LOG_LOG_LEVEL_H_

#include "bingo/string.h"

namespace bingo { namespace log {

enum log_level {
	LOG_LEVEL_OFF 	= 0x00,
	LOG_LEVEL_FATAL 	= 0x01,
	LOG_LEVEL_ERROR 	= 0x02,
	LOG_LEVEL_INFO 	= 0x03,
	LOG_LEVEL_DEBUG 	= 0x04,
	LOG_LEVEL_ALL 	= 0x05,
	LOG_LEVEL_UNKNOWN = 0xFF,
};

// log_level -> string
string level_to_string(log_level level);

// string -> log_level
log_level string_to_level(string& s);

} }


#endif /* BINGO_LOG_LOG_LEVEL_H_ */
