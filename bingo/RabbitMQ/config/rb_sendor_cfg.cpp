/*
 * rb_sendor_cfg.cpp
 *
 *  Created on: 2016-8-30
 *      Author: root
 */

#include "rb_sendor_cfg.h"

#include "bingo/error_what.h"

using namespace bingo::RabbitMQ::config;

// ------------------------------- rb_sendor_cfg -------------------------------------------- //

rb_sendor_cfg::rb_sendor_cfg() {
        // TODO Auto-generated constructor stub
        is_valid = false;
}

rb_sendor_cfg::~rb_sendor_cfg() {
        // TODO Auto-generated destructor stub
}

bool rb_sendor_cfg::read_xml() {
        if (!xml_.read("./cfg.xml")) {
                xml_.err().clone(err_);
                err_.err_no(ERROR_TYPE_RABBITMQ_SENDOR_READ_CFG_FAIL);
                return false;
        } else {

                // Check rb_sendor_cfg.xml.
                is_valid = check_node();
                if (!is_valid)
                        err_.err_no(ERROR_TYPE_RABBITMQ_SENDOR_CHECK_NODE_FAIL);
                return is_valid;
        }
}

bingo::error_what& rb_sendor_cfg::err() {
        return err_;
}

bool rb_sendor_cfg::check_node() {

        // Check cfg.xml content:
        //	<application>
        //		<rabbitmq_sendor>
        //				<list>
        //					<item key="" type="1" ip="127.0.0.1" port="5672" username="guest" pwd="guest" host="/" exchange="" routingkey="hello"/>
        //					<item key="" type="2" ip="127.0.0.1" port="5672" username="guest" pwd="guest" host="/" exchange="" routingkey="hello"/>
        //				</list>
        //		</rabbitmq_sendor>
        //	</application>

        bingo::configuration::node* list = xml_.get_node("application.rabbitmq_sendor.list");
        if (list == 0) {
                xml_.err().clone(err_);
                return false;
        }

        int max = list->child.size();
        for (int i = 0; i < max; i++) {
                bingo::configuration::node* item = list->child[i];

                string s_key;
                string s_ip;
                string s_port;
                string s_username;
                string s_pwd;
                string s_host;
                string s_exchange;
                string s_routingkey;
                int n_type = 0;

                // type
                if (item->attrs["type"] == 0) {
                        err_.err_message("type's attribute is empty!");
                        return false;
                } else
                        n_type = lexical_cast<int>(item->attrs["type"]->value);
                if (((rb_cfg_type) n_type) == RB_CFG_TYPE_SIMPLE ||
                        ((rb_cfg_type) n_type) == RB_CFG_TYPE_WORKER ||
                        ((rb_cfg_type) n_type) == RB_CFG_TYPE_PUBLISH_AND_SUBSCRIBE) {

                } else {
                        continue;
                }
                // key
                if (item->attrs["key"] == 0) {
                        err_.err_message("key's attribute is empty!");
                        return false;
                } else
                        s_key = item->attrs["key"]->value;

                // key-length, max size is 40.
                if (s_key.length() > RABBITMQ_MAPKEY_MAX_SIZE) {
                        err_.err_message("size of key is more than 40!");
                        return false;
                }

                // ip
                if (item->attrs["ip"] == 0) {
                        err_.err_message("ip's attribute is empty!");
                        return false;
                } else
                        s_ip = item->attrs["ip"]->value;

                // port
                if (item->attrs["port"] == 0) {
                        err_.err_message("port's attribute is empty!");
                        return false;
                } else
                        s_port = item->attrs["port"]->value;
                uint16_t n_port = lexical_cast<uint16_t>(s_port);

                // username
                if (item->attrs["username"] == 0) {
                        err_.err_message("username's attribute is empty!");
                        return false;
                } else
                        s_username = item->attrs["username"]->value;

                // pwd
                if (item->attrs["pwd"] == 0) {
                        err_.err_message("pwd's attribute is empty!");
                        return false;
                } else
                        s_pwd = item->attrs["pwd"]->value;

                // host
                if (item->attrs["host"] == 0) {
                        err_.err_message("host's attribute is empty!");
                        return false;
                } else
                        s_host = item->attrs["host"]->value;

                // exchange
                if (item->attrs["exchange"] == 0) {
                        err_.err_message("exchange's attribute is empty!");
                        return false;
                } else
                        s_exchange = item->attrs["exchange"]->value;

                // routingkey
                if (item->attrs["routingkey"] == 0) {
                        err_.err_message("routingkey's attribute is empty!");
                        return false;
                } else
                        s_routingkey = item->attrs["routingkey"]->value;

                rb_sendor_cfg_value* v = new rb_sendor_cfg_value();
                v->host = s_host.c_str();
                v->ip = s_ip.c_str();
                v->port = n_port;
                v->pwd = s_pwd.c_str();
                v->exchange = s_exchange.c_str();
                v->routingkey = s_routingkey.c_str();
                v->username = s_username.c_str();
                v->map_key = s_key.c_str();
                v->type = (rb_cfg_type) n_type;

                this->value_.insert(s_key, v);
        }
        return true;
}

