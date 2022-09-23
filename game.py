SW = 320
SH = 240
q = 0
d = 0
px = SW >> 1
py = SH >> 1

pal = [((min(255,n)*65536)+(min(255,n*3/2)*256)+(min(255,n*2))) for n in range(0,256)]


def draw_rect(x, y, w, h, c):
    i = 0
    while i < w:
        set_pixel(x + i, y, c)
        set_pixel(x + i, y + h - 1, c)
        i += 1

    j = 0
    while j < h:
        set_pixel(x,     y + j, c)
        set_pixel(x + w - 1, y + j, c)
        j += 1

def fill_rect(x, y, w, h, c):
    j = 0
    while j < w:
        i = 0
        while i < h:
            set_pixel(x + i, y + j, c)
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
            fill_rect(x-2, y-2, 4, 4, c)
            err -= dy
            if err < 0:
                y += sy
                err += dx
            x += sx
    else:
        err = dy / 2.0
        while y != y1:
            fill_rect(x-2, y-2, 4, 4, c)
            err -= dx
            if err < 0:
                x += sx
                err += dy
            y += sy        
    fill_rect(x-2, y-2, 4, 4, c)





set_mode(SW,SH)

while q == 0:

    t = get_ticks()
    m = get_mouse_pos()

    e = gfx_get_event()
    while e is not None:
        if e.type == 12 or e.type == 2:
            _quit = 1
        e = gfx_get_event()

    if m.b == 1:
        if d == 0:
            d = 1
        else:
            draw_line(px,py,m.x,m.y,(255 << 16) + (255 << 8) + 255)
        px=m.x
        py=m.y
    elif m.b == 4:
        q = 1
    else:
        d = 0
        
    update(0,0,SW,SH)


