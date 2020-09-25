/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			string.hpp
 */

#ifndef _STRING_HPP
#define _STRING_HPP

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

template<typename Out>
void split(string &str, char delimiter, Out res);
vector<string> split(string &str, char delimiter);
vector<string> parse_string(string str);

#endif