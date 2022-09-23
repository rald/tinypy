tvm *_tinit(void) {
	int i;
	tvm *tp = malloc(sizeof(tvm));
	tp->cur = 0;
	tp->jmp = 0;
	tp->ex = None;
	tp->root = list(0);
	for (i=0; i<256; i++) { tp->chars[i][0]=i; }
	tgc_init(tp);
	tp->_regs = list(tp);
	for (i=0; i<REGS; i++) { set(tp,tp->_regs,None,None); }
	tp->builtins = dict(tp);
	tp->modules = dict(tp);
	tp->_params = list(tp);
	for (i=0; i<FRAMES; i++) { set(tp,tp->_params,None,list(tp)); }
	set(tp,tp->root,None,tp->builtins);
	set(tp,tp->root,None,tp->modules);
	set(tp,tp->root,None,tp->_regs);
	set(tp,tp->root,None,tp->_params);
	set(tp,tp->builtins,string("MODULES"),tp->modules);
	set(tp,tp->modules,string("BUILTINS"),tp->builtins);
	set(tp,tp->builtins,string("BUILTINS"),tp->builtins);
	tp->regs = tp->_regs.list.val->items;
	full(tp);
	return tp;
}
void deinit(TP) {
	while (tp->root.list.val->len) {
		_list_pop(tp,tp->root.list.val,0,"deinit");
	}
	full(tp); full(tp);
	tdelete(tp,tp->root);
	tgc_deinit(tp);
	tfree(tp);
}
void frame(TP,obj globals,code *codes,obj *ret_dest) {
	frame_ f;
	f.globals = globals;
	f.codes = codes;
	f.cur = f.codes;
	f.jmp = 0;
	f.regs = (tp->cur <= 0?tp->regs:tp->frames[tp->cur].regs+tp->frames[tp->cur].cregs);
	f.ret_dest = ret_dest;
	f.lineno = 0;
	f.line = string("");
	f.name = string("?");
	f.fname = string("?");
	f.cregs = 0;
	if (f.regs+256 >= tp->regs+REGS || tp->cur >= FRAMES-1) { traise(,"frame: stack overflow %d",tp->cur); }
	tp->cur += 1;
	tp->frames[tp->cur] = f;
}
void _traise(TP,obj e) {
	if (!tp || !tp->jmp) {
		printf("\nException:\n%s\n",STR(e));
		exit(-1);
		return;
	}
	if (e.type != NONE) { tp->ex = e; }
	grey(tp,e);
	longjmp(tp->buf,1);
}
void print_stack(TP) {
	printf("\n");
	int i;
	for (i=0; i<=tp->cur; i++) {
		if (!tp->frames[i].lineno) { continue; }
		printf("File \"%s\", line %d, in %s\n  %s\n",
			STR(tp->frames[i].fname),tp->frames[i].lineno,STR(tp->frames[i].name),STR(tp->frames[i].line));
	}
	printf("\nException:\n%s\n",STR(tp->ex));
}
void handle(TP) {
	int i;
	for (i=tp->cur; i>=0; i--) {
		if (tp->frames[i].jmp) { break; }
	}
	if (i >= 0) {
		tp->cur = i;
		tp->frames[i].cur = tp->frames[i].jmp;
		tp->frames[i].jmp = 0;
		return;
	}
	print_stack(tp);
	exit(-1);
}
void _call(TP,obj *dest, obj fnc, obj params) {
	if (fnc.type == DICT) {
		return _call(tp,dest,get(tp,fnc,string("__call__")),params);
	}
	if (fnc.type == FNC && !(fnc.fnc.ftype&1)) {
		*dest = _tcall(tp,fnc);
		grey(tp,*dest);
		return;
	}
	if (fnc.type == FNC) {
		frame(tp,fnc.fnc.val->globals,fnc.fnc.fval,dest);
		if ((fnc.fnc.ftype&2)) {
			tp->frames[tp->cur].regs[0] = params;
			_list_insert(tp,params.list.val,0,fnc.fnc.val->self);
		} else {
			tp->frames[tp->cur].regs[0] = params;
		}
		return;
	}
	tparams_v(tp,1,fnc); print(tp);
	traise(,"call: %s is not callable",STR(fnc));
}
void treturn(TP, obj v) {
	obj *dest = tp->frames[tp->cur].ret_dest;
	if (dest) { *dest = v; grey(tp,v); }
	memset(tp->frames[tp->cur].regs,0,tp->frames[tp->cur].cregs*sizeof(obj));
	tp->cur -= 1;
}
enum {
	IEOF,IADD,ISUB,IMUL,IDIV,IPOW,IAND,IOR,ICMP,IGET,ISET,
	INUMBER,ISTRING,IGGET,IGSET,IMOVE,IDEF,IPASS,IJUMP,ICALL,
	IRETURN,IIF,IDEBUG,IEQ,ILE,ILT,IDICT,ILIST,INONE,ILEN,
	ILINE,IPARAMS,IIGET,IFILE,INAME,INE,IHAS,IRAISE,ISETJMP,
	IMOD,ILSH,IRSH,IITER,IDEL,IREGS,
	ITOTAL
};
#define VA ((int)e.regs.a)
#define VB ((int)e.regs.b)
#define VC ((int)e.regs.c)
#define RA regs[e.regs.a]
#define RB regs[e.regs.b]
#define RC regs[e.regs.c]
#define UVBC (unsigned short)(((VB<<8)+VC))
#define SVBC (short)(((VB<<8)+VC))
#define GA grey(tp,RA)
#define SR(v) f->cur = cur; return(v);
int step(TP) {
	frame_ *f = &tp->frames[tp->cur];
	obj *regs = f->regs;
	code *cur = f->cur;
	while(1) {
	code e = *cur;
	switch (e.i) {
		case IEOF: treturn(tp,None); SR(0); break;
		case IADD: RA = add(tp,RB,RC); break;
		case ISUB: RA = sub(tp,RB,RC); break;
		case IMUL: RA = mul(tp,RB,RC); break;
		case IDIV: RA = tdiv(tp,RB,RC); break;
		case IPOW: RA = tpow(tp,RB,RC); break;
		case IAND: RA = and(tp,RB,RC); break;
		case IOR:  RA = or(tp,RB,RC); break;
		case IMOD:  RA = mod(tp,RB,RC); break;
		case ILSH:  RA = lsh(tp,RB,RC); break;
		case IRSH:  RA = rsh(tp,RB,RC); break;
		case ICMP: RA = number(cmp(tp,RB,RC)); break;
		case INE: RA = number(cmp(tp,RB,RC)!=0); break;
		case IEQ: RA = number(cmp(tp,RB,RC)==0); break;
		case ILE: RA = number(cmp(tp,RB,RC)<=0); break;
		case ILT: RA = number(cmp(tp,RB,RC)<0); break;
		case IPASS: break;
		case IIF: if (bool(tp,RA)) { cur += 1; } break;
		case IGET: RA = get(tp,RB,RC); GA; break;
		case IITER:
			if (RC.number.val < len(tp,RB).number.val) {
				RA = iter(tp,RB,RC); GA;
				RC.number.val += 1;
				cur += 1;
			}
			break;
		case IHAS: RA = has(tp,RB,RC); break;
		case IIGET: iget(tp,&RA,RB,RC); break;
		case ISET: set(tp,RA,RB,RC); break;
		case IDEL: del(tp,RA,RB); break;
		case IMOVE: RA = RB; break;
		case INUMBER:
			RA = number(*(num*)(*++cur).string.val);
			cur += sizeof(num)/4;
			continue;
		case ISTRING:
			RA = string_n((*(cur+1)).string.val,UVBC);
			cur += (UVBC/4)+1;
			break;
		case IDICT: RA = dict_n(tp,VC/2,&RB); break;
		case ILIST: RA = list_n(tp,VC,&RB); break;
		case IPARAMS: RA = tparams_n(tp,VC,&RB); break;
		case ILEN: RA = len(tp,RB); break;
		case IJUMP: cur += SVBC; continue; break;
		case ISETJMP: f->jmp = cur+SVBC; break;
		case ICALL: _call(tp,&RA,RB,RC); cur++; SR(0); break;
		case IGGET:
			if (!iget(tp,&RA,f->globals,RB)) {
				RA = get(tp,tp->builtins,RB); GA;
			}
			break;
		case IGSET: set(tp,f->globals,RA,RB); break;
		case IDEF:
			RA = def(tp,(*(cur+1)).string.val,f->globals);
			cur += SVBC; continue;
			break;
		case IRETURN: treturn(tp,RA); SR(0); break;
		case IRAISE: _traise(tp,RA); SR(0); break;
		case IDEBUG:
			tparams_v(tp,3,string("DEBUG:"),number(VA),RA); print(tp);
			break;
		case INONE: RA = None; break;
		case ILINE:
			f->line = string_n((*(cur+1)).string.val,VA*4-1);
			cur += VA; f->lineno = UVBC;
			break;
		case IFILE: f->fname = RA; break;
		case INAME: f->name = RA; break;
		case IREGS: f->cregs = VA; break;
		default: traise(0,"step: invalid instruction %d",e.i); break;
	}
	cur += 1;
	}
	SR(0);
}
void run(TP,int cur) {
	if (tp->jmp) { traise(,"run(%d) called recusively",cur); }
	tp->jmp = 1; if (setjmp(tp->buf)) { handle(tp); }
	while (tp->cur >= cur && step(tp) != -1);
	tp->cur = cur-1; tp->jmp = 0;
}
obj call(TP, char *mod, char *fnc, obj params) {
	obj tmp;
	obj r = None;
	tmp = get(tp,tp->modules,string(mod));
	tmp = get(tp,tmp,string(fnc));
	_call(tp,&r,tmp,params);
	run(tp,tp->cur);
	return r;
}
obj import(TP,char *fname, char *name, void *codes) {
	if (!((fname && strstr(fname,".tpc")) || codes)) {
		return call(tp,"py2bc","import_fname",tparams_v(tp,2,string(fname),string(name)));
	}
	obj code = None;
	if (!codes) {
		tparams_v(tp,1,string(fname));
		code = load(tp);
		codes = code.string.val;
	} else {
		code = data(tp,codes);
	}
	obj g = dict(tp);
	set(tp,g,string("__name__"),string(name));
	set(tp,g,string("__code__"),code);
	frame(tp,g,codes,0);
	set(tp,tp->modules,string(name),g);
	if (!tp->jmp) { run(tp,tp->cur); }
	return g;
}
obj exec_(TP) {
	obj code = OBJ();
	obj globals = OBJ();
	frame(tp,globals,(void*)code.string.val,0);
	return None;
}
obj import_(TP) {
	obj mod = OBJ();
	if (has(tp,tp->modules,mod).number.val) {
		return get(tp,tp->modules,mod);
	}
	char *s = STR(mod);
	obj r = import(tp,STR(add(tp,mod,string(".tpc"))),s,0);
	return r;
}
#define IB(a,b) set(tp,ctx,string(a),fnc(tp,b))
obj builtins(TP) {
	obj ctx = tp->builtins;
	IB("print",print); IB("range",range); IB("min",min);
	IB("max",max); IB("bind",bind); IB("copy",copy);
	IB("import",import_); IB("len",len_); IB("assert",assert);
	IB("str",str2); IB("float",tfloat); IB("system",tsystem);
	IB("istype",istype); IB("chr",chr); IB("save",save);
	IB("load",load); IB("fpack",fpack); IB("abs",tabs);
	IB("int",tint); IB("exec",exec_); IB("exists",exists);
	IB("mtime",mtime); IB("number",tfloat); IB("round",tround);
	IB("ord",ord); IB("merge",merge);
	return ctx;
}
void args(TP,int argc, char *argv[]) {
	obj self = list(tp);
	int i;
	for (i=1; i<argc; i++) { _list_append(tp,self.list.val,string(argv[i])); }
	set(tp,tp->builtins,string("ARGV"),self);
}
obj tmain(TP,char *fname, void *code) {
	return import(tp,fname,"__main__",code);
}
obj compile(TP, obj text, obj fname) {
	return call(tp,"BUILTINS","compile",tparams_v(tp,2,text,fname));
}
obj exec(TP,obj code, obj globals) {
	obj r=None;
	frame(tp,globals,(void*)code.string.val,&r);
	run(tp,tp->cur);
	return r;
}
obj eval(TP, char *text, obj globals) {
	obj code = compile(tp,string(text),string("<eval>"));
	return exec(tp,code,globals);
}
tvm *tinit(int argc, char *argv[]) {
	tvm *tp = _tinit();
	builtins(tp);
	args(tp,argc,argv);
	compiler(tp);
	return tp;
}
