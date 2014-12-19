#include "types.h"
#include "defs.h"
#include "graphbase.h"
#include "guilayout.h"
#include "guientity_div.h"

//==============================================
uchar drawDiv(dom* elem, uint x, uint y, uint w, uint h)
{
    uint i,j;
    div *ent=(div*)(elem->entity);
    if (ent->bgColor.a!=0)
        return 1;
    uint xs=getABSposx(elem)+x;
    uint ys=getABSposy(elem)+y;

    for (j=0;j<h;j++)
    {
        if (y+j==0 || y+j==elem->height-1)
        {
            for (i=0;i<w;i++)
                setPixelColor(xs+i,ys+j,rgb(0,0,0));
        }
        else
        {
            for (i=0;i<w;i++)
                setPixelColor(xs+i,ys+j,ent->bgColor.c);
            if (x==0)
                setPixelColor(xs,ys+j,rgb(0,0,0));
            if (x+w==elem->width)
                setPixelColor(xs+w-1,ys+j,rgb(0,0,0));
        }
    }
    return 1;
}
div* div_createDom(uint _id, uint x, uint y, uint w, uint h, dom* parent)
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
    t->ds._id=_id;
    t->ds.trans=1;
    t->bgColor.a=1;

    t->ds.entity=(void*)t;
    t->ds.onRender=drawDiv;

    prepend(parent,&t->ds);

    return t;
}

void _cascade_release(dom *elem)
{
    if (elem->frater!=0)
        _cascade_release(elem->frater);
    if (elem->descent!=0)
        _cascade_release(elem->descent);
    kfree((char*)(elem->entity));
}
void div_release(div* elem)
{
    if (elem->ds.parent!=0)
        delete(&elem->ds);
    _cascade_release(&elem->ds);
}
div* div_changeBgcolor(div* elem, color32 color)
{
    elem->bgColor=color;
    elem->ds.trans=color.a;

    reDraw(&elem->ds);

    return elem;
}
