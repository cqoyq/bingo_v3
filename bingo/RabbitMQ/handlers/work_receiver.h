/*
 * work_receiver.h
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_HANDLERS_WORK_RECEIVER_H_
#define BINGO_RABBITMQ_HANDLERS_WORK_RECEIVER_H_

#include "bingo/log/log_handler.h"
using namespace bingo::log;

#include "../config/rb_receiver_cfg.h"
#include "../rb_receiver.h"
using namespace bingo::RabbitMQ;

namespace bingo { namespace RabbitMQ { namespace handlers {

class work_receiver : public rb_receiver {
public:
	work_receiver(const char* cfg_file);
	virtual ~work_receiver();

	// Override receiver::connet_to_server() method.
	void connet_to_server(log_handler*& log);

private:
	config::rb_recevier_cfg cfg_;

};

} } }

#endif /* BINGO_RABBITMQ_HANDLERS_WORK_RECEIVER_H_ */
