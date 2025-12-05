#pragma once
#include <string>
#include <exception>

/* Base exception all other exceptions defined throughout this code inherit from. */
class Converter_Exception : public std::exception {
public:
	Converter_Exception(std::string _str) :str(_str) {};

	virtual const char* what() const throw()
	{
		return str.c_str();
	}
private:
	std::string str;
};

class Failed_Main_Creation_Exception : public Converter_Exception {
public:
	Failed_Main_Creation_Exception(std::string _str) : Converter_Exception{ _str } {};
};