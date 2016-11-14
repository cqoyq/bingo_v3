/*
 * log_factory.h
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#ifndef BINGO_LOG_LOG_FACTORY_H_
#define BINGO_LOG_LOG_FACTORY_H_

#include "log_handler.h"

#include "bingo/RabbitMQ/rabbitmq_factory.h"
using namespace bingo::RabbitMQ;

namespace bingo { namespace log {

class log_factory : public log_handler {
public:
	log_factory();
	virtual ~log_factory();

	// Create local logger, return true if success, fail to call err() to check error.
	bool make_local_logger();

	// Create rabbitmq logger, return true if success, fail to call err() to check error.
	bool make_rabbitmq_logger(rabbitmq_factory* factory);

	// Create fail, to show error information.
	error_what& err();

	// Override log_handler::handle() method.
	void handle(int level, const char* tag, std::string& info);

private:
	log_handler* hdr_;

	error_what e_what_;

};

} }

#endif /* BINGO_LOG_LOG_FACTORY_H_ */
