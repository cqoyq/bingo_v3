/*
 * sendorcfg.cpp
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#include "sendor_cfg.h"

#include "bingo/error_what.h"

#include <boost/lexical_cast.hpp>

using namespace bingo::TCP::config;

sendor_cfg::sendor_cfg() {
	// TODO Auto-generated constructor stub
	is_valid = false;
}

sendor_cfg::~sendor_cfg() {
	// TODO Auto-generated destructor stub
}

bool sendor_cfg::read_xml(){
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

bingo::error_what& sendor_cfg::err(){
	return err_;
}

bool sendor_cfg::check_node(){

	// Check cfg.xml content:
	//	<application>
	//		<tcp_sendor>
	//				<list>
	//					<item key="" ip="127.0.0.1" port="5672"/>
	//					<item key="" ip="127.0.0.1" port="5672"/>
	//				</list>
	//		</tcp_sendor>
	//	</application>

	bingo::config::node* list = xml_.get_node("application.tcp_sendor.list");
	if(list == 0) {
		xml_.err().clone(err_);
		return false;
	}

	int max =  list->child.size();
	for (int i = 0; i <max; i++) {
		bingo::config::node* item = list->child[i];

		string s_key, s_ip, s_port;

		// key
		if(item->attrs["key"] == 0) {
			err_.err_message("key's attribute is empty!");
			return false;
		}else
			s_key = item->attrs["key"]->value;

		// ip
		if(item->attrs["ip"] == 0) {
			err_.err_message("ip's attribute is empty!");
			return false;
		}else
			s_ip = item->attrs["ip"]->value;

		// port
		if(item->attrs["port"] == 0) {
			err_.err_message("port's attribute is empty!");
			return false;
		}else
			s_port = item->attrs["port"]->value;
		uint16_t n_port = lexical_cast<uint16_t>(s_port);

		tcp_sendor_cfg_value* v = new tcp_sendor_cfg_value();
		v->ip     	= s_ip.c_str();
		v->port		= n_port;
		v->map_key = s_key.c_str();


		this->value_.insert(s_key, v);
	}
	return true;
}

tcp_sendor_cfg_value* sendor_cfg::get_cfg(string key){
	if(value_.find(key) == value_.end())
		return 0;
	else
		return &(value_[key]);
}

int sendor_cfg::size(){
	return value_.size();
}

tcp_sendor_cfg_value* sendor_cfg::operator[](int index){
	typedef boost::ptr_map<string, tcp_sendor_cfg_value>::value_type ptr_mp_value;

	int max = (value_.size() > 0) ? value_.size() - 1 : 0;
	if(index > max)
		return 0;
	else{
		tcp_sendor_cfg_value* p = 0;
		int n = 0;
		foreach_(ptr_mp_value v, value_){
			if(n == index){
				p = v.second;
				break;
			}
			n++;
		}
		return p;
	}
}

