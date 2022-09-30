sw = 320
sh = 240

quit = False

mouse = None
drag = None
diffx = 0
diffy = 0

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

bw=16
bh=16

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
    global px,py
    global mouse
    global drag,diffx,diffy

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

    w=8*ps
    h=2*ps

    if mouse.b == 4: 
        if drag == None:
            if inrect(mouse.x,mouse.y,px,py,w,h):
                diffx = px - mouse.x
                diffy = py - mouse.y
                drag  = "palette"
        elif drag == "palette": 
            px = mouse.x + diffx
            py = mouse.y + diffy

            if px<0: px=0
            if py<0: py=0
            if px>sw-w: px=sw-w
            if py>sh-h: py=sh-h

            
    else:
        if drag == "palette":
            drag = None

    return res



def grid(x,y,w,h,s,c):
    global pi
    global bp
    global gx,gy
    global mouse
    global drag,diffx,diffy


    for j in range(0,h):
        for i in range(0,w):
            x0=x+i*s
            y0=y+j*s
            if x0>-s and x0<sw and y0>-s and y0<sh:
                k=bp[j*w+i]
                fill_rect(x+i*s,y+j*s,s,s,pal[k])



    x0=x
    y0=y
    x1=x+w*s
    y1=y+h*s
    
    if x1 >= sw: x1 = int(sw/s)*s
    if y1 >= sh: y1 = int(sh/s)*s      

    gw=x1-x0+1
    gh=y1-y0+1

    draw_rect(x0,y0,gw,gh,pal[0])

    
    for i in range(x0,x1+s,s):
        draw_line(i,y0,i,y1,c)

    for j in range(y0,y1+s,s):
        draw_line(x0,j,x1,j,c)
    


    if mouse.b == 4:      
        if drag == None:
            if inrect(mouse.x,mouse.y,gx,gy,gw,gh):
                diffx = gx - mouse.x
                diffy = gy - mouse.y
                drag  = "grid"
        elif drag == "grid": 
            gx = mouse.x + diffx
            gy = mouse.y + diffy
    else:
        if drag == "grid":
            drag = None


    if mouse.b == 1:
        if inrect(mouse.x,mouse.y,gx,gy,gw,gh):   
                mx = int((mouse.x-gx) / s)
                my = int((mouse.y-gy) / s)           

                if mx>=0 and mx<w and my>=0 and my<h:
                    bp[my*w+mx]=pi



def main():

    global sw,sh,
    global px,py,ps
    global xp,yp
    global st,et,lt
    global pal,quit,mouse,draw,font
    global gx,gy,gs,gc

    set_mode(sw,sh)


    st = get_ticks()
    
    while not quit:

        clrscr(pal[12])
        frame(0,0,sw,sh,pal[3])


        mouse = get_mouse_pos()

        e = gfx_get_event()
        while e is not None:
            if e.type == 12:
                quit = True

            e = gfx_get_event()


        grid(gx,gy,bw,bh,gs,gc)
        
        palette(px,py,ps)

        quit=close_button(cx,cy)

    
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

gx=8
gy=48
gs=8
gc=pal[0]

bp=[12 for _ in range(0,bw*bh)]


txt=load_file("font.txt").split("\n")
heading=txt[0].split(" ")
fw = int(heading[0])
fh = int(heading[1])
ff = int(heading[2])
ft = int(heading[3])
font="".join(txt[1:])



main()
