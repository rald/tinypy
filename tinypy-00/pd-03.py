sw = 320
sh = 240

quit = False

mouse = None
draw = False

xp = sw >> 1
yp = sh >> 1

pi=0
ps=16
px=2
py=2
ph = False

cx = sw - 16 - 2 - 1
cy = 2
ch = False

st=0
et=0
lt=0

hexstr="0123456789ABCDEF"



def hexval(c):
    j=0
    k=-1
    while j<16:
        if c==hexstr[j]:
            k=j
            break
        j+=1
    return k    



def hex2dec(h):
    d=0
    l=len(h)
    i=0
    while i<l:
        d*=16
        c=h[i]
        if c>='a' and c<='f': 
            c=chr(ord(c)-32)
        k=hexval(c)
        if k==-1:
            return -1
        d+=k
        i+=1
    return d



def pyrgb(r,g,b):
    return ((int(r)%256)*65536) + ((int(g)%256)*256) + (int(b)%256)



def draw_point(x, y, c):
    if x<=1 or x>=sw-1 or y<1 or y>=sh-1: return None
    set_pixel(x, y, c)
    


def draw_rect(x, y, w, h, c):
    i = 0
    while i < w:
        draw_point(x + i, y, c)
        draw_point(x + i, y + h - 1, c)
        i += 1

    j = 0
    while j < h:
        draw_point(x,         y + j, c)
        draw_point(x + w - 1, y + j, c)
        j += 1



def fill_rect(x, y, w, h, c):
    j = 0
    while j < h:
        i = 0
        while i < w:
            draw_point(x + i, y + j, c)
            i += 1
        j += 1



def draw_line(x0, y0, x1, y1, c):
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    x, y = x0, y0
    if x0 > x1: sx = -1 else: sx = 1
    if y0 > y1: sy = -1 else: sy = 1
    if dx > dy:
        err = dx / 2.0
        while x != x1:
            draw_point(x, y, c)
            err -= dy
            if err < 0:
                y += sy
                err += dx
            x += sx
    else:
        err = dy / 2.0
        while y != y1:
            draw_point(x, y, c)
            err -= dx
            if err < 0:
                x += sx
                err += dy
            y += sy        
    draw_point(x, y, c)
 


def draw_line2(x0, y0, x1, y1, c, r):
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    x, y = x0, y0
    if x0 > x1: sx = -1 else: sx = 1
    if y0 > y1: sy = -1 else: sy = 1
    if dx > dy:
        err = dx / 2.0
        while x != x1:
            fill_circle(x, y, r, c)
            err -= dy
            if err < 0:
                y += sy
                err += dx
            x += sx
    else:
        err = dy / 2.0
        while y != y1:
            fill_circle(x, y, r, c)
            err -= dx
            if err < 0:
                x += sx
                err += dy
            y += sy        
    fill_circle(x, y, r, c)



def draw_circle(x0,y0,r,c):

    x0=int(x0)
    y0=int(y0)
    r=int(r)

    f = 1 - r
    ddfx = 0;
    ddfy = -2 * r
    x = 0
    y = r

    draw_point(x0, y0 + r, c)
    draw_point(x0, y0 - r, c)
    draw_point(x0 + r, y0, c)
    draw_point(x0 - r, y0, c)

    while x < y:

        if f >= 0: 
            y -= 1
            ddfy += 2
            f += ddfy
        x += 1
        ddfx += 2
        f += ddfx + 1
            
        draw_point(x0 + x, y0 + y, c)
        draw_point(x0 - x, y0 + y, c)
        draw_point(x0 + x, y0 - y, c)
        draw_point(x0 - x, y0 - y, c)
        draw_point(x0 + y, y0 + x, c)
        draw_point(x0 - y, y0 + x, c)
        draw_point(x0 + y, y0 - x, c)
        draw_point(x0 - y, y0 - x, c)



def fill_circle(x0, y0, r, c):

    x0=int(x0)
    y0=int(y0)
    r=int(r)

    f = 1 - r
    ddfx = 0;
    ddfy = -2 * r
    x = 0
    y = r

    draw_line(x0, y0 + r, x0, y0 - r, c)
    draw_line(x0 + r, y0, x0 - r, y0, c)

    while x < y:

        if f >= 0: 
            y -= 1
            ddfy += 2
            f += ddfy
        x += 1
        ddfx += 2
        f += ddfx + 1
            
        draw_line(x0 + x, y0 + y, x0 - x, y0 - y, c)
        draw_line(x0 - x, y0 + y, x0 + x, y0 - y, c)
        draw_line(x0 + x, y0 - y, x0 - x, y0 + y, c)
        draw_line(x0 + y, y0 + x, x0 - y, y0 - x, c)
        draw_line(x0 - y, y0 + x, x0 + y, y0 - x, c)



