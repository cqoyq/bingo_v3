/*
 * work_receiver.h
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_HANDLERS_ROUTING_RECEIVER_H_
#define BINGO_RABBITMQ_HANDLERS_ROUTING_RECEIVER_H_

#include "bingo/log/log_handler.h"
using namespace bingo::log;

#include "../config/rb_receiver_cfg.h"
#include "../rb_receiver.h"
using namespace bingo::RabbitMQ;

namespace bingo { namespace RabbitMQ { namespace handlers {

class routing_receiver : public rb_receiver {
public:
	routing_receiver();
	virtual ~routing_receiver();

	// Override receiver::connet_to_server() method.
	void connet_to_server(log_handler*& log);

private:
	config::rb_routing_recevier_cfg cfg_;

};

} } }

#endif /* BINGO_RABBITMQ_HANDLERS_ROUTING_RECEIVER_H_ */
