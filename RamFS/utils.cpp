#include "utils.h"
#include <string.h>
#include <random>
#include <vector>
#include <string>

using std::default_random_engine;
using std::uniform_int_distribution;
using std::string;
using std::vector;

void logerr(const char* tag, const char* msg) {
	fprintf(stderr, "%s : %s\n", tag, msg);
}

void printmem(const void* po, size_t len) {
	const char* p = (char*)po;
	char ch[9]; ch[8] = '\0';
	for (size_t i = 0; i < len; ++i) {
		fprintf(stderr, "%02X ", *(p+i));
		ch[i % 8] = *(p + i);
		if (ch[i % 8] < 32 || ch[i % 8] > 126)
			if (ch[i % 8] == 0) 
				ch[i % 8] = '.';
			else
				ch[i % 8] = '#';
		if (i % 8 == 7) {
			fprintf(stderr, "\"%s\" ", ch);
			memset(ch, 0, 8);
		}
		if (i % 16 == 15)
			fprintf(stderr, "\n");
	}
	if (len % 8 != 7)
		fprintf(stderr, "\n");
}

void fillbuffer(char buffer[], size_t len) {
	static default_random_engine engine;
	static constexpr char charset[] = "0123456789ABCDEF";
	static uniform_int_distribution<int> dist(0,15);
	for (size_t i = 0; i < len; ++i)
		buffer[i] = charset[dist(engine)];
}

vector<string> split(const string& str, const char* c)
{
	char *cstr, *p;
	vector<string> res;
	if (str.front() != '/') {
		res.push_back("//");
	}
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
