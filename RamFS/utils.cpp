#include "utils.h"
#include <string.h>

void logerr(const char* tag, const char* msg) {
	fprintf(stderr, "%s : %s\n", tag, msg);
}

void printmem(const char* p, size_t len) {
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
