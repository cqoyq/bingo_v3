/*
 * pack_handler.h
 *
 *  Created on: 2016-9-12
 *      Author: root
 */

#ifndef BINGO_TCP_PACK_AND_UNPACK_PACK_HANDLER_H_
#define BINGO_TCP_PACK_AND_UNPACK_PACK_HANDLER_H_

#include <string>
#include <string.h>
#include <vector>
using namespace std;

#include "bingo/configuration/node.h"
#include "net_layer.h"

namespace bingo { namespace TCP { namespace pack_and_unpack {

class pack_handler
{
public:
	virtual ~pack_handler(){}
   /* Pack data to stream.
    * return 0 if success, otherwise return -1. */
   virtual int handler(bingo::configuration::node*& in_data, net_layer*& in_net, char*& out_data, size_t& out_data_size)=0;

protected:
private:

};

}  }  }

#endif /* BINGO_TCP_PACK_AND_UNPACK_PACK_HANDLER_H_ */
