/*
 * local_cfg.h
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#ifndef BINGO_LOG_CONFIG_LOCAL_CFG_H_
#define BINGO_LOG_CONFIG_LOCAL_CFG_H_

#include "bingo/error_what.h"
#include "bingo/string.h"
#include "bingo/config/xml/xml_parser.h"
using namespace bingo::config::xml;

#include "../log_level.h"

namespace bingo { namespace log { namespace config {

enum local_logger_type {
	LOCAL_LOGGER_TYPE_EVERY_DAY				= 0x01,
	LOCAL_LOGGER_TYPE_EVERY_HOUR			= 0x02,
	LOCAL_LOGGER_TYPE_ONE_FILE					= 0x03,
	LOCAL_LOGGER_TYPE_UNKNOWN				= 0xFF,
};

// string -> local_logger_type
local_logger_type string_to_local_logger_type(string& s);

struct local_cfg_value {
	string filename;
	string path;
	log_level level;
	local_logger_type type;
};

class local_cfg {
public:
	local_cfg();
	virtual ~local_cfg();

	// Read xml configuration file.
	// return true if success, cfg::value is configuration information. if the return is false,
	// call cfg::err() to check error.
	bool read_xml();

	// Returned error message.
	error_what& err();

	// Whether read configuration success, to success then return true;
	bool is_valid;

	// Configuration value object.
	local_cfg_value value;
private:
	error_what err_;
	xml_parser xml_;

	// Check node is valid in cfg.xml.
	bool check_node();
};

} } }

#endif /* BINGO_LOG_CONFIG_LOCAL_CFG_H_ */
