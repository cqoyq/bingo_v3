/*
 * local_log_visitor.h
 *
 *  Created on: 2016-8-31
 *      Author: root
 */

#ifndef BINGO_LOG_HANDLERS_LOCAL_LOG_VISITOR_H
#define BINGO_LOG_HANDLERS_LOCAL_LOG_VISITOR_H

#include "bingo/string.h"
#include "bingo/error_what.h"
#include "../config/local_cfg.h"
#include "../log_handler.h"
#include "../logger.h"
using namespace bingo::log;

#include <string>
#include <string.h>
#include <vector>
using namespace std;

namespace bingo {
      namespace log {
            namespace handlers {

                  class local_log_visitor : public log_handler {
                  public:
                        local_log_visitor();

                        virtual ~local_log_visitor();

                        // Read configuration information ,fail and return false, then call err().
                        bool read_config(const char* config_file);

                        // Get error information.
                        error_what& err();

                        // Override log_handler::handle() method.
                        void handle(int level, const char* tag, std::string& info)override;

                        // Override log_handler::set_level() method.
                        void set_level(log_level level) override;


                        config::local_cfg& config();

                  private:
                        config::local_cfg config_;
                        logger* logger_;
                        error_what e_what_;
                  };

            }
      }
}

#endif /* BINGO_LOG_HANDLERS_LOCAL_LOG_VISITOR_H */
