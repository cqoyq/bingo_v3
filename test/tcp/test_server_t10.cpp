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

// Test that the client connect to server. the server will check heart-jump interval 10 seconds, if the
// heart-jump is timeout, server will close socket.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t10)

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
int my_parse::max_wait_for_heartjump_seconds = 10;
int my_parse::max_wait_for_authentication_pass_seconds = 15;

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

		size_t size = (u8_t)rev_data[3];
		remain_size = size + 2 + 4 + 1;

		return 0;
	}

	int read_pk_full_complete_func(
				my_handler::pointer p,
				char*& rev_data,
				size_t& rev_data_size,
				error_what& e_what){

		if(rev_data[1] == 0x23){
			// heartjump
			p->set_heartjump_datetime();
			cout << "hdr:" << p.get() << ",do set_heartjump_datetime()" << endl;
		}

		return 0;
	}

	int active_send_in_ioservice_func(
			my_handler::pointer p,
			package*& pk,
			error_what& e_what){

		char* snd_p = pk->header();
		if(snd_p[1] == 0x01){
			// authencation is pass
			p->set_authentication_pass();
			cout << "hdr:" << p.get() << ",do set_authentication_pass()" << endl;
		}

		return 0;
	}



	void catch_error_func(my_handler::pointer p, error_what& e_what){
		cout << "hdr:" << p.get() << ",err_code:" << e_what.err_no() << ",do catch_error()" << endl;
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		error_what e_what;
		if(my_mgr::instance()->erase(p.get(), e_what) == 0){
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success" << endl;
		}else{
//			cout << "hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << err_code << endl;
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

		my_mgr::instance()->push(ptr.get());
//		cout << "hdr:" << ptr.get() << ",do accept_success_func() success" << endl;
		handler_pointers.push_back(ptr.get());
		return 0;
	}
};
//typedef tcp_server<my_handler, my_mgr, my_parse> my_server;



void run_thread(){

	this_thread::sleep(seconds(10));

	char data[5] = {0x68, 0x01, 0x00, 0x00, 0x16};
	size_t data_size = 5;

	error_what e_what;
	BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[0], &data[0], data_size, e_what) == 0);
	BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[1], &data[0], data_size, e_what) == 0);
	BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[2], &data[0], data_size, e_what) == 0);

}

BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	boost::thread t1(run_thread);		// Set socket that authentication is pass.

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
	//	2017-02-22T22:59:44,thr_id:403eec0,call start_accept()
	//	2017-02-22T22:59:44,thr_id:403eec0,set heartjump time:2017-02-22T22:59:44.385557,hdr:0x838bda0
	//	2017-02-22T22:59:44,thr_id:403eec0,set authentication pass time:2017-02-22T22:59:44.451665,hdr:0x838bda0
	//
	//	2017-02-22T22:59:48,thr_id:403eec0,call start_accept()
	//	2017-02-22T22:59:48,thr_id:403eec0,set heartjump time:2017-02-22T22:59:48.885749,hdr:0x838f7c0
	//	2017-02-22T22:59:48,thr_id:403eec0,set authentication pass time:2017-02-22T22:59:48.886961,hdr:0x838f7c0

	//	2017-02-22T22:59:50,thr_id:403eec0,call start_accept()
	//	2017-02-22T22:59:50,thr_id:403eec0,set heartjump time:2017-02-22T22:59:50.669215,hdr:0x8392fa0
	//	2017-02-22T22:59:50,thr_id:403eec0,set authentication pass time:2017-02-22T22:59:50.670206,hdr:0x8392fa0
	//
	//	2017-02-22T22:59:52,thr_id:403eec0,call start_accept()
	//	2017-02-22T22:59:52,thr_id:403eec0,set heartjump time:2017-02-22T22:59:52.760752,hdr:0x83967a0
	//	2017-02-22T22:59:52,thr_id:403eec0,set authentication pass time:2017-02-22T22:59:52.761731,hdr:0x83967a0

	//	hdr:0x838bda0,do set_authentication_pass()		// first socket authenticate is pass
	//	hdr:0x838f7c0,do set_authentication_pass()		// second socket authenticate is pass
	//	hdr:0x8392fa0,do set_authentication_pass()		// third socket authenticate is pass

	//	2017-02-22T22:59:54,thr_id:403eec0,start: check_heartjump

	//	2017-02-22T22:59:58,thr_id:403eec0,set heartjump time:2017-02-22T22:59:58.572289,hdr:0x8392fa0
	//	2017-02-22T22:59:59,thr_id:403eec0,start: check_authentication_pass
	//	2017-02-22T23:00:02,thr_id:403eec0,set heartjump time:2017-02-22T23:00:02.571131,hdr:0x838f7c0
	//	2017-02-22T23:00:02,thr_id:403eec0,set heartjump time:2017-02-22T23:00:02.573658,hdr:0x8392fa0

	//	2017-02-22T23:00:04,thr_id:403eec0,start: check_heartjump
	//	2017-02-22T23:00:04,thr_id:403eec0,check heartjump time, p1_:2017-02-22T22:59:54.507265,p1:2017-02-22T23:00:04,p1_< p1:1,hdr:0x838bda0
	//	hdr:0x838bda0,err_code:7,do catch_error()
	//	2017-02-22T23:00:04,thr_id:403eec0,check heartjump time, p1_:2017-02-22T23:00:02.571131,p1:2017-02-22T23:00:04,p1_< p1:1,hdr:0x838f7c0
	//	2017-02-22T23:00:04,thr_id:403eec0,check heartjump time, p1_:2017-02-22T23:00:02.573658,p1:2017-02-22T23:00:04,p1_< p1:1,hdr:0x8392fa0
	//	2017-02-22T23:00:04,thr_id:403eec0,handler:0x838bda0,destory!

	//	2017-02-22T23:00:06,thr_id:403eec0,set heartjump time:2017-02-22T23:00:06.572274,hdr:0x8392fa0
	//	2017-02-22T23:00:06,thr_id:403eec0,set heartjump time:2017-02-22T23:00:06.573990,hdr:0x838f7c0

	//	2017-02-22T23:00:10,thr_id:403eec0,set heartjump time:2017-02-22T23:00:10.574464,hdr:0x8392fa0
	//	2017-02-22T23:00:10,thr_id:403eec0,set heartjump time:2017-02-22T23:00:10.576346,hdr:0x838f7c0

	//	2017-02-22T23:00:14,thr_id:403eec0,start: check_authentication_pass
	//	2017-02-22T23:00:14,thr_id:403eec0,set heartjump time:2017-02-22T23:00:14.573709,hdr:0x8392fa0
	//	2017-02-22T23:00:14,thr_id:403eec0,set heartjump time:2017-02-22T23:00:14.576578,hdr:0x838f7c0

	//	2017-02-22T23:00:14,thr_id:403eec0,start: check_heartjump
	//	2017-02-22T23:00:18,thr_id:403eec0,set heartjump time:2017-02-22T23:00:18.575008,hdr:0x8392fa0
	//	2017-02-22T23:00:18,thr_id:403eec0,set heartjump time:2017-02-22T23:00:18.576701,hdr:0x838f7c0

	//	2017-02-22T23:00:22,thr_id:403eec0,set heartjump time:2017-02-22T23:00:22.576697,hdr:0x8392fa0
	//	2017-02-22T23:00:22,thr_id:403eec0,set heartjump time:2017-02-22T23:00:22.579785,hdr:0x838f7c0

	//	2017-02-22T23:00:24,thr_id:403eec0,start: check_heartjump
	//	2017-02-22T23:00:26,thr_id:403eec0,set heartjump time:2017-02-22T23:00:26.577452,hdr:0x8392fa0
	//	2017-02-22T23:00:26,thr_id:403eec0,set heartjump time:2017-02-22T23:00:26.579121,hdr:0x838f7c0

	//	2017-02-22T23:00:29,thr_id:403eec0,start: check_authentication_pass
	//	2017-02-22T23:00:30,thr_id:403eec0,set heartjump time:2017-02-22T23:00:30.578633,hdr:0x8392fa0
	//	2017-02-22T23:00:30,thr_id:403eec0,set heartjump time:2017-02-22T23:00:30.580282,hdr:0x838f7c0

	//	2017-02-22T23:00:34,thr_id:403eec0,set heartjump time:2017-02-22T23:00:34.579827,hdr:0x838f7c0
	//	2017-02-22T23:00:34,thr_id:403eec0,set heartjump time:2017-02-22T23:00:34.581738,hdr:0x8392fa0

	//	2017-02-22T23:00:34,thr_id:403eec0,start: check_heartjump
	//	2017-02-22T23:00:44,thr_id:403eec0,start: check_authentication_pass
	//	2017-02-22T23:00:44,thr_id:403eec0,start: check_heartjump
	//	2017-02-22T23:00:54,thr_id:403eec0,start: check_heartjump
	//	2017-02-22T23:00:59,thr_id:403eec0,start: check_authentication_pass
	//	2017-02-22T23:01:04,thr_id:403eec0,start: check_heartjump


}

void run_client(int& n){

	if(n == 2)
		this_thread::sleep(seconds(2));
	if(n == 3)
		this_thread::sleep(seconds(4));

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

			if(n != 1)
				while(true){
					char data2[11] = {0x68, 0x23, 0x00, 0x00, 0x01, 0x00, 0x8f, 0x72, 0xd8, 0x0d, 0x16};

					sock.write_some(buffer(&data2[0], 11)); //发送数据

					this_thread::sleep(seconds(4));
				}

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

	t1.join();
	t2.join();
	t3.join();


	// output
	//	 n:1,client start.
	//	 n:1,client msg : remote_endpoint : 127.0.0.1
	//	 n:2,client start.
	//	 n:2,client msg : remote_endpoint : 127.0.0.1
	//	 n:3,client start.
	//	 n:3,client msg : remote_endpoint : 127.0.0.1
	//	 n:1,receviced data:68 01 00 00 16
	//	 n:2,receviced data:68 01 00 00 16
	//	 n:3,receviced data:68 01 00 00 16
	//	 n:1,exception:read_some: End of file      // first socket has closed by server.

}

BOOST_AUTO_TEST_SUITE_END()

