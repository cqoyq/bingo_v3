/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tcp_ssl_server.h
 * Author: root
 *
 * Created on 2017年7月17日, 上午11:38
 */

#ifndef TCP_SSL_SERVER_H
#define TCP_SSL_SERVER_H


#include "bingo/type.h"
#include "bingo/define.h"
#include "bingo/error_what.h"

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ssl.hpp>
using namespace boost;
using namespace boost::asio;


namespace bingo {
      namespace TCP {

            template<typename HANDLER, typename SOCKET_MANAGER, typename PARSER>
            class tcp_ssl_server {
            public:
                  typedef HANDLER* pointer;

                  tcp_ssl_server(boost::asio::io_service& io_service, string& ipv4, u16_t& port) :
                  ios_(io_service),
                  heartjump_timer_(io_service),
                  authentication_timer_(io_service),
                  acceptor_(io_service,
                  ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(ipv4), port)),
                  context_(boost::asio::ssl::context::sslv23) {
                        context_.set_options(
                                boost::asio::ssl::context::default_workarounds
                                | boost::asio::ssl::context::no_sslv2
                                | boost::asio::ssl::context::single_dh_use);
                        context_.set_password_callback(boost::bind(&tcp_ssl_server::get_password, this));
                        context_.use_certificate_chain(buffer(PARSER::certificate_chain_buf.c_str(), PARSER::certificate_chain_buf.length()));
                        context_.use_private_key(buffer(PARSER::private_key_buf.c_str(), PARSER::private_key_buf.length()), boost::asio::ssl::context::pem);
                        context_.use_tmp_dh(buffer(PARSER::tmp_dh_buf.c_str(), PARSER::tmp_dh_buf.length()));

                        start_accept();

                        // Start to inspect heartjump
                        check_heartjump();

                        // Start to inspect authentication pass
                        check_authentication();
                  }

                  void start_accept() {
                        // Make new tcp_svr_handler object.
                        pointer new_handler = new HANDLER(ios_, context_);

                        // Start to wait for connect.
                        acceptor_.async_accept(new_handler->socket(),
                                boost::bind(&tcp_ssl_server::accept_handler, this, new_handler,
                                boost::asio::placeholders::error));
                  }

            protected:

                  std::string get_password() const {
                        return PARSER::password;
                  }
                  // Accept connect callback.

                  void accept_handler(pointer new_handler,
                          const boost::system::error_code& ec) {

                        if (!ec) {

#ifdef BINGO_TCP_SERVER_DEBUG
                              message_out_with_thread("call accept_handler()")
#endif

                              // Call accept_handle_success_func()
                              error_what e_what;
                              if (accept_success_func(new_handler, e_what) == 0)
                                    // Start to aync-read.
                                    new_handler->start();
                              else {

                                    new_handler->catch_error(e_what);

                                    // Active close socket.
                                    new_handler->close_socket();
                              }


                        } else {
                              // Call accept_error_func()
                              accept_error_func(ec);

                              delete new_handler;
                        }

                        // Start another one.
                        start_accept();
                  }

                  // Check authentication timer.

                  void check_authentication() {
                        if (PARSER::max_wait_for_authentication_pass_seconds > 0) {
                              boost::posix_time::seconds s(PARSER::max_wait_for_authentication_pass_seconds);
                              boost::posix_time::time_duration td = s;
                              authentication_timer_.expires_from_now(td);
                              authentication_timer_.async_wait(bind(&tcp_ssl_server::authentication_handler,
                                      this,
                                      boost::asio::placeholders::error));
                        }
                  }

                  // Check authentication timer callback.

                  void authentication_handler(const system::error_code& ec) {
                        if (!ec) {
#ifdef BINGO_TCP_SERVER_DEBUG
                              message_out_with_thread("start: check_authentication_pass")
#endif
                                      SOCKET_MANAGER::instance()->check_authentication_pass();
                              check_authentication();
                        }
                  }

                  // Check heartjump timer.

                  void check_heartjump() {
                        if (PARSER::max_wait_for_heartjump_seconds > 0) {
                              boost::posix_time::seconds s(PARSER::max_wait_for_heartjump_seconds);
                              boost::posix_time::time_duration td = s;
                              heartjump_timer_.expires_from_now(td);
                              heartjump_timer_.async_wait(bind(&tcp_ssl_server::heartjump_handler,
                                      this,
                                      boost::asio::placeholders::error));
                        }
                  }

                  // Check heartjump timer callback.

                  void heartjump_handler(const system::error_code& ec) {
                        if (!ec) {
#ifdef BINGO_TCP_SERVER_DEBUG
                              message_out_with_thread("start: check_heartjump")
#endif
                                      SOCKET_MANAGER::instance()->check_heartjump();
                              check_heartjump();
                        }
                  }
            public:
                  // These method will been called after inner handle.

                  virtual int accept_success_func(pointer /*ptr*/, error_what& /*e_what*/) {
                        return 0;
                  }

                  virtual void accept_error_func(const system::error_code& /*ec*/) {
                  }

            protected:
                  boost::asio::ssl::context context_;

                  boost::asio::io_service& ios_;
                  ip::tcp::acceptor acceptor_;

                  deadline_timer heartjump_timer_;
                  deadline_timer authentication_timer_;
            };
      }
}

#endif /* TCP_SSL_SERVER_H */

