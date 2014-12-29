#include "types.h"
#include "defs.h"
#include "graphbase.h"
#include "guilayout.h"
#include "ex_mem.h"
#include "guientity_img.h"
#include "guientity_attrvalue.h"

//==============================================
uchar drawImg(dom* elem, uint x, uint y, uint w, uint h)
{
    uint i,j;
    uchar picw,pich;
    color32 k;
    img *ent=(img*)(elem->entity);
    uint xs=getABSposx(elem)+x;
    uint ys=getABSposy(elem)+y;
    uint tmp;
    if (ent->imgContent==0)
        return 1;

    if (ent->isBigData==0)
    {
        picw=*(uchar*)(ent->imgContent);
        pich=*((uchar*)(ent->imgContent)+1);
        for (j=0;j<h;j++)
        {
            if (ent->isRepeat==0 && y+j>=pich) break;
            for (i=0;i<w;i++)
            {
                if (ent->isRepeat==0 && x+i>=picw) break;
                k=*(color32*)(ent->imgContent+2+ ((y+j)%pich*picw+(x+i)%picw)*sizeof(color32) );
                if (k.a==0)
                    setPixelColor(xs+i,ys+j,k.c);
                else if (k.a<255)
                    setPixelColor(xs+i,ys+j,mingle(getPixelColor(xs+i,ys+j),k));
            }
        }
    }
    else
    {
        picw=*(uchar*)(getPointer(ent->imgContent,0));
        pich=*(uchar*)(getPointer(ent->imgContent,1));
        for (j=0;j<h;j++)
        {
            if (ent->isRepeat==0 && y+j>=pich) break;
            for (i=0;i<w;i++)
            {
                if (ent->isRepeat==0 && x+i>=picw) break;
                tmp=2+((y+j)%pich*picw+(x+i)%picw)*sizeof(color32);

                k=rgba(*(uchar*)getPointer(ent->imgContent,tmp),*(uchar*)getPointer(ent->imgContent,tmp+1),
                    *(uchar*)getPointer(ent->imgContent,tmp+2),*(uchar*)getPointer(ent->imgContent,tmp+3));
                if (k.a==0)
                    setPixelColor(xs+i,ys+j,k.c);
                else if (k.a<255)
                    setPixelColor(xs+i,ys+j,mingle(getPixelColor(xs+i,ys+j),k));
            }
        }
    }

    return 1;
}
uint img_createDom(uint x, uint y, uint w, uint h, uint parent)
{
    img *t;
    if((t = (img*)kalloc()) == 0)
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

    t->ds.entity=(void*)t;
    t->ds.onRender=drawImg;

    t->isBigData=0;
    t->isRepeat=0;
    t->imgContent=0;

    if (parent==0xffffffff)
        prepend(bingolingo,&t->ds);
    else
        prepend((dom*)parent,&t->ds);

    return (uint)t;
}

void img_release(uint elem_)
{
    img* elem=(img*)elem_;
    img_setContent(elem_,0,0,0);
    if (elem->ds.parent!=0)
        delete(&elem->ds);
    _cascade_release(&elem->ds);
}

void img_setContent(uint elem, void* cont, uchar isBig, uchar isRep)
{
    img* ent=(img*)elem;
    if (ent->imgContent!=0)
    {
        if (ent->isBigData==0)
            kfree(ent->imgContent);
        else
            ex_free(ent->imgContent);
    }
    ent->imgContent=cont;
    ent->isBigData=isBig;
    ent->isRepeat=isRep;

    reDraw(&ent->ds);
}

uint img_setAttr(uint elem_, int attr, void *val)
{
    img *elem=(img*)elem_;
    uint i,j;
    contentStruct* p;
    void* q;

    switch (attr)
    {
    case GUIATTR_IMG_X:
        i=elem->ds.x;
        elem->ds.x=*((uint*)val);
        reDraw_(elem->ds.parent,i,elem->ds.y,elem->ds.width,elem->ds.height);
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_IMG_Y:
        j=elem->ds.y;
        elem->ds.y=*((uint*)val);
        reDraw_(elem->ds.parent,elem->ds.x,j,elem->ds.width,elem->ds.height);
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_IMG_WIDTH:
        i=elem->ds.width;
        elem->ds.width=*((uint*)val);
        reDraw_(elem->ds.parent,elem->ds.x,elem->ds.y,i,elem->ds.height);
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_IMG_HEIGHT:
        j=elem->ds.height;
        elem->ds.height=*((uint*)val);
        reDraw_(elem->ds.parent,elem->ds.x,elem->ds.y,elem->ds.width,j);
        reDraw(&elem->ds);
        return 0;
    case GUIATTR_IMG_CONTENT:
        p=(contentStruct*)val;
        i=*(uchar*)p->pics;
        j=*(uchar*)(p->pics+1);
        j=i*j*4+2;
        if (j<4096)
        {
            q=kalloc();
            memmove(q,p->pics,j);
            img_setContent(elem_,q,0,p->isRepeat);
        }
        else
        {
            q=ex_alloc(j);
            for (i=0;i<j;i++)
            {
                *(char*)(getPointer(q,i))=*(char*)(p->pics+i);
            }
            img_setContent(elem_,q,1,p->isRepeat);
        }
        return 0;
    case GUIATTR_IMG_REFRESH:
        reDraw(elem->ds.parent);
        return 0;
    default:
        return -1;
    }
    return 0;
}
uint img_getAttr(uint elem_, int attr, void *des)
{

    switch (attr)
    {
    case GUIATTR_IMG_X:
        return -1;
        break;
    case GUIATTR_IMG_Y:
        return -1;
        break;
    case GUIATTR_IMG_WIDTH:
        return -1;
        break;
    case GUIATTR_IMG_HEIGHT:
        return -1;
        break;
    default:
        return -1;
    }
    return 0;
}
