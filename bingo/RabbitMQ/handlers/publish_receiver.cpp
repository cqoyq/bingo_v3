/*
 * publish_receiver.cpp
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#include "../visitor/asiohandler.h"

#include "publish_receiver.h"

using namespace bingo::RabbitMQ::handlers;

publish_receiver::publish_receiver() : rb_receiver() {
        // TODO Auto-generated constructor stub
        is_valid = cfg_.read_xml();
        if (!is_valid) {
                cfg_.err().clone(e_what_);
        }
}

publish_receiver::~publish_receiver() {
        // TODO Auto-generated destructor stub
}

void publish_receiver::connet_to_server(log_handler*& log) {

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
                                        channel.bindQueue(cfg_.value.exchange.c_str(), name, "");
                                        channel.consume(name, AMQP::noack).onReceived(receiveMessageCallback);
                                };

                        AMQP::SuccessCallback success = [&]() {
                                channel.declareQueue(AMQP::exclusive).onSuccess(callback);
                        };

                        channel.declareExchange(cfg_.value.exchange.c_str(), AMQP::fanout).onSuccess(success);

                        ioService.run();

                        if (log) {
                                string s_msg = "ioService is exit!";
                                log->handle(bingo::log::LOG_LEVEL_INFO, RABBITMQ_PUBLISH_RECEIVER_ERROR, s_msg);
                        }
                }
        }
}

