/*
 * file_logger_no_postfix.cpp
 *
 *  Created on: 2016-11-8
 *      Author: root
 */

#include "file_logger_no_postfix.h"

using namespace bingo::log::handlers ;

file_logger_no_postfix::file_logger_no_postfix( const char* directory , const char* filename ) :
file_logger_every_day( directory , filename ) {

}

file_logger_no_postfix::~file_logger_no_postfix( ) {

} ;

void file_logger_no_postfix::get_filename_postfix( string& filename ) {
}

