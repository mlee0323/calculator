#include "calc.h"
#include "std_lib_facilities.h"

Calculator::Calculator()
{
	names.push_back(pi);
	names.push_back(e);
}



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
		cin.putback(ch); 
		double val;
		cin >> val;
		return Token(number, val);
	}
	default:
		if (isalpha(ch)) {
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch;
			cin.putback(ch); 
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

Token_stream ts;

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch = 0; 
	while (cin >> ch)
		if (ch == c) return;
}



double Calculator::get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;

	error("get: undefined name ", s);

}
void Calculator::set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

bool Calculator::is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

double Calculator::primary()
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
double Calculator::term()
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
double Calculator::advanced()
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
double Calculator::expression()
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
double Calculator::declaration()
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
double Calculator::statement()
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
void Calculator::clean_up_mess()
{
	ts.ignore(print);
}
void Calculator::calculate()
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

