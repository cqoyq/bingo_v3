/*
 * cfg.h
 *
 *  Created on: 2016-10-19
 *      Author: root
 */

#ifndef BINGO_DATABASE_CONFIG_CFG_H_
#define BINGO_DATABASE_CONFIG_CFG_H_

#include "bingo/configuration/xml/xml_parser.h"
using namespace bingo::configuration::xml;

#include "bingo/string.h"
#include "bingo/type.h"

namespace bingo { namespace database { namespace config {

struct cfg_value {
	string ip;
	string dbname;
	string user;
	string pwd;
	u16_t port;
	string key;
};

class cfg_value_set {
public:
	cfg_value_set();
	virtual ~cfg_value_set();

	void push_back(cfg_value* v);

	// Get cfg_value pointer by key, success that return not-null pointer,
	// otherwise return 0;
	cfg_value* get(const char* key);

private:
	boost::ptr_vector<cfg_value> set_;
};

class cfg {
public:
	cfg();
	virtual ~cfg();

	// Read xml configuration file.
	// return true if success, cfg::value is configuration information. if the return is false,
	// call cfg::err() to check error.
	bool read_xml();

	// Returned error message.
	error_what& err();

	// Whether read configuration success, to success then return true;
	bool is_valid;

	// Configuration value object.
	cfg_value_set value;
private:
	error_what err_;
	xml_parser xml_;

	// Check node is valid in cfg.xml.
	bool check_node();
};

} } }

#endif /* BINGO_DATABASE_CONFIG_CFG_H_ */
