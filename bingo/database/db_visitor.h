/*
 * db_visitor.h
 *
 *  Created on: 2016-9-21
 *      Author: root
 */

#ifndef BINGO_DATABASE_DB_VISITOR_H_
#define BINGO_DATABASE_DB_VISITOR_H_

#include <boost/ptr_container/ptr_vector.hpp>

#include "db_result.h"
#include "bingo/error_what.h"

namespace bingo { namespace database {

class db_visitor {
public:
	virtual ~db_visitor(){};

	// Query single result, fail and return false, to check err().
	// @sql is single sql-string, @result is query result, @e_what is error.
	virtual bool query_result(const char* sql, db_result*& result) = 0;

	// Query multi-result,  fail and return false, to check err().
	virtual bool query_result(vector<string>& sqls, boost::ptr_vector<db_result>& results) = 0;

	// Execute multi-sql-string,  fail and return false, to check err().
	virtual bool query_result(vector<string>& sqls) = 0;

	// Get error information.
	error_what& err(){
		return e_what_;
	}

protected:
	error_what e_what_;
};

} }


#endif /* BINGO_DATABASE_DB_VISITOR_H_ */
