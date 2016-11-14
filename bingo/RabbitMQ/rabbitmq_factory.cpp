/*
 * rabbitmqfactory.cpp
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#include "handlers/single_sendor.h"
#include "handlers/single_receiver.h"
using namespace bingo::RabbitMQ::handlers;
#include "bingo/error_what.h"

#include "rabbitmq_factory.h"

using namespace bingo::RabbitMQ;

rabbitmq_factory::rabbitmq_factory() :
		snd_(0),
		rev_(0),
		logger_(0){
	// TODO Auto-generated constructor stub
}

rabbitmq_factory::~rabbitmq_factory() {
	// TODO Auto-generated destructor stub
	if(snd_) delete snd_;
	if(rev_) delete rev_;
}


bool rabbitmq_factory::make_p2p_sendor(log_handler*& log){
	logger_ = log;

	single_sendor* p=  new single_sendor();
	snd_ = p;
	if(!snd_->is_valid){
		e_what_.err_no(ERROR_TYPE_RABBITMQ_MAKE_SINGLE_SENDOR_FAIL);
		e_what_.err_message(p->err().err_message().c_str());

		if(logger_){

			string s_msg = "single sendor fail!, error:" + e_what_.err_message();
			logger_->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
		}
	}
	else{
		// Connect to MQ server.
		snd_->connet_to_server(logger_);
	}
	return snd_->is_valid;
}

void rabbitmq_factory::transfer_data_by_key(string& key, char* data, size_t data_size){
	if(snd_)
		snd_->transfer_data_by_key(key, data, data_size);
}

void rabbitmq_factory::transfer_data_by_routingkey(string& routingkey, char* data, size_t data_size){
	if(snd_)
		snd_->transfer_data_by_routingkey(routingkey, data, data_size);
}

error_what& rabbitmq_factory::err(){
	return e_what_;
}

bool rabbitmq_factory::make_p2p_receiver(log_handler*& log, rb_receiver::rev_callback f){
	logger_ = log;

	single_receiver* p = new single_receiver();
	rev_ =p;
	if(!rev_->is_valid){
		e_what_.err_no(ERROR_TYPE_RABBITMQ_MAKE_SINGLE_RECEIVER_FAIL);
		e_what_.err_message(p->err().err_message().c_str());

		if(logger_){

			string s_msg = "make single receiver fail!, error:" + e_what_.err_message();
			logger_->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_RECEIVER_ERROR,  s_msg);
		}
	}
	else{
		// Set callback function.
		rev_->callback(f);

		// Connect to MQ server.
		rev_->connet_to_server(logger_);
	}
	return rev_->is_valid;
}


