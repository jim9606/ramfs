#include <time.h>
#include <stdint.h>
#include <iostream>

using namespace std;

int main() {
	time_t tt = time(0);
	cout << tt << endl << ctime(&tt) << endl;
	return 0;
}