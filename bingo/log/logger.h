/*
 * logger.h
 *
 *  Created on: 2016-2-19
 *      Author: root
 */

#ifndef BINGO_LOG_LOGGER_HEADER_H_
#define BINGO_LOG_LOGGER_HEADER_H_

#include <vector>
#include <iostream>
using namespace std;

#include <boost/thread/thread.hpp>
using namespace boost;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include "log_level.h"

namespace bingo { namespace log {

// Log class.
class logger {
public:
	logger();
	virtual ~logger();

	void log_f(const char* tag, string& data);

	void log_e(const char* tag, string& data);

	void log_i(const char* tag, string& data);

	void log_d(const char* tag, string& data);



	// Current log's level, Default is LOG_LEVEL_ALL.
	log_level level();

	void level(log_level v);

public:
	virtual void log_out(string& msg){};

protected:

	mutex mu_;

	// Log Level, LOG_LEVEL_ALL is default.
	log_level cur_level_;
};


} }

#endif /* BINGO_LOG_LOGGER_HEADER_H_ */
