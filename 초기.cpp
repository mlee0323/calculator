#include "std_lib_facilities.h"
#include <cmath>

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
	void ignore(char c); //기호형 c없음 char c
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

Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
	case '^':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		cin.putback(ch); // cin.unget()
		double val;
		cin >> val;
		return Token(number, val);
	}
	default:
		if (isalpha(ch)) {
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
			cin.putback(ch);  // cin.unget() cin.putback(ch)
			if (s == "let") return Token(let);
			if (s == "quit") return Token(quit);
			if (s == "sqrt") return Token(sqrt_func);
			if (s == "sin") return Token(sin_func);
			if (s == "cos") return Token(cos_func);
			if (s == "tan") return Token(tan_func);
			return Token(name, s);
		}
		error("Bad token");
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch = 0; //=0 없음 
	while (cin >> ch)
		if (ch == c) return;
}

struct Variable {
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) { }
};

vector<Variable> names;

Variable pi = { "pi", 3.14159 };
Variable e = { "e", 2.71828 };


double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;

	error("get: undefined name ", s);
}

void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

Token_stream ts;

double expression();
double advanced();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')') error("'(' expected");
		return d;
	}
	case '-':
		return -primary();
	case '+':
		return primary();
	case number:
		return t.value;
	case name:
		return get_value(t.name);
	case sqrt_func:
	{
		t = ts.get();
		if (t.kind != '(')
			error("'(' expected after sqrt");
		double d = expression();
		if (d < 0)
			error("sqrt() function cannot be applied to negative numbers");
		t = ts.get();
		if (t.kind != ')')
			error("')' expected after sqrt");
		return sqrt(d);
	}
	case sin_func:
	{
		t = ts.get();
		if (t.kind != '(')
			error("'(' expected after sin");
		double d = expression();
		t = ts.get();
		if (t.kind != ')')
			error("')' expected after sin");
		return sin(d);
	}
	case cos_func:
	{
		t = ts.get();
		if (t.kind != '(')
			error("'(' expected after cos");
		double d = expression();
		t = ts.get();
		if (t.kind != ')')
			error("')' expected after cos");
		return cos(d);
	}
	case tan_func:
	{
		t = ts.get();
		if (t.kind != '(')
			error("'(' expected after tan");
		double d = expression();
		t = ts.get();
		if (t.kind != ')')
			error("')' expected after tan");
		return tan(d);
	}
	default:
		error("primary expected");
	}
}

double term()
{
	double left = advanced();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= advanced();
			break;
		case '/':
		{
			double d = advanced();
			if (d == 0) error("divide by zero");
			left /= d;
			break;
		}
		default:
			ts.putback(t);
			return left;
		}
	}
}

double advanced()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		if (t.kind == '^') {
			double exponent = advanced();
			return pow(left, exponent);
		}
		else {
			ts.putback(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.putback(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != name) error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d));
	return d;
}


double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	default:
		ts.putback(t);
		return expression();
	}
}



void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.putback(t);
		cout << result << statement() << endl;  //advanced
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}


int main()
try {
	names.push_back(pi);
	names.push_back(e);
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	keep_window_open();
	return 1;
}
catch (...) {
	cerr << "exception\n";
	keep_window_open();
	return 2;
}
