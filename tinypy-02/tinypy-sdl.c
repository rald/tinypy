#include <SDL/SDL.h>

#include "tinypy.h"


char *real_load_file(char *path) {
    char *source = NULL;

    FILE *fp = fopen(path, "r");

    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufsize = ftell(fp);
            if (bufsize == -1) { /* Error */ }

            source = malloc(sizeof(char) * (bufsize + 1));

            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if (newLen == 0) {
                fprintf(stderr,"Error: cannot read file %s", path);
                free(source);
                source = NULL;
            } else {
                source[++newLen] = '\0';
            }
        }
        fclose(fp);
    } else {
        perror(path);
    }
    
    return source;
}

tp_obj load_file(TP) {
	return tp_string(real_load_file(TP_STR()));
}

int real_save_file(char *path,char *source,size_t length) {
    FILE *fp=fopen(path,"w");

    if(fp==NULL) {
        perror(path);
        return 1;
    }

    if(fwrite(source,sizeof(char),length,fp)<length) {
        perror(path);
        return 2;
    }

    return 0;
}

tp_obj save_file(TP) {
	return tp_number(real_save_file(TP_STR(),TP_STR(),TP_NUM()));
}

tp_obj delay(TP) {
	SDL_Delay(TP_NUM());
	return None;
}

SDL_Surface *s;
tp_obj set_mode(TP) {
    int w = TP_NUM();
    int h = TP_NUM();
    SDL_Init(SDL_INIT_EVERYTHING);
    s = SDL_SetVideoMode(w,h,32,0);
    return None;
}

Uint32 real_get_pixel(int x, int y) {
	if (x<s->clip_rect.x || y <s->clip_rect.y || x >= (s->clip_rect.x+s->clip_rect.w) || y >=(s->clip_rect.y+s->clip_rect.h)) { return 0; }
	Uint32 *p = (Uint32*)((Uint8*)s->pixels+y*s->pitch);
	return (Uint32)(*(p+x));
}

tp_obj get_pixel(TP) {
	int x = TP_NUM();
	int y = TP_NUM();
	int c = real_get_pixel(x,y);
	return tp_number(c);
}


void real_set_pixel(int x, int y, int c) {
    if (x<s->clip_rect.x || y <s->clip_rect.y || x >= (s->clip_rect.x+s->clip_rect.w) || y >=(s->clip_rect.y+s->clip_rect.h)) { return ; }  
    Uint32 *p = (Uint32*)((Uint8*)s->pixels+y*s->pitch);
    *(p+x) = (Uint32)c;
}

tp_obj set_pixel(TP) {
    int x = TP_NUM();
    int y = TP_NUM();
    int c = TP_NUM();
    
    real_set_pixel(x,y,c);
    return None;
}


tp_obj update(TP) {
    int x = TP_NUM();
    int y = TP_NUM();
    int w = TP_NUM();
    int h = TP_NUM();
    SDL_Rect r;
    r.x=x;r.y=y;r.w=w;r.h=h;
    SDL_UpdateRects(s, 1, &r);
    return None;
}

tp_obj get_ticks(TP) {
    return tp_number(SDL_GetTicks());
}

tp_obj _sdl_event_get(TP,tp_obj self, tp_obj k) {
    SDL_Event *e = self.data.val;
    char *key = STR(k);
    if (strcmp(key,"type")==0) { return tp_number(e->type); }
    return None;
}

tp_obj gfx_get_event(TP) {
    SDL_Event *e = malloc(sizeof(SDL_Event));
    if (SDL_PollEvent(e)) {
        tp_obj r = tp_data(tp,e);
        r.data.meta->get = _sdl_event_get;
        return r;
    }
    return None;
}

tp_obj get_mouse_pos(TP) {
    int x,y,b;
    b = SDL_GetMouseState(&x,&y);
    tp_obj r;
    r = tp_dict_n(tp,3,(tp_obj[]){
        tp_string("x"),tp_number(x),
        tp_string("y"),tp_number(y),
        tp_string("b"),tp_number(b)
        });
    return r;
}


void sdl_init(TP) {
    tp_obj context = tp->builtins;
    tp_set(tp,context,tp_string("load_file"),tp_fnc(tp,load_file));
    tp_set(tp,context,tp_string("save_file"),tp_fnc(tp,save_file));
    tp_set(tp,context,tp_string("delay"),tp_fnc(tp,delay));
    tp_set(tp,context,tp_string("get_pixel"),tp_fnc(tp,get_pixel));


    tp_set(tp,context,tp_string("set_mode"),tp_fnc(tp,set_mode));
    tp_set(tp,context,tp_string("set_pixel"),tp_fnc(tp,set_pixel));
    tp_set(tp,context,tp_string("update"),tp_fnc(tp,update));
    tp_set(tp,context,tp_string("get_ticks"),tp_fnc(tp,get_ticks));
    tp_set(tp,context,tp_string("gfx_get_event"),tp_fnc(tp,gfx_get_event));
    tp_set(tp,context,tp_string("get_mouse_pos"),tp_fnc(tp,get_mouse_pos));
}

int main(int argc, char *argv[]) {
    tp_vm *tp = tp_init(argc,argv);
    sdl_init(tp);
    tp_call(tp,"py2bc","tinypy",None);
    tp_deinit(tp);
    return(0);
}

//

