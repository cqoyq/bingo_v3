/*
 * server_t1.cpp
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#include <boost/test/unit_test.hpp>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::asio::ip;

#include "../../bingo/TCP/tcp_server.h"
#include "../../bingo/TCP/tcp_svr_handler.h"
#include "../../bingo/TCP/tcp_svr_hdr_manager.h"
using namespace bingo::TCP;
#include "../../bingo/type.h"
#include "../../bingo/string.h"
#include "../../bingo/singleton.h"
using namespace bingo;

// Test that the four client connect success. the server will send message to every client 10 times,
// and then the server disconnect.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t6)

// ----------------------------- TCP_MESSAGE_PACKAGE ------------------------------ //
#pragma pack(1)
struct my_package{
	char data[256];
};
#pragma pack()


// ----------------------------- PARSER ------------------------------ //
struct my_parse{
	static const size_t header_size;								// Parse size of package's header.
	static int max_wait_for_heartjump_seconds;						// If the value is 0, then server don't check heartjump.
	static int max_wait_for_authentication_pass_seconds;			// If the value is 0, then server don't check authentication pass.


};
const size_t my_parse::header_size = 4;
int my_parse::max_wait_for_heartjump_seconds = 0;
int my_parse::max_wait_for_authentication_pass_seconds = 0;

// ----------------------------- SOCKET_MANAGER ------------------------------ //
class my_handler;
typedef bingo::singleton_v0<tcp_svr_hdr_manager<my_handler> > my_mgr;

// ----------------------------- HANDLER ------------------------------ //
//typedef tcp_svr_handler<my_parse,			// Define static header_size
//	 	 				my_package			// Message
//	 	 	> my_handler;
class my_handler : public tcp_svr_handler<my_parse, my_package>{
public:
	my_handler(boost::asio::io_service& io_service) :
		tcp_svr_handler<my_parse,my_package>(io_service){

	}

	int read_pk_header_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				size_t& remain_size,
				error_what& e_what){

//		string_ex t;
//		cout << "hdr:" << p.get() << ",do read_pk_header_complete_func(), header data:" <<
//					t.stream_to_string(rev_data, rev_data_size) << endl;

		remain_size = 5 + 2 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				error_what& e_what){

//		string_ex t;
//		cout << "hdr:" << p.get() << ",do read_pk_full_complete_func(), data:" <<
//				t.stream_to_string(rev_data, rev_data_size) << endl;

		return 0;
	}


	void write_pk_full_complete_func(
				pointer p,
				char*& snd_p,
				size_t& snd_size,
				const boost::system::error_code& ec){
		string_ex t;
		message_out_with_thread( ",hdr:" << p.get() << ",do write_pk_full_complete_func(), data:" <<
				t.stream_to_string(snd_p, snd_size) );
	}



	void catch_error_func(my_handler::pointer p, error_what& e_what){
		message_out_with_thread( ",hdr:" << p.get() << ",err_code:" << e_what.err_no() << ",do catch_error()" );
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
//		u8_t err_code = 0;
//		if(my_mgr::instance()->erase(p.get(), err_code) == 0){
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success" << endl;
//		}else{
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << err_code << endl;
//		}
	}
};





// ----------------------------- SERVER ------------------------------ //

vector<void*> handler_pointers;

class my_server : public tcp_server<my_handler, my_mgr, my_parse>{
public:
	my_server(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
		tcp_server<my_handler, my_mgr, my_parse>(io_service, ipv4, port){

	}

	int accept_success_func(my_server::pointer ptr, error_what& e_what){

		my_mgr::instance()->push(ptr.get());
		message_out_with_thread( ",hdr:" << ptr.get() << ",do accept_success_insert_handler_mgr()" )
		handler_pointers.push_back(ptr.get());

		return 0;
	}
};
//typedef tcp_server<my_handler, my_mgr, my_parse> my_server;

void run_thread(int& n){

	this_thread::sleep(seconds(10));

	if(handler_pointers.size() > 0){

		u32_t idx = n -1;
		error_what e_what;


		for (int i = 0; i < 10; ++i) {

			char data[16] = {0x68, 0x01, 0x00, 0x00, 0xd9, 0x8c, 0xee, 0x47, 0x16, 0x01, 0x01, 0x8f, 0x72, 0xd8, 0x0d, 0x16};
			size_t data_size = 16;

			data[1] = (u8_t)n;
			data[3] = (u8_t)i;

//			bingo::string_ex t;
//			cout << "thread data:" << t.stream_to_string(&data[0], data_size) << endl;

			BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[idx], &data[0], data_size, e_what) == 0);
		}

		BOOST_CHECK(my_mgr::instance()->send_close(handler_pointers[idx], e_what) == 0);
	}
}


BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	boost::thread t1(run_thread, 1);
	boost::thread t2(run_thread, 2);
	boost::thread t3(run_thread, 3);
	boost::thread t4(run_thread, 4);

	try{
		 boost::asio::io_service io_service;
		 string ipv4 = "127.0.0.1";
		 u16_t port = 18000;
		 my_server server(io_service, ipv4, port);

		 io_service.run();

		 t1.join();
		 t2.join();
		 t3.join();
		 t4.join();

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	my_mgr::release();

	// ouput:
	//	2017-02-21T16:28:47,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:28:47,thr_id:403eec0,set heartjump time:2017-02-21T16:28:47.328278,hdr:0x8389fb0															-- one socket
	//	2017-02-21T16:28:47,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:47.340331,hdr:0x8389fb0
	//
	//	2017-02-21T16:28:52,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:28:52,thr_id:403eec0,,hdr:0x8389fb0,do accept_success_insert_handler_mgr()
	//	2017-02-21T16:28:52,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:52.430643,hdr:0x8389fb0

	//	2017-02-21T16:28:52,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:28:52,thr_id:403eec0,set heartjump time:2017-02-21T16:28:52.553469,hdr:0x838de10															-- two socket
	//	2017-02-21T16:28:52,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:52.554913,hdr:0x838de10

	//	2017-02-21T16:28:52,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:28:52,thr_id:403eec0,,hdr:0x838de10,do accept_success_insert_handler_mgr()
	//	2017-02-21T16:28:52,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:52.600153,hdr:0x838de10

	//	2017-02-21T16:28:52,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:28:52,thr_id:403eec0,set heartjump time:2017-02-21T16:28:52.603072,hdr:0x8391c00															-- three socket
	//	2017-02-21T16:28:52,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:52.604840,hdr:0x8391c00

	//	2017-02-21T16:28:52,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:28:52,thr_id:403eec0,,hdr:0x8391c00,do accept_success_insert_handler_mgr()
	//	2017-02-21T16:28:52,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:52.619227,hdr:0x8391c00

	//	2017-02-21T16:28:52,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:28:52,thr_id:403eec0,set heartjump time:2017-02-21T16:28:52.621648,hdr:0x8395a10															-- four socket
	//	2017-02-21T16:28:52,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:52.622850,hdr:0x8395a10

	//	2017-02-21T16:28:52,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:28:52,thr_id:403eec0,,hdr:0x8395a10,do accept_success_insert_handler_mgr()
	//	2017-02-21T16:28:52,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:52.627360,hdr:0x8395a10

	//	2017-02-21T16:28:52,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:28:52,thr_id:403eec0,set heartjump time:2017-02-21T16:28:52.631362,hdr:0x8399760
	//	2017-02-21T16:28:52,thr_id:403eec0,set authentication pass time:2017-02-21T16:28:52.632696,hdr:0x8399760
	//
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,err_code:9,do catch_error()				// error_no : ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_SERVER
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 01 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 02 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 03 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 04 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 06 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 07 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 08 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x838de10,do write_pk_full_complete_func(), data:68 02 00 09 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,err_code:9,do catch_error()
	//	2017-02-21T16:28:57,thr_id:403eec0,handler:0x838de10,destory!
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,err_code:9,do catch_error()
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,err_code:9,do catch_error()
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 01 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 02 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 03 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 04 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 06 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 07 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 08 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8391c00,do write_pk_full_complete_func(), data:68 03 00 09 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,handler:0x8391c00,destory!
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 01 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 02 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 03 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 04 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 06 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 07 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 08 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8389fb0,do write_pk_full_complete_func(), data:68 01 00 09 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,handler:0x8389fb0,destory!
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 01 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 02 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 03 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 04 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 06 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 07 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 08 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,,hdr:0x8395a10,do write_pk_full_complete_func(), data:68 04 00 09 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	2017-02-21T16:28:57,thr_id:403eec0,handler:0x8395a10,destory!


}

void run_client(int& n){

	try
	{
		cout << "n:" << n << ",client start." << endl;

		io_service ios;
		ip::tcp::socket sock(ios);						//创建socket对象

		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 18000);	//创建连接端点


		sock.connect(ep);								//socket连接到端点

		cout << "n:" << n << ",client msg : remote_endpoint : " << sock.remote_endpoint().address() << endl;	//输出远程连接端点信息

		char data[16] = {0x68, 0x33, 0x00, 0x05, 0xd9, 0x8c, 0xee, 0x47, 0x16, 0x01, 0x01, 0x8f, 0x72, 0xd8, 0x0d, 0x16};

		sock.write_some(buffer(&data[0], 16)); //发送数据

		while(true){
			vector<char> str(256, 0);						//定义一个vector缓冲区
			size_t received_size = sock.read_some(buffer(str));					//使用buffer()包装缓冲区数据

			bingo::string_ex tool;
			cout << "n:" << n << ",receviced data:" << tool.stream_to_string(&str[0], received_size) << endl;

	//		this_thread::sleep(seconds(10));
		}

		sock.close();

	}
	catch(std::exception& e)							//捕获错误
	{
		cout << "n:" << n << ",exception:" << e.what() << endl;
	}

}

BOOST_AUTO_TEST_CASE(t_client){
	boost::thread t1(run_client, 1);
	boost::thread t2(run_client, 2);
	boost::thread t3(run_client, 3);
	boost::thread t4(run_client, 4);

	 t1.join();
	 t2.join();
	 t3.join();
	 t4.join();

	 // output
	//	 n:2,client start.
	//	 n:3,client start.
	//	 n:1,client start.
	//	 n:1,client msg : remote_endpoint : 127.0.0.1
	//	 n:3,client msg : remote_endpoint : 127.0.0.1
	//	 n:4,client start.
	//	 n:4,client msg : remote_endpoint : 127.0.0.1
	//	 n:2,client msg : remote_endpoint : 127.0.0.1
	//	 n:3,receviced data:68 02 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	 n:3,receviced data:68 02 00 01 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 02 00 02 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 02 00 03 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 02 00 04 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 02 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 02 00 06 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 02 00 07 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 02 00 08 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 02 00 09 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	 n:2,receviced data:68 01 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 01 00 01 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	 n:2,receviced data:68 01 00 02 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 01 00 03 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 01 00 04 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 01 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 01 00 06 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 01 00 07 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 01 00 08 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 01 00 09 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	 n:2,exception:read_some: End of file
	//	 n:3,exception:read_some: End of file
	//	 n:4,receviced data:68 04 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 04 00 01 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 04 00 02 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 04 00 03 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 04 00 04 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 04 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 04 00 06 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 04 00 07 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	 n:4,receviced data:68 04 00 08 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 04 00 09 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	 n:4,exception:read_some: End of file
	//	 n:1,receviced data:68 03 00 00 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 01 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 02 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 03 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 04 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 05 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 06 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 07 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 08 d9 8c ee 47 16 01 01 8f 72 d8 0d 16 68 03 00 09 d9 8c ee 47 16 01 01 8f 72 d8 0d 16
	//	 n:1,exception:read_some: End of file

}

BOOST_AUTO_TEST_SUITE_END()

