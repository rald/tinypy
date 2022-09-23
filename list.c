void _list_realloc(_list *self,int len) {
	if (!len) { len=1; }
	self->items = realloc(self->items,len*sizeof(obj));
	self->alloc = len;
}
void _list_set(TP,_list *self,int k, obj v, char *error) {
	if (k >= self->len) { traise(,"%s: KeyError: %d\n",error,k); }
	self->items[k] = v;
	grey(tp,v);
}
void _list_free(_list *self) {
	tfree(self->items);
	tfree(self);
}
obj _list_get(TP,_list *self,int k,char *error) {
	if (k >= self->len) { traise(None,"%s: KeyError: %d\n",error,k); }
	return self->items[k];
}
void _list_insertx(TP,_list *self, int n, obj v) {
	if (self->len >= self->alloc) {
		_list_realloc(self,self->alloc*2);
	}
	if (n < self->len) { memmove(&self->items[n+1],&self->items[n],sizeof(obj)*(self->len-n)); }
	self->items[n] = v;
	self->len += 1;
}
void _list_appendx(TP,_list *self, obj v) {
	_list_insertx(tp,self,self->len,v);
}
void _list_insert(TP,_list *self, int n, obj v) {
	_list_insertx(tp,self,n,v);
	grey(tp,v);
}
void _list_append(TP,_list *self, obj v) {
	_list_insert(tp,self,self->len,v);
}
obj _list_pop(TP,_list *self, int n, char *error) {
	obj r = _list_get(tp,self,n,error);
	if (n != self->len-1) { memmove(&self->items[n],&self->items[n+1],sizeof(obj)*(self->len-(n+1))); }
	self->len -= 1;
	return r;
}
int _list_find(TP,_list *self, obj v) {
	int n;
	for (n=0; n<self->len; n++) {
		if (cmp(tp,v,self->items[n]) == 0) {
			return n;
		}
	}
	return -1;
}
obj tindex(TP) {
	obj self = OBJ();
	obj v = OBJ();
	int i = _list_find(tp,self.list.val,v);
	if (i < 0) { traise(None,"tindex(%s,%s) - item not found",STR(self),STR(v)); }
	return number(i);
}
_list *_list_new(void) {
	return malloc(sizeof(_list));
}
obj _list_copy(TP, obj rr) {
	_list *o = rr.list.val;
	_list *r = _list_new(); *r = *o;
	r->items = malloc(sizeof(obj)*o->alloc);
	memcpy(r->items,o->items,sizeof(obj)*o->alloc);
	return track(tp,(obj)(list_){LIST,r});
}
obj append(TP) {
	obj self = OBJ();
	obj v = OBJ();
	_list_append(tp,self.list.val,v);
	return None;
}
obj pop(TP) {
	obj self = OBJ();
	return _list_pop(tp,self.list.val,self.list.val->len-1,"pop");
}
obj insert(TP) {
	obj self = OBJ();
	int n = NUM();
	obj v = OBJ();
	_list_insert(tp,self.list.val,n,v);
	return None;
}
obj extend(TP) {
	obj self = OBJ();
	obj v = OBJ();
	int i;
	for (i=0; i<v.list.val->len; i++) {
		_list_append(tp,self.list.val,v.list.val->items[i]);
	}
	return None;
}
obj list(TP) {
	obj r = (obj)(list_){LIST,_list_new()};
	return tp?track(tp,r):r;
}
obj list_n(TP,int n,obj *argv) {
	obj r = list(tp); _list_realloc(r.list.val,n);
	int i; for (i=0; i<n; i++) { _list_append(tp,r.list.val,argv[i]); }
	return r;
}
int _sort_cmp(obj *a,obj *b) {
	return cmp(0,*a,*b);
}
obj sort(TP) {
	obj self = OBJ();
	qsort(self.list.val->items, self.list.val->len, sizeof(obj), (int(*)(const void*,const void*))_sort_cmp);
	return None;
}
