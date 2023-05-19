#ifndef CALC_H
#define CALC_H

#include <string>
#include <vector>
#include "std_lib_facilities.h"

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string n) :kind(ch), value(0), name(n) {}
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }
	Token get();
	void putback(Token t) { buffer = t; full = true; }
	void ignore(char c); //��ȣ�� c���� char c
};


const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
const char sqrt_func = 'S';
const char sin_func = 's';
const char cos_func = 'c';
const char tan_func = 't';



class Calculator
{
public:
	Calculator();
	void calculate();
private:
	struct Variable {
		string name;
		double value;
		Variable(string n, double v) :name(n), value(v) { }
	};

	vector<Variable> names;

	Variable pi = { "pi", 3.14159 };
	Variable e = { "e", 2.71828 };


	double get_value(string s);

	void set_value(string s, double d);

	bool is_declared(string s);

	
	//double expression();
	//double advanced();
	double primary();
	double term();
	double advanced();
	double expression();
	double declaration();
	double statement();
	void clean_up_mess();

	const string prompt = "> ";
	const string result = "= ";
};
extern Token_stream ts;


#endif  // CALC_H