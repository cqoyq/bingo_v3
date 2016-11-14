/*
 * sendor.h
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_SENDOR_H_
#define BINGO_RABBITMQ_SENDOR_H_

#include "bingo/define.h"
#include "bingo/error_what.h"
#include "bingo/log/log_handler.h"
#include "bingo/thread/thread_data_type.h"
using namespace bingo;
using namespace bingo::log;

// --------------------------- rabbitmq message ------------------------- //

#pragma pack(1)
struct rb_package{
	char message[RABBITMQ_MESSAGE_MAX_SIZE];
};
#pragma pack()

typedef bingo::thread::task_message_data<rb_package> 	rb_data_message;
typedef bingo::thread::task_exit_data				   					rb_exit_message;

namespace bingo { namespace RabbitMQ {

class rb_sendor {
public:
	rb_sendor(){
		is_valid = false;
	}
	virtual ~rb_sendor(){};

	// Connect to MQ server.
	virtual void connet_to_server(log_handler*& log)=0;

	// Send data to MQ server by key.
	virtual void transfer_data_by_key(string& key, char*& data, size_t& data_size)=0;

	// Send data to MQ server by routingkey.
	virtual void transfer_data_by_routingkey(string& routingkey, char*& data, size_t& data_size)=0;

	bool is_valid;

	error_what& err(){
		return e_what_;
	}

protected:
	error_what e_what_;
};

} }

#endif /* BINGO_RABBITMQ_SENDOR_H_ */
