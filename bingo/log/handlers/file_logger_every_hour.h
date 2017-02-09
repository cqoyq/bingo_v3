/*
 * file_logger_every_day.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_LOG_HANDLERS_FILE_LOGGER_EVERY_HOUR_HEADER_H_
#define BINGO_LOG_HANDLERS_FILE_LOGGER_EVERY_HOUR_HEADER_H_

#include "file_logger_every_day.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace bingo { namespace log { namespace handlers {

class file_logger_every_hour : public file_logger_every_day {
public:
	file_logger_every_hour(const char* directory, const char* filename);
	virtual ~file_logger_every_hour();

protected:
	// Override file_logger_every_day::get_filename_postfix() method.
	void get_filename_postfix(string& filename);
};

} } }

#endif /* BINGO_LOG_HANDLERS_FILE_LOGGER_EVERY_HOUR_HEADER_H_ */
