/*
 * single_cfg_value.h
 *
 *  Created on: 2016-9-6
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_CONFIG_RB_CFG_VALUE_H_
#define BINGO_RABBITMQ_CONFIG_RB_CFG_VALUE_H_

#include "bingo/string.h"

namespace bingo { namespace RabbitMQ { namespace config {

enum rb_cfg_type {
	RB_CFG_TYPE_SIMPLE				= 0x01,
	RB_CFG_TYPE_WORKER				= 0x02,
	RB_CFG_TYPE_PUBLISH_AND_SUBSCRIBE		= 0x03,
                      RB_CFG_TYPE_ROUTING                                                                          = 0x04,
                      RB_CFG_TYPE_TOPIC                                                                                 = 0x05,
};
struct rb_cfg_value {
	string ip;
	uint16_t port;
	string username;
	string pwd;
	string host;
	string routingkey;
	string exchange;
	rb_cfg_type type;
};

} } }

#endif /* BINGO_RABBITMQ_CONFIG_RB_CFG_VALUE_H_ */
