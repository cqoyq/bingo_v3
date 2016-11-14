/*
 * db_factory.h
 *
 *  Created on: 2016-10-19
 *      Author: root
 */

#ifndef BINGO_DATABASE_DB_FACTORY_H_
#define BINGO_DATABASE_DB_FACTORY_H_

#include <bingo/database/mysql/all.h>
using namespace bingo::database;
using namespace bingo::database::mysql;

#include "config/cfg.h"

namespace bingo { namespace database {

class db_factory : public bingo::database::db_visitor {
public:
	db_factory();
	virtual ~db_factory();

	// Create mysql pool. success and return true. fail to call err() get error.
	// the make_mysql_pool() method has called without main thread.
	// @num is number of connector.
	bool make_mysql_pool(int num, const char* cfg_key);

	// Create mysql visitor. success and return true. fail to call err() get error.
	// @num is number of connector.
	bool make_mysql_visitor(const char* cfg_key);




	bool query_result(const char* sql, db_result*& result);

	bool query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results);

	bool query_result(vector<string>& sqls);

private:

	config::cfg cfg_;
	db_connector conn_info_;

	db_visitor* visitor_;

};

} }

#endif /* BINGO_DATABASE_DB_FACTORY_H_ */
