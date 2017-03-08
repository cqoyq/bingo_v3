/*
 * file_logger.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include "../../bingo/RabbitMQ/rabbitmq_factory.h"
#include "../../bingo/log/log_factory.h"
#include "../../bingo/log/log_level.h"
#include "../../bingo/string.h"
using namespace bingo;
using namespace bingo::log;
using namespace bingo::RabbitMQ;

#include <boost/thread.hpp>
using namespace boost;

// Test one sendor and one receiver, the message is good transfered from sendor to receiver.
BOOST_AUTO_TEST_SUITE(test_rabbitmq_simple)

void run_sendor(rabbitmq_factory*& p){
    this_thread::sleep(seconds(5));
    
    string info = "this is a test";
    char* p_info = const_cast<char*>(info.c_str());
    message_out_with_time("send msg:" << info );
    
    string routingkey = "test_simple";
    p->transfer_data_by_routingkey(routingkey, p_info, info.length());
};

BOOST_AUTO_TEST_CASE(t_sendor){
     rabbitmq_factory factory;
    boost::thread t(run_sendor, &factory);
    
    log_handler* logger = 0;
    factory.make_simple_sendor(logger);
    
    t.join();
    
    // output:
    //    send msg:this is a test, time:2017-03-07T21:54:24
}

void recv(std::string& msg){
   message_out_with_time("receive msg:" << msg );
}

BOOST_AUTO_TEST_CASE(t_receiver){
    rabbitmq_factory factory;
    
    log_handler* logger = 0;
    factory.make_simple_receiver(logger, recv);
    
    // output:
    //    receive msg:this is a test, time:2017-03-07T21:54:24
}

BOOST_AUTO_TEST_SUITE_END()



