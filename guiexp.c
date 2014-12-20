#include "types.h"
#include "defs.h"
#include "graphbase.h"
#include "guilayout.h"
#include "guientity.h"
#include "guiexp.h"

void tryOnce()
{
    div *r=div_createDom(0,0,0,WIDTH_RES,HEIGHT_RES,bingolingo);
    div_changeBgcolor(r,rgba(255,255,255,0));

    div *l3=div_createDom(10,500,0,500,100,&r->ds);
    div_changeBgcolor(l3,rgba(0,255,0,0));

    div *l1=div_createDom(11,70,20,500,100,&r->ds);
    div_changeBgcolor(l1,rgba(200,100,35,0));
    div_changeBgcolor(l1,rgba(255,0,0,128));


        div *l2=div_createDom(20,50,50,500,100,&l1->ds);
        div_changeBgcolor(l2,rgba(0,0,255,200));
        div *l4=div_createDom(21,400,60,10,10,&l1->ds);
        div_changeBgcolor(l4,rgba(0,255,255,0));


    div *l9=div_createDom(11,90,220,200,30,&r->ds);
    div_changeBgcolor(l9,rgba(200,100,35,0));
}
