#pragma once
#include <stdio.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

void logerr(const char* tag, const char* msg);

void printmem(const void* po, size_t len);

void fillbuffer(char buffer[],size_t len);

vector<string> split(const string& str, const char* c);
