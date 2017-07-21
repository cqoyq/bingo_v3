/*
 * rbmq_log_visitor.h
 *
 *  Created on: 2016-9-17
 *      Author: root
 */

#ifndef BINGO_LOG_HANDLERS_RBMQ_LOG_VISITOR_H_
#define BINGO_LOG_HANDLERS_RBMQ_LOG_VISITOR_H_

#include <string>
#include <string.h>
#include <vector>
using namespace std;

#include "../config/rbmq_cfg.h"
#include "../log_handler.h"
using namespace bingo::log;

#include "bingo/RabbitMQ/rabbitmq_factory.h"
using namespace bingo::RabbitMQ;

namespace bingo {
      namespace log {
            namespace handlers {

                  class rbmq_log_visitor : public log_handler {
                  public:
                        rbmq_log_visitor();
                        virtual ~rbmq_log_visitor();

                        // Read configuration information.
                        bool read_config(rabbitmq_factory*& factory);

                        // Override log_handler::handle() method.
                        void handle(int level, const char* tag, std::string& info) override;

                        // Override log_handler::set_level() method.
                        void set_level(log_level level) override;


                        config::rbmq_cfg& config();

                        error_what& err();

                  private:
                        config::rbmq_cfg config_;
                        rabbitmq_factory* factory_;
                        error_what e_what_;
                  };

            }
      }
}

#endif /* BINGO_LOG_HANDLERS_RBMQ_LOG_VISITOR_H_ */
