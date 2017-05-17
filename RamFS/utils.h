#pragma once
#include<stdio.h>

void logerr(const char* tag, const char* msg);
void printmem(void* p, size_t len, FILE *target = stderr);
