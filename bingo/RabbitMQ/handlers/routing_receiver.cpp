/*
 * routing_receiver.cpp
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#include "../visitor/asiohandler.h"

#include "routing_receiver.h"

using namespace bingo::RabbitMQ::handlers;

routing_receiver::routing_receiver() : rb_receiver() {
        // TODO Auto-generated constructor stub
        is_valid = cfg_.read_xml();  
        if (!is_valid) {
                cfg_.err().clone(e_what_);
        }
}

routing_receiver::~routing_receiver() {
        // TODO Auto-generated destructor stub
}

void routing_receiver::connet_to_server(log_handler*& log) {

        if (is_valid) {
                while (true) {
                        boost::asio::io_service ioService;
                        AsioHandler handler(ioService, log, true);
                        handler.connect(cfg_.value.ip.c_str(), cfg_.value.port);

                        AMQP::Connection connection(&handler, AMQP::Login(
                                cfg_.value.username.c_str(), cfg_.value.pwd.c_str()),
                                cfg_.value.host.c_str());

                        AMQP::Channel channel(&connection);

                        auto receiveMessageCallback = [&](const AMQP::Message &message,
                                uint64_t deliveryTag,
                                bool redelivered) {
                                string s_msg;
                                s_msg = message.message();
                                if (f_) f_(s_msg);
                        };

                        AMQP::QueueCallback callback =
                                [&](const std::string &name, int msgcount, int consumercount) {

                                        foreach_(string& n, cfg_.routingkeyset) {
                                                channel.bindQueue(cfg_.value.exchange.c_str(), "", n.c_str());
                                                channel.consume(name, AMQP::noack).onReceived(receiveMessageCallback);
                                        }

                                };

                        AMQP::SuccessCallback success = [&]() {
                                channel.declareQueue(AMQP::exclusive).onSuccess(callback);
                        };

                        channel.declareExchange(cfg_.value.exchange.c_str(), AMQP::direct).onSuccess(success);

                        ioService.run();

                        if (log) {
                                string s_msg = "ioService is exit!";
                                log->handle(bingo::log::LOG_LEVEL_INFO, RABBITMQ_ROUTING_RECEIVER_ERROR, s_msg);
                        }
                }
        }
}

