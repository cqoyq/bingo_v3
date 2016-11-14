/*
 * receivercfg.cpp
 *
 *  Created on: 2016-9-27
 *      Author: root
 */

#include "receiver_cfg.h"

#include "bingo/error_what.h"

#include <boost/lexical_cast.hpp>

using namespace bingo::TCP::config;

tcp_receiver_cfg::tcp_receiver_cfg() {
	// TODO Auto-generated constructor stub
	is_valid = false;
}

tcp_receiver_cfg::~tcp_receiver_cfg() {
	// TODO Auto-generated destructor stub
}

bool tcp_receiver_cfg::read_xml(){
	if(!xml_.read("./cfg.xml")){
		xml_.err().clone(err_);
		err_.err_no(ERROR_TYPE_TCP_READ_CFG_FAIL);
		return false;
	}else{

		// Check single_sendor_cfg.xml.
		is_valid = check_node();
		if(!is_valid) err_.err_no(ERROR_TYPE_TCP_CHECK_CFG_NODE_FAIL);
		return is_valid;
	}
}

bingo::error_what& tcp_receiver_cfg::err(){
	return err_;
}

bool tcp_receiver_cfg::check_node(){

	// Check cfg.xml content:
	//	<application>
	//		<tcp_receiver>
	//				<ip></ip>
	//				<port></port>
	//		</tcp_receiver>
	//	</application>

	bingo::config::node* ip = xml_.get_node("application.tcp_receiver.ip");
	if(ip == 0) {
		xml_.err().clone(err_);
		return false;
	}
	value_.ip = ip->value;

	bingo::config::node* port = xml_.get_node("application.tcp_receiver.port");
	if(port == 0) {
		xml_.err().clone(err_);
		return false;
	}
	value_.port = lexical_cast<u16_t>(port->value);

	return true;
}

tcp_recevier_cfg_value& tcp_receiver_cfg::get_cfg(){
	return value_;
}

