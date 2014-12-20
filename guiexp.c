#include "types.h"
#include "defs.h"
#include "graphbase.h"
#include "guilayout.h"
#include "guientity.h"
#include "guiexp.h"
#include "ex_mem.h"

void tryOnce()
{
    uint r=div_createDom(0,0,WIDTH_RES,HEIGHT_RES,0xffffffff);
    div_changeBgcolor(r,rgba(255,255,255,0));

    uint l3=div_createDom(500,0,500,100,r);
    div_changeBgcolor(l3,rgba(0,255,0,0));

    uint l1=div_createDom(70,20,500,100,r);
    div_changeBgcolor(l1,rgba(200,100,35,0));
    div_changeBgcolor(l1,rgba(255,0,0,128));


        uint l2=div_createDom(50,50,500,100,l1);
        div_changeBgcolor(l2,rgba(0,0,255,200));
        uint l4=div_createDom(400,60,10,10,l1);
        div_changeBgcolor(l4,rgba(0,255,255,0));


    uint l9=div_createDom(90,220,200,30,r);
    div_changeBgcolor(l9,rgba(200,100,35,0));
}
