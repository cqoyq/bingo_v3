/*
 * logfactory.cpp
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#include "log_factory.h"

#include "handlers/local_log_visitor.h"
#include "handlers/rbmq_log_visitor.h"
using namespace bingo::log::handlers;

using namespace bingo::log;

log_factory::log_factory() {
	// TODO Auto-generated constructor stub
	hdr_ = 0;
}

log_factory::~log_factory() {
	// TODO Auto-generated destructor stub
	if(hdr_) delete hdr_;
}

bool log_factory::make_local_logger(){
	local_log_visitor* p = new local_log_visitor();
	hdr_ = p;

	if(!p->read_config()){
		p->err().clone(e_what_);
		return false;
	}else{
		return true;
	}
}

bool log_factory::make_rabbitmq_logger(rabbitmq_factory* factory){
	rbmq_log_visitor* p = new rbmq_log_visitor();

	hdr_ = p;

	if(!p->read_config(factory)){
		p->err().clone(e_what_);
		return false;
	}else{
		return true;
	}
}

bingo::error_what& log_factory::err(){
	return e_what_;
}

void log_factory::handle(int level, const char* tag, std::string& info){
	hdr_->handle(level, tag, info);
}

