/*
 * simple_receiver.cpp
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#include "../visitor/asiohandler.h"

#include "simple_receiver.h"

using namespace bingo::RabbitMQ::handlers;

simple_receiver::simple_receiver() : rb_receiver() {
	// TODO Auto-generated constructor stub
	is_valid = cfg_.read_xml();
	if(!is_valid) {
		cfg_.err().clone(e_what_);
	}
}

simple_receiver::~simple_receiver() {
	// TODO Auto-generated destructor stub
}

void simple_receiver::connet_to_server(log_handler*& log){

	if(is_valid){
		while(true){
				boost::asio::io_service ioService;
				AsioHandler handler(ioService, log, true);
				handler.connect(cfg_.value.ip.c_str(), cfg_.value.port);

				AMQP::Connection connection(&handler, AMQP::Login(
						cfg_.value.username.c_str(), cfg_.value.pwd.c_str()),
						cfg_.value.host.c_str());

				AMQP::Channel channel(&connection);
				channel.declareQueue(cfg_.value.routingkey.c_str());

				channel.onError([&](const char *message) {

					// report error
					if(log){
						string s_msg = message;
						log->handle(bingo::log::LOG_LEVEL_ERROR,  RABBITMQ_SINGLE_RECEIVER_ERROR, s_msg);
					}
				});

				// callback function that is called when the consume operation starts
				auto startCb = [&](const std::string &consumertag) {

					if(log){
						string s_msg = "consume operation started!" ;
						log->handle(bingo::log::LOG_LEVEL_INFO,  RABBITMQ_SINGLE_RECEIVER_INFO, s_msg);
					}
				};

				// callback function that is called when the consume operation failed
				auto errorCb = [](const char *message) {

			//		std::cout << "consume operation failed" << std::endl;
				};

				// callback operation when a message was received
				auto messageCb = [&](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
					string s_msg;
					s_msg = message.message();
#ifdef BINGO_MQ_DEBUG
					if(log){
						log->handle(bingo::log::LOG_LEVEL_DEBUG,  RABBITMQ_SINGLE_RECEIVER_INFO, s_msg);
					}
#endif
					if(f_) f_(s_msg);

					// acknowledge the message
			//        channel.ack(deliveryTag);
				};


				channel.consume("", AMQP::noack)
					.onReceived(messageCb)
					.onSuccess(startCb)
					.onError(errorCb);

				ioService.run();

				if(log){
					string s_msg = "ioService is exit!";
					log->handle(bingo::log::LOG_LEVEL_INFO,  RABBITMQ_SINGLE_RECEIVER_ERROR, s_msg);
				}
		}
	}
}