rb_sendor_cfg_value* rb_sendor_cfg::get_by_key(string key) {
        if (value_.find(key) == value_.end())
                return 0;
        else
                return &(value_[key]);
}

rb_sendor_cfg_value* rb_sendor_cfg::get_by_routingkey(string routingkey) {
        typedef boost::ptr_map<string, rb_sendor_cfg_value>::iterator my_iter;
        my_iter iter = value_.begin();
        while (iter != value_.end()) {
                if ((*iter).second ->routingkey.compare(routingkey.c_str()) == 0) {
                        return (*iter).second;
                }
                iter++;
        }

        return 0;
}

int rb_sendor_cfg::size() {
        return value_.size();
}

rb_sendor_cfg_value* rb_sendor_cfg::operator[](int index) {
        typedef boost::ptr_map<string, rb_sendor_cfg_value>::value_type ptr_mp_value;

        int max = (value_.size() > 0) ? value_.size() - 1 : 0;
        if (index > max)
                return 0;
        else {
                rb_sendor_cfg_value* p = 0;
                int n = 0;

                foreach_(ptr_mp_value v, value_) {
                        if (n == index) {
                                p = v.second;
                                break;
                        }
                        n++;
                }
                return p;
        }
}

rb_sendor_cfg_value* rb_sendor_cfg::at(int index) {
        typedef boost::ptr_map<string, rb_sendor_cfg_value>::value_type ptr_mp_value;

        int max = (value_.size() > 0) ? value_.size() - 1 : 0;
        if (index > max)
                return 0;
        else {
                rb_sendor_cfg_value* p = 0;
                int n = 0;

                foreach_(ptr_mp_value v, value_) {
                        if (n == index) {
                                p = v.second;
                                break;
                        }
                        n++;
                }
                return p;
        }
}


// ------------------------------- rb_routing_sendor_cfg -------------------------------------------- //

rb_routing_sendor_cfg::rb_routing_sendor_cfg() {
        is_valid = false;
}

rb_routing_sendor_cfg::~rb_routing_sendor_cfg() {

}

bool rb_routing_sendor_cfg::read_xml() {
        if (!xml_.read("./cfg.xml")) {
                xml_.err().clone(err_);
                err_.err_no(ERROR_TYPE_RABBITMQ_SENDOR_READ_CFG_FAIL);
                return false;
        } else {

                // Check rb_sendor_cfg.xml.
                is_valid = check_node();
                if (!is_valid)
                        err_.err_no(ERROR_TYPE_RABBITMQ_SENDOR_CHECK_NODE_FAIL);
                return is_valid;
        }
}

bingo::error_what& rb_routing_sendor_cfg::err() {
        return err_;
}

