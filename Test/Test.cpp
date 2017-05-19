#include <time.h>
#include <stdint.h>
#include <iostream>

using namespace std;

int main() {
	int t = 5;
	int &p = t;
	p = 6;
	cout << t;
	return 0;
}