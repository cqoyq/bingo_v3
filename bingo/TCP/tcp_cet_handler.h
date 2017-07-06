/*
 * tcp_cet_handler.h
 *
 *  Created on: 2016-7-1
 *      Author: root
 */

#ifndef BINGO_TCP_CET_HANDLER_HEADER_H_
#define BINGO_TCP_CET_HANDLER_HEADER_H_

#include "bingo/type.h"
#include "bingo/define.h"
#include "bingo/mem_guard.h"
#include "bingo/error_what.h"

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
using namespace boost;
using namespace boost::asio;

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

namespace bingo {
        namespace TCP {

                template< typename PARSER , typename TCP_MESSAGE_PACKAGE>
                class tcp_cet_handler
                : public boost::enable_shared_from_this<tcp_cet_handler<PARSER , TCP_MESSAGE_PACKAGE> > {
                public:
                        typedef boost::shared_ptr<tcp_cet_handler> pointer;
                        typedef mem_guard<TCP_MESSAGE_PACKAGE> package;

                        // Construct and destruct
                        tcp_cet_handler( boost::asio::io_service& io_service ,
                                boost::function<void( ) > f , // reconnect func
                                boost::function<void(bool ) > f1 // authenticate pass func
                                )
                        : ios_( io_service ) ,
                        socket_( io_service ) ,
                        is_valid_( true ) ,
                        package_size_( sizeof (TCP_MESSAGE_PACKAGE ) ) ,
                        f_( f ) ,
                        f1_( f1 ) {
                        }
                        virtual ~tcp_cet_handler( ) {
#ifdef BINGO_TCP_CLIENT_DEBUG
                                message_out_with_thread( "hdr:" << this << ",destory!" );
#endif
                        }
                        ip::tcp::socket& socket( ) {
                                return socket_;
                        }

                        // Start to connect to server.
                        void start( ) {

                                package* pk = new package( );
                                error_what e_what;
                                if (make_first_sended_package_func( pk , e_what ) == 0) {

                                        // If pk have no data, then start to asyc-read.
                                        if (pk->length( ) == 0) {

                                                free_snd_buffer( pk );

                                                // Start async read data, read finish to call read_handler().
                                                boost::asio::async_read( socket_ ,
                                                        boost::asio::buffer( rev_mgr_.current( ) , PARSER::header_size ) ,
                                                        boost::bind( &tcp_cet_handler::read_handler ,
                                                        this->shared_from_this( ) ,
                                                        boost::asio::placeholders::error ,
                                                        boost::asio::placeholders::bytes_transferred ,
                                                        PARSER::header_size ) );
                                        } else {
                                                boost::asio::async_write( socket_ ,
                                                        buffer( pk->header( ) , pk->length( ) ) ,
                                                        boost::bind( &tcp_cet_handler::write_first_handler ,
                                                        this->shared_from_this( ) ,
                                                        boost::asio::placeholders::error ,
                                                        boost::asio::placeholders::bytes_transferred ,
                                                        pk ) );
                                        }

                                } else {

                                        free_snd_buffer( pk );

                                        // Throw exception, err_code is error_tcp_client_close_socket_because_make_first_package.
                                        catch_error( e_what );

                                        close_socket( );
                                        close_completed( 0 );
                                }


                        }

                        // Close socket.
                        void close_socket( ) {
                                // This function is used to close the socket. Any asynchronous send, receive
                                // or connect operations will be cancelled immediately, and will complete
                                // with the boost::asio::error::operation_aborted error.
                                socket_.close( );
                        }

                        // Catch error information.
                        void catch_error( error_what& e_what ) {
                                catch_error_func( this->shared_from_this( ) , e_what );
                        }

                        // These method will been called in thread. 
                        void send_data_in_thread( char*& sdata , size_t& sdata_size ) {

                                error_what e_what;
                                package* new_data = 0;

                                // Malloc send buffer.
                                if (malloc_snd_buffer( sdata , sdata_size , new_data , e_what ) == 0) {

                                        ios_.post( bind( &tcp_cet_handler::active_send , this->shared_from_this( ) , new_data ) );
                                } else {

                                        send_close_in_thread( e_what );
                                }


                        }
                        void send_close_in_thread( error_what& e_what ) {
                                error_what* p_err = new error_what( );
                                e_what.clone( *p_err );
                                ios_.post( bind( &tcp_cet_handler::active_close , this->shared_from_this( ) , p_err ) );
                        }

                        // Set heartjump time.
                        void set_heartjump_datetime( ) {
                                p1_ = boost::posix_time::microsec_clock::local_time( );

                        }

                        // Check whether heartjump is timeout.
                        bool check_heartjump_timeout( ) {

                                ptime now = boost::posix_time::microsec_clock::local_time( );
                                ptime p1 = now - seconds( PARSER::max_interval_seconds_check_heartjump );

                                if (p1_ < p1) {
                                        return true;
                                } else {
                                        return false;
                                }
                        }

                protected:
                        // Set authenticate status.
                        void set_authenticate_pass( bool is_pass ) {
                                f1_( is_pass );
                        }

                        // Reconnect to server.
                        void reconnet( ) {
                                f_( );
                        }

                        // Call the method after receive data.
                        void read_handler( const boost::system::error_code& ec ,
                                size_t bytes_transferred ,
                                size_t bytes_requested ) {

                                if (!is_valid_)
                                        return;

                                // Check error.
                                if (ec) {
                                        close( ec );
                                        return;
                                }

                                // Set mblk's reader data length, and move rd_ptr().
                                error_what e_what;
                                if (rev_mgr_.change_length( bytes_transferred , e_what ) == -1) {

                                        e_what.err_no( ERROR_TYPE_TCP_PACKAGE_REV_MESSAGE_EXCEED_MAX_SIZE );
                                        e_what.err_message( "receive message is more than max size!" );
                                        catch_error( e_what );

                                        close_socket( );
                                        close_completed( ec.value( ) );

                                        return;
                                }

                                if (bytes_transferred < bytes_requested) {

                                        // Continue read remain data.
                                        size_t remain = bytes_requested - bytes_transferred;

                                        boost::asio::async_read( socket_ ,
                                                buffer( rev_mgr_.current( ) , remain ) ,
                                                bind( &tcp_cet_handler::read_handler ,
                                                this->shared_from_this( ) ,
                                                boost::asio::placeholders::error ,
                                                boost::asio::placeholders::bytes_transferred ,
                                                remain ) );

                                } else if (rev_mgr_.length( ) == PARSER::header_size) {

                                        size_t remain = 0;

                                        char* p = rev_mgr_.header( );
                                        if (read_pk_header_complete_func(
                                                this->shared_from_this( ) , p , rev_mgr_.length( ) , remain , e_what ) == -1) {

                                                // Throw error, and err_code is error_tcp_package_header_is_wrong.
                                                catch_error( e_what );

                                                close_socket( );
                                                close_completed( ec.value( ) );

                                                return;
                                        }

                                        // Check whether remain size is valid.
                                        if (!rev_mgr_.check_block_remain_space( remain )) {

                                                e_what.err_no( ERROR_TYPE_TCP_PACKAGE_REV_MESSAGE_EXCEED_MAX_SIZE );
                                                e_what.err_message( "receive message is more than max size!" );
                                                catch_error( e_what );

                                                close_socket( );
                                                close_completed( ec.value( ) );

                                                return;
                                        }

                                        // Continue read remain data.
                                        boost::asio::async_read( socket_ ,
                                                buffer( rev_mgr_.current( ) , remain ) ,
                                                bind( &tcp_cet_handler::read_handler ,
                                                this->shared_from_this( ) ,
                                                boost::asio::placeholders::error ,
                                                boost::asio::placeholders::bytes_transferred ,
                                                remain ) );


                                } else {
                                        // finish to recevie Message block.

                                        char* p = rev_mgr_.header( );
                                        if (read_pk_full_complete_func(
                                                this->shared_from_this( ) , p , rev_mgr_.length( ) , e_what ) == -1) {

                                                //  Throw error, and err_code is error_tcp_package_body_is_wrong.
                                                catch_error( e_what );

                                                close_socket( );
                                                close_completed( ec.value( ) );

                                                return;
                                        }

                                        rev_mgr_.clear( );

                                        // Start new async read data.
                                        boost::asio::async_read( socket_ ,
                                                boost::asio::buffer( rev_mgr_.current( ) , PARSER::header_size ) ,
                                                boost::bind( &tcp_cet_handler::read_handler ,
                                                this->shared_from_this( ) ,
                                                boost::asio::placeholders::error ,
                                                boost::asio::placeholders::bytes_transferred ,
                                                PARSER::header_size ) );
                                }
                        }

                        // Call the method after connect to server success and send first data success.
                        void write_first_handler( const boost::system::error_code& ec ,
                                size_t bytes_transferred ,
                                package*& pk ) {

                                char* p = pk->header( );
                                write_pk_full_complete_func( this->shared_from_this( ) , p , bytes_transferred , ec );


                                free_snd_buffer( pk );

                                // Start async read data, read finish to call read_handler().
                                boost::asio::async_read( socket_ ,
                                        boost::asio::buffer( rev_mgr_.current( ) , PARSER::header_size ) ,
                                        boost::bind( &tcp_cet_handler::read_handler ,
                                        this->shared_from_this( ) ,
                                        boost::asio::placeholders::error ,
                                        boost::asio::placeholders::bytes_transferred ,
                                        PARSER::header_size ) );
                        }

                        // Call the method after receive data.
                        void write_handler( const boost::system::error_code& ec ,
                                size_t bytes_transferred ,
                                package*& pk ) {

                                char* p = pk->header( );
                                write_pk_full_complete_func( this->shared_from_this( ) , p , bytes_transferred , ec );

                                free_snd_buffer( pk );

                        }

                        // The method will been called in ios thread after call send_data_in_thread().
                        void active_send( package*& pk ) {

                                if (is_valid_) {


                                        error_what e_what;
                                        if (active_send_in_ioservice_func( this->shared_from_this( ) , pk , e_what ) == -1) {

                                                catch_error( e_what );

                                                free_snd_buffer( pk );

                                                return;
                                        }

                                        boost::asio::async_write( socket_ ,
                                                buffer( pk->header( ) , pk->length( ) ) ,
                                                boost::bind( &tcp_cet_handler::write_handler ,
                                                this->shared_from_this( ) ,
                                                boost::asio::placeholders::error ,
                                                boost::asio::placeholders::bytes_transferred ,
                                                pk ) );

                                } else {

                                        free_snd_buffer( pk );
                                }
                        }

                        // The method will been called in ios thread after call send_close_in_thread().
                        void active_close( error_what*& e_what ) {

                                if (is_valid_) {
                                        //                                        error_what e_what;
                                        //                                        e_what.err_no( ERROR_TYPE_TCP_CLIENT_CLOSE_SOCKET_BY_CLIENT );
                                        //                                        e_what.err_message( "tcp client socket active close!" );

                                        catch_error( *e_what );
                                        close_socket( );
                                        close_completed( 0 );
                                }

                                delete e_what;
                        }

                        // The method will been called in inner.
                        void close( const boost::system::error_code& ec ) {

                                using namespace boost::system::errc;
                                if (ec.value( ) != operation_canceled) {

                                        error_what e_what;
                                        e_what.err_no( ERROR_TYPE_TCP_CLIENT_CLOSE_SOCKET_BY_SERVER );
                                        e_what.err_message( "tcp client socket is been closed by server!" );

                                        catch_error( e_what );

                                        // Passive close
                                        close_socket( );
                                }

                                close_completed( ec.value( ) );
                        }

                        // Clean something after socket close, and reconnect.
                        void close_completed( int ec_value ) {

                                is_valid_ = false;

                                close_complete_func( this->shared_from_this( ) , ec_value );

                                // Start to reconnet.
                                reconnet( );
                        }

                        // Malloc package space.
                        int malloc_snd_buffer( char*& ori_data ,
                                size_t& ori_data_size ,
                                package*& pk ,
                                error_what& e_what ) {

                                if (ori_data_size > package_size_) {
                                        e_what.err_no( ERROR_TYPE_TCP_PACKAGE_SND_MESSAGE_EXCEED_MAX_SIZE );
                                        e_what.err_message( "tcp send message is more than max size!" );
                                        return -1;
                                }

                                pk = new package( );
                                pk->copy( ori_data , ori_data_size , e_what );

                                return 0;
                        }

                        // Free package space.
                        void free_snd_buffer( package*& pk ) {

                                delete pk;
                                pk = 0;
                        }

                public:
                        virtual void catch_error_func( pointer /*p*/ , error_what& /*e_what*/ ) {

                        }
                        virtual void close_complete_func( pointer /*p*/ , int& /*ec_value*/ ) {

                        }
                        virtual int read_pk_header_complete_func(
                                pointer /*p*/ ,
                                char*& /*rev_data*/ ,
                                size_t& /*rev_data_size*/ ,
                                size_t& /*remain_size*/ ,
                                error_what& /*e_what*/ ) {

                                return 0;
                        }
                        virtual int read_pk_full_complete_func(
                                pointer /*p*/ ,
                                char*& /*rev_data*/ ,
                                size_t& /*rev_data_size*/ ,
                                error_what& /*e_what*/ ) {
                                return 0;
                        }
                        virtual void write_pk_full_complete_func(
                                pointer /*p*/ ,
                                char*& /*snd_p*/ ,
                                size_t& /*snd_size*/ ,
                                const boost::system::error_code& /*ec*/ ) {

                        }
                        virtual int active_send_in_ioservice_func( pointer /*p*/ , package*& /*pk*/ , error_what& /*e_what*/ ) {

                                return 0;
                        }
                        virtual int make_first_sended_package_func( package*& /*pk*/ , error_what& /*e_what*/ ) {

                                return 0;
                        }

                protected:
                        boost::asio::io_service& ios_;
                        ip::tcp::socket socket_;

                        bool is_valid_;

                        package rev_mgr_;
                        size_t package_size_;

                        boost::function<void(bool ) > f1_; // authenticate pass callback
                        boost::function<void( ) > f_; // reconnect callback.
                        ptime p1_; // Save heart jump datetime.

                        enum {
                                CLOSE_COMPLETED_EC_TYPE_MAKE_FIRST_PACKAGE_FAIL = 0x01 ,
                        };
                };

        }
}


#endif /* BINGO_TCP_CET_HANDLER_HEADER_H_ */
