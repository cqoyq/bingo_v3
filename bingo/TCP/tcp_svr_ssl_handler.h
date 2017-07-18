/*
 * tcp_svr_ssl_handler.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_TCP_SVR_SSL_HANDLER_HEADER_H_
#define BINGO_TCP_SVR_SSL_HANDLER_HEADER_H_

#include "bingo/type.h"
#include "bingo/define.h"
#include "bingo/mem_guard.h"
#include "bingo/error_what.h"

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ssl.hpp>
using namespace boost;
using namespace boost::asio;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

namespace bingo {
      namespace TCP {

            template<typename PARSER, typename TCP_MESSAGE_PACKAGE>
            class tcp_svr_ssl_handler {
            public:

                  typedef mem_guard<TCP_MESSAGE_PACKAGE> package;
                  typedef tcp_svr_ssl_handler* pointer;
                  typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

                  // Construct and destruct

                  tcp_svr_ssl_handler(boost::asio::io_service& io_service, boost::asio::ssl::context& context) :
                  ios_(io_service),
                  socket_(io_service, context),
                  is_valid_(true),
                  is_authentication_pass_(false),
                  package_size_(sizeof (TCP_MESSAGE_PACKAGE)) {

                        set_heartjump_datetime();
                        set_active_heartjump_datetime();
                        set_authentication_pass_datetime();
                  }

                  virtual ~tcp_svr_ssl_handler() {
#ifdef BINGO_TCP_SERVER_DEBUG
                        message_out_with_thread("handler:" << this << ",destory!");
#endif
                  }

                  // Get scoket

                  ssl_socket::lowest_layer_type& socket() {
                        return socket_.lowest_layer();
                  }

                  // Start to handle tcp.

                  virtual void start() {

                        // Set active heart-jump start to time.
                        set_active_heartjump_datetime();

                        // Set authentication start to time.
                        set_authentication_pass_datetime();

                        // Start ssl handshake.
                        socket_.async_handshake(boost::asio::ssl::stream_base::server,
                                boost::bind(&tcp_svr_ssl_handler::handle_handshake, this,
                                boost::asio::placeholders::error));


                  }

                  void handle_handshake(const boost::system::error_code& error) {
                        if (!error) {
                              
                              // Start async read data, read finish to call read_handler().
                              boost::asio::async_read(socket_,
                                      boost::asio::buffer(rev_mgr_.current(), PARSER::header_size),
                                      boost::bind(&tcp_svr_ssl_handler::read_handler,
                                      this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred,
                                      PARSER::header_size));
                        } else {
                              // Close socket.
                              if (is_valid_) {
                                    error_what e_what;
                                    e_what.err_message("ssl handshake is fail");
                                    e_what.err_no(ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_SSL);
                                    catch_error(e_what);
                                    //                              close_socket();
                                    close_completed(0);
                              }

                              delete this;

                        }
                  }

                  // Close socket.

                  void close_socket() {
                        //   This function is used to close the socket. Any asynchronous send, receive
                        //   or connect operations will be cancelled immediately, and will complete
                        //   with the boost::asio::error::operation_aborted error.
                        socket().close();
                  }

                  // Catch error infomation.

                  void catch_error(error_what& e_what) {
                        catch_error_func(this, e_what);
                  }





                  // Call the method from tcp_handler_manager.

                  bool check_heartjump_timeout() {
                        if (is_authentication_pass_) {
                              ptime now = boost::posix_time::microsec_clock::local_time();
                              ptime p1 = now - seconds(PARSER::max_wait_for_heartjump_seconds);

#ifdef BINGO_TCP_SERVER_DEBUG
                              message_out_with_thread("check heartjump time, p1_:" << to_iso_extended_string(p1_) <<
                                      ",p1:" << to_iso_extended_string(p1) <<
                                      ",p1_< p1:" << (p1_ < p1) <<
                                      ",hdr:" << this);
#endif

                              if (p1_ < p1) {
                                    return true;
                              } else {
                                    return false;
                              }

                        } else {
                              return false;
                        }

                  }

                  // Call this to update p1_ when detect the heartjump package.

                  void set_heartjump_datetime() {
                        p1_ = boost::posix_time::microsec_clock::local_time();

#ifdef BINGO_TCP_SERVER_DEBUG
                        message_out_with_thread("set heartjump time:" << to_iso_extended_string(p1_) << ",hdr:" << this);
#endif
                  }

                  // Call this to update p3_.

                  void set_active_heartjump_datetime() {
                        p3_ = boost::posix_time::microsec_clock::local_time();
                  }

                  // Call this from tcp_handler_manager.

                  bool check_authentication_pass() {

                        if (!is_authentication_pass_) {
                              // Check whether authentication pass is timeout.
                              ptime now = boost::posix_time::microsec_clock::local_time();
                              ptime p2 = now - seconds(PARSER::max_wait_for_authentication_pass_seconds);

                              if (p2_ < p2)
                                    return false;
                              else
                                    return true;

                        } else {
                              return true;
                        }
                  }

                  // Call this to update is_authentication_pass_ is true, and then update heartjump datetime.
                  // when detect authentication is pass.

                  void set_authentication_pass() {
                        is_authentication_pass_ = true;

                        // Set heart-jump start to time.
                        set_heartjump_datetime();
                  }

                  // Retrival is_authentication_pass_ status.

                  bool get_authentication_pass() {
                        return is_authentication_pass_;
                  }




                  // These method is called in thread. 

                  void send_heartjump_in_thread() {
                        ios_.post(bind(&tcp_svr_ssl_handler::active_send_heartjump, this));
                  }

                  void send_data_in_thread(char*& sdata, size_t& sdata_size) {

                        error_what e_what;
                        package* new_data = 0;

                        // Malloc send buffer.
                        if (malloc_snd_buffer(sdata, sdata_size, new_data, e_what) == 0) {

                              ios_.post(bind(&tcp_svr_ssl_handler::active_send_data, this, new_data));
                        } else {

                              send_close_in_thread(e_what);

                        }


                  }

                  void send_close_in_thread(error_what& e_what) {
                        error_what* p_err = new error_what();
                        e_what.clone(*p_err);
                        ios_.post(bind(&tcp_svr_ssl_handler::active_close, this, p_err));
                  }




                  // Call the method in ios thread. the method is called by send_heartjump_in_thread().

                  void active_send_heartjump() {
                        if (is_valid_) {
                              active_send_heartjump_in_ioservice_func(this);
                        }
                  }

                  // Call the method to active close socket ios thread. 

                  void active_close(error_what*& e_what) {

                        if (is_valid_) {

                              catch_error(*e_what);
                              close_socket();
                              close_completed(0);

                        }

                        delete e_what;
                  }

                  // Call the method in ios thread. the method is called by send_data_in_thread().

                  void active_send_data(package*& pk) {

                        if (is_valid_) {


                              error_what e_what;
                              if (active_send_data_in_ioservice_func(this, pk, e_what) == -1) {

                                    catch_error(e_what);

                                    free_snd_buffer(pk);

                                    return;
                              }

                              boost::asio::async_write(socket_,
                                      buffer(pk->header(), pk->length()),
                                      boost::bind(&tcp_svr_ssl_handler::write_handler,
                                      this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred,
                                      pk));

                        } else {

                              free_snd_buffer(pk);
                        }
                  }


            protected:
                  // Call the method after receive data.

                  void read_handler(const boost::system::error_code& ec,
                          size_t bytes_transferred,
                          size_t bytes_requested) {

                        if (!is_valid_)
                              return;

                        // Check error.
                        if (ec) {
                              close(ec);
                              return;
                        }

                        // Set mblk's reader data length, and move rd_ptr().
                        error_what e_what;
                        if (rev_mgr_.change_length(bytes_transferred, e_what) == -1) {

                              e_what.err_no(ERROR_TYPE_TCP_PACKAGE_REV_MESSAGE_EXCEED_MAX_SIZE);
                              e_what.err_message("tcp receive message is more than max size!");
                              catch_error(e_what);

                              close_socket();
                              close_completed(ec.value());

                              return;
                        }

                        if (bytes_transferred < bytes_requested) {

                              // Continue read remain data.
                              size_t remain = bytes_requested - bytes_transferred;

                              boost::asio::async_read(socket_,
                                      buffer(rev_mgr_.current(), remain),
                                      bind(&tcp_svr_ssl_handler::read_handler,
                                      this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred,
                                      remain));

                        } else if (rev_mgr_.length() == PARSER::header_size) {

                              size_t remain = 0;


                              char* p = rev_mgr_.header();
                              if (read_pk_header_complete_func(
                                      this,
                                      p,
                                      rev_mgr_.length(),
                                      remain,
                                      e_what) == -1) {

                                    // Throw error, and err_code is error_tcp_package_header_is_wrong.
                                    catch_error(e_what);

                                    close_socket();
                                    close_completed(ec.value());

                                    return;
                              }


                              // Check whether remain size is valid.
                              if (!rev_mgr_.check_block_remain_space(remain)) {

                                    e_what.err_no(ERROR_TYPE_TCP_PACKAGE_REV_MESSAGE_EXCEED_MAX_SIZE);
                                    e_what.err_message("tcp receive message is more than max size!");
                                    catch_error(e_what);

                                    close_socket();
                                    close_completed(ec.value());

                                    return;
                              }

                              // Continue read other data.
                              boost::asio::async_read(socket_,
                                      buffer(rev_mgr_.current(), remain),
                                      bind(&tcp_svr_ssl_handler::read_handler,
                                      this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred,
                                      remain));


                        } else {
                              // finish to recevie Message block.

                              char* p = rev_mgr_.header();
                              if (read_pk_full_complete_func(
                                      this,
                                      p,
                                      rev_mgr_.length(),
                                      e_what) == -1) {

                                    //  Throw error, and err_code is error_tcp_package_body_is_wrong.
                                    catch_error(e_what);

                                    close_socket();
                                    close_completed(ec.value());

                                    return;
                              }


                              rev_mgr_.clear();

                              // Start new async read data.
                              boost::asio::async_read(socket_,
                                      boost::asio::buffer(rev_mgr_.current(), PARSER::header_size),
                                      boost::bind(&tcp_svr_ssl_handler::read_handler,
                                      this,
                                      boost::asio::placeholders::error,
                                      boost::asio::placeholders::bytes_transferred,
                                      PARSER::header_size));
                        }
                  }

                  // Call the method after send data.

                  void write_handler(const boost::system::error_code& ec, size_t bytes_transferred, package*& pk) {


                        char* p = pk->header();
                        write_pk_full_complete_func(this, p, bytes_transferred, ec);


                        free_snd_buffer(pk);
                  }




                  // Call the method to close socket in ios thread, the method is called by inner.

                  void close(const boost::system::error_code& ec) {

                        using namespace boost::system::errc;
                        if (ec.value() != operation_canceled) {

                              error_what e_what;
                              e_what.err_no(ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_CLIENT);
                              e_what.err_message("tcp server socket is been closed by client!");

                              catch_error(e_what);

                              // Passive close
                              close_socket();
                        }

                        close_completed(ec.value());
                  }

                  // Call the method to clean something after close socket. 

                  void close_completed(int ec_value) {

                        is_valid_ = false;

                        close_complete_func(this, ec_value);
                  }

                  // Malloc packge space.

                  int malloc_snd_buffer(
                          char*& ori_data,
                          size_t& ori_data_size,
                          package*& pk,
                          error_what& e_what) {

                        if (ori_data_size > package_size_) {
                              e_what.err_no(ERROR_TYPE_TCP_PACKAGE_SND_MESSAGE_EXCEED_MAX_SIZE);
                              e_what.err_message("tcp send message is more than max size!");
                              return -1;
                        }

                        pk = new package();
                        pk->copy(ori_data, ori_data_size, e_what);

                        return 0;
                  }

                  // Free package space.

                  void free_snd_buffer(package*& pk) {

                        delete pk;
                        pk = 0;
                  }



                  // Call this to update p2_.

                  void set_authentication_pass_datetime() {
                        p2_ = boost::posix_time::microsec_clock::local_time();

#ifdef BINGO_TCP_SERVER_DEBUG
                        message_out_with_thread("set authentication pass time:" << to_iso_extended_string(p2_) << ",hdr:" << this);
#endif
                  }

            public:
                  // These method will been called after inner handle.

                  virtual void catch_error_func(pointer /*p*/, error_what& /*e_what*/) {

                  }

                  virtual void close_complete_func(pointer /*p*/, int& /*ec_value*/) {

                  }

                  virtual int read_pk_header_complete_func(pointer /*p*/, char*& /*rev_data*/, size_t& /*rev_data_size*/, size_t& /*remain_size*/, error_what& /*e_what*/) {
                        return 0;
                  }

                  virtual int read_pk_full_complete_func(pointer /*p*/, char*& /*rev_data*/, size_t& /*rev_data_size*/, error_what& /*e_what*/) {
                        return 0;
                  }

                  virtual void write_pk_full_complete_func(pointer /*p*/, char*& /*snd_p*/, size_t& /*snd_size*/, const boost::system::error_code& /*ec*/) {

                  }

                  virtual int active_send_data_in_ioservice_func(pointer /*p*/, package*& /*pk*/, error_what& /*e_what*/) {

                        return 0;
                  }

                  virtual void active_send_heartjump_in_ioservice_func(pointer /*p*/) {
                        return;
                  }

            protected:
                  boost::asio::io_service& ios_;
                  ssl_socket socket_;

                  bool is_valid_;

                  package rev_mgr_;
                  size_t package_size_;

                  ptime p1_; // Save passive heart jump datetime.
                  ptime p3_; // Save active heart jump datetime.

                  ptime p2_; // Save authentication pass datetime.
                  bool is_authentication_pass_; // When authentication pass, this is true, default is false.
            };

      }
}

#endif /* BINGO_TCP_SVR_SSL_HANDLER_HEADER_H_ */
