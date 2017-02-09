/*
 * xml_parser.cpp
 *
 *  Created on: 2016-2-20
 *      Author: root
 */

#include "xml_parser.h"

using namespace bingo::configuration::xml;

xml_parser::xml_parser(bool is_pretty) : is_pretty_(is_pretty){
	node_ = 0;
}

xml_parser::~xml_parser(){
	if(node_ != 0) delete node_;
}

bool xml_parser::read(const char* filepath){
	try{
		pt::read_xml(filepath, pt_, pt::xml_parser::no_comments);
		return true;
	}
	catch(boost::exception& e){
		e_what_.err_no(ERROR_TYPE_CONFIG_READ_FILE_FAIL);
		e_what_.err_message(current_exception_cast<std::exception>()->what());

		return false;
	}
}

bool xml_parser::read(stringstream& stream){
	try{
		pt::read_xml(stream, pt_, pt::xml_parser::no_comments);
		return true;
	}
	catch(boost::exception& e){
		e_what_.err_no(ERROR_TYPE_CONFIG_READ_STREAM_FAIL);
		e_what_.err_message(current_exception_cast<std::exception>()->what());

		return false;
	}
}


bool xml_parser::get_value(const char* pt_path, string& out_value){

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

bool xml_parser::get_attr(const char* pt_path, const char* attrname, string& out_value){

	bool is_find = false;

	pt::ptree pt = pt_.get_child(pt_path);
	foreach_(pt::ptree::value_type &v1, pt){
		 if(v1.first == "<xmlattr>"){
			 foreach_(pt::ptree::value_type &vAttr, v1.second){
				 if(vAttr.first.compare(attrname) == 0){
					 out_value = vAttr.second.data();
					 is_find = true;
					 break;
				 }
			 }
			 if(is_find) break;
		 }
	}
	if(!is_find) {
		e_what_.err_no(ERROR_TYPE_CONFIG_GET_ATTRIBUTE_FAIL);
		e_what_.err_message("attribute no exist!");
		return false;
	}else
		return true;

}

node* xml_parser::get_node(const char* pt_path){
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


void xml_parser::write(stringstream& stream, node* n){

	pt::ptree wtree;
	make_ptree(n, wtree);

	if(is_pretty_){
		boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
		pt::write_xml(stream, wtree, settings);
	}else
		pt::write_xml(stream, wtree);
}

void xml_parser::write(const char* file, node* n){

	pt::ptree wtree;
	make_ptree(n, wtree);

	if(is_pretty_){
		boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);
		write_xml(file, wtree, std::locale(), settings);
	}else
		write_xml(file, wtree, std::locale());
}

void xml_parser::parse_ptree(node* n, pt::ptree& tree){
	n->value.append(tree.get_value<string>().c_str());

	foreach_(pt::ptree::value_type &v1, tree){
		 if(v1.first == "<xmlattr>"){
			 foreach_(pt::ptree::value_type &vAttr, v1.second){
				 // first do it.
				 string name(vAttr.first);
				 string value(vAttr.second.data());

				 n->attrs.push_back(new node_attr(name.c_str(),value.c_str()));
			 }
		 }else{
			 // First node do it.
			 string name(v1.first);
			 node* child = new node(name.c_str());

			 parse_ptree(child, v1.second);

			 n->child.push_back(child);
		 }

	}
}

void xml_parser::make_ptree(node* n, pt::ptree& tree){

	pt::ptree& addnode = tree.add(n->name, n->value);


	if(n->attrs.size() > 0){

		pt::ptree& addattr = addnode.add("<xmlattr>","");
		foreach_(node_attr& k, n->attrs.collection()){
			// Create attribute node.
			pt::ptree::value_type tvalue(pair<string, pt::ptree>(k.name,pt::ptree()));
			tvalue.second.put_value(k.value);
			addattr.push_back(tvalue);
		}
	}

	if(n->child.size() > 0){
		// Have child, then node value is ''.
		addnode.put_value("");

		foreach_(node& m, n->child.collection()){
			make_ptree(&m, addnode);
		}
	}
}


bingo::error_what& xml_parser::err(){
	return e_what_;
}
