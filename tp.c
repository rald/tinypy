#ifndef COMPILER
#define COMPILER 1
#endif
#include "tp.h"
#include "list.c"
#include "dict.c"
#include "misc.c"
#include "string.c"
#include "builtins.c"
#include "gc.c"
#include "ops.c"
void compiler(TP);
#include "vm.c"
#if COMPILER
#include "bc.c"
void compiler(TP) {
	import(tp,0,"tokenize",tokenize);
	import(tp,0,"parse",parse);
	import(tp,0,"encode",encode);
	import(tp,0,"py2bc",py2bc);
	call(tp,"py2bc","_init",None);
}
#else
void compiler(TP) { }
#endif
