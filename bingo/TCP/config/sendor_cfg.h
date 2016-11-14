/*
 * sendor_cfg.h
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#ifndef BINGO_TCP_CONFIG_SENDOR_CFG_H_
#define BINGO_TCP_CONFIG_SENDOR_CFG_H_

#include "bingo/config/xml/xml_parser.h"
using namespace bingo::config::xml;
#include "cfg_value.h"

#include <boost/ptr_container/ptr_map.hpp>

namespace bingo { namespace TCP  { namespace config {

class sendor_cfg {
public:
	sendor_cfg();
	virtual ~sendor_cfg();

	// Read xml configuration file.
	// return true if success, cfg::value is configuration information. if the return is false,
	// call cfg::error_what() to check error.
	bool read_xml();

	// Return error information.
	error_what& err();



	// Whether read configuration success, to success then return true;
	bool is_valid;

	// Get configuration information by key, return not-null pointer if success,
	// otherwise return 0.
	tcp_sendor_cfg_value* get_cfg(string key);

	// Return map size.
	int size();

	// Get item by [], if @index > map max size, then return 0;
	tcp_sendor_cfg_value* operator[](int index);

private:
	error_what err_;
	xml_parser xml_;

	// Check node is valid in cfg.xml.
	bool check_node();

	// Configuration value object.
	boost::ptr_map<string, tcp_sendor_cfg_value>  value_;

};

} } }

#endif /* BINGO_TCP_CONFIG_SENDOR_CFG_H_ */
