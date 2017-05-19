#include <time.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class testRight {
public:
	testRight() {
		vector.push_back("test");
	}
	void append(testRight temp) {
		vector.push_back(temp.vector.front());
		for (auto item : vector) {
			cout << item << endl;
		}
	}
private:
	vector<string> vector;
};

vector<string> split(const string& str, const char* c)
{
	char *cstr, *p;
	vector<string> res;
	cstr = new char[str.size() + 1];
	strcpy(cstr, str.c_str());
	p = strtok(cstr, c);
	while (p != NULL)
	{
		res.push_back(p);
		p = strtok(NULL, c);
	}
	return res;
}

int main() {
	testRight a;
	testRight b;
	a.append(b);
	//string mystring = "/data/file.txt";
	//vector<string> vector = split(mystring, "/");
	//if (mystring.front() == '/') {
	//	vector.insert(vector.begin(), "/");
	//}
	//for (auto item : vector) {
	//	cout << item << endl;
	//}
	return 0;
}