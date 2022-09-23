obj str(TP,obj self) {
	int type = self.type;
	if (type == tSTRING) { return self; }
	if (type == NUMBER) {
		num v = self.number.val;
		if ((fabs(v)-fabs((long)v)) < 0.000001) { return tprintf(tp,"%ld",(long)v); }
		return tprintf(tp,"%f",v);
	} else if(type == DICT) {
		return tprintf(tp,"<dict 0x%x>",self.dict.val);
	} else if(type == LIST) {
		return tprintf(tp,"<list 0x%x>",self.list.val);
	} else if (type == NONE) {
		return string("None");
	} else if (type == DATA) {
		return tprintf(tp,"<data 0x%x>",self.data.val);
	} else if (type == FNC) {
		return tprintf(tp,"<fnc 0x%x>",self.fnc.val);
	}
	return string("<?>");
}
int bool(TP,obj v) {
	switch(v.type) {
		case NUMBER: return v.number.val != 0;
		case NONE: return 0;
		case tSTRING: return v.string.len != 0;
		case LIST: return v.list.val->len != 0;
		case DICT: return v.dict.val->len != 0;
	}
	return 1;
}
obj has(TP,obj self, obj k) {
	int type = self.type;
	if (type == DICT) {
		if (_dict_find(tp,self.dict.val,k) != -1) { return True; }
		return False;
	} else if (type == tSTRING && k.type == tSTRING) {
		char *p = strstr(STR(self),STR(k));
		return number(p != 0);
	} else if (type == LIST) {
		return number(_list_find(tp,self.list.val,k)!=-1);
	}
	traise(None,"has(%s,%s)",STR(self),STR(k));
}
void del(TP,obj self, obj k) {
	int type = self.type;
	if (type == DICT) {
		_dict_del(tp,self.dict.val,k,"del");
		return;
	}
	traise(,"del(%s,%s)",STR(self),STR(k));
}
obj iter(TP,obj self, obj k) {
	int type = self.type;
	if (type == LIST || type == tSTRING) { return get(tp,self,k); }
	if (type == DICT && k.type == NUMBER) {
		return self.dict.val->items[_dict_next(tp,self.dict.val)].key;
	}
	traise(None,"iter(%s,%s)",STR(self),STR(k));
}
obj get(TP,obj self, obj k) {
	int type = self.type;
	if (type == DICT) {
		return _dict_get(tp,self.dict.val,k,"get");
	} else if (type == LIST) {
		if (k.type == NUMBER) {
			int l = len(tp,self).number.val;
			int n = k.number.val;
			n = (n<0?l+n:n);
			return _list_get(tp,self.list.val,n,"get");
		} else if (k.type == tSTRING) {
			if (strcmp("append",STR(k)) == 0) {
				return method(tp,self,append);
			} else if (strcmp("pop",STR(k)) == 0) {
				return method(tp,self,pop);
			} else if (strcmp("index",STR(k)) == 0) {
				return method(tp,self,tindex);
			} else if (strcmp("sort",STR(k)) == 0) {
				return method(tp,self,sort);
			} else if (strcmp("extend",STR(k)) == 0) {
				return method(tp,self,extend);
			} else if (strcmp("*",STR(k)) == 0) {
				tparams_v(tp,1,self); obj r = copy(tp);
				self.list.val->len=0;
				return r;
			}
		} else if (k.type == NONE) {
			return _list_pop(tp,self.list.val,0,"get");
		}
	} else if (type == tSTRING) {
		if (k.type == NUMBER) {
			int l = self.string.len;
			int n = k.number.val;
			n = (n<0?l+n:n);
			if (n >= 0 && n < l) { return string_n(tp->chars[(unsigned char)self.string.val[n]],1); }
		} else if (k.type == tSTRING) {
			if (strcmp("join",STR(k)) == 0) {
				return method(tp,self,join);
			} else if (strcmp("split",STR(k)) == 0) {
				return method(tp,self,split);
			} else if (strcmp("index",STR(k)) == 0) {
				return method(tp,self,str_index);
			} else if (strcmp("strip",STR(k)) == 0) {
				return method(tp,self,strip);
			} else if (strcmp("replace",STR(k)) == 0) {
				return method(tp,self,replace);
			}
		}
	} else if (type == DATA) {
		return self.data.meta->get(tp,self,k);
	}
	if (k.type == LIST) {
		int a,b,l;
		obj tmp;
		l = len(tp,self).number.val;
		tmp = get(tp,k,number(0));
		if (tmp.type == NUMBER) { a = tmp.number.val; }
		else if(tmp.type == NONE) { a = 0; }
		else { traise(None,"%s is not a number",STR(tmp)); }
		tmp = get(tp,k,number(1));
		if (tmp.type == NUMBER) { b = tmp.number.val; }
		else if(tmp.type == NONE) { b = l; }
		else { traise(None,"%s is not a number",STR(tmp)); }
		a = _max(0,(a<0?l+a:a)); b = _min(l,(b<0?l+b:b));
		if (type == LIST) {
			return list_n(tp,b-a,&self.list.val->items[a]);
		} else if (type == tSTRING) {
			obj r = string_t(tp,b-a);
			char *ptr = r.string.val;
			memcpy(ptr,self.string.val+a,b-a); ptr[b-a]=0;
			return track(tp,r);
		}
	}
	traise(None,"get(%s,%s)",STR(self),STR(k));
}
int iget(TP,obj *r, obj self, obj k) {
	if (self.type == DICT) {
		int n = _dict_find(tp,self.dict.val,k);
		if (n == -1) { return 0; }
		*r = self.dict.val->items[n].val;
		grey(tp,*r);
		return 1;
	}
	if (self.type == LIST && !self.list.val->len) { return 0; }
	*r = get(tp,self,k); grey(tp,*r);
	return 1;
}
void set(TP,obj self, obj k, obj v) {
	int type = self.type;
	if (type == DICT) {
		_dict_set(tp,self.dict.val,k,v);
		return;
	} else if (type == LIST) {
		if (k.type == NUMBER) {
			_list_set(tp,self.list.val,k.number.val,v,"set");
			return;
		} else if (k.type == NONE) {
			_list_append(tp,self.list.val,v);
			return;
		} else if (k.type == tSTRING) {
			if (strcmp("*",STR(k)) == 0) {
				tparams_v(tp,2,self,v); extend(tp);
				return;
			}
		}
	} else if (type == DATA) {
		self.data.meta->set(tp,self,k,v);
		return;
	}
	traise(,"set(%s,%s,%s)",STR(self),STR(k),STR(v));
}
obj add(TP,obj a, obj b) {
	if (a.type == NUMBER && a.type == b.type) {
		return number(a.number.val+b.number.val);
	} else if (a.type == tSTRING && a.type == b.type) {
		int al = a.string.len, bl = b.string.len;
		obj r = string_t(tp,al+bl);
		char *s = r.string.val;
		memcpy(s,a.string.val,al); memcpy(s+al,b.string.val,bl);
		return track(tp,r);
	} else if (a.type == LIST && a.type == b.type) {
		tparams_v(tp,1,a); obj r = copy(tp);
		tparams_v(tp,2,r,b); extend(tp);
		return r;
	}
	traise(None,"add(%s,%s)",STR(a),STR(b));
}
obj mul(TP,obj a, obj b) {
	if (a.type == NUMBER && a.type == b.type) {
		return number(a.number.val*b.number.val);
	} else if (a.type == tSTRING && b.type == NUMBER) {
		int al = a.string.len; int n = b.number.val;
		obj r = string_t(tp,al*n);
		char *s = r.string.val;
		int i; for (i=0; i<n; i++) { memcpy(s+al*i,a.string.val,al); }
		return track(tp,r);
	}
	traise(None,"mul(%s,%s)",STR(a),STR(b));
}
obj len(TP,obj self) {
	int type = self.type;
	if (type == tSTRING) {
		return number(self.string.len);
	} else if (type == DICT) {
		return number(self.dict.val->len);
	} else if (type == LIST) {
		return number(self.list.val->len);
	}
	traise(None,"len(%s)",STR(self));
}
int cmp(TP,obj a, obj b) {
	if (a.type != b.type) { return a.type-b.type; }
	switch(a.type) {
		case NONE: return 0;
		case NUMBER: return _sign(a.number.val-b.number.val);
		case tSTRING: {
			int v = memcmp(a.string.val,b.string.val,_min(a.string.len,b.string.len));
			if (v == 0) { v = a.string.len-b.string.len; }
			return v;
		}
		case LIST: {
			int n,v; for(n=0;n<_min(a.list.val->len,b.list.val->len);n++) {
		obj aa = a.list.val->items[n]; obj bb = b.list.val->items[n];
			if (aa.type == LIST && bb.type == LIST) { v = aa.list.val-bb.list.val; } else { v = cmp(tp,aa,bb); }
			if (v) { return v; } }
			return a.list.val->len-b.list.val->len;
		}
		case DICT: return a.dict.val - b.dict.val;
		case FNC: return a.fnc.val - b.fnc.val;
		case DATA: return a.data.val - b.data.val;
	}
	traise(0,"cmp(%s,%s)",STR(a),STR(b));
}
#define OP(name,expr) \
	obj name(TP,obj _a,obj _b) { \
	if (_a.type == NUMBER && _a.type == _b.type) { \
		num a = _a.number.val; num b = _b.number.val; \
		return number(expr); \
	} \
	traise(None,"%s(%s,%s)",#name,STR(_a),STR(_b)); \
}
OP(and,((long)a)&((long)b));
OP(or,((long)a)|((long)b));
OP(mod,((long)a)%((long)b));
OP(lsh,((long)a)<<((long)b));
OP(rsh,((long)a)>>((long)b));
OP(sub,a-b);
OP(tdiv,a/b);
OP(tpow,pow(a,b));
