/*
 * define.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_DEFINE_HEADER_H_
#define BINGO_DEFINE_HEADER_H_

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

#include <boost/thread.hpp>
using namespace boost;

namespace bingo {

#define message_out_with_thread(str) { \
		ptime p1 = second_clock::local_time(); \
		cout << to_iso_extended_string(p1) << ",thr_id:" << this_thread::get_id() << "," << str << endl; \
		}

#define message_out(str) cout << str << endl;
#define message_out_with_time(str) { \
		ptime p1 = second_clock::local_time(); \
		cout  << to_iso_extended_string(p1) << "," << str << endl; \
		}

}
// ----------------------------  rabbitmq ------------------------------------- //
#define RABBITMQ_MESSAGE_MAX_SIZE                                                                                                                             2048
#define RABBITMQ_ASIOHANDLERLOG_TAG                                                                                                                       "rabbitmq_asiohandler"
#define RABBITMQ_MAX_WAIT_FOR_HEARTBEAT_SECONDS                                                                                    90
#define RABBITMQ_MAPKEY_MAX_SIZE                                                                                                                                40
#define RABBITMQ_SINGLE_SENDOR_ERROR                                                                                                                   "rabbitmq_simple_sendor_error"
#define RABBITMQ_SINGLE_SENDOR_INFO                                                                                                                        "rabbitmq_simple_sendor_info"
#define RABBITMQ_SINGLE_RECEIVER_ERROR                                                                                                                "rabbitmq_simple_receiver_error"
#define RABBITMQ_SINGLE_RECEIVER_INFO                                                                                                                      "rabbitmq_simple_receiver_info"
#define RABBITMQ_WORK_SENDOR_ERROR                                                                                                                      "rabbitmq_work_sendor_error"
#define RABBITMQ_WORK_SENDOR_INFO                                                                                                                           "rabbitmq_work_sendor_info"
#define RABBITMQ_WORK_RECEIVER_ERROR                                                                                                                   "rabbitmq_work_receiver_error"
#define RABBITMQ_WORK_RECEIVER_INFO                                                                                                                        "rabbitmq_work_receiver_info"
#define RABBITMQ_PUBLISH_SENDOR_ERROR                                                                                                                "rabbitmq_publish_sendor_error"
#define RABBITMQ_PUBLISH_SENDOR_INFO                                                                                                                     "rabbitmq_publish_sendor_info"
#define RABBITMQ_PUBLISH_RECEIVER_ERROR                                                                                                              "rabbitmq_publish_receiver_error"
#define RABBITMQ_PUBLISH_RECEIVER_INFO                                                                                                                  "rabbitmq_publish_receiver_info"
#define RABBITMQ_ROUTING_SENDOR_ERROR                                                                                                              "rabbitmq_routing_sendor_error"
#define RABBITMQ_ROUTING_SENDOR_INFO                                                                                                                   "rabbitmq_routing_sendor_info"
#define RABBITMQ_ROUTING_RECEIVER_ERROR                                                                                                            "rabbitmq_routing_receiver_error"
#define RABBITMQ_ROUTING_RECEIVER_INFO                                                                                                                 "rabbitmq_routing_receiver_info"
#define RABBITMQ_TOPIC_SENDOR_ERROR                                                                                                                      "rabbitmq_topic_sendor_error"
#define RABBITMQ_TOPIC_SENDOR_INFO                                                                                                                           "rabbitmq_topic_sendor_info"
#define RABBITMQ_TOPIC_RECEIVER_ERROR                                                                                                                    "rabbitmq_topic_receiver_error"
#define RABBITMQ_TOPIC_RECEIVER_INFO                                                                                                                        "rabbitmq_topic_receiver_info"



#endif /* BINGO_DEFINE_HEADER_H_ */
