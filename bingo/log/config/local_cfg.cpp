/*
 * locallocal_cfg.cpp
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#include "local_cfg.h"

using namespace bingo::log::config;

local_logger_type bingo::log::config::string_to_local_logger_type(string& s){
	if(s.compare("every_day") == 0){
		return LOCAL_LOGGER_TYPE_EVERY_DAY;
	}else if(s.compare("every_hour") == 0){
		return LOCAL_LOGGER_TYPE_EVERY_HOUR;
	}else if(s.compare("one_file") == 0){
		return LOCAL_LOGGER_TYPE_ONE_FILE;
	}else
		return LOCAL_LOGGER_TYPE_UNKNOWN;
}

local_cfg::local_cfg() {
	// TODO Auto-generated constructor stub
	is_valid = false;
}

local_cfg::~local_cfg() {
	// TODO Auto-generated destructor stub
}

bool local_cfg::read_xml(){
	if(!xml_.read("./cfg.xml")){
		xml_.err().clone(err_);
		err_.err_no(ERROR_TYPE_LOG_READ_CFG_FAIL);
		return false;
	}else{

		// Check local_cfg.xml.
		is_valid = check_node();
		if(!is_valid)
			err_.err_no(ERROR_TYPE_LOG_CHECK_CFG_NODE_FAIL);
		return is_valid;
	}
}

bingo::error_what& local_cfg::err(){
	return err_;
}


bool local_cfg::check_node(){
	// Check local_cfg.xml content:
	//	<application>
	//		<local_log_visitor>
	//				<type>every_day</type>	// 日志类型（every_day, every_hour, one_file）
	//				<filename></filename>		// 日志名称
	//				<path></path>					// 日志所在路径
	//				<level>all</level>					// 日志层级，大于配置层级的日志，才会写入日志
	//		</ local_log_visitor >
	//	</application>

	string type;
	if(!xml_.get_value("application.local_log_visitor.type", type)){
		xml_.err().clone(err_);
		return false;
	}

	// Check whether type value is valid.
	local_logger_type ntype= string_to_local_logger_type(type);
	if(ntype == LOCAL_LOGGER_TYPE_UNKNOWN){
		err_.err_message("the type value is invalid in cfg.xml!");
		return false;
	}
	value.type = ntype;

	if(!xml_.get_value("application.local_log_visitor.filename", value.filename)) {
		xml_.err().clone(err_);
		return false;
	}

	if(!xml_.get_value("application.local_log_visitor.path", value.path)) {
		xml_.err().clone(err_);
		return false;
	}

	string slevel;
	if(!xml_.get_value("application.local_log_visitor.level", slevel)) {
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

	return true;
}
