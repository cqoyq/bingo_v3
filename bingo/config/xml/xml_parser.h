/*
 * xml_parser.h
 *
 *  Created on: 2016-2-19
 *      Author: root
 */

#ifndef BINGO_CONFIG_XML_PARSER_HEADER_H_
#define BINGO_CONFIG_XML_PARSER_HEADER_H_

#include "bingo/foreach_.h"
#include "bingo/error_what.h"
#include "bingo/config/node.h"
using bingo::config::node;

#include <boost/exception/all.hpp>
#include <boost/current_function.hpp>

#include <boost/property_tree/xml_parser.hpp>
namespace pt = boost::property_tree;
using namespace boost;

namespace bingo { namespace config { namespace xml {

class xml_parser {
public:
	xml_parser(bool is_pretty = false);
	virtual ~xml_parser();

	// Read data from file, fail return false, then check err().
	bool read(const char* filepath);

	// Read data from steam, fail return false, then check err().
	bool read(stringstream& stream);

	// Get value, fail return false, then check err().
	bool get_value(const char* pt_path, string& out_value);

	// Get attribute, fail return false, then check err().
	bool get_attr(const char* pt_path, const char* attrname, string& out_value);

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


#endif /* BINGO_CONFIG_XML_PARSER_HEADER_H_ */
