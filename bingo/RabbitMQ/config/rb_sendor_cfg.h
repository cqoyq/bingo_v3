/*
 * rb_sendor_cfg.h
 *
 *  Created on: 2016-8-30
 *      Author: root
 */

#ifndef BINGO_RABBITMQ_CONFIG_RB_SENDOR_CFG_H_
#define BINGO_RABBITMQ_CONFIG_RB_SENDOR_CFG_H_

#include "bingo/define.h"
#include "bingo/string.h"
#include "bingo/configuration/xml/xml_parser.h"
using namespace bingo::configuration::xml;

#include "../visitor/asiohandler.h"
#include "rb_cfg_value.h"

#include <boost/ptr_container/ptr_map.hpp>
using namespace boost;

namespace bingo {
        namespace RabbitMQ {
                namespace config {

                        struct rb_sendor_cfg_value : public rb_cfg_value {

                                rb_sendor_cfg_value() {
                                        channel = 0;
                                }

                                virtual ~rb_sendor_cfg_value() {
                                }

                                string map_key;
                                AMQP::Channel* channel;
                        };

                        class rb_sendor_cfg {
                        public:
                                rb_sendor_cfg();
                                virtual ~rb_sendor_cfg();

                                // Read xml configuration file.
                                // return true if success, cfg::value is configuration information. if the return is false,
                                // call cfg::err_msg() to check error.
                                bool read_xml(const char* cfg_file);

                                // Returned error message.
                                error_what& err();

                                // Whether read configuration success, to success then return true;
                                bool is_valid;

                                // Get configuration information by key, return not-null pointer if success,
                                // otherwise return 0.
                                rb_sendor_cfg_value* get_by_key(string key);

                                // Get configuration information by routingkey, return not-null pointer if success,
                                // otherwise return 0.
                                rb_sendor_cfg_value* get_by_routingkey(string routingkey);

                                // Return map size.
                                int size();

                                // Get item by [], if @index > map max size, then return 0;
                                rb_sendor_cfg_value* operator[](int index);

                                // Get item by [], if @index > map max size, then return 0;
                                rb_sendor_cfg_value* at(int index);

                        protected:
                                error_what err_;
                                xml_parser xml_;

                                // Check node is valid in cfg.xml.
                                bool check_node();

                                // Configuration value object.
                                boost::ptr_map<string, rb_sendor_cfg_value> value_;
                        };

                        struct rb_routing_sendor_cfg_value : public rb_cfg_value {

                                rb_routing_sendor_cfg_value() {
                                        channel = 0;
                                }

                                virtual ~rb_routing_sendor_cfg_value() {
                                        routingkeyset.clear();
                                }

                                AMQP::Channel* channel;
                                std::vector<pair<string, string>> routingkeyset; // Value is routingkey, Key is mapkey.
                        };

                        class rb_routing_sendor_cfg {
                        public:
                                rb_routing_sendor_cfg();
                                virtual ~rb_routing_sendor_cfg();

                                // Read xml configuration file.
                                // return true if success, cfg::value is configuration information. if the return is false,
                                // call cfg::err_msg() to check error.
                                bool read_xml(const char* cfg_file);

                                // Returned error message.
                                error_what& err();

                                // Whether read configuration success, to success then return true;
                                bool is_valid;

                                // Get configuration information by key, return not-null pointer if success,
                                // otherwise return 0.
                                rb_routing_sendor_cfg_value* get_by_key(string key);

                                // Get key map to routingkey value.
                                string map_to_routingkey(string key);

                                // Return map size.
                                int size();

                                // Get item by [], if @index > map max size, then return 0;
                                rb_routing_sendor_cfg_value* operator[](int index);

                        protected:
                                error_what err_;
                                xml_parser xml_;

                                // Check node is valid in cfg.xml.
                                bool check_node();
                                
                                // Check <item>'s type attribute in cfg.xml.
                                virtual bool check_rb_type(int type);

                                // Configuration value object.
                                boost::ptr_vector<rb_routing_sendor_cfg_value> value_;
                        };
                        
                        class rb_topic_sendor_cfg : public rb_routing_sendor_cfg {
                        public:
                                  rb_topic_sendor_cfg();
                                  virtual ~rb_topic_sendor_cfg();
                        protected:
                                  bool check_rb_type(int type);
                        };

                }
        }
}

#endif /* BINGO_RABBITMQ_CONFIG_RB_SENDOR_CFG_H_ */
