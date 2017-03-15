/*
 * rb_recevier_cfg.h
 *
 *  Created on: 2016-8-30
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_CONFIG_RB_RECEIVER_CFG_H_
#define BINGO_RABBITMQ_CONFIG_RB_RECEIVER_CFG_H_

#include "bingo/define.h"
#include "bingo/string.h"
#include "bingo/configuration/xml/xml_parser.h"
using namespace bingo::configuration::xml;

#include "rb_cfg_value.h"

namespace bingo {
          namespace RabbitMQ {
                    namespace config {

                              class rb_recevier_cfg {
                              public:
                                        rb_recevier_cfg();
                                        virtual ~rb_recevier_cfg();

                                        // Read xml configuration file.
                                        // return true if success, cfg::value is configuration information. if the return is false,
                                        // call cfg::err_msg() to check error.
                                        bool read_xml(const char* cfg_file);

                                        // Returned error message.
                                        error_what& err();

                                        // Whether read configuration success, to success then return true;
                                        bool is_valid;

                                        // Configuration value object.
                                        rb_cfg_value value;
                              protected:
                                        error_what err_;
                                        xml_parser xml_;

                                        // Check node is valid in cfg.xml.
                                        virtual bool check_node();
                              };

                              class rb_routing_recevier_cfg : public rb_recevier_cfg {
                              public:
                                        rb_routing_recevier_cfg();
                                        virtual ~rb_routing_recevier_cfg();

                                        // Routingkey set
                                        std::vector<string> routingkeyset;

                              protected:
                                        bool check_node();
                              };

                    }
          }
}

#endif /* BINGO_RABBITMQ_CONFIG_RB_RECEIVER_CFG_H_ */
