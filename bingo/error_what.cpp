/*
 * error_what.cpp
 *
 *  Created on: 2016-9-28
 *      Author: root
 */

#include "error_what.h"

using namespace bingo;

error_what::error_what(){
	err_no_ = 0;
}

error_what::~error_what(){

}

string& error_what::err_message(){
	return err_;
}

void error_what::err_message(const char* err){
	err_ = err;
}

int error_what::err_no(){
	return err_no_;
}

void error_what::err_no(int n){
	err_no_ = n;
}

void error_what::clone(error_what& obj){
	obj.err_no(err_no_);
	obj.err_message(err_.c_str());
}

void error_what::set_error(int err_no, const char* err_msg){
	err_no_ = err_no;
	err_ = err_msg;
}



