/*
 * node.h
 *
 *  Created on: 2016-2-19
 *      Author: root
 */

#ifndef BINGO_CONFIG__NODE_H_
#define BINGO_CONFIG__NODE_H_

#include <exception>
#include <string>
using namespace std;

#include <boost/ptr_container/ptr_vector.hpp>

#include "bingo/foreach_.h"

namespace bingo { namespace configuration {

// Node attribute class
struct node_attr{
	string name;
	string value;
	node_attr(const char* name, const char* value):
		name(name), value(value){}
};

// Node attribute set class
class node_attr_set{
public:
	// Push one attribute, return this.
	node_attr_set& push_back(node_attr* n);

	// Get attribute by index, fail return 0.
	node_attr* operator[](int index);

	// Get attribute by name, fail return 0.
	node_attr* operator[](const char* attr_name);

	// Get size of collection.
	size_t size();

	// Get collection.
	boost::ptr_vector<node_attr>& collection();

private:
	boost::ptr_vector<node_attr> set;
};

struct node;

// Node set class.
class node_set{
public:
	// Push one node to collection, return added node.
	node* push_back(node* n);

	// Get one node by index, fail return 0.
	node* operator[](int index);

	// Get one node by name, fail return 0.
	node* operator[](const char* name);

	// Return size of collection.
	size_t size();

	// Get collection.
	boost::ptr_vector<node>& collection();

private:
	boost::ptr_vector<node> set;
};

// Node information class.
struct node{
	string 						name;
	string 						value;
	node_attr_set			attrs;
	node_set					child;

	node(){}
	node(const char* name, const char* value = ""):
		name(name), value(value){};

	template<typename HANDLER>
	void write(stringstream& stream){
		HANDLER hdr;
		hdr.write(stream, this);
	}

	template<typename HANDLER>
	void write(const char* file){
		HANDLER hdr;
		hdr.write(file, this);
	}

private:

};

}  }


#endif /* BINGO_CONFIG__NODE_H_ */