def draw_char(b,w,h,t,s,x,y,c):
    f=ord(c)
    j=0
    while j<h:
        i=0
        while i<w:
            k=b[f*w*h+j*h+i]
            l=hexval(k)
            if l != t:
                fill_rect(x+i*s,y+j*s,s,s,pal[0])
            i+=1
        j+=1



def draw_text(b,w,h,t,s,x,y,d):
    i=x
    j=y
    for k in d:
        draw_char(b,w,h,t,s,i,j,k)
        i+=w
        if i+w>=sw:
            i=0
            j+=h
            if j+h>=sh:
                j=0



def inrect(x, y, rx, ry, rw, rh):
    return x>=rx and x<rx+rw and y>=ry and y<ry+rh



def clrscr(c):
    j = 0
    while j < sh:
        i = 0
        while i < sw:
            set_pixel(i, j, c)
            i += 1
        j += 1



def frame(x, y, w, h, c):
    i = 0
    while i < w:
        set_pixel(x + i, y, c)
        set_pixel(x + i, y + h - 1, c)
        i += 1

    j = 0
    while j < h:
        set_pixel(x,         y + j, c)
        set_pixel(x + w - 1, y + j, c)
        j += 1



def button(x, y, w, h, c, s):
    global pal,mouse,draw

    res = False

    if s:
        fill_rect(x,y,w,h,pal[12])
        fill_rect(x+3,y+3,w-6,h-6,c)
        draw_rect(x+2,y+2,w-4,h-4,pal[0])
    else:
        fill_rect(x,y,w,h,c)
        draw_rect(x,y,w,h,pal[0])

    if inrect(mouse.x,mouse.y, x,y,w,h):
        draw = False
        if mouse.b == 1:
            fill_rect(x,y,w,h,pal[12])
            fill_rect(x+3,y+3,w-6,h-6,c)
            draw_rect(x+2,y+2,w-4,h-4,pal[0])
            res = True

    return res



def close_button(x,y):

    global ch,pal

    res = False

    down = button(x,y,16,16,pal[2],False)

    if not ch:
        if down:
            ch = True
    else:
        if not down:
            ch = False
            res = True

    return res



def palette(x, y, ps):

    global pi,ph,pal

    res = False

    k=0
    while k<16:

        i=int(k % 8)
        j=int(k / 8)

        if k == pi:
            s = True
        else:
            s = False

        down = button(x+i*ps+1,y+j*ps+1,ps,ps,pal[k],s)
            
        if not ph:
            if down:
                hold = True
                pi=k
        elif not down:
            ph = False
            res = True
                
        k+=1

    return res



def main():

    global sw,sh,pal,quit,mouse,draw
    global px,py,ps
    global xp,yp
    global st,et,lt
    global bw,bh,bf,bt
    global font

    set_mode(sw,sh)

    clrscr(pal[12)
    frame(0,0,sw,sh,pal[3])

    st = get_ticks()
    
    while not quit:

        mouse = get_mouse_pos()

        e = gfx_get_event()
        while e is not None:
            if e.type == 12:
                quit = True

            e = gfx_get_event()

        quit=close_button(cx,cy)

        palette(px,py,ps)

        et=get_ticks()
        lt=et-st

        update(0,0,sw,sh)

    delay(1000)





pal=[
    hex2dec("1a1c2c"),
    hex2dec("5d275d"),
    hex2dec("b13e53"),
    hex2dec("ef7d57"),
    hex2dec("ffcd75"),
    hex2dec("a7f070"),
    hex2dec("38b764"),
    hex2dec("257179"),
    hex2dec("29366f"),
    hex2dec("3b5dc9"),
    hex2dec("41a6f6"),
    hex2dec("73eff7"),
    hex2dec("f4f4f4"),
    hex2dec("94b0c2"),
    hex2dec("566c86"),
    hex2dec("333c57")   
]

txt=load_file("font.txt").split("\n")

heading=txt[0].split(" ")
bw = int(heading[0])
bh = int(heading[1])
bf = int(heading[2])
bt = int(heading[3])

font="".join(txt[1:])

main()
