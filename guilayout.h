/*
**  Written by Levy, BingoLingo
**  Intending to define the layout struct of pieces,
**  each of which is presented on the screen in the
**  form of rectangle. The structure is like what
**  HTML does, ie, DOM. Each Dom has several attr so
**  that the layout engine could base on it to draw
**  something and it's belonging-nodes.
*/
/*
**  Premise headers:
**      types.h
*/

typedef struct ori_dom
{
    uint _id;       //globally unique, used for identify the dom.

    uint x;         //the x value relative to its parent node.
    uint y;         //the y value relative to its parent node.
    uint width;     //ok to exceed the one of parent
    uint height;    //ok to exceed the one of parent

    struct ori_dom* focus;
    uchar trans;     //Reference: Minecraft. If true, always render the dom behind it.

    uchar (*onPoint)(struct ori_dom*,uint,uint,uint);
    uchar (*onRender)(struct ori_dom*,uint,uint,uint,uint);
    uchar (*onFocus)(struct ori_dom*,void*);

    struct ori_dom* parent;
    struct ori_dom* frater;
    struct ori_dom* descent;

    void* entity;

} dom;

typedef uchar (*pointEvent)(dom*,uint,uint,uint);
typedef uchar (*drawEvent)(dom*,uint,uint,uint,uint);
typedef uchar (*focusEvent)(dom*,void*);

void initDom();
dom* prepend(dom* src, dom* des);
dom* delete(dom* src);
dom* setFocus(dom* src);
void setABSFocus(dom* src);
uint getABSposx(dom* src);
uint getABSposy(dom* src);
void reDraw(dom *src);
void reDraw_(dom *src,uint x,uint y,uint w,uint h);
void outputDom(dom* src, uint lay);
void _cascade_release(dom *elem);

dom* bingolingo;
dom* del;

void passRenderEvent(dom* now,uint x,uint y,uint w,uint h);
void passFocusEvent(dom* now,void* pkg);
int passPointEvent(dom* now,uint x,uint y,uint typ);
void faireFocus(dom *now);
