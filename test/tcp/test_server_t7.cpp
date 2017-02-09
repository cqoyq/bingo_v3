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

// Test that the five client connect to server. the server will allow to connect max of three client,
// more connector will been diconnected.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t7)

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
//		string_ex t;
//		cout << "hdr:" << p.get() << ",do write_pk_full_complete_func(), data:" <<
//				t.stream_to_string(snd_p, snd_size) << endl;
	}



	void catch_error_func(my_handler::pointer p, error_what& e_what){
		message_out_with_thread( ",hdr:" << p.get() << ",err_code:" << e_what.err_no() << ",do catch_error()");
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		error_what e_what;
		if(my_mgr::instance()->erase(p.get(), e_what) == 0){
			message_out_with_thread(",hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success");
		}else{
			message_out_with_thread(",hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << e_what.err_no())
		}
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

		if(my_mgr::instance()->size() == 3){

			message_out_with_thread( ",hdr:" << ptr.get() << ",do accept_success_func() fail");
			e_what.err_no(ERROR_TYPE_HANDLER_MANAGER_QUEUE_IS_FULL);
			e_what.err_message("hdr queue is full!");

			return -1;
		}else{
			my_mgr::instance()->push(ptr.get());
			message_out_with_thread( ",hdr:" << ptr.get() << ",do accept_success_func() success" );
			handler_pointers.push_back(ptr.get());
			return 0;
		}
	}
};
//typedef tcp_server<my_handler, my_mgr, my_parse> my_server;



BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	try{
		 boost::asio::io_service io_service;
		 string ipv4 = "127.0.0.1";
		 u16_t port = 18000;
		 my_server server(io_service, ipv4, port);

		 io_service.run();

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	my_mgr::release();

	// ouput:
	// ---- one socket ----
	//	2017-02-21T16:38:09,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:38:09,thr_id:403eec0,set heartjump time:2017-02-21T16:38:09.247190,hdr:0x83896f0
	//	2017-02-21T16:38:09,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:09.258217,hdr:0x83896f0
	//
	//	2017-02-21T16:38:18,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:38:18,thr_id:403eec0,,hdr:0x83896f0,do accept_success_func() success
	//	2017-02-21T16:38:18,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:18.722576,hdr:0x83896f0

	// ---- two socket ----
	//	2017-02-21T16:38:18,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:38:18,thr_id:403eec0,set heartjump time:2017-02-21T16:38:18.796918,hdr:0x838d550
	//	2017-02-21T16:38:18,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:18.798109,hdr:0x838d550

	//	2017-02-21T16:38:18,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:38:18,thr_id:403eec0,,hdr:0x838d550,do accept_success_func() success
	//	2017-02-21T16:38:18,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:18.848549,hdr:0x838d550

	// ---- three socket ----
	//	2017-02-21T16:38:18,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:38:18,thr_id:403eec0,set heartjump time:2017-02-21T16:38:18.851641,hdr:0x8391340
	//	2017-02-21T16:38:18,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:18.853010,hdr:0x8391340

	//	2017-02-21T16:38:18,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:38:18,thr_id:403eec0,,hdr:0x8391340,do accept_success_func() success
	//	2017-02-21T16:38:18,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:18.862167,hdr:0x8391340

	// ---- four socket ----
	//	2017-02-21T16:38:18,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:38:18,thr_id:403eec0,set heartjump time:2017-02-21T16:38:18.864506,hdr:0x8395150
	//	2017-02-21T16:38:18,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:18.865595,hdr:0x8395150
	//	2017-02-21T16:38:18,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:38:18,thr_id:403eec0,,hdr:0x8395150,do accept_success_func() fail
	//	2017-02-21T16:38:18,thr_id:403eec0,,hdr:0x8395150,err_code:16,do catch_error()					// error_no: ERROR_TYPE_HANDLER_MANAGER_QUEUE_IS_FULL

	// ---- five socket ----
	//	2017-02-21T16:38:18,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:38:18,thr_id:403eec0,set heartjump time:2017-02-21T16:38:18.897044,hdr:0x8398730
	//	2017-02-21T16:38:18,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:18.898295,hdr:0x8398730
	//	2017-02-21T16:38:18,thr_id:403eec0,handler:0x8395150,destory!
	//	2017-02-21T16:38:18,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:38:18,thr_id:403eec0,,hdr:0x8398730,do accept_success_func() fail
	//	2017-02-21T16:38:18,thr_id:403eec0,,hdr:0x8398730,err_code:16,do catch_error()

	//	2017-02-21T16:38:18,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:38:18,thr_id:403eec0,set heartjump time:2017-02-21T16:38:18.921215,hdr:0x839c170
	//	2017-02-21T16:38:18,thr_id:403eec0,set authentication pass time:2017-02-21T16:38:18.923059,hdr:0x839c170
	//	2017-02-21T16:38:18,thr_id:403eec0,handler:0x8398730,destory!




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
	boost::thread t5(run_client, 5);

	 t1.join();
	 t2.join();
	 t3.join();
	 t4.join();
	 t5.join();

	 // output
	//	 n:2,client start.
	//	 n:1,client start.
	//	 n:3,client start.
	//	 n:3,client msg : remote_endpoint : 127.0.0.1
	//	 n:2,client msg : remote_endpoint : 127.0.0.1
	//	 n:1,client msg : remote_endpoint : 127.0.0.1
	//	 n:n:4,client start.
	//	 n:4,client msg : remote_endpoint : 127.0.0.1
	//	 5,client start.
	//	 n:5,client msg : remote_endpoint : 127.0.0.1
	//	 n:4,exception:read_some: Connection reset by peer
	//	 n:5,exception:read_some: Connection reset by peer

}

BOOST_AUTO_TEST_SUITE_END()

