#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#ifndef u8
#define u8 unsigned char
#endif

#ifndef i64
#define i64 long long int
#endif

#ifndef u64
#define u64 unsigned i64
#endif

#if 1
	#define unittest(ss) int exit_code = 0; void main()
	#define unit_assert(v) { printf("Checking...(%s:%d)(" #v ")...", __FILE__, __LINE__); fflush(stdout); if (v) printf("Ok\n"); else { printf("Error\n"); exit_code = -1; } fflush(stdout); }
#else
	#define unittest(ss) void __unittest_##ss()
	#define unit_assert(v)
#endif