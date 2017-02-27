/*
 * mem_guard.cpp
 *
 *  Created on: 2016-1-30
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <iostream>
using namespace std;

#include "../bingo/string.h"
#include "../bingo/mem_guard.h"
#include "../bingo/error_what.h"
using namespace bingo;


BOOST_AUTO_TEST_SUITE(test_mem_guard)

#pragma pack(1)
struct info{
	bool  is_add;     // size is 1
	u8_t  m[30];
	int   k;              // size is 4
};
#pragma pack()

BOOST_AUTO_TEST_CASE(t){

	cout << "sizeof T:" << sizeof(info) << endl;		// output: 35 bytes

	mem_guard<info>* my = new mem_guard<info>();

	BOOST_CHECK_EQUAL((char*)my, my->header());
	BOOST_CHECK_EQUAL(sizeof(info), my->size());					// my->size() = sizeof(T)
	BOOST_CHECK_EQUAL((char*)my, (char*)my->object());				// my->object() = my->header() = my

	// Check T value change.
	char* header = my->header();
	BOOST_CHECK_EQUAL(*header, 0x00);
	my->object()->is_add = true;
	BOOST_CHECK_EQUAL(*header, 0x01);

	BOOST_CHECK_EQUAL(*(header + 1 + 2), 0x00);
	BOOST_CHECK_EQUAL(*(header + 1 + 3), 0x00);
	my->object()->m[2] = 0x10;
	my->object()->m[3] = 0x20;
	BOOST_CHECK_EQUAL(*(header + 1 + 2), 0x10);
	BOOST_CHECK_EQUAL(*(header + 1 + 3), 0x20);

	my->object()->k = 1234; // 1234 = 0x04D2
	BOOST_CHECK_EQUAL((u8_t)*(header + 1 + 30),	0xD2);
	BOOST_CHECK_EQUAL((u8_t)*(header + 1 + 30 + 1), 	0x04);

	// Check current() and length()
	{
		error_what e_what;

		char a[10] = {0x00};
		memset(a, 0x01, 10);
		BOOST_CHECK_EQUAL(my->append(a, 10, e_what), 0);

		BOOST_CHECK_EQUAL(my->current(), my->header() + 10);
		BOOST_CHECK_EQUAL(my->length(), 10);

		memset(a, 0x02, 10);
		BOOST_CHECK_EQUAL(my->append(a, 10, e_what), 0);

		BOOST_CHECK_EQUAL(my->current(), my->header() + 20);
		BOOST_CHECK_EQUAL(my->length(), 20);

		my->clear();

		char b[35] = {0x00};
		BOOST_CHECK_EQUAL(my->copy(b, 35, e_what), 0);
		BOOST_CHECK(my->current() == 0);							// arrive end of data_
		BOOST_CHECK_EQUAL(my->length(), 35);
	}

	// Check copy()
	{
		my->clear();

		bingo::string_ex t;
		error_what e_what;

		char a[35] = {0x00};
		memset(a, 0xFE, 34);
		BOOST_CHECK_EQUAL(my->copy(a, 35, e_what), 0);
		cout << "call copy():" << t.stream_to_string(my->header(), my->size()) << endl;
		// output
		// call copy() one:fe fe fe fe fe fe fe fe fe fe
		//				   fe fe fe fe fe fe fe fe fe fe
		//                 fe fe fe fe fe fe fe fe fe fe
		//                 fe fe fe fe 00

		char b[50] = {0xFE};
		BOOST_CHECK_EQUAL(my->copy(b, 50, e_what), -1);			// error
		BOOST_CHECK_EQUAL(e_what.err_no(), (int)ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_COPY);
	}

	// Check append()
	{
		my->clear();												// clear() will clear all old data.

		bingo::string_ex t;
		error_what e_what;

		char a[10] = {0x00};
		memset(a, 0x01, 10);
		BOOST_CHECK_EQUAL(my->append(a, 10, e_what), 0);

		memset(a, 0x02, 10);
		BOOST_CHECK_EQUAL(my->append(a, 10, e_what), 0);

		memset(a, 0x03, 10);
		BOOST_CHECK_EQUAL(my->append(a, 10, e_what), 0);

		memset(a, 0x04, 10);
		BOOST_CHECK_EQUAL(my->append(a, 10, e_what), -1);			// error
		BOOST_CHECK_EQUAL(e_what.err_no(), (int)ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_APPEND);

		cout << "call append():" << t.stream_to_string(my->header(), my->size()) << endl;
		// output:
		// call append():01 01 01 01 01 01 01 01 01 01
		//				 02 02 02 02 02 02 02 02 02 02
		//				 03 03 03 03 03 03 03 03 03 03
		//				 00 00 00 00 00
	}


	delete my;

}

BOOST_AUTO_TEST_SUITE_END()

