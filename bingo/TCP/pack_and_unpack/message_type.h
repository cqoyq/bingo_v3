/*
 * message_type.h
 *
 *  Created on: 2016-9-12
 *      Author: root
 */

#ifndef  BINGO_TCP_PACK_AND_UNPACK_MESSAGE_TYPE_H_
#define BINGO_TCP_PACK_AND_UNPACK_MESSAGE_TYPE_H_

#include <string>
using namespace std;

namespace bingo { namespace TCP { namespace pack_and_unpack {

// Message type enumerator.
#ifdef BINGO_SART_MESSAGE_TYPE
enum message_type {
	 	// Account message
		MESSAGE_TYPE_ACCOUNT_CREATE_NEW				= 0x01,
		MESSAGE_TYPE_ACCOUNT_MODIFY_PWD				= 0x02,
		// Report message
		MESSAGE_TYPE_REPORT_CREATE_SLIDE				= 0x20,		// 32
		MESSAGE_TYPE_REPORT_CREATE_SLIDE_CONTROL			= 0x21,		// 33
		MESSAGE_TYPE_REPORT_CREATE_SLIDE_TABLE_CONTROL		= 0x22,		// 34
		MESSAGE_TYPE_REPORT_MANAGE_DATA_VISITOR			= 0x23,		// 35
		MESSAGE_TYPE_REPORT_MANAGE_DATA_COMMAND			= 0x24,		// 36
		MESSAGE_TYPE_REPORT_MANAGE_DATA_INPUT_PARAMETER		= 0x25,		// 37
		MESSAGE_TYPE_REPORT_MANAGE_DATA_OUTPUT_PARAMETER                               = 0x26,		// 38
		MESSAGE_TYPE_REPORT_CREATE_TEMPLETE				= 0x27,		// 39
		MESSAGE_TYPE_REPORT_ERASE_TEMPLETE				= 0x28,		// 40
		MESSAGE_TYPE_REPORT_CREATE_REPORT				= 0x29,		// 41
		MESSAGE_TYPE_REPORT_UPLOAD_PIC					= 0x2A,		// 42
		MESSAGE_TYPE_REPORT_MANAGER_VARIABLE				= 0x2B,		// 43
		MESSAGE_TYPE_REPORT_GET_DATA					= 0x2C,		// 44
		MESSAGE_TYPE_REPORT_GET_PIC					= 0x2D,		// 45
		MESSAGE_TYPE_REPORT_MANAGE_DATA_VISITOR_ARG			= 0x2E,		// 46
		MESSAGE_TYPE_REPORT_EXECUTE_VISITOR				= 0x2F,		// 47
		MESSAGE_TYPE_REPORT_RUN_VISITOR				= 0x30,		// 48
};
#endif
} } }

#endif /* BINGO_TCP_PACK_AND_UNPACK_MESSAGE_TYPE_H_ */