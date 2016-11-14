/*
 * file_logger_every_day.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_LOG_HANDLERS_FILE_LOGGER_EVERY_DAY_HEADER_H_
#define BINGO_LOG_HANDLERS_FILE_LOGGER_EVERY_DAY_HEADER_H_

#include "../logger.h"

#include <sys/stat.h>
#include <fcntl.h>

#include <fstream>
using namespace std;

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace bingo { namespace log { namespace handlers {

class file_logger_every_day : public logger {
public:
	file_logger_every_day(const char* directory, const char* filename);
	virtual ~file_logger_every_day();

	// Override logger::log_out() method.
	void log_out(string& msg);

	// Get log file's postfix.
	virtual void get_filename_postfix(string& filename);

private:
	string d_;
	string f_;
};

} } }

#endif /* BINGO_LOG_HANDLERS_FILE_LOGGER_EVERY_DAY_HEADER_H_ */
