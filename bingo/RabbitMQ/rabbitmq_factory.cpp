/*
 * rabbitmqfactory.cpp
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#include "handlers/simple_sendor.h"
#include "handlers/simple_receiver.h"
#include "handlers/work_sendor.h"
#include "handlers/work_receiver.h"
#include "handlers/publish_sendor.h"
#include "handlers/publish_receiver.h"
#include "handlers/routing_sendor.h"
#include "handlers/routing_receiver.h"
#include "handlers/topic_sendor.h"
#include "handlers/topic_receiver.h"
using namespace bingo::RabbitMQ::handlers;
#include "bingo/error_what.h"

#include "rabbitmq_factory.h"

using namespace bingo::RabbitMQ;

rabbitmq_factory::rabbitmq_factory( ) :
snd_( 0 ) ,
rev_( 0 ) ,
logger_( 0 ) {
          // TODO Auto-generated constructor stub

          cfg_file_ = "./cfg.xml";
}

rabbitmq_factory::~rabbitmq_factory( ) {
          // TODO Auto-generated destructor stub
          if ( snd_ ) delete snd_;
          if ( rev_ ) delete rev_;
}

void rabbitmq_factory::set_cfg( const char* cfg_file ) {
          cfg_file_ = cfg_file;
}

bool rabbitmq_factory::make_simple_sendor( log_handler*& log ) {
          logger_ = log;

          simple_sendor* p = new simple_sendor( cfg_file_.c_str( ) );
          snd_ = p;
          if ( !snd_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_SINGLE_SENDOR_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "single sendor fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_SINGLE_SENDOR_ERROR , s_msg );
                    }
          } else {
                    // Connect to MQ server.
                    snd_->connet_to_server( logger_ );
          }
          return snd_->is_valid;
}

bool rabbitmq_factory::make_simple_receiver( log_handler*& log , rb_receiver::rev_callback f ) {
          logger_ = log;

          simple_receiver* p = new simple_receiver( cfg_file_.c_str( ) );
          rev_ = p;
          if ( !rev_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_SINGLE_RECEIVER_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "make single receiver fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_SINGLE_RECEIVER_ERROR , s_msg );
                    }
          } else {
                    // Set callback function.
                    rev_->callback( f );

                    // Connect to MQ server.
                    rev_->connet_to_server( logger_ );
          }
          return rev_->is_valid;
}

bool rabbitmq_factory::make_work_sendor( log_handler*& log ) {
          logger_ = log;

          work_sendor* p = new work_sendor( cfg_file_.c_str( ) );
          snd_ = p;
          if ( !snd_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_WORK_SENDOR_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "work sendor fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_WORK_SENDOR_ERROR , s_msg );
                    }
          } else {
                    // Connect to MQ server.
                    snd_->connet_to_server( logger_ );
          }
          return snd_->is_valid;
}

bool rabbitmq_factory::make_work_receiver( log_handler*& log , rb_receiver::rev_callback f ) {
          logger_ = log;

          work_receiver* p = new work_receiver( cfg_file_.c_str( ) );
          rev_ = p;
          if ( !rev_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_WORK_RECEIVER_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "make work receiver fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_WORK_RECEIVER_ERROR , s_msg );
                    }
          } else {
                    // Set callback function.
                    rev_->callback( f );

                    // Connect to MQ server.
                    rev_->connet_to_server( logger_ );
          }
          return rev_->is_valid;
}

bool rabbitmq_factory::make_publish_sendor( log_handler*& log ) {
          logger_ = log;

          publish_sendor* p = new publish_sendor( cfg_file_.c_str( ) );
          snd_ = p;
          if ( !snd_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_PUBLISH_SENDOR_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "publish sendor fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_SENDOR_ERROR , s_msg );
                    }
          } else {
                    // Connect to MQ server.
                    snd_->connet_to_server( logger_ );
          }
          return snd_->is_valid;
}

bool rabbitmq_factory::make_publish_receiver( log_handler*& log , rb_receiver::rev_callback f ) {
          logger_ = log;

          publish_receiver* p = new publish_receiver( cfg_file_.c_str( ) );
          rev_ = p;
          if ( !rev_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_PUBLISH_RECEIVER_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "make publish receiver fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_PUBLISH_RECEIVER_ERROR , s_msg );
                    }
          } else {
                    // Set callback function.
                    rev_->callback( f );

                    // Connect to MQ server.
                    rev_->connet_to_server( logger_ );
          }
          return rev_->is_valid;
}

bool rabbitmq_factory::make_routing_sendor( log_handler*& log ) {
          logger_ = log;

          routing_sendor* p = new routing_sendor( cfg_file_.c_str( ) );
          snd_ = p;
          if ( !snd_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_ROUTING_SENDOR_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "routing sendor fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_ROUTING_SENDOR_ERROR , s_msg );
                    }
          } else {
                    // Connect to MQ server.
                    snd_->connet_to_server( logger_ );
          }
          return snd_->is_valid;
}

bool rabbitmq_factory::make_routing_receiver( log_handler*& log , rb_receiver::rev_callback f ) {
          logger_ = log;

          routing_receiver* p = new routing_receiver( cfg_file_.c_str( ) );
          rev_ = p;
          if ( !rev_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_ROUTING_RECEIVER_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "make routing receiver fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_ROUTING_RECEIVER_ERROR , s_msg );
                    }
          } else {
                    // Set callback function.
                    rev_->callback( f );

                    // Connect to MQ server.
                    rev_->connet_to_server( logger_ );
          }
          return rev_->is_valid;
}

bool rabbitmq_factory::make_topic_sendor( log_handler*& log ) {
          logger_ = log;

          topic_sendor* p = new topic_sendor( cfg_file_.c_str( ) );
          snd_ = p;
          if ( !snd_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_TOPIC_SENDOR_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "topic sendor fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_TOPIC_SENDOR_ERROR , s_msg );
                    }
          } else {
                    // Connect to MQ server.
                    snd_->connet_to_server( logger_ );
          }
          return snd_->is_valid;
}

bool rabbitmq_factory::make_topic_receiver( log_handler*& log , rb_receiver::rev_callback f ) {
          logger_ = log;

          topic_receiver* p = new topic_receiver( cfg_file_.c_str( ) );
          rev_ = p;
          if ( !rev_->is_valid ) {
                    e_what_.err_no( ERROR_TYPE_RABBITMQ_MAKE_TOPIC_RECEIVER_FAIL );
                    e_what_.err_message( p->err( ).err_message( ).c_str( ) );

                    if ( logger_ ) {

                              string s_msg = "make topic receiver fail!, error:" + e_what_.err_message( );
                              logger_->handle( bingo::log::LOG_LEVEL_ERROR , RABBITMQ_TOPIC_RECEIVER_ERROR , s_msg );
                    }
          } else {
                    // Set callback function.
                    rev_->callback( f );

                    // Connect to MQ server.
                    rev_->connet_to_server( logger_ );
          }
          return rev_->is_valid;

}

error_what& rabbitmq_factory::err( ) {
          return e_what_;
}

void rabbitmq_factory::transfer_data_by_key( string& key , char* data , size_t data_size ) {
          if ( snd_ )
                    snd_->transfer_data_by_key( key , data , data_size );
}

void rabbitmq_factory::transfer_data_by_routingkey( string& routingkey , char* data , size_t data_size ) {
          if ( snd_ )
                    snd_->transfer_data_by_routingkey( routingkey , data , data_size );
}





