/*
 * cfg.cpp
 *
 *  Created on: 2016-10-19
 *      Author: root
 */

#include "cfg.h"

#include <boost/lexical_cast.hpp>
using namespace boost;

using namespace bingo::database::config;

// ------------------------------------------- cfg_value_set ------------------------------------- //
cfg_value_set::cfg_value_set(){

}

cfg_value_set::~cfg_value_set(){

}

void cfg_value_set::push_back(cfg_value* v){
	set_.push_back(v);
}

cfg_value* cfg_value_set::get(const char* key){
	foreach_(cfg_value&n, set_){
		if(n.key.compare(key) == 0)
			return &n;
	}

	return 0;
}

// ------------------------------------------- cfg ------------------------------------- //

cfg::cfg() {
	// TODO Auto-generated constructor stub
	is_valid = false;
}

cfg::~cfg() {
	// TODO Auto-generated destructor stub
}

bool cfg::read_xml(){
	if(xml_.read("./cfg.xml") == -1){
		xml_.err().clone(err_);
		err_.err_no(ERROR_TYPE_DATABASE_READ_CFG_FAIL);
		return false;
	}else{

		// Check cfg.xml.
		is_valid = check_node();
		if(!is_valid) {
			err_.err_no(ERROR_TYPE_DATABASE_CHECK_CFG_NODE_FAIL);
		}
		return is_valid;
	}
}

bingo::error_what& cfg::err(){
	return err_;
}


bool cfg::check_node(){
	// Check cfg.xml content:
	//	<application>
	//		<database_connector>
	//				<connector key="" ip="" dbname="" user="" pwd="" port="">
	//		</database_connector>
	//	</application>

	// Check mysql_connector node.
	node* conn_set = xml_.get_node("application.database_connector") ;
	if(conn_set== 0) {
		xml_.err().clone(err_);
		return false;
	}

	// Check connector node.
	size_t max = conn_set->child.size();
	for (size_t i = 0; i < max; i++) {
		node* conn = conn_set->child[i];

		// Check attribute.
		bingo::configuration::node_attr* key_attr  = conn->attrs["key"];
		if(key_attr == 0) { err_.err_no(ERROR_TYPE_DATABASE_CHECK_CFG_NODE_FAIL); err_.err_message("connector node attribute 'key' isn't exist!"); return false;}

		bingo::configuration::node_attr* ip_attr  = conn->attrs["ip"];
		if(ip_attr == 0) { err_.err_no(ERROR_TYPE_DATABASE_CHECK_CFG_NODE_FAIL); err_.err_message("connector node attribute 'ip' isn't exist!"); return false;}

		bingo::configuration::node_attr* dbname_attr  = conn->attrs["dbname"];
		if(dbname_attr == 0) { err_.err_no(ERROR_TYPE_DATABASE_CHECK_CFG_NODE_FAIL); err_.err_message("connector node attribute 'dbname' isn't exist!"); return false;}

		bingo::configuration::node_attr* user_attr  = conn->attrs["user"];
		if(user_attr == 0) { err_.err_no(ERROR_TYPE_DATABASE_CHECK_CFG_NODE_FAIL); err_.err_message("connector node attribute 'user' isn't exist!"); return false;}

		bingo::configuration::node_attr* pwd_attr  = conn->attrs["pwd"];
		if(pwd_attr == 0) { err_.err_no(ERROR_TYPE_DATABASE_CHECK_CFG_NODE_FAIL); err_.err_message("connector node attribute 'pwd' isn't exist!"); return false;}

		bingo::configuration::node_attr* port_attr  = conn->attrs["port"];
		if(port_attr == 0) { err_.err_no(ERROR_TYPE_DATABASE_CHECK_CFG_NODE_FAIL); err_.err_message("connector node attribute 'port' isn't exist!"); return false;}

		cfg_value* v = new cfg_value();
		v->key = key_attr->value;
		v->dbname = dbname_attr->value;
		v->ip = ip_attr->value;
		v->pwd = pwd_attr->value;
		v->user = user_attr->value;
		v->port = lexical_cast<u16_t>(port_attr->value);

		value.push_back(v);
	}

	return true;
}

