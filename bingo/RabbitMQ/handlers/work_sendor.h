/*
 * work_sendor.h
 *
 *  Created on: 2017-2-18
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_HANDLERS_WORK_SENDOR_H_
#define BINGO_RABBITMQ_HANDLERS_WORK_SENDOR_H_

#include "bingo/singleton.h"
#include "bingo/error_what.h"
#include "bingo/define.h"
#include "bingo/log/log_handler.h"
using namespace bingo::log;

#include "../config/rb_sendor_cfg.h"
#include "../rb_sendor.h"
using namespace bingo::RabbitMQ;

namespace bingo { namespace RabbitMQ { namespace handlers {

class work_sendor : public rb_sendor {
public:
	work_sendor();
	virtual ~work_sendor();

	// Override sendor::connet_to_server() method.
	void connet_to_server(log_handler*& log);

	// Override sendor::transfer_data_by_key() method.
	void transfer_data_by_key(string& key, char*& in, size_t& in_size);

	// Override sendor::transfer_data_by_routingkey() method.
	void transfer_data_by_routingkey(string& routingkey, char*& in, size_t& in_size);

protected:
	// The @msg value is 'map_key + message_body'.
	void rb_top(rb_data_message*& msg, bingo_empty_type&);

private:
	config::rb_sendor_cfg cfg_;

	// Sendor thread group.
	thread_group tg_;
};

} } }

#endif /* BINGO_RABBITMQ_HANDLERS_WORK_SENDOR_H_ */
