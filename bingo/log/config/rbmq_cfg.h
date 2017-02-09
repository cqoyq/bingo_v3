/*
 * rbmq_cfg.h
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#ifndef BINGO_LOG_CONFIG_RBMQ_CFG_H_
#define BINGO_LOG_CONFIG_RBMQ_CFG_H_

#include "bingo/error_what.h"
#include "bingo/string.h"
#include "bingo/configuration/xml/xml_parser.h"
using namespace bingo::configuration::xml;

#include "../log_level.h"

namespace bingo { namespace log { namespace config {

struct rbmq_cfg_value {
	string key;
	log_level level;
};

class rbmq_cfg {
public:
	rbmq_cfg();
	virtual ~rbmq_cfg();

	// Read xml configuration file.
	// return true if success, cfg::value is configuration information. if the return is false,
	// call cfg::err() to check error.
	bool read_xml();

	// Returned error message.
	error_what& err();

	// Whether read configuration success, to success then return true;
	bool is_valid;

	// Configuration value object.
	rbmq_cfg_value value;
private:
	error_what err_;
	xml_parser xml_;

	// Check node is valid in cfg.xml.
	bool check_node();
};

} } }

#endif /* BINGO_LOG_CONFIG_RBMQ_CFG_H_ */
