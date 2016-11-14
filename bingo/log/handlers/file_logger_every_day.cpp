/*
 * file_logger_every_day.cpp
 *
 *  Created on: 2016-11-8
 *      Author: root
 */

#include "file_logger_every_day.h"

using namespace bingo::log::handlers;

file_logger_every_day::file_logger_every_day(const char* directory, const char* filename):
		logger(){
	d_ = directory;
	f_ = filename;
}

file_logger_every_day::~file_logger_every_day(){

};

void file_logger_every_day::log_out(string& msg){

	try {

		{
			// lock part field.
			mutex::scoped_lock lock(mu_);

			// Whether LOG_DIRECTORY is exist.
			if(access(d_.c_str(), 0) == -1){
				// the directory isn't exist.
				// make new directory.
				mkdir(d_.c_str(), S_IRWXU + S_IRGRP + S_IXGRP + S_IXOTH);
			}
		}


		// Get filename.
		string filename(d_.c_str());
		filename.append("/");
		filename.append(f_.c_str());

		// Get file's postfix.
		get_filename_postfix(filename);

		{
			// lock part field.
			mutex::scoped_lock lock(mu_);

			// Whether file is exist.
			if(access(filename.c_str(), 0) == -1){
				// the file isn't exist.
				// make new file.

				ofstream fs(filename.c_str());
				fs << msg.c_str() << endl;
				fs.close();

			}else{

				ofstream fs(filename.c_str(), ios_base::app);
				fs << msg.c_str() << endl;
				fs.close();
			}
		}


	}catch(std::exception& err){
	}
}

void file_logger_every_day::get_filename_postfix(string& filename){
	filename.append(".");

	date now = day_clock::local_day();
	int year = now.year();
	int month = now.month();
	int day = now.day();
	char cnow[9];
	memset(&cnow[0], 0x00, 9);
	sprintf(&cnow[0], "%4d%02d%02d", year, month, day);
	filename.append(cnow);
}



