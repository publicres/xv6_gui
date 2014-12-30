#include "types.h"
#include "defs.h"
#include "graphbase.h"
#include "guilayout.h"
#include "eventpackage.h"

dom domRoot,delRoot;
dom* bingolingo=0;
dom* del=0;

void initDom()
{
    domRoot._id=(uint)(&domRoot);
    domRoot.x=domRoot.y=0;
    domRoot.width=WIDTH_RES;
    domRoot.height=HEIGHT_RES;
    domRoot.focus=0;
    domRoot.trans=0;

    domRoot.onPoint=0;
    domRoot.onRender=0;
    domRoot.onFocus=0;

    domRoot.parent=0;
    domRoot.frater=0;
    domRoot.descent=0;
    domRoot.entity=0;

    bingolingo=&domRoot;
    delRoot=domRoot;
    del=&delRoot;
    prepend(bingolingo,del);
}

dom* prepend(dom* src, dom* des)
{
    des->frater=src->descent;
    des->parent=src;
    src->descent=des;

    reDraw(des);

    return src;
}
void outputDom(dom* src, uint lay)
{
    if (src==0) return;
    uint i;
    for (i=0;i<lay;i++)
        cprintf("*");
    cprintf("%d\n",src->_id);
    outputDom(src->descent,lay+1);
    outputDom(src->frater,lay);
}
dom* delete(dom* src)
{
    dom *p=src->parent;
    uint x=src->x;
    uint y=src->y;
    uint w=src->width;
    uint h=src->height;

    if (p->descent==src)
    {
        p->descent=src->frater;
    }
    else
    {
        dom* q=p->descent;
        while (q->frater!=src)
            q=q->frater;
        q->frater=src->frater;
    }
    if (p->focus==src)
        p->focus=0;
    src->parent=0;
    src->frater=0;

    reDraw_(p,x,y,w,h);

    return src;
}
void _cascade_release(dom *elem)
{
    if (elem->frater!=0)
        _cascade_release(elem->frater);
    if (elem->descent!=0)
        _cascade_release(elem->descent);
    kfree((char*)(elem->entity));
}

dom* setFocus(dom* src)
{
    src->parent->focus=src;
    faireFocus(src);
    return src;
}
void setABSFocus(dom* src)
{
    dom* tsrc=src->parent;
    while (tsrc!=0)
    {
        tsrc->focus=src;
        tsrc=tsrc->parent;
        src=src->parent;
    }
    faireFocus(&domRoot);
}

uint getABSposx(dom* src)
{
    if (src==0) return 0;
    return src->x+getABSposx(src->parent);
}
uint getABSposy(dom* src)
{
    if (src==0) return 0;
    return src->y+getABSposy(src->parent);
}
//===========================================
#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)
//===========================================
void reDraw(dom *src)
{
    reDraw_(src,0,0,src->width,src->height);
}
void reDraw_(dom *src,uint x,uint y,uint w,uint h)
{
    passRenderEvent(bingolingo,getABSposx(src)+x,getABSposy(src)+y,w,h);
    sync(getABSposx(src)+x,getABSposy(src)+y,w,h);
}
//===========================================
void passFocusEvent(dom* now,void* pkg)
{
    if (now==0) return;
    if (now->onFocus==0 || now->onFocus(now,pkg)!=0)
        if (now->focus!=0)
            passFocusEvent(now->focus,pkg);
}
int passPointEvent(dom* now,uint x,uint y,uint typ)
{
    while (now!=0 && (now->x>x || now->x+now->width<=x || now->y>y || now->y+now->height<=y))
        now=now->frater;
    if (now==0)
        return 0;
    if (!passPointEvent(now->descent,x-now->x,y-now->y,typ))
        if (now->onPoint!=0)
            now->onPoint(now,x-now->x,y-now->y,typ);
    return 1;
    // if (now->onPoint==0 || now->onPoint(now,x-now->x,y-now->y,typ)!=0)
    //     passPointEvent(now->descent,x-now->x,y-now->y,typ);
}
void passRenderEvent(dom* now,uint x,uint y,uint w,uint h)
{
    if (w<=0 || h<=0)
        return;
    while (now!=0 && (x+w<=now->x || x>=now->x+now->width || y+h<=now->y || y>=now->y+now->height))
        now=now->frater;
    if (now==0)
        return;
    uint _x,_y,_w,_h;
    _x=max(x,now->x);
    _y=max(y,now->y);
    _w=min(x+w,now->x+now->width)-_x;
    _h=min(y+h,now->y+now->height)-_y;
    if (now->trans!=0)
    {
        passRenderEvent(now->frater,x,y,w,h);
    }
    else
    {
        passRenderEvent(now->frater,x,y,_x-x,h);
        passRenderEvent(now->frater,_x+_w,y,x+w-_x-_w,h);
        passRenderEvent(now->frater,_x,y,_w,_y-y);
        passRenderEvent(now->frater,_x,_y+_h,_w,y+h-_y-_h);
    }
    if (now->onRender==0 || now->onRender(now,_x-now->x,_y-now->y,_w,_h)!=0)
        passRenderEvent(now->descent,_x-now->x,_y-now->y,_w,_h);
}
//===========================================
void faireFocus(dom *now)
{
    focusEventPkg t;
    t._id=0x00;
    passFocusEvent(now,&t);
}
