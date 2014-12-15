#include "types.h"
#include "defs.h"
#include "memlayout.h"
#include "graphbase.h"

uint GraphicMem=KERNBASE+0x1028;

void tryToWrite()
{
    uint baseAdd=*((uint*)GraphicMem);
    uchar *base=(uchar*)baseAdd;
    cprintf("Bits per pixel: %x\n",*((uchar*)(KERNBASE+0x1019)));
    uint x,y;
    for (x=0;x<WIDTH_RES;x++)
        for (y=0;y<HEIGHT_RES;y++)
        {
            base[0]=0xFF;
            base[1]=0x00;
            base[2]=0x00;
            base+=3;
        }

}
