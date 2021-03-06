/*
 * publish_sendor.cpp
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#include "../visitor/asiohandler.h"

#include "bingo/thread/all.h"

#include "publish_sendor.h"

// --------------------------- rabbitmq logger  ------------------------- //

log_handler* rb_publish_sendor_logger;

// --------------------------- rabbitmq sendor thread ------------------------- //

void make_publish_sendor_thr( bingo::RabbitMQ::config::rb_sendor_cfg_value*& p ) {

          //	cout << "make_sendor_thr map-key:" << p->map_key << ",p:" << p << endl;

          while ( true ) {
                    boost::asio::io_service ioService;

                    AsioHandler handler( ioService , rb_publish_sendor_logger );
                    handler.connect( p->ip.c_str( ) , p->port );

                    AMQP::Connection connection( &handler , AMQP::Login(
                            p->username.c_str( ) , p->pwd.c_str( ) ) ,
                            p->host.c_str( ) );

                    AMQP::Channel channel( &connection );

                    // Save channel pointer.
                    p->channel = &channel;

                    channel.declareExchange( p->exchange.c_str( ) , AMQP::fanout ).onSuccess( [&]( ) {
                              //                                                channel.publish(p->exchange.c_str(), "", msg);
                              if ( rb_publish_sendor_logger ) {
                                        string s_msg = "exchange declare success!";
                                        rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_INFO , RABBITMQ_PUBLISH_SENDOR_INFO , s_msg );
                              }
                    } );

                    ioService.run( );

                    if ( rb_publish_sendor_logger ) {
                              string s_msg = "ioService is exit!";
                              rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_INFO , RABBITMQ_PUBLISH_SENDOR_INFO , s_msg );
                    }

          }
}

// --------------------------- rabbitmq many_to_one task ------------------------- //

class rb_publish_sendor_task : public bingo::thread::many_to_one<rb_data_message , rb_exit_message> {
public:

          rb_publish_sendor_task( thr_top_callback& f ) :
          bingo::thread::many_to_one<rb_data_message , rb_exit_message>( f ) {

          }

          virtual ~rb_publish_sendor_task( ) {
          }
};
typedef bingo::singleton_v1<rb_publish_sendor_task ,
rb_publish_sendor_task::thr_top_callback
> RB_PUBLISH_SENDOR_TASK_TYPE;

// --------------------------- rabbitmq task callback ------------------------- //

using namespace bingo::RabbitMQ::handlers;

void publish_sendor::rb_top( rb_data_message*& msg , bingo_empty_type& ) {

          if ( is_valid ) {

                    char* header = msg->data.header( );

                    char* data = header + RABBITMQ_MAPKEY_MAX_SIZE;
                    size_t data_size = msg->data.length( ) - RABBITMQ_MAPKEY_MAX_SIZE;

                    // Get key.
                    char key[RABBITMQ_MAPKEY_MAX_SIZE + 1];
                    memset( key , 0x00 , RABBITMQ_MAPKEY_MAX_SIZE + 1 );
                    memcpy( key , header , RABBITMQ_MAPKEY_MAX_SIZE );
                    string s_key = key;

                    config::rb_sendor_cfg_value* p = cfg_.get_by_key( s_key );
                    // Check map-key.
                    if ( p == 0 ) {

                              if ( rb_publish_sendor_logger ) {
                                        string s_msg = "send task data fail! map-key isn't exist, map-key:";
                                        s_msg.append( s_key );
                                        rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                              }
                    } else {
                              // Check AMQP::Channel*.
                              if ( p->channel == 0 ) {

                                        if ( rb_publish_sendor_logger ) {
                                                  string s_msg = "send task data fail! channel is empty.";
                                                  rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                                        }

                                        return;
                              }

                              // Send message body.
                              bool re = p->channel->publish( p->exchange.c_str( ) , "" , data , data_size );
                              if ( !re ) {
                                        if ( rb_publish_sendor_logger ) {
                                                  string s_msg = "send task data fail!";
                                                  rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                                        }
                              } else {
#ifdef BINGO_MQ_DEBUG
                                        if ( rb_publish_sendor_logger ) {
                                                  string_ex t;
                                                  string s_msg = "send task data success! map-key:";
                                                  s_msg.append( s_key );
                                                  s_msg.append( ",data_size:#" );
                                                  s_msg.append( lexical_cast<string>( data_size ) );
                                                  s_msg.append( "#,data:#" );
                                                  //							s_msg.append(t.stream_to_string(data, data_size));

                                                  char s_d[data_size + 1];
                                                  memset( s_d , 0x00 , data_size + 1 );
                                                  memcpy( s_d , data , data_size );
                                                  s_msg.append( s_d );
                                                  s_msg.append( "#" );

                                                  rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_DEBUG , RABBITMQ_PUBLISH_SENDOR_INFO , s_msg );
                                        }
#endif
                              }

                    }
          }
}

void publish_sendor::transfer_data_by_routingkey( string& routingkey , char*& in , size_t& in_size ) {
          // Check whether has read cfg.xml file, if don't read, then return directly;
          if ( !is_valid ) {
                    return;
          }

          // Check routingkey.
          config::rb_sendor_cfg_value* cfg_value = cfg_.get_by_routingkey( routingkey );
          if ( cfg_value == 0 ) {

                    if ( rb_publish_sendor_logger ) {
                              string s_msg = "transfer data is fail, routingkey isn't exist! routingkey:";
                              s_msg.append( routingkey.c_str( ) );
                              rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                    }
                    return;
          }

          // Get key.
          string key = cfg_value->map_key;

          // Make message package, package construct: mapkey(40) + message_body.
          size_t message_size = RABBITMQ_MAPKEY_MAX_SIZE + in_size;
          char message[message_size];
          memset( message , 0x00 , message_size );
          memcpy( message , key.c_str( ) , key.length( ) );
          memcpy( message + RABBITMQ_MAPKEY_MAX_SIZE , in , in_size );

          // Make rb_data_message object.
          error_what err;
          rb_data_message* data = new rb_data_message( );

          if ( data->data.copy( message , message_size , err ) == -1 ) {

                    if ( rb_publish_sendor_logger ) {
                              string s_msg = "create task data error, err_no:" + lexical_cast<string>( err.err_no( ) );
                              rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                    }

                    delete data;
                    return;
          }

          // Send data to MQ server.
          if ( RB_PUBLISH_SENDOR_TASK_TYPE::instance( )->put( data , err ) == -1 ) {

                    if ( rb_publish_sendor_logger ) {
                              string s_msg = "put data to task error, err_no:" + lexical_cast<string>( err.err_no( ) );
                              rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                    }
                    delete data;
          }
}

void publish_sendor::transfer_data_by_key( string& key , char*& in , size_t& in_size ) {
          // Check whether has read cfg.xml file, if don't read, then return directly;
          if ( !is_valid ) {
                    return;
          }

          // Check key.
          if ( key.length( ) > RABBITMQ_MAPKEY_MAX_SIZE ) {
                    if ( rb_publish_sendor_logger ) {
                              string s_msg = "transfer data is fail, size of key is more then 40!";
                              rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                    }
                    return;
          }

          // Make message package, package construct: mapkey(40) + message_body.
          size_t message_size = RABBITMQ_MAPKEY_MAX_SIZE + in_size;
          char message[message_size];
          memset( message , 0x00 , message_size );
          memcpy( message , key.c_str( ) , key.length( ) );
          memcpy( message + RABBITMQ_MAPKEY_MAX_SIZE , in , in_size );

          // Make rb_data_message object.
          error_what err;
          rb_data_message* data = new rb_data_message( );

          if ( data->data.copy( message , message_size , err ) == -1 ) {
                    if ( rb_publish_sendor_logger ) {
                              string s_msg = "create task data error, err_no:" + lexical_cast<string>( err.err_no( ) );
                              rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                    }

                    delete data;
                    return;
          }

          // Send data to MQ server.
          if ( RB_PUBLISH_SENDOR_TASK_TYPE::instance( )->put( data , err ) == -1 ) {
                    if ( rb_publish_sendor_logger ) {
                              string s_msg = "put data to task error, err_no:" + lexical_cast<string>( err.err_no( ) );
                              rb_publish_sendor_logger->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                    }
                    delete data;
          }
}

publish_sendor::publish_sendor( const char* cfg_file ) : rb_sendor( ) {
          // TODO Auto-generated constructor stub

          is_valid = cfg_.read_xml( cfg_file );
          if ( !is_valid ) {
                    cfg_.err( ).clone( e_what_ );
          } else {
                    RB_PUBLISH_SENDOR_TASK_TYPE::construct(
                            bind( &publish_sendor::rb_top , this , _1 , _2 ) // thread_task queue top callback
                            );
          }
}

publish_sendor::~publish_sendor( ) {
          // TODO Auto-generated destructor stub
          if ( is_valid ) RB_PUBLISH_SENDOR_TASK_TYPE::release( );
}

void publish_sendor::connet_to_server( log_handler*& log ) {
          if ( is_valid ) {

                    rb_publish_sendor_logger = log;

                    int max = cfg_.size( );
                    for ( int i = 0; i < max; i++ ) {

                              config::rb_sendor_cfg_value* p = cfg_[i];

                              if ( p->type == config::RB_CFG_TYPE_PUBLISH_AND_SUBSCRIBE )
                                        tg_.create_thread( bind( make_publish_sendor_thr , p ) );

                    }

                    tg_.join_all( );


          }
}

