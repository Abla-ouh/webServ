#pragma once
#include "iostream"
#include "exception"

using namespace std;

class noSemiColonEnd : public exception
{
	public:
		const char* what() const throw(){return ("Line not ended with semi-colon\n");};
};

class unvalidArgument : public exception
{
	public:
		const char* what() const throw(){return ("invalid number of arguments\n");};
};

class unvalidDirective : public exception
{
	public:
		const char* what() const throw(){return ("invalid directive\n");};
};

class unvalidDigit : public exception
{
	public:
		const char* what() const throw(){return ("unvalid Digit\n");};
};

class unvalidRange : public exception
{
	public:
		const char* what() const throw(){return ("Too Low Or Too High Number\n");};
};

class curlyBracesError : public exception
{
	public:
		const char* what() const throw(){return ("Error In Curly Braces\n");};
};

class failedOpenFile : public exception
{
	public:
		const char* what() const throw(){return ("failed open file\n");};
};

class unvalidHost : public exception
{
	public:
		const char* what() const throw(){return ("Unvalid host\n");};
};

class unknownError : public exception
{
	public:
		const char* what() const throw(){return ("unknown Error\n");};
};

class unvalidConfigFile : public exception
{
	public:
		const char* what() const throw(){return ("unvalid Config File\n");};
};
