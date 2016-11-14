/*
 * receiver.h
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_RECEIVER_H_
#define BINGO_RABBITMQ_RECEIVER_H_

#include "bingo/define.h"
#include "bingo/error_what.h"
#include "bingo/log/log_handler.h"
using namespace bingo;
using namespace bingo::log;

namespace bingo { namespace RabbitMQ {

class rb_receiver {
public:
	// Call this function after receive message from server.
	typedef boost::function<void(std::string&)> 		rev_callback;

	rb_receiver(){
		f_ = 0;
		is_valid = false;
	}
	virtual ~rb_receiver(){}

	// Connect to MQ server.
	virtual void connet_to_server(log_handler*& log)=0;

	// Set rev_callback value.
	void callback(rev_callback& callback){
		f_ = callback;
	}

	bool is_valid;

	error_what& err(){
		return e_what_;
	}

protected:
	// Receive message callback function.
	rev_callback f_;

	error_what e_what_;
};

} }

#endif /* BINGO_RABBITMQ_RECEIVER_H_ */
