/*
 * project: 	ISA project - POP3 server
 * author: 		Jan Ondruch, xondru14
 * file:			string.cpp
 */

#ifndef _STRING_CPP
#define _STRING_CPP

#include "string.hpp"


template<typename Out>

/**
 * Splits the string by some delimiter.
 * @param str string input string
 * @param delimiter char delimiter
 * @param result Out template the output result
 */
void split(string &str, char delimiter, Out res) 
{
  stringstream ss;

  ss.str(str);
  string item;

  while (getline(ss, item, delimiter)) {
    *(res++) = item;
  }
}

/**
 * Splits the string by some delimiter.
 * @param s string input string
 * @param delimiter char delimiter
 * @return  vector<string> of the split elements
 */
vector<string> split(string &str, char delimiter)
{
  vector<string> items;

  split(str, delimiter, back_inserter(items));
  return items;
}

/**
 * Parses string and splits it into tokens.
 * @param str string input string to be split
 * @return vector<string> tokens
 */
vector<string> parse_string(string str)
{
	vector<string> tokens = split(str, ' ');
	return tokens;
}

#endif