/*
 * thread_data_type.h
 *
 *  Created on: 2016-7-4
 *      Author: root
 */

#ifndef BINGO_THREAD_THREAD_DATA_TYPE_HEADER_H_
#define BINGO_THREAD_THREAD_DATA_TYPE_HEADER_H_

#include "bingo/type.h"
#include "bingo/mem_guard.h"

namespace bingo { namespace thread {

enum{
	THREAD_DATA_TYPE_DATA				= 0x00,
	THREAD_DATA_TYPE_EXIT_THREAD		= 0x01,
};

template<typename MESSAGE>
struct task_message_data {
	u8_t type;												// Type of task data
	mem_guard<MESSAGE> data;			// Task message

	task_message_data():type(THREAD_DATA_TYPE_DATA){}
	task_message_data(u8_t t):type(t){}
};

#pragma pack(1)
struct task_data_placeholder{
	char message[1];
};
#pragma pack()


struct task_exit_data : public task_message_data<task_data_placeholder> {
	task_exit_data():task_message_data<task_data_placeholder>(THREAD_DATA_TYPE_EXIT_THREAD){}
};

} }

#endif /* BINGO_THREAD_THREAD_DATA_TYPE_HEADER_H_ */
