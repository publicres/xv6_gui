#include "types.h"
#include "defs.h"
#include "memlayout.h"
#include "graphbase.h"

uchar *base;

void initGraphMode()
{
    uint GraphicMem=KERNBASE+0x1028;
    uint baseAdd=*((uint*)GraphicMem);
    base=(uchar*)baseAdd;
    WIDTH_RES =*((ushort*)(KERNBASE+0x1012));
    HEIGHT_RES =*((ushort*)(KERNBASE+0x1014));

    cprintf("@Resolution Width: %d\n", WIDTH_RES);
    cprintf("@Resolution Height:  %d\n", HEIGHT_RES);
    cprintf("@Bits per pixel:    %d\n",*((uchar*)(KERNBASE+0x1019)));

    cprintf("@Video card drivers initialized successfully.\n");
}

void setPixelColor(uint x, uint y,color24 color)
{

    uint pos=(y*WIDTH_RES+x)*3;
    color24* add=(color24*)(base+pos);
    *add=color;
}
void setRangeColor(color24 *src, uint len, uint x, uint y)
{
    uint pos=(y*WIDTH_RES+x)*3;
    color24* add=(color24*)(base+pos);
    memmove(add,src,len*3);
}

color24 rgb(uchar r,uchar g,uchar b)
{
    color24 t;
    t.r=r;
    t.g=g;
    t.b=b;
    return t;
}
color32 rgba(uchar r, uchar g, uchar b, uchar a)
{
    color32 t;
    t.c=rgb(r,g,b);
    t.a=a;
    return t;
}
