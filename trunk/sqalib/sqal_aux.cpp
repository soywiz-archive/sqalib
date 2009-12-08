#include "sqal_internal.h"

// Grabbed from sqstd sqstdaux.cpp and slightly modified.

void sqal_printcallstack(HSQUIRRELVM v) {
	SQPRINTFUNCTION pf = sq_geterrorfunc(v);
	if (!pf) return;

	SQStackInfos si;
	SQInteger    i;
	SQFloat      f;
	SQInteger    level;
	SQInteger    seq;
	const SQChar *s, *name = NULL;

	pf(v, _SC("\nCALLSTACK\n"));
	for (level = 1; SQ_SUCCEEDED(sq_stackinfos(v, level, &si)); level++) {
		const SQChar *fn  = _SC("unknown"), *src = _SC("unknown");

		if (si.funcname) fn  = si.funcname;
		if (si.source  ) src = si.source;

		pf(v, _SC("*FUNCTION [%s()] %s line [%d]\n"), fn, src, si.line);
	}

	pf(v, _SC("\nLOCALS\n"));
	for (level = 0; level < 10; level++) {
		for (seq = 0; (name = sq_getlocal(v, level, seq)); seq++) {
			switch (sq_gettype(v, -1)) {
				case OT_NULL         : pf(v, _SC("[%s] NULL\n"         ), name); break;
				case OT_USERPOINTER  : pf(v, _SC("[%s] USERPOINTER\n"  ), name); break;
				case OT_TABLE        : pf(v, _SC("[%s] TABLE\n"        ), name); break;
				case OT_ARRAY        : pf(v, _SC("[%s] ARRAY\n"        ), name); break;
				case OT_CLOSURE      : pf(v, _SC("[%s] CLOSURE\n"      ), name); break;
				case OT_NATIVECLOSURE: pf(v, _SC("[%s] NATIVECLOSURE\n"), name); break;
				case OT_GENERATOR    : pf(v, _SC("[%s] GENERATOR\n"    ), name); break;
				case OT_USERDATA     : pf(v, _SC("[%s] USERDATA\n"     ), name); break;
				case OT_THREAD       : pf(v, _SC("[%s] THREAD\n"       ), name); break;
				case OT_CLASS        : pf(v, _SC("[%s] CLASS\n"        ), name); break;
				case OT_INSTANCE     : pf(v, _SC("[%s] INSTANCE\n"     ), name); break;
				case OT_WEAKREF      : pf(v, _SC("[%s] WEAKREF\n"      ), name); break;
				case OT_INTEGER      : sq_getinteger(v, -1, &i); pf(v, _SC("[%s] %d\n"    ), name, i); break;
				case OT_FLOAT        : sq_getfloat  (v, -1, &f); pf(v, _SC("[%s] %.14g\n" ), name, f); break;
				case OT_STRING       : sq_getstring (v, -1, &s); pf(v, _SC("[%s] \"%s\"\n"), name, s); break;
				case OT_BOOL         : sq_getinteger(v, -1, &i); pf(v, _SC("[%s] %s\n"    ), name, i ? _SC("true") : _SC("false")); break;
				default: assert(0); break;
			}
			sq_pop(v,1);
		}
	}
}

static SQInteger _sqal_aux_printerror(HSQUIRRELVM v) {
	SQPRINTFUNCTION pf = sq_geterrorfunc(v);
	const SQChar *sErr = NULL;

	if (!pf || (sq_gettop(v) <= 0)) return 0;

	if(SQ_SUCCEEDED(sq_getstring(v, 2, &sErr))) {
		pf(v, _SC("\nAN ERROR HAS OCCURED [%s]\n"), sErr);
	} else {
		pf(v, _SC("\nAN ERROR HAS OCCURED [unknown]\n"));
	}
	sqal_printcallstack(v);

	return 0;
}

void _sqal_compiler_error(HSQUIRRELVM v, const SQChar *sErr, const SQChar *sSource, SQInteger line, SQInteger column) {
	SQPRINTFUNCTION pf = sq_geterrorfunc(v); if (!pf) return;

	pf(v, _SC("%s line = (%d) column = (%d) : error %s\n"), sSource, line, column, sErr);
}

void sqal_seterrorhandlers(HSQUIRRELVM v) {
	sq_setcompilererrorhandler(v, _sqal_compiler_error);
	sq_newclosure(v, _sqal_aux_printerror, 0);
	sq_seterrorhandler(v);
}
