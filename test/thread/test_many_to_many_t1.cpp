/*
 * many_to_one_1.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <iostream>
using namespace std;

#include "../../bingo/singleton.h"
#include "../../bingo/thread/all.h"
using namespace bingo::thread;

#include <boost/thread/thread.hpp>
using namespace boost;

BOOST_AUTO_TEST_SUITE(test_thread_many_to_many_t1)

// --------------------------- message ------------------------- //

#pragma pack(1)
struct mypackage{
	char message[256];
};
#pragma pack()

typedef task_message_data<mypackage> 	my_data_message;
typedef task_exit_data				    my_exit_message;

// --------------------------- many_to_one ------------------------- //

typedef many_to_many<my_data_message,
					my_exit_message
	> my_task_by_thread;
typedef bingo::singleton_v2<my_task_by_thread, my_task_by_thread::thr_top_callback, int> my_task;

void top(my_data_message*& data, bingo_empty_type&){
}

BOOST_AUTO_TEST_CASE(t){

	my_task::construct(
			top,					// thread_task queue top callback
			4						// amount of svc() thread
			);

	this_thread::sleep(seconds(4));

	my_task::release();

	// output
	//	2017-02-21T09:14:22,thr_id:976a700,call svc()!
	//	2017-02-21T09:14:22,thr_id:976a700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:14:22,thr_id:8f69700,call svc()!
	//	2017-02-21T09:14:22,thr_id:8f69700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:14:22,thr_id:a76c700,call svc()!
	//	2017-02-21T09:14:22,thr_id:a76c700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:14:22,thr_id:9f6b700,call svc()!
	//	2017-02-21T09:14:22,thr_id:9f6b700,start cond_get_.wait(mu_)!

	//	2017-02-21T09:14:26,thr_id:403eec0,wait for all thread exit,  ~many_to_many()

	//	2017-02-21T09:14:26,thr_id:976a700,received thread_data_type_exit_thread, and exit!
	//	2017-02-21T09:14:26,thr_id:8f69700,received thread_data_type_exit_thread, and exit!
	//	2017-02-21T09:14:26,thr_id:a76c700,received thread_data_type_exit_thread, and exit!
	//	2017-02-21T09:14:26,thr_id:9f6b700,received thread_data_type_exit_thread, and exit!

	//	2017-02-21T09:14:26,thr_id:403eec0,all thread exit,  ~many_to_many()



}

BOOST_AUTO_TEST_SUITE_END()


