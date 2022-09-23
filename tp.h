#ifndef H
#define H
#include <setjmp.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#define malloc(x) calloc((x),1)
#define realloc(x,y) realloc(x,y)
#define tfree(x) free(x)
enum {
	NONE,NUMBER,tSTRING,DICT,
	LIST,FNC,DATA,
};
typedef double num;
typedef struct number_ {
	int type;
	num val;
} number_;
typedef struct string_ {
	int type;
	struct _string *info;
	char *val;
	int len;
} string_;
typedef struct list_ {
	int type;
	struct _list *val;
} list_;
typedef struct dict_ {
	int type;
	struct _dict *val;
} dict_;
typedef struct fnc_ {
	int type;
	struct _fnc *val;
	int ftype;
	void *fval;
} fnc_;
typedef struct data_ {
	int type;
	struct _data *info;
	void *val;
	struct meta *meta;
} data_;
typedef union obj {
	int type;
	number_ number;
	struct { int type; int *data; } gci;
	string_ string;
	dict_ dict;
	list_ list;
	fnc_ fnc;
	data_ data;
} obj;
typedef struct _string {
	int gci;
	char s[1];
} _string;
typedef struct _list {
	int gci;
	obj *items;
	int len;
	int alloc;
} _list;
typedef struct item {
	int used;
	int hash;
	obj key;
	obj val;
} item;
typedef struct _dict {
	int gci;
	item *items;
	int len;
	int alloc;
	int cur;
	int mask;
	int used;
} _dict;
typedef struct _fnc {
	int gci;
	obj self;
	obj globals;
} _fnc;
typedef union code {
	unsigned char i;
	struct { unsigned char i,a,b,c; } regs;
	struct { char val[4]; } string;
	struct { float val; } number;
} code;
typedef struct frame_ {
	code *codes;
	code *cur;
	code *jmp;
	obj *regs;
	obj *ret_dest;
	obj fname;
	obj name;
	obj line;
	obj globals;
	int lineno;
	int cregs;
} frame_;
#define GCMAX 4096
#define FRAMES 256
#define REGS 16384
typedef struct tvm {
	obj builtins;
	obj modules;
	frame_ frames[FRAMES];
	obj _params;
	obj params;
	obj _regs;
	obj *regs;
	obj root;
	jmp_buf buf;
	int jmp;
	obj ex;
	char chars[256][2];
	int cur;
	_list *white;
	_list *grey;
	_list *black;
	_dict *strings;
	int steps;
} tvm;
#define TP tvm *tp
typedef struct meta {
	int type;
	obj (*get)(TP,obj,obj);
	void (*set)(TP,obj,obj,obj);
	void (*free)(TP,obj);
} meta;
typedef struct _data {
	int gci;
	meta meta;
} _data;
#define None ((obj){NONE})
#define True number(1)
#define False number(0)
#define STR(v) ((str(tp,(v))).string.val)
void set(TP,obj,obj,obj);
obj get(TP,obj,obj);
obj len(TP,obj);
obj str(TP,obj);
int cmp(TP,obj,obj);
void _traise(TP,obj);
obj tprintf(TP,char *fmt,...);
obj track(TP,obj);
void grey(TP,obj);
#define traise(r,fmt,...) { \
	_traise(tp,tprintf(tp,fmt,__VA_ARGS__)); \
	return r; \
}
#define __params (tp->params)
#define OBJ() (get(tp,__params,None))
inline static obj type(TP,int t,obj v) {
	if (v.type != t) { traise(None,"_type(%d,%s)",t,STR(v)); }
	return v;
}
#define TYPE(t) type(tp,t,OBJ())
#define NUM() (TYPE(NUMBER).number.val)
#define tSTR() (STR(TYPE(tSTRING)))
#define DEFAULT(d) (__params.list.val->len?get(tp,__params,None):(d))
#define LOOP(e) \
	int __l = __params.list.val->len; \
	int __i; for (__i=0; __i<__l; __i++) { \
	(e) = _list_get(tp,__params.list.val,__i,"LOOP");
#define END \
	}
inline static int _min(int a, int b) { return (a<b?a:b); }
inline static int _max(int a, int b) { return (a>b?a:b); }
inline static int _sign(num v) { return (v<0?-1:(v>0?1:0)); }
inline static obj number(num v) { return (obj)(number_){NUMBER,v}; }
inline static obj string(char *v) { return (obj)(string_){tSTRING,0,v,strlen(v)}; }
inline static obj string_n(char *v,int n) {
	return (obj)(string_){tSTRING,0,v,n};
}
#endif
