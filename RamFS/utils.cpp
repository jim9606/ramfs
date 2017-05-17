#include"utils.h"
#include<string.h>

void logerr(const char* tag, const char* msg) {
	fprintf(stderr, "%s : %s\n", tag, msg);
}

void printmem(void* p, size_t len, FILE *target) {
	for (size_t i = 0; i < len; ++i) {
		fprintf(target, "%02x ", *((char*)p+i));
		if (i % 8 == 0) fprintf(target, "\n");
	}
}
