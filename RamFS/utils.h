#pragma once
#include <stdio.h>

void logerr(const char* tag, const char* msg);

void printmem(const char* p, size_t len);

void fillbuffer(char buffer[],size_t len);