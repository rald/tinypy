int lua_hash(void *v,int l) {
	int i,step = (l>>5)+1;
	int h = l + (l >= 4?*(int*)v:0);
	for (i=l; i>=step; i-=step) {
		h = h^((h<<5)+(h>>2)+((unsigned char *)v)[i-1]);
	}
	return h;
}
void _dict_free(_dict *self) {
	tfree(self->items);
	tfree(self);
}
int thash(TP,obj v) {
	switch (v.type) {
		case NONE: return 0;
		case NUMBER: return lua_hash(&v.number.val,sizeof(num));
		case tSTRING: return lua_hash(v.string.val,v.string.len);
		case DICT: return lua_hash(&v.dict.val,sizeof(void*));
		case LIST: {
			int r = v.list.val->len; int n; for(n=0; n<v.list.val->len; n++) {
			obj vv = v.list.val->items[n]; r += vv.type != LIST?thash(tp,v.list.val->items[n]):lua_hash(&vv.list.val,sizeof(void*)); } return r;
		}
		case FNC: return lua_hash(&v.fnc.val,sizeof(void*));
		case DATA: return lua_hash(&v.data.val,sizeof(void*));
	}
	traise(0,"thash(%s)",STR(v));
}
void _dict_hash_set(TP,_dict *self, int hash, obj k, obj v) {
	int i,idx = hash&self->mask;
	for (i=idx; i<idx+self->alloc; i++) {
		int n = i&self->mask;
		if (self->items[n].used > 0) { continue; }
		if (self->items[n].used == 0) { self->used += 1; }
		self->items[n] = (item){1,hash,k,v};
		self->len += 1;
		return;
	}
	traise(,"_dict_hash_set(%d,%d,%s,%s)",self,hash,STR(k),STR(v));
}
void _dict_realloc(TP,_dict *self,int len) {
	len = _max(8,len);
	item *items = self->items;
	int i,alloc = self->alloc;
	self->items = malloc(len*sizeof(item));
	self->alloc = len; self->mask = len-1;
	self->len = 0; self->used = 0;
	for (i=0; i<alloc; i++) {
		if (items[i].used != 1) { continue; }
		_dict_hash_set(tp,self,items[i].hash,items[i].key,items[i].val);
	}
	tfree(items);
}
int _dict_hash_find(TP,_dict *self, int hash, obj k) {
	int i,idx = hash&self->mask;
	for (i=idx; i<idx+self->alloc; i++) {
		int n = i&self->mask;
		if (self->items[n].used == 0) { break; }
		if (self->items[n].used < 0) { continue; }
		if (self->items[n].hash != hash) { continue; }
		if (cmp(tp,self->items[n].key,k) != 0) { continue; }
		return n;
	}
	return -1;
}
int _dict_find(TP,_dict *self,obj k) {
	return _dict_hash_find(tp,self,thash(tp,k),k);
}
void _dict_setx(TP,_dict *self,obj k, obj v) {
	int hash = thash(tp,k); int n = _dict_hash_find(tp,self,hash,k);
	if (n == -1) {
		if (self->len >= (self->alloc/2)) {
			_dict_realloc(tp,self,self->alloc*2);
		} else if (self->used >= (self->alloc*3/4)) {
			_dict_realloc(tp,self,self->alloc);
		}
		_dict_hash_set(tp,self,hash,k,v);
	} else {
		self->items[n].val = v;
	}
}
void _dict_set(TP,_dict *self,obj k, obj v) {
	_dict_setx(tp,self,k,v);
	grey(tp,k); grey(tp,v);
}
obj _dict_get(TP,_dict *self,obj k, char *error) {
	int n = _dict_find(tp,self,k);
	if (n < 0) {
		traise(None,"%s: KeyError: %s\n",error,STR(k));
	}
	return self->items[n].val;
}
void _dict_del(TP,_dict *self,obj k, char *error) {
	int n = _dict_find(tp,self,k);
	if (n < 0) { traise(,"%s: KeyError: %s\n",error,STR(k)); }
	self->items[n].used = -1;
	self->len -= 1;
}
_dict *_dict_new(void) {
	_dict *self = malloc(sizeof(_dict));
	return self;
}
obj _dict_copy(TP,obj rr) {
	_dict *o = rr.dict.val;
	_dict *r = _dict_new(); *r = *o;
	r->items = malloc(sizeof(item)*o->alloc);
	memcpy(r->items,o->items,sizeof(item)*o->alloc);
	return track(tp,(obj)(dict_){DICT,r});
}
int _dict_next(TP,_dict *self) {
	if (!self->len) { traise(0,"_dict_next(...)",0); }
	while (1) {
		self->cur = ((self->cur + 1) & self->mask);
		if (self->items[self->cur].used > 0) {
			return self->cur;
		}
	}
}
obj merge(TP) {
	obj self = OBJ();
	obj v = OBJ();
	int i; for (i=0; i<v.dict.val->len; i++) {
		int n = _dict_next(tp,v.dict.val);
		_dict_set(tp,self.dict.val,
			v.dict.val->items[n].key,v.dict.val->items[n].val);
	}
	return None;
}
obj dict(TP) {
	obj r = (obj)(dict_){DICT,_dict_new()};
	return tp?track(tp,r):r;
}
obj dict_n(TP,int n, obj* argv) {
	obj r = dict(tp);
	int i; for (i=0; i<n; i++) { set(tp,r,argv[i*2],argv[i*2+1]); }
	return r;
}
