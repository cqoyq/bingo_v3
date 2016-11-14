/*
 * rbmqlogvisitor.cpp
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#include "rbmq_log_visitor.h"

#include "bingo/config/node.h"
using namespace bingo::config;

using namespace bingo::log::handlers;

rbmq_log_visitor::rbmq_log_visitor(){
	factory_ = 0;
}

rbmq_log_visitor::~rbmq_log_visitor(){
}

bool rbmq_log_visitor::read_config(rabbitmq_factory*& factory){
	if(config_.read_xml()){
		factory_ = factory;
		return true;
	}else{
		config_.err().clone(e_what_);
		return false;
	}
}

void rbmq_log_visitor::handle(int level, const char* tag, std::string& info){
	if(config_.is_valid && level <= (int)(config_.value.level) && factory_){

		string s_data = tag;
		s_data.append("$");
		s_data.append(level_to_string((log_level)level).c_str());
		s_data.append("$");
		s_data.append(info.c_str());
		char* p = const_cast<char*>( s_data.c_str());
		factory_->transfer_data_by_key(config_.value.key, p, s_data.length());
	}
}

log::config::rbmq_cfg& rbmq_log_visitor::config(){
	return config_;
}

error_what& rbmq_log_visitor::err(){
	return e_what_;
}
