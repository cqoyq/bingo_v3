/*
 * file_logger.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include "../../bingo/log/log_factory.h"
#include "../../bingo/log/log_level.h"
#include "../../bingo/singleton.h"
#include "../../bingo/string.h"
using namespace bingo;
using namespace bingo::log;

#include <boost/thread.hpp>
using namespace boost;

// Test that It is write log in multi-thread.
BOOST_AUTO_TEST_SUITE(test_log_file_logger)

typedef bingo::singleton_v0<log_factory> my_logger;

void run_thread(int& n){

	for (int i = 0; i < 10; ++i) {
		string_append ap;
		ap.add("thread n:")->add(n)->add(", counter:")->add(i);
		my_logger::instance()->handle(LOG_LEVEL_INFO, "test_local_logger", ap.to_string());
	}

}

BOOST_AUTO_TEST_CASE(t){

	my_logger::construct();

	// Make local logger.
	BOOST_CHECK_EQUAL(my_logger::instance()->make_local_logger(), true);
	this_thread::sleep(seconds(2));

	boost::thread t1(run_thread, 1);
	boost::thread t2(run_thread, 2);
	boost::thread t3(run_thread, 3);
	boost::thread t4(run_thread, 4);

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	my_logger::release();
}

class log_factory_without_postfix : public log_factory {
public:
	log_factory_without_postfix(): log_factory(){};
	virtual ~log_factory_without_postfix(){};
};

typedef bingo::singleton_v0<log_factory_without_postfix> my_logger_without_postfix;

BOOST_AUTO_TEST_CASE(t_without_postfix){
	my_logger_without_postfix::construct();

	// Make local logger.
	BOOST_CHECK_EQUAL(my_logger_without_postfix::instance()->make_local_logger("./logger_without_postfix.xml"), true);

	string info = "this is a test";
	my_logger_without_postfix::instance()->handle(LOG_LEVEL_DEBUG, "test_local_logger_without_postfix", info);
	my_logger_without_postfix::release();
}

BOOST_AUTO_TEST_SUITE_END()



