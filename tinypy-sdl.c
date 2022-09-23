#include "SDL.h"
#include "tinypy.h"
SDL_Surface *s;
obj set_mode(TP) {
	int w = NUM();
	int h = NUM();
	SDL_Init(SDL_INIT_EVERYTHING);
	s = SDL_SetVideoMode(w,h,32,0);
	return None;
}
void real_set_pixel(int x, int y, int c) {
	if (x<s->clip_rect.x || y <s->clip_rect.y || x >= (s->clip_rect.x+s->clip_rect.w) || y >=(s->clip_rect.y+s->clip_rect.h)) { return ; }
	Uint32 *p = (Uint32*)((Uint8*)s->pixels+y*s->pitch);
	*(p+x) = (Uint32)c;
}
obj set_pixel(TP) {
	int x = NUM();
	int y = NUM();
	int c = NUM();
	real_set_pixel(x,y,c);
	return None;
}
obj update(TP) {
	int x = NUM();
	int y = NUM();
	int w = NUM();
	int h = NUM();
	SDL_Rect r;
	r.x=x;r.y=y;r.w=w;r.h=h;
	SDL_UpdateRects(s, 1, &r);
	return None;
}
obj get_ticks(TP) {
	return number(SDL_GetTicks());
}
obj _sdl_event_get(TP,obj self, obj k) {
	SDL_Event *e = self.data.val;
	char *key = STR(k);
	if (strcmp(key,"type")==0) { return number(e->type); }
	return None;
}
obj gfx_get_event(TP) {
	SDL_Event *e = malloc(sizeof(SDL_Event));
	if (SDL_PollEvent(e)) {
		obj r = data(tp,e);
		r.data.meta->get = _sdl_event_get;
		return r;
	}
	return None;
}
obj get_mouse_pos(TP) {
	int x,y,b;
	b = SDL_GetMouseState(&x,&y);
	obj r;
	r = dict_n(tp,3,(obj[]){
		string("x"),number(x),
		string("y"),number(y),
		string("b"),number(b)
		});
	return r;
}
void sdl_init(TP) {
	obj context = tp->builtins;
	set(tp,context,string("set_mode"),fnc(tp,set_mode));
	set(tp,context,string("set_pixel"),fnc(tp,set_pixel));
	set(tp,context,string("update"),fnc(tp,update));
	set(tp,context,string("get_ticks"),fnc(tp,get_ticks));
	set(tp,context,string("gfx_get_event"),fnc(tp,gfx_get_event));
	set(tp,context,string("get_mouse_pos"),fnc(tp,get_mouse_pos));
}
int main(int argc, char *argv[]) {
	tvm *tp = tinit(argc,argv);
	sdl_init(tp);
	call(tp,"py2bc","tinypy",None);
	deinit(tp);
	return(0);
}
