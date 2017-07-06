/*
 * tcp_client.h
 *
 *  Created on: 2016-7-1
 *      Author: root
 */

#ifndef BINGO_TCP_CLIENT_HEADER_H_
#define BINGO_TCP_CLIENT_HEADER_H_

#include "bingo/type.h"
#include "bingo/define.h"
#include "bingo/error_what.h"

#include <iostream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/bind.hpp>
using namespace boost;
using namespace boost::asio;

namespace bingo {
        namespace TCP {

                template<
                typename HEARTJUMP ,
                typename HANDLER ,
                typename PARSER
                >
                class tcp_client {
                public:
                        typedef boost::shared_ptr<HANDLER> pointer;
                        tcp_client( boost::asio::io_service& io_service , string& ipv4 , u16_t& port ) :
                        ios_( io_service ) , ipv4_( ipv4 ) , port_( port ) ,
                        timer0_( io_service ) ,
                        timer1_( io_service ) ,
                        timer2_( io_service ) ,
                        is_authenticate_pass_( false ) ,
                        retry_delay_( PARSER::retry_delay_seconds ) {
                                start_connect( );

                                // Start to send heartjump package
                                schedule_timer1( );
                        }
                        virtual ~tcp_client( ) {
                        }

                        // Start to connect to server.
                        void start_connect( ) {

                                // Make new tcp_connection object.
                                pointer new_handler( new HANDLER( ios_ ,
                                        boost::bind( &tcp_client::reconnect , this ) ,
                                        boost::bind( &tcp_client::set_authenticate_pass , this , _1 ) ) );

                                // Start to connect.
                                new_handler->socket( ).async_connect(
                                        ip::tcp::endpoint( boost::asio::ip::address_v4::from_string( ipv4_ ) , port_ ) ,
                                        boost::bind( &tcp_client::connect_handler , this , new_handler ,
                                        boost::asio::placeholders::error ) );
                        }

                        //Set authenticate status.
                        void set_authenticate_pass( bool is_pass ) {
                                this->is_authenticate_pass_ = is_pass;
                        }

                private:

                        // Call the function when connect in handler again.
                        void reconnect( ) {
                                retry_delay_ = PARSER::retry_delay_seconds;

                                // If retry_delay_seconds is 0, then don't reconnect.
                                if (retry_delay_ == 0)
                                        return;

                                start_connect( );
                        }

                        // Call the method after connect to server success.
                        void connect_handler( pointer new_handler ,
                                const boost::system::error_code& ec ) {
                                if (!ec) {

                                        // Call connet_success_func()
                                        error_what e_what;
                                        if (connet_success_func( new_handler , e_what ) == 0) {
                                                // Start to aync-read.
                                                new_handler->start( );

                                                // Save hdr to handler_
                                                handler_ = new_handler.get( );

                                        } else {

                                                new_handler->catch_error( e_what );

                                                // Active close socket.
                                                new_handler->close_socket( );
                                        }

                                } else {

                                        // If retry_delay_seconds is 0, then don't reconnect.
                                        if (PARSER::retry_delay_seconds == 0)
                                                return;

                                        // Begin to reconnect.
                                        retry_delay_ *= 2;
                                        if (retry_delay_ > PARSER::max_retry_delay_seconds)
                                                retry_delay_ = PARSER::max_retry_delay_seconds;

                                        connet_fail_func( new_handler , retry_delay_ );

                                        // Start to reconnet.
                                        schedule_timer0( retry_delay_ );

                                        return;
                                }
                        }

                        // Schedule reconnect timer.
                        void schedule_timer0( int& expire_second ) {

                                boost::posix_time::seconds s( expire_second );
                                boost::posix_time::time_duration td = s;
                                timer0_.expires_from_now( td );
                                timer0_.async_wait( bind( &tcp_client::reconnect_timeout_handler , this , boost::asio::placeholders::error ) );
                        }

                        // Reconnect timer callback.
                        void reconnect_timeout_handler( const system::error_code& ec ) {

                                if (!ec)
                                        start_connect( );
                        }

                        // Schedule heartbeat timer.
                        void schedule_timer1( ) {

                                // If max_interval_seconds_on_heartjump is 0, then don't send heartjump.
                                if (PARSER::max_interval_seconds_on_heartjump > 0) {
                                        boost::posix_time::seconds s( PARSER::max_interval_seconds_on_heartjump );
                                        boost::posix_time::time_duration td = s;

                                        timer1_.expires_from_now( td );
                                        timer1_.async_wait( bind( &tcp_client::send_heartjump_timeout_handler , this ,
                                                boost::asio::placeholders::error ) );
                                }
                        }

                        // Heartbeat timer callback.
                        void send_heartjump_timeout_handler( const system::error_code& ec ) {

                                if (!ec) {

                                        if (is_authenticate_pass_) {
                                                char* p = HEARTJUMP::data;
                                                size_t size = HEARTJUMP::data_size;
                                                handler_->send_data_in_thread( p , size );
                                        }

                                        schedule_timer1( );
                                }

                        }

                        // Schedule check heartbeat timer.
                        void schedule_timer2( ) {
                                boost::posix_time::seconds s( PARSER::max_interval_seconds_check_heartjump );
                                boost::posix_time::time_duration td = s;

                                timer2_.expires_from_now( td );
                                timer2_.async_wait( bind( &tcp_client::check_heartjump_timeout_handler , this ,
                                        boost::asio::placeholders::error ) );
                        }

                        // Heartbeat timer callback.
                        void check_heartjump_timeout_handler( const system::error_code& ec ) {
                                if (!ec) {

                                        if (is_authenticate_pass_
                                                && !handler_->check_heartjump_timeout( )) {

                                                error_what e_what;
                                                e_what.err_no( ERROR_TYPE_TCP_CLIENT_CLOSE_SOCKET_BY_HEARTBEAT );
                                                e_what.err_message( "client close socket because heartjump" );
                                                handler_->send_close_in_thread( e_what );
                                        }

                                        schedule_timer1( );
                                }
                        }

                public:
                        virtual int connet_success_func( pointer /*ptr*/ , error_what& /*err_code*/ ) {
                                return 0;
                        }
                        virtual void connet_fail_func( pointer /*ptr*/ , int& /*retry_delay_seconds*/ ) {

                        }

                protected:
                        io_service &ios_;
                        HANDLER* handler_;

                        string ipv4_;
                        u16_t port_;

                        int retry_delay_; // reconnect delay seconds.
                        deadline_timer timer0_; // reconnect timer.

                        bool is_authenticate_pass_; // If authenticate is pass, then send heartbeat package.
                        deadline_timer timer1_; // heartjump timer.
                        deadline_timer timer2_; // check heartjump timer.
                };

        }
}


#endif /* BINGO_TCP_CLIENT_HEADER_H_ */
