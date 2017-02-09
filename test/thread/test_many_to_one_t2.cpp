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


BOOST_AUTO_TEST_SUITE(test_thread_many_to_one_t2)

// --------------------------- message ------------------------- //

#pragma pack(1)
struct mypackage{
	char message[256];
};
#pragma pack()

struct thread_task_data_one{

};

struct thread_task_data_type{
	thread_task_data_one* p;
};

typedef task_message_data<mypackage> 	my_data_message;
typedef task_exit_data				    my_exit_message;

// --------------------------- many_to_one ------------------------- //

typedef many_to_one<my_data_message,
					my_exit_message,
					thread_task_data_type
	> my_task_by_thread;
typedef bingo::singleton_v3<my_task_by_thread,
		my_task_by_thread::thr_top_callback,
		my_task_by_thread::thr_init_callback,
		my_task_by_thread::thr_free_callback> my_task;

void init( thread_task_data_type& data){
	data.p = new thread_task_data_one();
}

void top(my_data_message*& msg, thread_task_data_type&){

	string_ex t;
	message_out_with_thread("top data:" << t.stream_to_string(&msg->data.object()->message[0], 4))
}

void free(thread_task_data_type& data){
	delete data.p;
}

void run_input(){

	for (int i = 0; i < 5; ++i) {

		this_thread::sleep(seconds(2));

		my_data_message* msg = new my_data_message();
		error_what e_what;


		if(i == 1)
			msg->type = THREAD_DATA_TYPE_EXIT_THREAD;

		msg->data.object()->message[0] = (u8_t)i;

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

// Test thread normal handle data,
// the case must add 'debug_thread_task=y ConfigName=Test_Debug' when the code is compile.
BOOST_AUTO_TEST_CASE(t){

	my_task::construct(
			top,					// thread_task queue top callback
			init,					// init task data callback
			free					// free task data callback
			);

	boost::thread t1(run_input);
	t1.join();

	this_thread::sleep(seconds(4));

	my_task::release();

	// output
	//	2017-02-18T16:01:26,thr_id:8f69700,call svc()!
	//	2017-02-18T16:01:27,thr_id:8f69700,start cond_get_.wait(mu_)!

	//	2017-02-18T16:01:29,thr_id:976a700,put success, data:00 00 00 00
	//	2017-02-18T16:01:29,thr_id:8f69700,queue pop succes! type:0
	//	2017-02-18T16:01:29,thr_id:8f69700,top data:00 00 00 00
	//	2017-02-18T16:01:29,thr_id:8f69700,start cond_get_.wait(mu_)!

	//	2017-02-18T16:01:31,thr_id:976a700,put success, data:01 00 00 00
	//	2017-02-18T16:01:31,thr_id:8f69700,queue pop succes! type:1
	//	2017-02-18T16:01:31,thr_id:8f69700,received thread_data_type_exit_thread
	//	2017-02-18T16:01:31,thr_id:8f69700,is_thread_exit_ is true in is_empty()

	//	2017-02-18T16:01:33,thr_id:976a700,put error, err_code:56,data:02 00 00 00
	//	2017-02-18T16:01:35,thr_id:976a700,put error, err_code:56,data:03 00 00 00
	//	2017-02-18T16:01:37,thr_id:976a700,put error, err_code:56,data:04 00 00 00


}

BOOST_AUTO_TEST_SUITE_END()


