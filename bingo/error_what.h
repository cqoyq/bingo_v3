/*
 * error_what.h
 *
 *  Created on: 2016-9-16
 *      Author: root
 */

#ifndef BINGO_ERROR_WHAT_H_
#define BINGO_ERROR_WHAT_H_

#include <string>
using namespace std;

namespace bingo {

enum {
	// -------------------------------- tcp-package ------------------------------ //
	ERROR_TYPE_TCP_PACKAGE_REV_MESSAGE_EXCEED_MAX_SIZE			= 0x01,
	ERROR_TYPE_TCP_PACKAGE_QUEUE_IS_EMPTY					= 0x02,
	ERROR_TYPE_TCP_PACKAGE_HEADER_IS_ERROR				=0x03,
	ERROR_TYPE_TCP_PACKAGE_BODY_IS_ERROR					= 0x04,
	ERROR_TYPE_TCP_PACKAGE_SND_MESSAGE_EXCEED_MAX_SIZE			= 0x05,

	// -------------------------------- tcp-server ------------------------------ //
	ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_CLIENT				= 0x06,
	ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_HEARTBEAT			= 0x07,
	ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_AUTHENTICATION			= 0x08,
	ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_SERVER				= 0x09,

	// -------------------------------- tcp-client ------------------------------ //
	ERROR_TYPE_TCP_CLIENT_CLOSE_SOCKET_BY_SERVER				= 0x0A,
	ERROR_TYPE_TCP_CLIENT_CLOSE_SOCKET_BY_CLIENT				= 0x0B,
	ERROR_TYPE_TCP_CLIENT_CLOSE_SOCKET_BY_FIRST_PACKAGE_ERROR		= 0x0C,

	// -------------------------------- tcp-handler-manager ------------------------------ //
	ERROR_TYPE_HANDLER_MANAGER_QUEUE_IS_FULL   		 		= 0x10,
	ERROR_TYPE_HANDLER_NO_EXIST						= 0x11,

	// -------------------------------- tcp-configuration ------------------------------ //
	ERROR_TYPE_TCP_READ_CFG_FAIL						= 0x12,
	ERROR_TYPE_TCP_CHECK_CFG_NODE_FAIL					= 0x13,

	// -------------------------------- database ------------------------------ //
	ERROR_TYPE_DATABASE_CONNECT_FAIL					= 0x20,
	ERROR_TYPE_DATABASE_QUERY_FAIL						= 0x21,
	ERROR_TYPE_DATABASE_READ_CFG_FAIL					= 0x22,
	ERROR_TYPE_DATABASE_CHECK_CFG_NODE_FAIL				= 0x23,
	ERROR_TYPE_DATABASE_CFG_NODE_NO_EXIST					= 0x24,
	ERROR_TYPE_DATABASE_POOL_IS_EMPTY					= 0x25,
	ERROR_TYPE_DATABASE_FACTORY_NO_INSTANCE				= 0x26,

	// -------------------------------- process ------------------------------ //
	ERROR_TYPE_PROCESS_TASK_SEND_DATA_FAIL					= 0x30,
	ERROR_TYPE_PROCESS_TASK_RECEIVE_DATA_FAIL				= 0x31,

	// -------------------------------- thread ------------------------------ //
	ERROR_TYPE_THREAD_SVC_IS_EXITED						= 0x38,

	// -------------------------------- configuration ------------------------------ //
	ERROR_TYPE_CONFIG_READ_FILE_FAIL					= 0x40,
	ERROR_TYPE_CONFIG_READ_STREAM_FAIL					= 0x41,
	ERROR_TYPE_CONFIG_GET_VALUE_FAIL					= 0x42,
	ERROR_TYPE_CONFIG_GET_NODE_FAIL					= 0x43,
	ERROR_TYPE_CONFIG_GET_ATTRIBUTE_FAIL					= 0x44,

	// -------------------------------- logger ------------------------------ //
	ERROR_TYPE_LOG_READ_CFG_FAIL						= 0x50,
	ERROR_TYPE_LOG_CHECK_CFG_NODE_FAIL					= 0x51,
	ERROR_TYPE_LOG_LOCAL_LOG_TYPE_NO_EXIST				= 0x52,

	// -------------------------------- rabbitmq ------------------------------ //
	ERROR_TYPE_RABBITMQ_SENDOR_READ_CFG_FAIL				= 0x60,
	ERROR_TYPE_RABBITMQ_SENDOR_CHECK_NODE_FAIL				= 0x61,
	ERROR_TYPE_RABBITMQ_RECEIVER_READ_CFG_FAIL				= 0x62,
	ERROR_TYPE_RABBITMQ_RECEIVER_CHECK_NODE_FAIL				= 0x63,
	ERROR_TYPE_RABBITMQ_MAKE_SINGLE_SENDOR_FAIL				= 0x64,
	ERROR_TYPE_RABBITMQ_MAKE_SINGLE_RECEIVER_FAIL				= 0x65,
                     ERROR_TYPE_RABBITMQ_MAKE_WORK_SENDOR_FAIL				= 0x66,
	ERROR_TYPE_RABBITMQ_MAKE_WORK_RECEIVER_FAIL				= 0x67,

	// -------------------------------- mem_guard ------------------------------ //
	ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_COPY			= 0x70,
	ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_APPEND		= 0x71,
	ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_CHANGE		= 0x72,

};

// Bingo error information class.
class error_what {
public:
	error_what();
	virtual ~error_what();

	string& err_message();

	void err_message(const char* err);

	int err_no();

	void err_no(int n);

	void set_error(int err_no, const char* err_msg);

	void clone(error_what& obj);

private:
	string err_;
	int err_no_;
};

}


#endif /* BINGO_ERROR_WHAT_H_ */
