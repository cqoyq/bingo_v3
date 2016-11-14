/*
 * parse_handler.h
 *
 *  Created on: 2016-2-19
 *      Author: root
 */

#ifndef BINGO_CONFIG_JSON_PARSER_HEADER_H_
#define BINGO_CONFIG_JSON_PARSER_HEADER_H_

#include <iostream>

#include "bingo/foreach_.h"
#include "bingo/error_what.h"
#include "bingo/config/node.h"
using bingo::config::node;

#include <boost/property_tree/json_parser.hpp>
namespace pt = boost::property_tree;
using namespace boost;

#include <boost/exception/all.hpp>
#include <boost/current_function.hpp>

namespace bingo { namespace config { namespace json {

// Json parse without attribute
class json_parser {
public:
	json_parser(bool is_pretty = false);
	virtual ~json_parser();

	// Read data from file, fail return false, then check err().
	bool read(const char* filepath);

	// Read data from steam, fail return false, then check err().
	bool read(stringstream& stream);


	// Get value, fail return false, then check err().
	bool get_value(const char* pt_path, string& out_value);

	// Get node, fail return 0, then check err().
	node* get_node(const char* pt_path);

	// Return error.
	error_what& err();


	// Write to stream.
	void write(stringstream& stream, node* n);

	// Write to file.
	void write(const char* file, node* n);

protected:
	void parse_ptree(node* n, pt::ptree& tree);

	void make_ptree(node* n, pt::ptree& tree);

private:
	pt::ptree pt_;
	node* node_;

	bool is_pretty_;
	error_what e_what_;
};

} } }


#endif /* BINGO_CONFIG_JSON_PARSER_HEADER_H_ */
