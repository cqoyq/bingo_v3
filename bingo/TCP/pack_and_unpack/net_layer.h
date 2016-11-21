/*
 * net_layer.h
 *
 *  Created on: 2016-9-12
 *      Author: root
 */

#ifndef BINGO_TCP_PACK_AND_UNPACK_NET_LAYER_H_
#define BINGO_TCP_PACK_AND_UNPACK_NET_LAYER_H_

#include "bingo/type.h"

namespace bingo { namespace TCP { namespace pack_and_unpack {

struct net_layer {

	int				 	message_type;
	bingo::u32_t 		next;
	bingo::u32_t		next_num;
	bingo::u64_t		session;
	string				req_addr;

};

} } }

#endif /* BINGO_TCP_PACK_AND_UNPACK_NET_LAYER_H_ */
