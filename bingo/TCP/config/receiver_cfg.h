/*
 * receiver_cfg.h
 *
 *  Created on: 2016-9-27
 *      Author: root
 */

#ifndef BINGO_TCP_CONFIG_RECEIVER_CFG_H_
#define BINGO_TCP_CONFIG_RECEIVER_CFG_H_

#include "bingo/configuration/xml/xml_parser.h"
using namespace bingo::configuration::xml;
#include "cfg_value.h"

namespace bingo { namespace TCP  { namespace config {

class tcp_receiver_cfg {
public:
	tcp_receiver_cfg();
	virtual ~tcp_receiver_cfg();

	// Read xml configuration file.
	// return true if success, cfg::value is configuration information. if the return is false,
	// call cfg::error_what() to check error.
	bool read_xml();

	// Return error information.
	error_what& err();



	// Whether read configuration success, to success then return true;
	bool is_valid;

	// Get configuration information.
	tcp_recevier_cfg_value& get_cfg();

private:
	error_what err_;
	xml_parser xml_;

	// Check node is valid in cfg.xml.
	bool check_node();

	tcp_recevier_cfg_value value_;
};

} } }

#endif /* BINGO_TCP_CONFIG_RECEIVER_CFG_H_ */
