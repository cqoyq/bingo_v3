/*
 * parse_handler.cpp
 *
 *  Created on: 2016-2-20
 *      Author: root
 */

#include "json_parser.h"

using namespace bingo::configuration::json;

json_parser::json_parser(bool is_pretty) : is_pretty_(is_pretty){
	node_ = 0;
}
json_parser::~json_parser(){
	if(node_ != 0) delete node_;
}

bool json_parser::read(const char* filepath){
	try{
		pt::read_json(filepath, pt_);
		return true;
	}
	catch(boost::exception& e){
		e_what_.err_no(ERROR_TYPE_CONFIG_READ_FILE_FAIL);
		e_what_.err_message(current_exception_cast<std::exception>()->what());

		return false;
	}
}

bool json_parser::read(stringstream& stream){
	try{
		pt::read_json(stream, pt_);
		return true;
	}
	catch(boost::exception& e){
		e_what_.err_no(ERROR_TYPE_CONFIG_READ_STREAM_FAIL);
		e_what_.err_message(current_exception_cast<std::exception>()->what());
		return false;
	}
}


bool json_parser::get_value(const char* pt_path, string& out_value){

	try{
		out_value = pt_.get<string>(pt_path);
		return true;
	}
	catch(boost::exception& e){
		e_what_.err_no(ERROR_TYPE_CONFIG_GET_VALUE_FAIL);
		e_what_.err_message(current_exception_cast<std::exception>()->what());

		return false;
	}
}

node* json_parser::get_node(const char* pt_path){
	if(node_ !=0) {
		delete node_;
		node_ = 0;
	}

	try{
		pt::ptree pt = pt_.get_child(pt_path);

		node_ = new node();
		parse_ptree(node_, pt);

		return node_;
	}
	catch(boost::exception& e){
		e_what_.err_no(ERROR_TYPE_CONFIG_GET_NODE_FAIL);
		e_what_.err_message(current_exception_cast<std::exception>()->what());

		return 0;
	}
}



void json_parser::write(stringstream& stream, node* n){

	pt::ptree wtree;
	make_ptree(n, wtree);

	pt::write_json(stream, wtree, is_pretty_);
}

void json_parser::write(const char* file, node* n){

	pt::ptree wtree;
	make_ptree(n, wtree);

	pt::write_json(file, wtree, std::locale(), is_pretty_);
}

void json_parser::parse_ptree(node* n, pt::ptree& tree){
	n->value.append(tree.get_value<string>().c_str());

	foreach_(pt::ptree::value_type &v1, tree){
		 if(v1.first == "<xmlattr>"){

		 }else{
			 // First node do it.
			 string name(v1.first);
			 node* child = new node(name.c_str());

			 parse_ptree(child, v1.second);

			 n->child.push_back(child);
		 }

	}
}

void json_parser::make_ptree(node* n, pt::ptree& tree){

	pt::ptree& addnode = tree.add(n->name, n->value);

	if(n->child.size() > 0){
		// Have child, then node value is ''.
		addnode.put_value("");

		foreach_(node& m, n->child.collection()){
			make_ptree(&m, addnode);
		}
	}
}

bingo::error_what& json_parser::err(){
	return e_what_;
}

