/*
 * singlesendor.cpp
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#include "../visitor/asiohandler.h"

#include "bingo/thread/all.h"

#include "single_sendor.h"

// --------------------------- rabbitmq client  ------------------------- //

log_handler* rb_snd_log;

// --------------------------- make sendor thread ------------------------- //

void make_sendor_thr(bingo::RabbitMQ::config::rb_sendor_cfg_value*& p){

//	cout << "make_sendor_thr map-key:" << p->map_key << ",p:" << p << endl;

	while(true){
		boost::asio::io_service ioService;

		AsioHandler handler(ioService, rb_snd_log);
		handler.connect(p->ip.c_str(), p->port);

		AMQP::Connection connection(&handler, AMQP::Login(
				p->username.c_str(), p->pwd.c_str()),
				p->host.c_str());

		AMQP::Channel channel(&connection);

		// Save channel pointer.
		p->channel = &channel;

		channel.onError([&](const char *message) {

			// report error
	//		std::cout << "channel error: " << message << std::endl;
			if(rb_snd_log){
				string s_msg = message;
				rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR, s_msg);
			}
		});

		channel.onReady([&]()
		{
			if(handler.connected())
			{
	//				bool re = channel->publish("", "hello", "Hello World!");
	//				std::cout << " [x] Sent 'Hello World!'" << ",return:" << re << std::endl;
				if(rb_snd_log){
					string s_msg = "send ready ok! map-key:" + p->map_key;
					rb_snd_log->handle(bingo::log::LOG_LEVEL_INFO,  RABBITMQ_SINGLE_SENDOR_INFO, s_msg);
				}
			}
		});

		ioService.run();

		if(rb_snd_log){
			string s_msg = "ioService is exit!";
			rb_snd_log->handle(bingo::log::LOG_LEVEL_INFO,  RABBITMQ_SINGLE_SENDOR_INFO, s_msg);
		}

	}
}

// --------------------------- rabbitmq many_to_one task ------------------------- //

typedef bingo::thread::many_to_one<
		rb_data_message,
		rb_exit_message
	> rb_task_by_thread;
typedef bingo::singleton_v1<rb_task_by_thread,
		rb_task_by_thread::thr_top_callback
		> RB_TASK_TYPE;

// --------------------------- rabbitmq task callback ------------------------- //

using namespace bingo::RabbitMQ::handlers;

void single_sendor::rb_top(rb_data_message*& msg, bingo_empty_type&){

	if(is_valid){

		char* header = msg->data.header();

		char* data = header + RABBITMQ_MAPKEY_MAX_SIZE;
		size_t data_size = msg->data.length() - RABBITMQ_MAPKEY_MAX_SIZE;

		// Get key.
		char key[RABBITMQ_MAPKEY_MAX_SIZE + 1];
		memset(key, 0x00, RABBITMQ_MAPKEY_MAX_SIZE + 1);
		memcpy(key, header, RABBITMQ_MAPKEY_MAX_SIZE);
		string s_key = key;

		config::rb_sendor_cfg_value* p = cfg_.get_by_key(s_key);
		// Check map-key.
		if(p == 0){

			string s_msg = "send task data fail! map-key isn't exist, map-key:";
			s_msg.append(s_key);
			rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
		}else{
			// Check AMQP::Channel*.
			if(p->channel == 0){

				string s_msg = "send task data fail! channel is empty.";
				rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
				return ;
			}

			// Send message body.
			bool re = p->channel->publish("", p->routingkey.c_str(), data, data_size);
			if(!re){
				if(rb_snd_log){
					string s_msg = "send task data fail!";
					rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
				}
			}else{
#ifdef BINGO_MQ_DEBUG
				if(rb_snd_log){
					string_ex t;
					string s_msg = "send task data success! map-key:" ;
							s_msg.append(s_key);
							s_msg.append(",data_size:#");
							s_msg.append(lexical_cast<string>(data_size));
							s_msg.append("#,data:#");
//							s_msg.append(t.stream_to_string(data, data_size));

							char s_d[data_size+ 1];
							memset(s_d, 0x00, data_size + 1);
							memcpy(s_d, data, data_size);
							s_msg.append(s_d);
							s_msg.append("#");

					rb_snd_log->handle(bingo::log::LOG_LEVEL_DEBUG,  RABBITMQ_SINGLE_SENDOR_INFO,  s_msg);
				}
#endif
			}

		}
	}
}

void single_sendor::transfer_data_by_routingkey(string& routingkey, char*& in, size_t& in_size){
	// Check whether has read cfg.xml file, if don't read, then return directly;
	if(!is_valid){
		return ;
	}

	// Check routingkey.
	config::rb_sendor_cfg_value* cfg_value = cfg_.get_by_routingkey(routingkey);
	if(cfg_value == 0){
		if(rb_snd_log){

			string	s_msg = "transfer data is fail, routingkey isn't exist! routingkey:";
			s_msg.append(routingkey.c_str());
			rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
		}
		return;
	}

	// Get key.
	string key = cfg_value->map_key;

	// Make message package, package construct: mapkey(20) + message_body.
	size_t message_size = RABBITMQ_MAPKEY_MAX_SIZE + in_size;
	char message[message_size];
	memset(message , 0x00,  message_size);
	memcpy(message, key.c_str(), key.length());
	memcpy(message + RABBITMQ_MAPKEY_MAX_SIZE, in, in_size);

	// Make rb_data_message object.
	error_what err;
	rb_data_message* data = new rb_data_message();

	if(data->data.copy(message, message_size, err) == -1){
		if(rb_snd_log){

			string s_msg = "create task data error, err_no:" + lexical_cast<string>(err.err_no());
			rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
		}

		delete data;
		return;
	}

	// Send data to MQ server.
	if(RB_TASK_TYPE::instance()->put(data, err) == -1){
		if(rb_snd_log){
			string s_msg = "put data to task error, err_no:" + lexical_cast<string>(err.err_no());
			rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
		}
		delete data;
	}
}

void single_sendor::transfer_data_by_key(string& key, char*& in, size_t& in_size){
	// Check whether has read cfg.xml file, if don't read, then return directly;
	if(!is_valid){
		return ;
	}

	// Check key.
	if(key.length() > RABBITMQ_MAPKEY_MAX_SIZE){
		if(rb_snd_log){
			string s_msg = "transfer data is fail, size of key is more then 20!";
			rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
		}
		return;
	}

	// Make message package, package construct: mapkey(20) + message_body.
	size_t message_size = RABBITMQ_MAPKEY_MAX_SIZE + in_size;
	char message[message_size];
	memset(message , 0x00,  message_size);
	memcpy(message, key.c_str(), key.length());
	memcpy(message + RABBITMQ_MAPKEY_MAX_SIZE, in, in_size);

	// Make rb_data_message object.
	error_what err;
	rb_data_message* data = new rb_data_message();

	if(data->data.copy(message, message_size, err) == -1){
		if(rb_snd_log){
			string s_msg = "create task data error, err_no:" + lexical_cast<string>(err.err_no());
			rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
		}

		delete data;
		return;
	}

	// Send data to MQ server.
	if(RB_TASK_TYPE::instance()->put(data, err) == -1){
		if(rb_snd_log){
			string s_msg = "put data to task error, err_no:" + lexical_cast<string>(err.err_no());
			rb_snd_log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_SENDOR_ERROR,  s_msg);
		}
		delete data;
	}
}


single_sendor::single_sendor():  rb_sendor() {
	// TODO Auto-generated constructor stub

	is_valid = cfg_.read_xml();
	if(!is_valid){
		cfg_.err().clone(e_what_);
	}else{
		RB_TASK_TYPE::construct(
					bind(&single_sendor::rb_top, this, _1, _2) // thread_task queue top callback
						);
	}
}

single_sendor::~single_sendor() {
	// TODO Auto-generated destructor stub
	if(is_valid)	 RB_TASK_TYPE::release();
}

void single_sendor::connet_to_server(log_handler*& log){
	if(is_valid){

		rb_snd_log = log;

		int max = cfg_.size();
		for (int i = 0; i < max; i++) {

			config::rb_sendor_cfg_value* p = cfg_[i];

			if(p->type == config::RB_CFG_TYPE_P2P)
				tg_.create_thread(bind(make_sendor_thr, p));

		}

		tg_.join_all();


	}
}

