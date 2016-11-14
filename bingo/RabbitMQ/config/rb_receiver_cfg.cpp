/*
 * single_receiver_cfg.cpp
 *
 *  Created on: 2016-8-30
 *      Author: root
 */

#include "rb_receiver_cfg.h"

#include "bingo/error_what.h"

using namespace bingo::RabbitMQ::config;

rb_recevier_cfg::rb_recevier_cfg() {
	// TODO Auto-generated constructor stub
	is_valid = false;
}

rb_recevier_cfg::~rb_recevier_cfg() {
	// TODO Auto-generated destructor stub
}

bool rb_recevier_cfg::read_xml(){
	if(!xml_.read("./cfg.xml")){
		xml_.err().clone(err_);
		err_.err_no(ERROR_TYPE_RABBITMQ_RECEIVER_READ_CFG_FAIL);
		return false;
	}else{

		// Check rb_recevier_cfg.xml.
		is_valid = check_node();
		if(!is_valid) err_.err_no(ERROR_TYPE_RABBITMQ_RECEIVER_CHECK_NODE_FAIL);
		return is_valid;
	}
}

bingo::error_what& rb_recevier_cfg::err(){
	return err_;
}


bool rb_recevier_cfg::check_node(){

	// Check cfg.xml content:
	//	<application>
	//		<rabbitmq_receiver>
	//				<type>1</type>								// receiver type
	//				<ip>127.0.0.1</ip>						// rabbitmq's ip
	//				<port>5672</port>							// rabbitmq's port
	//				<username>guest</username>		// rabbitmq's login username
	//				<pwd>guest</pwd>						// rabbitmq's login password
	//				<host>/</host>								// rabbitmq's host
	//				<exchange>hello</exchange>			// rabbitmq's exchange
	//				<routingkey>hello</routingkey>		// rabbitmq's routingkey
	//		</rabbitmq_receiver>
	//	</application>
	string s_type;
	if(!xml_.get_value("application.rabbitmq_receiver.type", s_type)){
		xml_.err().clone(err_);
		return false;
	}
	value.type = (rb_cfg_type)(lexical_cast<int>(s_type));

	if(!xml_.get_value("application.rabbitmq_receiver.ip", value.ip)){
		xml_.err().clone(err_);
		return false;
	}

	string s_port;
	if(!xml_.get_value("application.rabbitmq_receiver.port", s_port)) {
		xml_.err().clone(err_);
		return false;
	}
	value.port = lexical_cast<uint16_t>(s_port);

	if(!xml_.get_value("application.rabbitmq_receiver.username", value.username)) {
		xml_.err().clone(err_);
		return false;
	}

	if(!xml_.get_value("application.rabbitmq_receiver.pwd", value.pwd)) {
		xml_.err().clone(err_);
		return false;
	}

	if(!xml_.get_value("application.rabbitmq_receiver.host", value.host)) {
		xml_.err().clone(err_);
		return false;
	}

	if(!xml_.get_value("application.rabbitmq_receiver.exchange", value.exchange)) {
		xml_.err().clone(err_);
		return false;
	}

	if(!xml_.get_value("application.rabbitmq_receiver.routingkey", value.routingkey)) {
		xml_.err().clone(err_);
		return false;
	}

	return true;
}
