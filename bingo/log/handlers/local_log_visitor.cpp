/*
 * local_log_visitor.h
 *
 *  Created on: 2016-8-31
 *      Author: root
 */

#include "local_log_visitor.h"

#include "file_logger_every_day.h"
#include "file_logger_every_hour.h"
#include "file_logger_no_postfix.h"

using namespace bingo::log::handlers ;

local_log_visitor::local_log_visitor( ) {
     logger_ = 0 ;
}

local_log_visitor::~local_log_visitor( ) {
     // TODO : implement
     if ( logger_ != 0 ) delete logger_ ;
}

bingo::error_what& local_log_visitor::err( ) {
     return e_what_ ;
}

bool local_log_visitor::read_config( const char* config_file ) {
     if ( config_.read_xml( config_file ) ) {

          string d = config_.value.path.c_str( ) ;
          string f = config_.value.filename.c_str( ) ;

          if ( config_.value.type == config::LOCAL_LOGGER_TYPE_EVERY_DAY ) {
               this->logger_ = new file_logger_every_day( d.c_str( ) , f.c_str( ) ) ;

          } else if ( config_.value.type == config::LOCAL_LOGGER_TYPE_EVERY_HOUR ) {
               this->logger_ = new file_logger_every_hour( d.c_str( ) , f.c_str( ) ) ;

          } else if ( config_.value.type == config::LOCAL_LOGGER_TYPE_ONE_FILE ) {
               this->logger_ = new file_logger_no_postfix( d.c_str( ) , f.c_str( ) ) ;
          }

          if ( logger_ ) {
               this->logger_ ->level( config_.value.level ) ;
               return true ;
          } else {
               e_what_.err_no( ERROR_TYPE_LOG_LOCAL_LOG_TYPE_NO_EXIST ) ;
               e_what_.err_message( "create local logger fail, type isn't exist!" ) ;
               return false ;
          }



     } else {
          config_.err( ).clone( e_what_ ) ;
          return false ;
     }
}

void local_log_visitor::handle( int level , const char* tag , std::string& info ) {
     // TODO : implement
     if ( config_.is_valid
             && level <= ( int ) ( config_.value.level ) ) {
          switch ( level ) {
               case LOG_LEVEL_DEBUG:
                    logger_->log_d( tag , info ) ;
                    break ;
               case LOG_LEVEL_ERROR:
                    logger_->log_e( tag , info ) ;
                    break ;
               case LOG_LEVEL_FATAL:
                    logger_->log_f( tag , info ) ;
                    break ;
               case LOG_LEVEL_INFO:
                    logger_->log_i( tag , info ) ;
                    break ;
               default:
                    logger_->log_d( tag , info ) ;
                    break ;
          }

     }
}

void local_log_visitor::set_level( log_level level ) {
     if ( config_.is_valid ) {
          config_.value.level = level ;
     }
}

config::local_cfg& local_log_visitor::config( ) {
     return config_ ;
}

