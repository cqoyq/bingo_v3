/*
 * mem_guard.h
 *
 *  Created on: 2016-1-30
 *      Author: root
 */

#ifndef BINGO_MEM_GUARD_HEADER_H_
#define BINGO_MEM_GUARD_HEADER_H_

#include "string.h"
#include "error_what.h"

namespace bingo {

//#define error_copy_data_exceed_max_size_message "the string_ex copy data is exceed max size!"
//#define error_append_data_exceed_max_size_message "the string_ex append data is exceed max size!"
//#define error_change_length_exceed_max_size_message "the string_ex change length is exceed max size!"

template<typename T>
class mem_guard{
private:
	char data_[sizeof(T)];

	size_t size_;
	size_t length_;

public:
	mem_guard()
		: size_(sizeof(T)),
		  length_(0)
	{
		memset(&data_[0], 0x00, size_);
	}

	void clear(){
		memset(&data_[0], 0x00, size_);
		length_ = 0;
	}

	// Get pointer to T.
	T* object(){
		return (T*)data_;
	}

	size_t& size(){
		return size_;
	}

	// Return header pointer of block.
	char* header(){
		return data_;
	}

	// Return length of data.
	size_t& length(){
		return length_;
	}

	// Return current pointer in block, value is 0 if arrive end of data_.
	char* current() const{
		return (length_ == size_)? 0: const_cast<char*>(data_ + length_);
	}

	// Copy data into block, success return 0, fail return -1.
	int copy(const char* data, size_t data_size, error_what& e_what){

		if(data_size > size_){

			e_what.err_no(ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_COPY);
			e_what.err_message("the string_ex copy data is exceed max size!");

			return -1;
		}

		memcpy(data_, data, data_size);
		length_ = data_size;

		return 0;
	}

	// Copy data into block, success return 0, fail return -1.
	int copy(const unsigned char* data, size_t data_size, error_what& e_what){

		if(data_size > size_){

			e_what.err_no(ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_COPY);
			e_what.err_message("the copy data is exceed max size!");
			return -1;
		}

		memcpy(data_, data, data_size);
		length_ = data_size;

		return 0;
	}

	// Append data into block, success return 0, fail return -1.
	int append(const char* data, size_t data_size, error_what& e_what){

		if((length_ + data_size) > size_){

			e_what.err_no(ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_APPEND);
			e_what.err_message("the append data is exceed max size!");

			return -1;
		}

		memcpy(data_ + length_, data, data_size);
		length_ += data_size;

		return 0;
	}

	// Check remain space in block, return true so pass.
	bool check_block_remain_space(size_t& from_data_size){

		if((length_ + from_data_size) > size_)
			return false;
		else
			return true;
	}



	// Change length_
	int change_length(size_t len, error_what& e_what){
		if((length_ + len) > size_){
			e_what.err_no(ERROR_TYPE_MEMGUARD_DATA_EXCEED_MAX_SIZE_WHEN_CHANGE);
			e_what.err_message("the change data is exceed max size!");

			return -1;
		}

		length_ += len;

		return 0;
	}

};

}


#endif /* BINGO_MEM_GUARD_HEADER_H_ */
