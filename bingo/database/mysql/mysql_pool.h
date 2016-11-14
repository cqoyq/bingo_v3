/*
 * mysql_pool.h
 *
 *  Created on: 2016-10-18
 *      Author: root
 */

#ifndef BINGO_DATABASE_MYSQL_MYSQL_POOL_H_
#define BINGO_DATABASE_MYSQL_MYSQL_POOL_H_

#include "bingo/error_what.h"
#include "bingo/database/db_connector.h"
#include "bingo/database/db_visitor.h"
#include "mysql_connector.h"

#include <queue>
using namespace std;

#include <boost/thread.hpp>
#include <boost/asio.hpp>
using namespace boost;
using namespace boost::asio;

namespace bingo { namespace database { namespace mysql {

class mysql_pool : public db_visitor {
public:
	typedef bingo::database::mysql::mysql_connector connector;
	mysql_pool();
	virtual ~mysql_pool();

	// Make pool success to return 0, otherwise return -1, fail to check err().
	int make_connector_pool(size_t n, db_connector* conn_info);

	// Pop connector from pool.
	connector* get_connector();

	//  Push connector to pool.
	void free_connector(connector*& conn);

	// Obtain pool size.
	size_t size();

	// Check heartbeat.
	void check_heartbeat();

protected:
	bool query_result(const char* sql, db_result*& result){
		return true;
	}

	bool query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results) {
		return true;
	}

	bool query_result(vector<string>& sqls){
		return true;
	}

protected:
	mutex mu_;

	queue<connector*> conns_;
	io_service ios_;

};

} } }

#endif /* BINGO_DATABASE_MYSQL_MYSQL_POOL_H_ */
