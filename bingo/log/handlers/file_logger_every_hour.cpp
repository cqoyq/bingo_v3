/*
 * file_logger_every_hour.cpp
 *
 *  Created on: 2016-11-8
 *      Author: root
 */

#include "file_logger_every_hour.h"

using namespace bingo::log::handlers ;

file_logger_every_hour::file_logger_every_hour( const char* directory , const char* filename ) :
file_logger_every_day( directory , filename ) {

} ;

file_logger_every_hour::~file_logger_every_hour( ) {

} ;

void file_logger_every_hour::get_filename_postfix( string& filename ) {
     filename.append( "." ) ;

     date now = day_clock::local_day( ) ;
     int year = now.year( ) ;
     int month = now.month( ) ;
     int day = now.day( ) ;

     ptime p1 = second_clock::local_time( ) ;
     int hour = p1.time_of_day( ).hours( ) ;

     char cnow[11] ;
     memset( &cnow[0] , 0x00 , 11 ) ;
     sprintf( &cnow[0] , "%4d%02d%02d%02d" , year , month , day , hour ) ;
     filename.append( cnow ) ;
}

