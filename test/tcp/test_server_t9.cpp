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

// Test that the client connect to server. the server will authenticate client, invalid to disconnect.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t9)

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
			message_out_with_thread( ",hdr:" << p.get() << ",do set_heartjump_datetime()" )
		}else if(rev_data[1] == 0x30){

			// authentication is pass
			p->set_authentication_pass();
			message_out_with_thread( ",hdr:" << p.get() << ",receive package do set_authentication_pass()" )
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
			message_out_with_thread(",hdr:" << p.get() << ",do set_authentication_pass()" )
		}

		return 0;
	}



	void catch_error_func(my_handler::pointer p, error_what& e_what){
		message_out_with_thread( ",hdr:" << p.get() << ",err_code:" << e_what.err_no() << ",do catch_error()" )
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



void run_thread(int& n){

	if(n == 1){

	}else{

		this_thread::sleep(seconds(10));

		char data[5] = {0x68, 0x01, 0x00, 0x00, 0x16};
		size_t data_size = 5;

		error_what e_what;
		if(handler_pointers.size() == 2){
			// Must 3 client connect.
			BOOST_CHECK(my_mgr::instance()->send_data(handler_pointers[1], &data[0], data_size, e_what) == 0);
		}
	}
}

BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

//	boost::thread t1(run_thread, 1);
	boost::thread t2(run_thread, 2);	// Set second socket that authentication is pass.

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
	//	2017-02-22T22:50:14,thr_id:403eec0,call start_accept()
	//	2017-02-22T22:50:14,thr_id:403eec0,set heartjump time:2017-02-22T22:50:14.535939,hdr:0x838b270
	//	2017-02-22T22:50:14,thr_id:403eec0,set authentication pass time:2017-02-22T22:50:14.547641,hdr:0x838b270

	//	2017-02-22T22:50:19,thr_id:403eec0,call start_accept()
	//	2017-02-22T22:50:19,thr_id:403eec0,set heartjump time:2017-02-22T22:50:19.460111,hdr:0x838eba0
	//	2017-02-22T22:50:19,thr_id:403eec0,set authentication pass time:2017-02-22T22:50:19.461343,hdr:0x838eba0

	//	2017-02-22T22:50:19,thr_id:403eec0,call start_accept()
	//	2017-02-22T22:50:19,thr_id:403eec0,set heartjump time:2017-02-22T22:50:19.471400,hdr:0x8392380
	//	2017-02-22T22:50:19,thr_id:403eec0,set authentication pass time:2017-02-22T22:50:19.472421,hdr:0x8392380
	//
	//
	//	2017-02-22T22:50:24,thr_id:403eec0,set heartjump time:2017-02-22T22:50:24.483335,hdr:0x838eba0
	//	2017-02-22T22:50:24,thr_id:403eec0,,hdr:0x838eba0,do set_authentication_pass()			// hdr:0x838eba0 authenticate is pass.

	//	2017-02-22T22:50:29,thr_id:403eec0,start: check_authentication_pass
	//	2017-02-22T22:50:44,thr_id:403eec0,start: check_authentication_pass
	//	2017-02-22T22:50:44,thr_id:403eec0,,hdr:0x838b270,err_code:8,do catch_error()       // hdr:0x838b270 active close socket.
	//	2017-02-22T22:50:44,thr_id:403eec0,handler:0x838b270,destory!

	//	2017-02-22T22:50:59,thr_id:403eec0,start: check_authentication_pass
	//	2017-02-22T22:51:14,thr_id:403eec0,start: check_authentication_pass


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

//		char data[16] = {0x68, 0x33, 0x00, 0x05, 0xd9, 0x8c, 0xee, 0x47, 0x16, 0x01, 0x01, 0x8f, 0x72, 0xd8, 0x0d, 0x16};
//		sock.write_some(buffer(&data[0], 16)); //发送数据

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

	 t1.join();
	 t2.join();


	 // output
	//	 n:2,client start.
	//	 n:2,client msg : remote_endpoint : 127.0.0.1
	//	 n:1,client start.
	//	 n:1,client msg : remote_endpoint : 127.0.0.1
	//	 n:1,receviced data:68 01 00 00 16
	//	 n:2,exception:read_some: End of file

}

BOOST_AUTO_TEST_SUITE_END()