bool rb_routing_sendor_cfg::check_node() {
        // Check cfg.xml content:
        //	<application>
        //		<rabbitmq_sendor>
        //				<list>
        //					<item type="4" ip="127.0.0.1" port="5672" username="guest" pwd="guest" host="/" exchange="">
        //                                                              <routingkey key="t1">test1</routingkey>
        //                                                              <routingkey key="t2">test2</routingkey>
        //                                                           </item>
        //				</list>
        //		</rabbitmq_sendor>
        //	</application>

        bingo::configuration::node* list = xml_.get_node("application.rabbitmq_sendor.list");
        if (list == 0) {
                xml_.err().clone(err_);
                return false;
        }

        int max = list->child.size();
        for (int i = 0; i < max; i++) {
                bingo::configuration::node* item = list->child[i];

                string s_ip;
                string s_port;
                string s_username;
                string s_pwd;
                string s_host;
                string s_exchange;
                int n_type = 0;


                // type
                if (item->attrs["type"] == 0) {
                        err_.err_message("type's attribute is empty!");
                        return false;
                } else
                        n_type = lexical_cast<int>(item->attrs["type"]->value);
                if (((rb_cfg_type) n_type) != RB_CFG_TYPE_ROUTING) {
                        continue;
                }

                // ip
                if (item->attrs["ip"] == 0) {
                        err_.err_message("ip's attribute is empty!");
                        return false;
                } else
                        s_ip = item->attrs["ip"]->value;

                // port
                if (item->attrs["port"] == 0) {
                        err_.err_message("port's attribute is empty!");
                        return false;
                } else
                        s_port = item->attrs["port"]->value;
                uint16_t n_port = lexical_cast<uint16_t>(s_port);

                // username
                if (item->attrs["username"] == 0) {
                        err_.err_message("username's attribute is empty!");
                        return false;
                } else
                        s_username = item->attrs["username"]->value;

                // pwd
                if (item->attrs["pwd"] == 0) {
                        err_.err_message("pwd's attribute is empty!");
                        return false;
                } else
                        s_pwd = item->attrs["pwd"]->value;

                // host
                if (item->attrs["host"] == 0) {
                        err_.err_message("host's attribute is empty!");
                        return false;
                } else
                        s_host = item->attrs["host"]->value;

                // exchange
                if (item->attrs["exchange"] == 0) {
                        err_.err_message("exchange's attribute is empty!");
                        return false;
                } else
                        s_exchange = item->attrs["exchange"]->value;

                rb_routing_sendor_cfg_value* v = new rb_routing_sendor_cfg_value();
                v->host = s_host.c_str();
                v->ip = s_ip.c_str();
                v->port = n_port;
                v->pwd = s_pwd.c_str();
                v->exchange = s_exchange.c_str();
                v->username = s_username.c_str();
                v->type = (rb_cfg_type) n_type;

                // routingkeyset

                foreach_(node& n, item->child.collection()) {

                        // key
                        string key;
                        if (n.attrs["key"] == 0) {
                                err_.err_message("routingkey's attribute is empty!");
                                return false;
                        } else
                                key = n.attrs["key"]->value;

                        // routingkey
                        string routingkey = n.value;

                        v->routingkeyset.push_back(make_pair(key, routingkey));
                }

                this->value_.push_back(v);
        }

        return true;
}

rb_routing_sendor_cfg_value* rb_routing_sendor_cfg::get_by_key(string key) {
      
        foreach_(rb_routing_sendor_cfg_value& n, value_) {
                int max = n.routingkeyset.size();
                for (int i = 0; i < max; i++) {
                        pair<string, string> k = n.routingkeyset[i];
                        if(k.first.compare(key.c_str()) == 0)
                                return &n;

                }
        }
        
        return 0;
}

string rb_routing_sendor_cfg::map_to_routingkey(string key) {

        foreach_(rb_routing_sendor_cfg_value& n, value_) {
                int max = n.routingkeyset.size();
                for (int i = 0; i < max; i++) {
                        pair<string, string> k = n.routingkeyset[i];
                        if(k.first.compare(key.c_str()) == 0)
                                return k.second;

                }
        }
        
        return string("");
}

int rb_routing_sendor_cfg::size() {
        return value_.size();
}

rb_routing_sendor_cfg_value* rb_routing_sendor_cfg::operator[](int index) {

        int max = (value_.size() > 0) ? value_.size() - 1 : 0;
        if (index > max)
                return 0;
        else {
                return &value_[index];
        }
}
