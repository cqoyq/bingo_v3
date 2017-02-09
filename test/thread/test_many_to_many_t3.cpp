/*
 * many_to_one_1.cpp
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <iostream>
using namespace std;

#include "../../bingo/string.h"
#include "../../bingo/singleton.h"
#include "../../bingo/thread/all.h"
using namespace bingo::thread;
using namespace bingo;

#include <boost/thread/thread.hpp>
using namespace boost;

BOOST_AUTO_TEST_SUITE(test_thread_many_to_many_t3)

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

void run_input(){

	for (int i = 0; i < 6; ++i) {

		this_thread::sleep(seconds(2));

		my_data_message* msg = new my_data_message();
		error_what e_what;

		msg->type = THREAD_DATA_TYPE_EXIT_THREAD;


		if(my_task::instance()->put(msg, e_what) == -1){			// Input T into queue

			string_ex t;
			message_out_with_thread("put error, err_code:" << e_what.err_no() << ",data:" << t.stream_to_string(&msg->data.object()->message[0], 4))

			delete msg;

		}else{

			string_ex t;
			message_out_with_thread("put success, data:" << t.stream_to_string(&msg->data.object()->message[0], 4))
		}
	}
}

BOOST_AUTO_TEST_CASE(t){

	my_task::construct(
			top,					// thread_task queue top callback
			4						// amount of svc() thread
			);


	boost::thread t1(run_input);
	t1.join();

	this_thread::sleep(seconds(10));	// Must wait for thread exit!

	my_task::release();

	// output
	//	2017-02-21T09:31:48,thr_id:976a700,call svc()!
	//	2017-02-21T09:31:48,thr_id:976a700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:31:48,thr_id:8f69700,call svc()!
	//	2017-02-21T09:31:48,thr_id:8f69700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:31:48,thr_id:9f6b700,call svc()!
	//	2017-02-21T09:31:48,thr_id:9f6b700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:31:48,thr_id:a76c700,call svc()!
	//	2017-02-21T09:31:48,thr_id:a76c700,start cond_get_.wait(mu_)!
	//
	//	2017-02-21T09:31:50,thr_id:af6d700,put success, data:00 00 00 00
	//	2017-02-21T09:31:50,thr_id:976a700,received thread_data_type_exit_thread, and exit!
	//	2017-02-21T09:31:50,thr_id:8f69700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:31:50,thr_id:9f6b700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:31:50,thr_id:a76c700,start cond_get_.wait(mu_)!
	//
	//	2017-02-21T09:31:52,thr_id:af6d700,put success, data:00 00 00 00
	//	2017-02-21T09:31:52,thr_id:8f69700,received thread_data_type_exit_thread, and exit!
	//	2017-02-21T09:31:52,thr_id:9f6b700,start cond_get_.wait(mu_)!
	//	2017-02-21T09:31:52,thr_id:a76c700,start cond_get_.wait(mu_)!
	//
	//	2017-02-21T09:31:54,thr_id:af6d700,put success, data:00 00 00 00
	//	2017-02-21T09:31:54,thr_id:9f6b700,received thread_data_type_exit_thread, and exit!
	//	2017-02-21T09:31:54,thr_id:a76c700,start cond_get_.wait(mu_)!
	//
	//	2017-02-21T09:31:56,thr_id:af6d700,put success, data:00 00 00 00
	//	2017-02-21T09:31:56,thr_id:a76c700,received thread_data_type_exit_thread, and exit!
	//
	//	2017-02-21T09:31:58,thr_id:af6d700,put error, err_code:56,data:00 00 00 00							// error_no:ERROR_TYPE_THREAD_SVC_IS_EXITED
	//	2017-02-21T09:32:00,thr_id:af6d700,put error, err_code:56,data:00 00 00 00
	//
	//	2017-02-21T09:32:10,thr_id:403eec0,all thread has exit,  ~many_to_many()



}

BOOST_AUTO_TEST_SUITE_END()


