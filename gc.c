void grey(TP,obj v) {
	if (v.type < tSTRING || (!v.gci.data) || *v.gci.data) { return; }
	*v.gci.data = 1;
	if (v.type == tSTRING || v.type == DATA) {
		_list_appendx(tp,tp->black,v);
		return;
	}
	_list_appendx(tp,tp->grey,v);
}
void follow(TP,obj v) {
	int type = v.type;
	if (type == LIST) {
		int n;
		for (n=0; n<v.list.val->len; n++) {
			grey(tp,v.list.val->items[n]);
		}
	}
	if (type == DICT) {
		int i;
		for (i=0; i<v.dict.val->len; i++) {
			int n = _dict_next(tp,v.dict.val);
			grey(tp,v.dict.val->items[n].key);
			grey(tp,v.dict.val->items[n].val);
		}
	}
	if (type == FNC) {
		grey(tp,v.fnc.val->self);
		grey(tp,v.fnc.val->globals);
	}
}
void reset(TP) {
	int n;
	for (n=0; n<tp->black->len; n++) {
		*tp->black->items[n].gci.data = 0;
	}
	_list *tmp = tp->white; tp->white = tp->black; tp->black = tmp;
}
void tgc_init(TP) {
	tp->white = _list_new();
	tp->strings = _dict_new();
	tp->grey = _list_new();
	tp->black = _list_new();
	tp->steps = 0;
}
void tgc_deinit(TP) {
	_list_free(tp->white);
	_dict_free(tp->strings);
	_list_free(tp->grey);
	_list_free(tp->black);
}
void tdelete(TP,obj v) {
	int type = v.type;
	if (type == LIST) {
		_list_free(v.list.val);
		return;
	} else if (type == DICT) {
		_dict_free(v.dict.val);
		return;
	} else if (type == tSTRING) {
		tfree(v.string.info);
		return;
	} else if (type == DATA) {
		if (v.data.meta && v.data.meta->free) {
			v.data.meta->free(tp,v);
		}
		tfree(v.data.info);
		return;
	} else if (type == FNC) {
		tfree(v.fnc.val);
		return;
	}
	traise(,"tdelete(%s)",STR(v));
}
void collect(TP) {
	int n;
	for (n=0; n<tp->white->len; n++) {
		obj r = tp->white->items[n];
		if (*r.gci.data) { continue; }
		if (r.type == tSTRING) {
			_dict_del(tp,tp->strings,r,"collect");
		}
		tdelete(tp,r);
	}
	tp->white->len = 0;
	reset(tp);
}
void _tgcinc(TP) {
	if (!tp->grey->len) { return; }
	obj v = _list_pop(tp,tp->grey,tp->grey->len-1,"_tgcinc");
	follow(tp,v);
	_list_appendx(tp,tp->black,v);
}
void full(TP) {
	while (tp->grey->len) {
		_tgcinc(tp);
	}
	collect(tp);
	follow(tp,tp->root);
}
void tgcinc(TP) {
	tp->steps += 1;
	if (tp->steps < GCMAX || tp->grey->len > 0) {
		_tgcinc(tp); _tgcinc(tp);
	}
	if (tp->steps < GCMAX || tp->grey->len > 0) { return; }
	tp->steps = 0;
	full(tp);
	return;
}
obj track(TP,obj v) {
	if (v.type == tSTRING) {
		int i = _dict_find(tp,tp->strings,v);
		if (i != -1) {
			tdelete(tp,v);
			v = tp->strings->items[i].key;
			grey(tp,v);
			return v;
		}
		_dict_setx(tp,tp->strings,v,True);
	}
	tgcinc(tp);
	grey(tp,v);
	return v;
}
