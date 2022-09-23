obj print(TP) {
	int n = 0;
	obj e;
	LOOP(e)
		if (n) { printf(" "); }
		printf("%s",STR(e));
		n += 1;
	END;
	printf("\n");
	return None;
}
obj bind(TP) {
	obj r = OBJ();
	obj self = OBJ();
	return fnc_new(tp,r.fnc.ftype|2,r.fnc.fval,self,r.fnc.val->globals);
}
obj min(TP) {
	obj r = OBJ();
	obj e;
	LOOP(e)
		if (cmp(tp,r,e) > 0) { r = e; }
	END;
	return r;
}
obj max(TP) {
	obj r = OBJ();
	obj e;
	LOOP(e)
		if (cmp(tp,r,e) < 0) { r = e; }
	END;
	return r;
}
obj copy(TP) {
	obj r = OBJ();
	int type = r.type;
	if (type == LIST) {
		return _list_copy(tp,r);
	} else if (type == DICT) {
		return _dict_copy(tp,r);
	}
	traise(None,"copy(%s)",STR(r));
}
obj len_(TP) {
	obj e = OBJ();
	return len(tp,e);
}
obj assert(TP) {
	int a = NUM();
	if (a) { return None; }
	traise(None,"%s","assert failed");
}
obj range(TP) {
	int a = NUM();
	int b = NUM();
	int c = DEFAULT(number(1)).number.val;
	obj r = list(tp);
	int i;
	for (i=a; i!=b; i+=c) { _list_append(tp,r.list.val,number(i)); }
	return r;
}
obj tsystem(TP) {
	char *s = tSTR();
	int r = system(s);
	return number(r);
}
obj istype(TP) {
	obj v = OBJ();
	char *t = tSTR();
	if (strcmp("string",t) == 0) { return number(v.type == tSTRING); }
	if (strcmp("list",t) == 0) { return number(v.type == LIST); }
	if (strcmp("dict",t) == 0) { return number(v.type == DICT); }
	if (strcmp("number",t) == 0) { return number(v.type == NUMBER); }
	traise(None,"is_type(%s,%s)",STR(v),t);
}
obj tfloat(TP) {
	obj v = OBJ();
	int ord = DEFAULT(number(0)).number.val;
	int type = v.type;
	if (type == NUMBER) { return v; }
	if (type == tSTRING) {
		if (strchr(STR(v),'.')) { return number(atof(STR(v))); }
		return(number(strtol(STR(v),0,ord)));
	}
	traise(None,"tfloat(%s)",STR(v));
}
obj save(TP) {
	char *fname = tSTR();
	obj v = OBJ();
	FILE *f;
	f = fopen(fname,"wb");
	if (!f) { traise(None,"save(%s,...)",fname); }
	fwrite(v.string.val,v.string.len,1,f);
	fclose(f);
	return None;
}
obj load(TP) {
	char *fname = tSTR();
	struct stat stbuf; stat(fname, &stbuf); long l = stbuf.st_size;
	FILE *f;
	f = fopen(fname,"rb");
	if (!f) { traise(None,"load(%s)",fname); }
	obj r = string_t(tp,l);
	char *s = r.string.val;
	fread(s,1,l,f);
	fclose(f);
	return track(tp,r);
}
obj fpack(TP) {
	num v = NUM();
	obj r = string_t(tp,sizeof(num));
	*(num*)r.string.val = v;
	return track(tp,r);
}
obj tabs(TP) {
	return number(fabs(tfloat(tp).number.val));
}
obj tint(TP) {
	return number((long)tfloat(tp).number.val);
}
num _roundf(num v) {
	num av = fabs(v); num iv = (long)av;
	av = (av-iv < 0.5?iv:iv+1);
	return (v<0?-av:av);
}
obj tround(TP) {
	return number(_roundf(tfloat(tp).number.val));
}
obj exists(TP) {
	char *s = tSTR();
	struct stat stbuf;
	return number(!stat(s,&stbuf));
}
obj mtime(TP) {
	char *s = tSTR();
	struct stat stbuf;
	if (!stat(s,&stbuf)) { return number(stbuf.st_mtime); }
	traise(None,"mtime(%s)",s);
}
