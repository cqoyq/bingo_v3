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

// Test one sendor and two receiver.
BOOST_AUTO_TEST_SUITE(test_rabbitmq_work)

void run_sendor(rabbitmq_factory*& p){
    this_thread::sleep(seconds(5));
    int index = 1;
    while(index <=100){
        string info = "this is a test, index";
        info.append(lexical_cast<string>(index));
        char* p_info = const_cast<char*>(info.c_str());
        message_out_with_time("send msg:" << info );
    
        string routingkey = "test_work";
        p->transfer_data_by_routingkey(routingkey, p_info, info.length());
    
        this_thread::sleep(seconds(1));
        index++;
    }
   
};

BOOST_AUTO_TEST_CASE(t_sendor){
     rabbitmq_factory factory;
    boost::thread t(run_sendor, &factory);
    
    log_handler* logger = 0;
    factory.make_work_sendor(logger);
    
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
    factory.make_work_receiver(logger, recv);
    
    // output:
    //    receive msg:this is a test, time:2017-03-07T21:54:24
}

BOOST_AUTO_TEST_SUITE_END()



