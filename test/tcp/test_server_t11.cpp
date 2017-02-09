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

// Test that the client connect to server. the server will send actively heart-jump to client interval 3 seconds.
BOOST_AUTO_TEST_SUITE(test_tcp_server_t11)

// ----------------------------- Heartbeat ------------------------------------------------ //
char heartbeat[6] = {0x68, 0x01, 0x02, 0x03, 0x04, 0x16};

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

	void active_send_heartjump_in_ioservice_func(pointer /*p*/){
		// Update heart-beat timestamp
		ptime p1 = p1_ + seconds(3);
		p1_ = p1;

		// Send heartjump
		error_what e_what;
		package* pk = new package();
		pk->copy(heartbeat, 6, e_what);

		boost::asio::async_write(socket(),
							buffer(pk->header(), pk->length()),
							boost::bind(&my_handler::write_handler,
									this->shared_from_this(),
									boost::asio::placeholders::error,
									boost::asio::placeholders::bytes_transferred,
									pk));
	}

};


// ----------------------------- SOCKET_MANAGER ------------------------------ //
typedef bingo::singleton_v0<tcp_svr_hdr_manager<my_handler> > my_mgr;


// ----------------------------- SERVER ------------------------------ //
class my_server : public tcp_server<my_handler, my_mgr, my_parse>{
public:
	my_server(boost::asio::io_service& io_service, string& ipv4, u16_t& port):
		tcp_server<my_handler, my_mgr, my_parse>(io_service, ipv4, port){

	}

	int accept_success_func(my_server::pointer ptr, error_what& /*e_what*/){
		my_mgr::instance()->push(ptr.get());
		return 0;
	}
};

void run_thread(){

	this_thread::sleep(seconds(10));

	int n = 10;
	for (int i = 0; i < n; ++i) {
		my_mgr::instance()->send_heartjump();
		this_thread::sleep(seconds(3));
	}
}


BOOST_AUTO_TEST_CASE(t_server){

	my_mgr::construct();	 			// Create tcp_handler_manager.

	boost::thread t1(run_thread);		// Start to send heart-beat.

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

	// ouput

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
			message_out_with_thread(",receviced data:" << tool.stream_to_string(&str[0], received_size));


		}

		sock.close();

		// output:
	//		2017-02-25T15:29:33,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:29:36,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:29:39,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:29:42,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:29:45,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:29:48,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:29:51,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:29:54,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:29:57,thr_id:403eec0,,receviced data:68 01 02 03 04 16
	//		2017-02-25T15:30:00,thr_id:403eec0,,receviced data:68 01 02 03 04 16



	}
	catch(std::exception& e)							//捕获错误
	{
		cout << "exception:" << e.what() << endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()

