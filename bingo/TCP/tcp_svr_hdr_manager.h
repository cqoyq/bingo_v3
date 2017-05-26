/*
 * tcp_svr_hdr_manager.h
 *
 *  Created on: 2016-6-30
 *      Author: root
 */

#ifndef BINGO_TCP_SVR_HDR_MANAGER_HEADER_H_
#define BINGO_TCP_SVR_HDR_MANAGER_HEADER_H_

#include "bingo/foreach_.h"
#include "bingo/error_what.h"
#include "tcp_handler_manager.h"

namespace bingo {
        namespace TCP {

                template<class HANDLER>
                struct find_all_of_handler_because_heartjump_timeout {
                        bool operator ()( tcp_handler_data& n ) {

                                HANDLER* p = static_cast<HANDLER*> ( n.handler_pointer );
                                if (p) {
                                        if (p->check_heartjump_timeout( )) {
                                                return true;
                                        }
                                }
                                return false;
                        }
                };

                template<class HANDLER>
                class tcp_svr_hdr_manager : public tcp_handler_manager<HANDLER> {
                public:
                        tcp_svr_hdr_manager( ) : tcp_handler_manager<HANDLER>( ) {
                        }
                        virtual ~tcp_svr_hdr_manager( ) {
                        }

                        // Send actively heart-beat to all client.
                        void send_heartjump_in_thread( ) {
                                // lock part field.
                                mutex::scoped_lock lock( this->mu_ );
                                foreach_( tcp_handler_data& n , this->sets_ ) {
                                        HANDLER* p = static_cast<HANDLER*> ( n.handler_pointer );
                                        if (p) {
                                                p->send_heartjump_in_thread( );
                                        }
                                }
                        }

                        // Check whether tcp_handler is heartjump timeout.
                        void check_heartjump_in_thread( ) {

                                // lock part field.
                                mutex::scoped_lock lock( this->mu_ );
                                foreach_( tcp_handler_data& n , this->sets_ | adaptors::filtered( find_all_of_handler_because_heartjump_timeout<HANDLER>( ) ) ) {
                                        HANDLER* p = static_cast<HANDLER*> ( n.handler_pointer );
                                        if (p) {
                                                error_what e_what;
                                                e_what.err_no( ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_HEARTBEAT );
                                                e_what.err_message( "tcp server socket is been closed by heartbeat!" );

                                                //                                                p->catch_error(e_what);
                                                //                                                p->close_socket();
                                                p->send_close_in_thread( e_what );
                                        }
                                }
                        }

                        // Check whether tcp_handler is heartjump timeout.
                        void check_heartjump( ) {

                                // lock part field.
                                mutex::scoped_lock lock( this->mu_ );
                                foreach_( tcp_handler_data& n , this->sets_ | adaptors::filtered( find_all_of_handler_because_heartjump_timeout<HANDLER>( ) ) ) {
                                        HANDLER* p = static_cast<HANDLER*> ( n.handler_pointer );
                                        if (p) {
                                                error_what e_what;
                                                e_what.err_no( ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_HEARTBEAT );
                                                e_what.err_message( "tcp server socket is been closed by heartbeat!" );

                                                //                                                p->catch_error(e_what);
                                                //                                                p->close_socket();
                                                p->send_close_in_thread( e_what );
                                        }
                                }
                        }

                        // Check whether tcp_handler is authentication pass.
                        void check_authentication_pass_in_thread( ) {

                                // lock part field.
                                mutex::scoped_lock lock( this->mu_ );
                                foreach_( tcp_handler_data& n , this->sets_ ) {
                                        HANDLER* p = static_cast<HANDLER*> ( n.handler_pointer );
                                        if (p) {
                                                if (!p->check_authentication_pass( )) {
                                                        error_what e_what;
                                                        e_what.err_no( ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_AUTHENTICATION );
                                                        e_what.err_message( "tcp server socket is been closed by authentication!" );

                                                        //                                                        p->catch_error(e_what);
                                                        //                                                        p->close_socket();
                                                        p->send_close_in_thread( e_what );
                                                }
                                        }
                                }
                        }

                        // Check whether tcp_handler is authentication pass.
                        void check_authentication_pass( ) {

                                // lock part field.
                                mutex::scoped_lock lock( this->mu_ );
                                foreach_( tcp_handler_data& n , this->sets_ ) {
                                        HANDLER* p = static_cast<HANDLER*> ( n.handler_pointer );
                                        if (p) {
                                                if (!p->check_authentication_pass( )) {

                                                        error_what e_what;
                                                        e_what.err_no( ERROR_TYPE_TCP_SERVER_CLOSE_SOCKET_BY_AUTHENTICATION );
                                                        e_what.err_message( "tcp server socket is been closed by authentication!" );

                                                        //                                                        p->catch_error(e_what);
                                                        //                                                        p->close_socket();
                                                        p->send_close_in_thread( e_what );
                                                }
                                        }
                                }
                        }
                };

        }
}

#endif /* BINGO_TCP_SVR_HDR_MANAGER_HEADER_H_ */
