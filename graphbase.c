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

void setPixelColor(uint x, uint y, uchar r, uchar g, uchar b)
{
    uint pos=(y*WIDTH_RES+x)*3;
    uchar *add=base+pos;
    *add=b;
    *(++add)=g;
    *(++add)=r;
}

void tryToWrite()
{
    uint i,j;

    for (j=0;j<HEIGHT_RES;j++)
        for (i=0;i<WIDTH_RES;i++)
            setPixelColor(i,j,i,i>>8,j);
}
