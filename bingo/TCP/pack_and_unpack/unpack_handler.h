/*
 * unpack_handler.h
 *
 *  Created on: 2016-9-12
 *      Author: root
 */

#ifndef BINGO_TCP_PACK_AND_UNPACK_UNPACK_HANDLER_H_
#define BINGO_TCP_PACK_AND_UNPACK_UNPACK_HANDLER_H_

#include <string>
#include <string.h>
#include <vector>
using namespace std;

#include "bingo/config/node.h"
#include "net_layer.h"

namespace bingo { namespace TCP { namespace pack_and_unpack {

class unpack_handler
{
public:
	virtual ~unpack_handler(){};
   /* Unpack stream to data.
    * return 0 if success, otherwise return -1. */
   virtual int handler(char*& in_data, size_t& in_data_size, net_layer*& in_net,  bingo::config::node*& out_data)=0;

protected:
private:

};

}  }  }  /* BINGO_TCP_PACK_AND_UNPACK_UNPACK_HANDLER_H_ */

#endif
