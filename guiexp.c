#include "types.h"
#include "defs.h"
#include "graphbase.h"
#include "guilayout.h"
#include "guientity.h"
#include "guiexp.h"
#include "ex_mem.h"

uint r,n;
uint hua[5];

void tryOnce()
{
    uint i;

    r=div_createDom(0,0,WIDTH_RES,HEIGHT_RES,0xffffffff);
    div_changeBgcolor(r,rgba(56,5,39,0));

    for (i=0;i<5;i++)
    {
        hua[i]=div_createDom(438+i*40,500,5,8,r);
        div_changeBgcolor(hua[i],rgba(0,0,0,0));
    }
    n=-1;
}
void lightOn(uint i)
{
    uchar j,k;
    for (j=0;j<255;j++)
        for (k=0;k<70;k++)
            div_changeBgcolor(hua[i],rgba(j,j,0,0));
}
void lightOff(uint i)
{
    uchar j,k;
    for (j=255;j>0;j--)
        for (k=0;k<70;k++)
            div_changeBgcolor(hua[i],rgba(j,j,0,0));
}
void toggleOn()
{
    if (n==-1)
    {
        lightOn(0);
        n=0;
        return;
    }
    lightOff(n);
    n=(n+1)%5;
    lightOn(n);
}
void endToggle()
{
    uchar i,j,k;
    for (j=0;j<255;j++)
        for (i=0;i<5;i++)
            for (k=0;k<8;k++)
                if (i==n)
                    div_changeBgcolor(hua[i],rgba(255,255,0,j));
                else
                    div_changeBgcolor(hua[i],rgba(0,0,0,j));
    //div_release(r);
}
