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

// Test that the client connect success, and the client disconnect.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t2)

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
		cout << "hdr:" << p.get() << ",err_code:" << e_what.err_no() << ",do catch_error()" << endl;
	}

	void close_complete_func(my_handler::pointer p, int& ec_value){
		cout << "hdr:" << p.get() << ",ec_value:" << ec_value << ",do close_complete()" << endl;
	}
};


// ----------------------------- SOCKET_MANAGER ------------------------------ //
typedef bingo::singleton_v0<tcp_svr_hdr_manager<my_handler> > my_mgr;


// ----------------------------- SERVER ------------------------------ //
//class my_server : public tcp_server<my_handler, my_mgr, my_parse>{
//public:
//	my_server(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
//		tcp_server<my_handler, my_mgr, my_parse>(io_service, ipv4, port){
//
//	}
//
//	int accept_success_func(my_server::pointer /*ptr*/, error_what& /*e_what*/){
//		return 0;
//	}
//};
typedef tcp_server<my_handler, my_mgr, my_parse> my_server;


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
	//	2017-02-21T15:56:46,thr_id:403eec0,call start_accept()
	//	2017-02-21T15:56:46,thr_id:403eec0,set heartjump time:2017-02-21T15:56:46.932614,hdr:0x8385920
	//	2017-02-21T15:56:46,thr_id:403eec0,set authentication pass time:2017-02-21T15:56:46.944330,hdr:0x8385920
	//
	//	2017-02-21T15:56:53,thr_id:403eec0,call accept_handler()
	//	2017-02-21T15:56:53,thr_id:403eec0,set authentication pass time:2017-02-21T15:56:53.175607,hdr:0x8385920
	//	2017-02-21T15:56:53,thr_id:403eec0,call start_accept()
	//	2017-02-21T15:56:53,thr_id:403eec0,set heartjump time:2017-02-21T15:56:53.241164,hdr:0x8389080
	//	2017-02-21T15:56:53,thr_id:403eec0,set authentication pass time:2017-02-21T15:56:53.243153,hdr:0x8389080
	//	hdr:0x8385920,err_code:6,do catch_error()					// error_no: ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_CLIENT
	//	hdr:0x8385920,ec_value:2,do close_complete()
	//	2017-02-21T15:56:53,thr_id:403eec0,handler:0x8385920,destory!

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

