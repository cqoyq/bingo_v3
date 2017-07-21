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

#include "log_level.h"

namespace bingo {
      namespace log {

            class log_handler {
            public:

                  log_handler() {
                  };

                  virtual ~log_handler() {
                  };

                  virtual void handle(int level, const char* tag, std::string& info) = 0;

                  virtual void set_level(log_level level) {
                  };
            };

      }
}

#endif /* BINGO_LOG_LOG_HANDLER_H */
