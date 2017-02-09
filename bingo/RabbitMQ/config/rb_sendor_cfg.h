/*
 * rb_sendor_cfg.h
 *
 *  Created on: 2016-8-30
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_CONFIG_RB_SENDOR_CFG_H_
#define BINGO_RABBITMQ_CONFIG_RB_SENDOR_CFG_H_

#include "bingo/define.h"
#include "bingo/string.h"
#include "bingo/configuration/xml/xml_parser.h"
using namespace bingo::configuration::xml;

#include "../visitor/asiohandler.h"
#include "rb_cfg_value.h"

#include <boost/ptr_container/ptr_map.hpp>
using namespace boost;

namespace bingo { namespace RabbitMQ { namespace config {

struct rb_sendor_cfg_value : public rb_cfg_value {
	rb_sendor_cfg_value(){
		channel = 0;
	}
	virtual ~rb_sendor_cfg_value(){}

	string				 map_key;
	AMQP::Channel* channel;
};

class rb_sendor_cfg {
public:
	rb_sendor_cfg();
	virtual ~rb_sendor_cfg();

	// Read xml configuration file.
	// return true if success, cfg::value is configuration information. if the return is false,
	// call cfg::err_msg() to check error.
	bool read_xml();

	// Returned error message.
	error_what& err();

	// Whether read configuration success, to success then return true;
	bool is_valid;

	// Get configuration information by key, return not-null pointer if success,
	// otherwise return 0.
	rb_sendor_cfg_value* get_by_key(string key);

	// Get configuration information by routingkey, return not-null pointer if success,
	// otherwise return 0.
	rb_sendor_cfg_value* get_by_routingkey(string routingkey);

	// Return map size.
	int size();

	// Get item by [], if @index > map max size, then return 0;
	rb_sendor_cfg_value* operator[](int index);

private:
	error_what err_;
	xml_parser xml_;

	// Check node is valid in cfg.xml.
	bool check_node();

	// Configuration value object.
	boost::ptr_map<string, rb_sendor_cfg_value>  value_;
};

} } }

#endif /* BINGO_RABBITMQ_CONFIG_RB_SENDOR_CFG_H_ */
