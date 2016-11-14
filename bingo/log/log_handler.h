/*
 * log_handler.h
 *
 *  Created on: 2016-11-8
 *      Author: root
 */

#ifndef BINGO_LOG_LOG_HANDLER_H
#define BINGO_LOG_LOG_HANDLER_H

#include <string>
#include <string.h>
#include <vector>
using namespace std;

namespace bingo { namespace log {

class log_handler
{
public:
	virtual ~log_handler(){}
   virtual void handle(int level, const char* tag, std::string& info)=0;
};

}  }

#endif /* BINGO_LOG_LOG_HANDLER_H */
