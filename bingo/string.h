/*
 * string.h
 *
 *  Created on: 2016-6-29
 *      Author: root
 */

#ifndef BINGO_STRING_HEADER_H_
#define BINGO_STRING_HEADER_H_

#include <string>
#include <memory.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
using namespace std;

#include "type.h"

// uuid
#include <boost/uuid/uuid.hpp>     //uuid使用
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
using namespace boost::uuids;

namespace bingo {

        // -------------------------------------------------------  string_ex -------------------------------------------------------- //

        class string_ex {
        private:
                stringstream oss_;
                string str_;

                enum {
                        MAX_SIZE_OF_CHR = 8 ,
                };
                char chr_[MAX_SIZE_OF_CHR];
        public:
                string_ex( ) {
                        memset( &chr_[0] , 0x00 , MAX_SIZE_OF_CHR );
                }
                virtual ~string_ex( ) {
                }

        private:
                void clear( );

                // char -> int
                // for example: 'd' -> 11
                int char_to_int( string chr );

        public:
                // byte's stream -> string.
                // for example: {0x00, 0x01, 0x02} -> '00 01 02'
                const char* stream_to_string( const char* p , size_t p_size );

                // string -> byte's stream.
                // for example: '01 02 03' -> {0x01, 0x02, 0x03}
                // @out must be initialized and allocate memory.
                void string_to_stream( string& in , char*& out );

                // byte's stream -> string.
                // for example: {0x66, 0x59, 0x48, 0x61} -> 'fYHa'
                const char* stream_to_char( const char* p , size_t p_size );

                // short -> byte of stream.
                // for example: 260 -> {0x04, 0x01}
                const char* short_to_stream( u16_t value );

                // byte of stream -> short
                // for example: {0x04, 0x01} -> 260
                u16_t stream_to_short( const char* p );

                // memory's address -> long.
                // for example: char* p = new char[10];
                //              p -> '12548741244422'
                u64_t pointer_to_long( const void* p );

                // Ip string -> int.
                u32_t ip_to_int( string& s );

                // Int -> ip string
                const char* int_to_ip( u32_t n );


                // Convert T to string.
                template<typename T>
                const char* convert( const T& t ) {
                        clear( );
                        oss_ << t;
                        oss_ >> str_;
                        return str_.c_str( );
                }

                const char* convert( const char*& t );

                const char* convert( const char& t );

                const char* convert( const u8_t& t );

                // uuid
                static string uuid_to_string( boost::uuids::uuid& u );
                static void string_to_uuid( string& in , boost::uuids::uuid& u );
                static string make_random_uuid( );
                template<typename KEY>
                static string make_name_uuid( string& name ) {
                        uuid name_com = string_generator( )( KEY::value );
                        name_generator ngen( name_com );

                        uuid u1 = ngen( name );

                        return uuid_to_string( u1 );
                }

                // sha1, result is 20 bytes.
                static void sha1_to_stream( string& in_stream , string& out_stream );

                // crc32, result is 4 bytes.
                static void crc32_to_stream( string& in_stream , string& out_stream );
                static u32_t crc32_to_int( string& in_stream );
        };

        // -------------------------------------------------------  string_append -------------------------------------------------------- //

        class string_append {
        public:
                template<typename T>
                string_append* add( T t ) {
                        string_ex st;
                        data.append( st.convert( t ) );
                        return this;

                };
                string& to_string( ) {
                        return data;
                }

        private:
                string data;
        };

}

#endif /* BINGO_STRING_HEADER_H_ */
