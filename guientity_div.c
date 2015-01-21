#include "types.h"
#include "defs.h"
#include "graphbase.h"
#include "guilayout.h"
#include "guientity_div.h"
#include "guientity_attrvalue.h"
#include "events.h"

//==============================================
uchar drawDiv_trans(dom* elem, int x, int y, int w, int h);

uchar drawDiv(dom* elem, int x, int y, int w, int h)
{
    int i,j;
    div *ent=(div*)(elem->entity);
    int xs;
    int ys;

    if (ent->bgColor.a>0)
    {
        if (ent->bgColor.a==255)
            return 1;
        drawDiv_trans(elem,x,y,w,h);
        return 1;
    }

    xs=getABSposx(elem)+x;
    ys=getABSposy(elem)+y;
    for (j=0;j<h;j++)
    {
        for (i=0;i<w;i++)
            setPixelColor(xs+i,ys+j,ent->bgColor.c);
    }
    return 1;
}
uchar drawDiv_trans(dom* elem, int x, int y, int w, int h)
{
    int i,j;
    div *ent=(div*)(elem->entity);
    int xs=getABSposx(elem)+x;
    int ys=getABSposy(elem)+y;

    for (j=0;j<h;j++)
    {
        for (i=0;i<w;i++)
            setPixelColor(xs+i,ys+j,mingle(getPixelColor(xs+i,ys+j),ent->bgColor));
    }
    return 1;
}
uint div_createDom(int x, int y, int w, int h, uint parent, int pid)
{
    div *t;
    if((t = (div*)kalloc()) == 0)
    {
        panic("Bingolingo!");
        return 0;
    }
    memset((uchar*)(&(t->ds)),0,sizeof(dom));
    t->ds.x=x;
    t->ds.y=y;
    t->ds.width=w;
    t->ds.height=h;
    t->ds._id=(uint)t;
    t->ds.trans=255;
    t->bgColor.a=255;

    t->ds.entity=(void*)t;
    t->ds.onRender=drawDiv;
    t->ds.pid=pid;
    t->ds.onPoint=typicalPointEvent;
    t->ds.onFocus=typicalFocusEvent;
    t->ds.isIntegral=0;

    if (parent==0xffffffff)
        prepend(del,&t->ds);
    else
        prepend((dom*)parent,&t->ds);

    return (uint)t;
}

void div_release(uint elem_)
{
    div* elem=(div*)elem_;
    if (elem->ds.parent!=0)
        delete(&elem->ds);
    _cascade_release(&elem->ds);
}
uint div_changeBgcolor(uint elem_, color32 color)
{
    div* elem=(div*)elem_;
    elem->bgColor=color;
    elem->ds.trans=color.a;

    reDraw(&elem->ds);

    return (uint)elem;
}

void div_setXY(uint elem_, int x, int y)
{
    div *elem=(div*)elem_;
    int i,j;

    i=elem->ds.x;
    j=elem->ds.y;
    elem->ds.x=x;
    elem->ds.y=y;
    reDraw_(elem->ds.parent,i,j,elem->ds.width,elem->ds.height);
    reDraw(&elem->ds);
}

uint div_setAttr(uint elem_, int attr, void *val)
{
    div *elem=(div*)elem_;
    int i,j;

    switch (attr)
    {
    case GUIATTR_DIV_X:
        i=elem->ds.x;
        elem->ds.x=*((int*)val);
        reDraw_(elem->ds.parent,i,elem->ds.y,elem->ds.width,elem->ds.height);
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_DIV_Y:
        j=elem->ds.y;
        elem->ds.y=*((int*)val);
        reDraw_(elem->ds.parent,elem->ds.x,j,elem->ds.width,elem->ds.height);
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_DIV_WIDTH:
        i=elem->ds.width;
        elem->ds.width=*((uint*)val);
        reDraw_(elem->ds.parent,elem->ds.x,elem->ds.y,i,elem->ds.height);
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_DIV_HEIGHT:
        j=elem->ds.height;
        elem->ds.height=*((uint*)val);
        reDraw_(elem->ds.parent,elem->ds.x,elem->ds.y,elem->ds.width,j);
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_DIV_BGCOLOR:
        elem->bgColor=*((color32*)val);
        elem->ds.trans=((color32*)val)->a;
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_DIV_REFRESH:
        reDraw(elem->ds.parent);
        return 0;
    case GUIATTR_DIV_INTEG:
        elem->ds.isIntegral=1-elem->ds.isIntegral;
        return 0;
    default:
        return -1;
    }
    return 0;
}
uint div_getAttr(uint elem_, int attr, void *des)
{
    div* elem=(div*)elem_;

    switch (attr)
    {
    case GUIATTR_DIV_X:
        return -1;
        break;
    case GUIATTR_DIV_Y:
        return -1;
        break;
    case GUIATTR_DIV_WIDTH:
        return -1;
        break;
    case GUIATTR_DIV_HEIGHT:
        return -1;
        break;
    case GUIATTR_DIV_BGCOLOR:
        *((color32*)des)=elem->bgColor;
        break;
    default:
        return -1;
    }
    return 0;
}
