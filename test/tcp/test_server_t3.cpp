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
using namespace boost::posix_time;
using namespace boost::asio::ip;

#include "../../bingo/TCP/tcp_server.h"
#include "../../bingo/TCP/tcp_svr_handler.h"
#include "../../bingo/TCP/tcp_svr_hdr_manager.h"
using namespace bingo::TCP;
#include "../../bingo/type.h"
#include "../../bingo/string.h"
#include "../../bingo/singleton.h"
using namespace bingo;

// Test that the client connect success, the server disconnect after 15 seconds.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t3)

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

	void catch_error_func(my_handler::pointer p, error_what& e_what){
		ptime p1 = second_clock::local_time();
		cout << to_iso_extended_string(p1) << ",hdr:" << p.get() << ",err_code:" << e_what.err_no() << ",do catch_error()" << endl;
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		ptime p1 = second_clock::local_time();
		error_what e_what;
		if(my_mgr::instance()->erase(p.get(), e_what) == 0){
			cout << to_iso_extended_string(p1) << ",hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),success" << endl;
		}else{
			cout << to_iso_extended_string(p1) << ",hdr:" << p.get() << ",do close_completed_erase_hander_mgr(),err_code:" << e_what.err_no() << endl;
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
		ptime p1 = second_clock::local_time();
		my_mgr::instance()->push(ptr.get());
		cout <<  to_iso_extended_string(p1)  << ",hdr:" << ptr.get() << ",do accept_success_insert_handler_mgr()" << endl;
		handler_pointers.push_back(ptr.get());

		return 0;
	}
};
//typedef tcp_server<my_handler, my_mgr, my_parse> my_server;



void run_thread(){

	this_thread::sleep(seconds(15));

	if(handler_pointers.size() > 0){
		error_what e_what;
		BOOST_CHECK(my_mgr::instance()->send_close(handler_pointers[0], e_what) == 0);
	}
};

BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	boost::thread t(run_thread);

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
	//	2017-02-21T16:11:14,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:11:15,thr_id:403eec0,set heartjump time:2017-02-21T16:11:15.043352,hdr:0x8386950
	//	2017-02-21T16:11:15,thr_id:403eec0,set authentication pass time:2017-02-21T16:11:15.053615,hdr:0x8386950
	//
	//	2017-02-21T16:11:24,thr_id:403eec0,call accept_handler()
	//	2017-02-21T16:11:24,hdr:0x8386950,do accept_success_insert_handler_mgr()
	//	2017-02-21T16:11:24,thr_id:403eec0,set authentication pass time:2017-02-21T16:11:24.962534,hdr:0x8386950
	//	2017-02-21T16:11:25,thr_id:403eec0,call start_accept()
	//	2017-02-21T16:11:25,thr_id:403eec0,set heartjump time:2017-02-21T16:11:25.120331,hdr:0x838a7b0
	//	2017-02-21T16:11:25,thr_id:403eec0,set authentication pass time:2017-02-21T16:11:25.122672,hdr:0x838a7b0
	//
	//	2017-02-21T16:11:29,hdr:0x8386950,err_code:9,do catch_error()        // error_no: ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_SERVER
	//	2017-02-21T16:11:30,hdr:0x8386950,do close_completed_erase_hander_mgr(),success
	//	2017-02-21T16:11:30,thr_id:403eec0,handler:0x8386950,destory!


}

BOOST_AUTO_TEST_CASE(t_client){
	try
	{
		cout << "client start." << endl;

		io_service ios;
		ip::tcp::socket sock(ios);						//创建socket对象

		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 18000);	//创建连接端点


		sock.connect(ep);								//socket连接到端点

		cout << "client msg : remote_endpoint : " << sock.remote_endpoint().address() << endl;	//输出远程连接端点信息

		while(true){
			vector<char> str(256, 0);						//定义一个vector缓冲区
			size_t received_size = sock.read_some(buffer(str));					//使用buffer()包装缓冲区数据

			bingo::string_ex tool;
			cout << "receviced data:" << tool.stream_to_string(&str[0], received_size) << endl;


		}

		sock.close();

		// output:
		//		client start.
		//		client msg : remote_endpoint : 127.0.0.1


	}
	catch(std::exception& e)							//捕获错误
	{
		cout << "exception:" << e.what() << endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()

