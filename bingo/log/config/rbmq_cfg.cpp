/*
 * rbmqcfg.cpp
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#include "rbmq_cfg.h"

using namespace bingo::log::config;

rbmq_cfg::rbmq_cfg() {
	// TODO Auto-generated constructor stub
	is_valid = false;
}

rbmq_cfg::~rbmq_cfg() {
	// TODO Auto-generated destructor stub
}

bool rbmq_cfg::read_xml(){
	if(!xml_.read("./cfg.xml")){
		xml_.err().clone(err_);
		err_.err_no(ERROR_TYPE_LOG_READ_CFG_FAIL);
		return false;
	}else{

		// Check cfg.xml.
		is_valid = check_node();
		if(!is_valid)
			err_.err_no(ERROR_TYPE_LOG_CHECK_CFG_NODE_FAIL);
		return is_valid;
	}
}

bingo::error_what& rbmq_cfg::err(){
	return err_;
}


bool rbmq_cfg::check_node(){
	// Check cfg.xml content:
	//	<application>
	//		<rbmq_log_visitor>
	//				<rabbitmq_key>key</rabbitmq_key>						// the key in rabbitmq-set
	//				<log_level>4</log_level>											// rabbitmq log level
	//		</rbmq_log_visitor>
	//	</application>

	string slevel;
	if(!xml_.get_value("application.rbmq_log_visitor.log_level", slevel)) {
		xml_.err().clone(err_);
		return false;
	}

	// Check whether s_level value is valid.
	log_level level = string_to_level(slevel);
	if(level == LOG_LEVEL_UNKNOWN ){
		err_.err_message("the level value is invalid in cfg.xml!");
		return false;
	}
	value.level = level;

	if(!xml_.get_value("application.rbmq_log_visitor.rabbitmq_key", value.key)) {
		xml_.err().clone(err_);
		return false;
	}

	return true;
}

