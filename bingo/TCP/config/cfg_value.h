/*
 * cfg_value.h
 *
 *  Created on: 2016-9-7
 *      Author: root
 */

#ifndef BINGO_TCP_CONFIG_CFG_VALUE_H_
#define BINGO_TCP_CONFIG_CFG_VALUE_H_

#include "bingo/string.h"

namespace bingo { namespace TCP  { namespace config {

struct tcp_sendor_cfg_value {
public:
	tcp_sendor_cfg_value(){
		port = 0;
		handler = 0;
	}
string ip;
u16_t port;
string map_key;
void* handler;
};

struct tcp_recevier_cfg_value{
	string ip;
	u16_t port;
};

} } }

#endif /* BINGO_TCP_CONFIG_CFG_VALUE_H_ */
