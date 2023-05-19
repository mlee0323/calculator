#include "calc.h"
#include "std_lib_facilities.h"



int main()
try {

	Calculator c;
	c.calculate();
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
