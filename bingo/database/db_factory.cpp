/*
 * db_factory.cpp
 *
 *  Created on: 2016-10-19
 *      Author: root
 */

#include "db_factory.h"

using namespace bingo::database;

db_factory::db_factory() {
	// TODO Auto-generated constructor stub
	visitor_ = 0;
}

db_factory::~db_factory() {
	// TODO Auto-generated destructor stub
	if(visitor_) delete visitor_;
}

bool db_factory::make_mysql_visitor(const char* cfg_key){
	// Read cfg.xml
	if(!cfg_.read_xml()){
		cfg_.err().clone(e_what_);
		return false;
	}

	// Get cfg.
	config::cfg_value* cfg = cfg_.value.get(cfg_key) ;
	if(cfg == 0){
		e_what_.err_no(ERROR_TYPE_DATABASE_CFG_NODE_NO_EXIST);
		e_what_.err_message("get configuration fail! key isn't exist.");
		return false;
	}

	// Make mysql visitor.
	conn_info_.dbname(cfg->dbname.c_str());
	conn_info_.ip(cfg->ip.c_str());
	conn_info_.port(cfg->port);
	conn_info_.user(cfg->user.c_str());
	conn_info_.pwd(cfg->pwd.c_str());
	db_connector* p = &conn_info_;

	visitor_ = new mysql_visitor(p);

	return true;
}

bool db_factory::make_mysql_pool(int num, const char* cfg_key){
	// Read cfg.xml
	if(!cfg_.read_xml()){
		cfg_.err().clone(e_what_);
		return false;
	}

	// Get cfg.
	config::cfg_value* cfg = cfg_.value.get(cfg_key) ;
	if(cfg == 0){
		e_what_.err_no(ERROR_TYPE_DATABASE_CFG_NODE_NO_EXIST);
		e_what_.err_message("get configuration fail! key isn't exist.");
		return false;
	}

	// Make mysql pool.
	conn_info_.dbname(cfg->dbname.c_str());
	conn_info_.ip(cfg->ip.c_str());
	conn_info_.port(cfg->port);
	conn_info_.user(cfg->user.c_str());
	conn_info_.pwd(cfg->pwd.c_str());
	db_connector* p = &conn_info_;

	mysql_pool* mysql_pool_ = new mysql_pool();
	visitor_ = mysql_pool_;
	mysql_pool_->make_connector_pool(num, p);

	return true;
}

bool db_factory::query_result(const char* sql, db_result*& result){

	if(visitor_ == 0){
		e_what_.err_no(ERROR_TYPE_DATABASE_FACTORY_NO_INSTANCE);
		e_what_.err_message("db factory isn't create install!");
		return false;
	}

	// Mysql pool
	mysql_pool* mysql_pool_ = dynamic_cast<mysql_pool*>(visitor_);
	if(mysql_pool_){
		mysql_connector* conn = mysql_pool_->get_connector();
		if(conn){
			bool is_succ = conn->query_result(sql, result);
			if(!is_succ){
				conn->err().clone(e_what_);
			}
			mysql_pool_->free_connector(conn);
			return is_succ;
		}else{
			e_what_.err_no(ERROR_TYPE_DATABASE_POOL_IS_EMPTY);
			e_what_.err_message("mysql pool is empty!");
			return false;

		}
	}

	// Mysql visitor
	mysql_visitor* mysql_visitor_ = dynamic_cast<mysql_visitor*>(visitor_);
	if(mysql_visitor_){
		bool is_succ = mysql_visitor_->query_result(sql, result);
		if(!is_succ){
			mysql_visitor_->err().clone(e_what_);
		}
		return is_succ;
	}

	e_what_.err_no(ERROR_TYPE_DATABASE_FACTORY_NO_INSTANCE);
	e_what_.err_message("db factory hasn't instance!");
	return false;
}

bool db_factory::query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results){
	if(visitor_ == 0){
		e_what_.err_no(ERROR_TYPE_DATABASE_FACTORY_NO_INSTANCE);
		e_what_.err_message("db factory isn't create install!");
		return false;
	}

	// Mysql pool
	mysql_pool* mysql_pool_ = dynamic_cast<mysql_pool*>(visitor_);
	if(mysql_pool_){
		mysql_connector* conn = mysql_pool_->get_connector();
		if(conn){
			bool is_succ = conn->query_result(sqls, results);
			if(!is_succ){
				conn->err().clone(e_what_);
			}
			mysql_pool_->free_connector(conn);
			return is_succ;
		}else{
			e_what_.err_no(ERROR_TYPE_DATABASE_POOL_IS_EMPTY);
			e_what_.err_message("mysql pool is empty!");
			return false;

		}
	}

	// Mysql visitor
	mysql_visitor* mysql_visitor_ = dynamic_cast<mysql_visitor*>(visitor_);
	if(mysql_visitor_){
		bool is_succ = mysql_visitor_->query_result(sqls, results);
		if(!is_succ){
			mysql_visitor_->err().clone(e_what_);
		}
		return is_succ;
	}

	e_what_.err_no(ERROR_TYPE_DATABASE_FACTORY_NO_INSTANCE);
	e_what_.err_message("db factory hasn't instance!");
	return false;
}

bool db_factory::query_result(vector<string>& sqls){
	if(visitor_ == 0){
		e_what_.err_no(ERROR_TYPE_DATABASE_FACTORY_NO_INSTANCE);
		e_what_.err_message("db factory isn't create install!");
		return false;
	}

	// Mysql pool
	mysql_pool* mysql_pool_ = dynamic_cast<mysql_pool*>(visitor_);
	if(mysql_pool_){
		mysql_connector* conn = mysql_pool_->get_connector();
		if(conn){
			bool is_succ = conn->query_result(sqls);
			if(!is_succ){
				conn->err().clone(e_what_);
			}
			mysql_pool_->free_connector(conn);
			return is_succ;
		}else{
			e_what_.err_no(ERROR_TYPE_DATABASE_POOL_IS_EMPTY);
			e_what_.err_message("mysql pool is empty!");
			return false;

		}
	}

	// Mysql visitor
	mysql_visitor* mysql_visitor_ = dynamic_cast<mysql_visitor*>(visitor_);
	if(mysql_visitor_){
		bool is_succ = mysql_visitor_->query_result(sqls);
		if(!is_succ){
			mysql_visitor_->err().clone(e_what_);
		}
		return is_succ;
	}

	e_what_.err_no(ERROR_TYPE_DATABASE_FACTORY_NO_INSTANCE);
	e_what_.err_message("db factory hasn't instance!");
	return false;
}
