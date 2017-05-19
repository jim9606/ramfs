#include <time.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct cc {
	char c[100];
}d;

struct dd {
	char c[5];
};
struct c2 {
	cc c;
}d2;
int main() {
	strcpy(d.c, "12345678901234567890");
	dd *p = (dd*)((char*)&d + 3);
	cout << p->c;
	return 0;
}