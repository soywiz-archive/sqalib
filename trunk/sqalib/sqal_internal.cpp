#include "sqal_internal.h"

extern "C" {

void _sqal_register(HSQUIRRELVM v, char *name, SQFUNCTION func, char *check_typemask, int check_nparams) {
	sq_pushstring(v, name, -1);
	sq_newclosure(v, func, 0);
	sq_setparamscheck(v, check_nparams, check_typemask);
	sq_setnativeclosurename(v, -1, name);
	sq_createslot(v, -3);
}

}