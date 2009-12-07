#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include <sqpcheader.h>
#include <sqvm.h>
#include <sqstring.h>
#include <sqtable.h>
#include <sqarray.h>
#include <sqfuncproto.h>
#include <sqclosure.h>
#include <sqclass.h>

#include <squirrel.h>

extern "C" {

#ifndef u8
#define u8 unsigned char
#endif

#ifndef i32
#define i32 int
#endif

#ifndef i64
#define i64 long long int
#endif

#ifndef u32
#define u32 unsigned i32
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

// Functions.

#define SQA_FUNC(NAME) __sqfunc_##NAME
#define SQA_METHOD(CLASS, NAME) CLASS##__sqmethod_##NAME

#define DSQA_FUNC(NAME) static SQInteger SQA_FUNC(NAME)(HSQUIRRELVM v)
#define DSQA_METHOD(CLASS, NAME) static SQInteger SQA_METHOD(CLASS, NAME)(HSQUIRRELVM v)

}

// Register function.
void _sqal_register(HSQUIRRELVM v, char *name, SQFUNCTION func, char *check_typemask = ".", int check_nparams = 0);
void _sqal_register_constant(HSQUIRRELVM v, char *name, SQFloat vv);
void _sqal_register_constant(HSQUIRRELVM v, char *name, int vv);
//#define sqal_register(NAME, NPARAMS, TYPEMASK) _sqal_register(v, "__sqfunc" #NAME, __sqfunc_##NAME, (TYPEMASK), (NPARAMS))
#define sqal_register_simple(NAME, TYPEMASK) _sqal_register(v, #NAME, __sqfunc_##NAME, (TYPEMASK), SQ_MATCHTYPEMASKSTRING)
#define sqal_register_constant(NAME, VALUE) _sqal_register_constant(v, NAME, VALUE);
