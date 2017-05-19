#include <time.h>
#include <stdint.h>
#include <iostream>

using namespace std;

int main() {
	uint8_t t = 0b00000001,p;
	p = ~t;
	cout << (int)(uint8_t)(p | ~p);
	return 0;
}