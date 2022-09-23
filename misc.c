obj *ptr(obj o) {
	obj *ptr = malloc(sizeof(obj)); *ptr = o;
	return ptr;
}
obj _dcall(TP,obj fnc(TP)) {
	return fnc(tp);
}
obj _tcall(TP,obj fnc) {
	if (fnc.fnc.ftype&2) {
		_list_insert(tp,tp->params.list.val,0,fnc.fnc.val->self);
	}
	return _dcall(tp,fnc.fnc.fval);
}
obj fnc_new(TP,int t, void *v, obj s, obj g) {
	obj r;
	r.type = FNC;
	_fnc *self = malloc(sizeof(_fnc));
	self->self = s;
	self->globals = g;
	r.fnc.ftype = t;
	r.fnc.val = self;
	r.fnc.fval = v;
	return track(tp,r);
}
obj def(TP,void *v, obj g) {
	return fnc_new(tp,1,v,None,g);
}
obj fnc(TP,obj v(TP)) {
	return fnc_new(tp,0,v,None,None);
}
obj method(TP,obj self,obj v(TP)) {
	return fnc_new(tp,2,v,self,None);
}
obj data(TP,void *v) {
	obj r = (obj)(data_){DATA,malloc(sizeof(_data)),v,0};
	r.data.meta = &r.data.info->meta;
	return track(tp,r);
}
obj tparams(TP) {
	tp->params = tp->_params.list.val->items[tp->cur];
	obj r = tp->_params.list.val->items[tp->cur];
	r.list.val->len = 0;
	return r;
}
obj tparams_n(TP,int n, obj argv[]) {
	obj r = tparams(tp);
	int i; for (i=0; i<n; i++) { _list_append(tp,r.list.val,argv[i]); }
	return r;
}
obj tparams_v(TP,int n,...) {
	obj r = tparams(tp);
	va_list a; va_start(a,n);
	int i; for(i=0; i<n; i++) { _list_append(tp,r.list.val,va_arg(a,obj)); }
	va_end(a);
	return r;
}
