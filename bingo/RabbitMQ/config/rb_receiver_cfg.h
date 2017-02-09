/*
 * rb_recevier_cfg.h
 *
 *  Created on: 2016-8-30
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_CONFIG_RB_RECEIVER_CFG_H_
#define BINGO_RABBITMQ_CONFIG_RB_RECEIVER_CFG_H_

#include "bingo/define.h"
#include "bingo/string.h"
#include "bingo/configuration/xml/xml_parser.h"
using namespace bingo::configuration::xml;

#include "rb_cfg_value.h"

namespace bingo { namespace RabbitMQ { namespace config {

class rb_recevier_cfg {
public:
	rb_recevier_cfg();
	virtual ~rb_recevier_cfg();

	// Read xml configuration file.
	// return true if success, cfg::value is configuration information. if the return is false,
	// call cfg::err_msg() to check error.
	bool read_xml();

	// Returned error message.
	error_what& err();

	// Whether read configuration success, to success then return true;
	bool is_valid;

	// Configuration value object.
	rb_cfg_value value;
private:
	error_what err_;
	xml_parser xml_;

	// Check node is valid in cfg.xml.
	bool check_node();
};

} } }

#endif /* BINGO_RABBITMQ_CONFIG_RB_RECEIVER_CFG_H_ */
