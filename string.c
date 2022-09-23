obj string_t(TP, int n) {
	obj r = string_n(0,n);
	r.string.info = malloc(sizeof(_string)+n);
	r.string.val = r.string.info->s;
	return r;
}
obj tprintf(TP,char *fmt,...) {
	va_list arg;
	va_start(arg, fmt);
	int l = vsnprintf(NULL, 0, fmt,arg);
	obj r = string_t(tp,l);
	char *s = r.string.val;
	va_end(arg);
	va_start(arg, fmt);
	vsprintf(s,fmt,arg);
	va_end(arg);
	return track(tp,r);
}
int _str_index(obj s, obj k) {
	int i=0;
	while ((s.string.len - i) >= k.string.len) {
		if (memcmp(s.string.val+i,k.string.val,k.string.len) == 0) {
			return i;
		}
		i += 1;
	}
	return -1;
}
obj join(TP) {
	obj delim = OBJ();
	obj val = OBJ();
	int l=0,i;
	for (i=0; i<val.list.val->len; i++) {
		if (i!=0) { l += delim.string.len; }
		l += str(tp,val.list.val->items[i]).string.len;
	}
	obj r = string_t(tp,l);
	char *s = r.string.val;
	l = 0;
	for (i=0; i<val.list.val->len; i++) {
		if (i!=0) {
			memcpy(s+l,delim.string.val,delim.string.len); l += delim.string.len;
		}
		obj e = str(tp,val.list.val->items[i]);
		memcpy(s+l,e.string.val,e.string.len); l += e.string.len;
	}
	return track(tp,r);
}
obj string_slice(TP,obj s, int a, int b) {
	obj r = string_t(tp,b-a);
	char *m = r.string.val;
	memcpy(m,s.string.val+a,b-a);
	return track(tp,r);
}
obj split(TP) {
	obj v = OBJ();
	obj d = OBJ();
	obj r = list(tp);
	int i;
	while ((i=_str_index(v,d))!=-1) {
		_list_append(tp,r.list.val,string_slice(tp,v,0,i));
		v.string.val += i + d.string.len; v.string.len -= i + d.string.len;
	}
	_list_append(tp,r.list.val,string_slice(tp,v,0,v.string.len));
	return r;
}
obj find(TP) {
	obj s = OBJ();
	obj v = OBJ();
	return number(_str_index(s,v));
}
obj str_index(TP) {
	obj s = OBJ();
	obj v = OBJ();
	int n = _str_index(s,v);
	if (n >= 0) { return number(n); }
	traise(None,"str_index(%s,%s)",s,v);
}
obj str2(TP) {
	obj v = OBJ();
	return str(tp,v);
}
obj chr(TP) {
	int v = NUM();
	return string_n(tp->chars[(unsigned char)v],1);
}
obj ord(TP) {
	char *s = tSTR();
	return number(s[0]);
}
obj strip(TP) {
	char *v = tSTR();
	int i, l = strlen(v); int a = l, b = 0;
	for (i=0; i<l; i++) {
		if (v[i] != ' ' && v[i] != '\n' && v[i] != '\t' && v[i] != '\r') {
			a = _min(a,i); b = _max(b,i+1);
		}
	}
	if ((b-a) < 0) { return string(""); }
	obj r = string_t(tp,b-a);
	char *s = r.string.val;
	memcpy(s,v+a,b-a);
	return track(tp,r);
}
obj replace(TP) {
	obj s = OBJ();
	obj k = OBJ();
	obj v = OBJ();
	obj p = s;
	int i,n = 0;
	while ((i = _str_index(p,k)) != -1) {
		n += 1;
		p.string.val += i + k.string.len; p.string.len -= i + k.string.len;
	}
	int l = s.string.len + n * (v.string.len-k.string.len);
	int c;
	obj rr = string_t(tp,l);
	char *r = rr.string.val;
	char *d = r;
	obj z; z = p = s;
	while ((i = _str_index(p,k)) != -1) {
		p.string.val += i; p.string.len -= i;
		memcpy(d,z.string.val,c=(p.string.val-z.string.val)); d += c;
		p.string.val += k.string.len; p.string.len -= k.string.len;
		memcpy(d,v.string.val,v.string.len); d += v.string.len;
		z = p;
	}
	memcpy(d,z.string.val,(s.string.val + s.string.len) - z.string.val);
	return track(tp,rr);
}
