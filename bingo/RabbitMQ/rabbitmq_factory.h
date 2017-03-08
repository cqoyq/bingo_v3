/*
 * rabbitmq_factory.h
 *
 *  Created on: 2016-9-2
 *      Author: root
 */

#ifndef  BINGO_RABBITMQ_RABBITMQ_FACTORY_H_
#define BINGO_RABBITMQ_RABBITMQ_FACTORY_H_

#include "rb_sendor.h"
#include "rb_receiver.h"

#include "bingo/log/log_handler.h"
using namespace bingo::log;

namespace bingo { namespace RabbitMQ {

class rabbitmq_factory {
public:
	rabbitmq_factory();
	virtual ~rabbitmq_factory();

	// Create simple sendor object, if success then return true, otherwise return false.
	// when return false, call err() to check error.
	// call this method in alone thread.
	bool make_simple_sendor(log_handler*& log);

	// Create simple receiver object, if success then return true, otherwise return false.
	// call this method in alone thread.
	bool make_simple_receiver(log_handler*& log, rb_receiver::rev_callback f);


        
                     // Create work sendor object,    
                     bool make_work_sendor(log_handler*& log);
                     
                     // Create work receiver object, 
                     bool make_work_receiver(log_handler*& log, rb_receiver::rev_callback f);


                     
	// Create fail, to show error information.
	error_what& err();


	// Send data to receiver by key.
	void transfer_data_by_key(string& key, char* data, size_t data_size);

	// Send data to receiver by routingkey.
	void transfer_data_by_routingkey(string& routingkey, char* data, size_t data_size);

protected:

private:
   // Sendor and Receiver.
   rb_sendor* 	snd_;
   rb_receiver* 	rev_;

   // Logger output
   log_handler* logger_;

   // Error information.
   error_what e_what_;
};

} }

#endif /* BINGO_RABBITMQ_RABBITMQ_FACTORY_H_ */
