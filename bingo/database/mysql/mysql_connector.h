/*
 * mysql_connector.h
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#ifndef BINGO_DATABASE_MYSQL_MYSQL_CONNECTOR_H_
#define BINGO_DATABASE_MYSQL_MYSQL_CONNECTOR_H_

#include "bingo/error_what.h"
#include "bingo/database/db_connector.h"
#include "bingo/database/db_visitor.h"

#include <mysql.h>

namespace bingo { namespace database { namespace mysql {

// Connector handle class.
class mysql_connector : public db_visitor {
public:
	mysql_connector(db_connector* conn_info);
	virtual ~mysql_connector();

	// Connect success return true. otherwise false, to check error.
	bool connect();

	// Obtain mysql*.
	MYSQL*& get_mysql();

	// Whether connect success, if success that value is true.
	// default is false.
	bool is_connect_success;


	bool query_result(const char* sql, db_result*& result);

	bool query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results);

	bool query_result(vector<string>& sqls);

protected:
	db_connector* conn_info_;
	MYSQL* mysql_conn_;


};

} } }

#endif /* BINGO_DATABASE_MYSQL_MYSQL_CONNECTOR_H_ */
