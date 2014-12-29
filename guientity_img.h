/*
**  Premise headers:
**      types.h
**      guilayout.h
**      graphbase.h
*/

typedef struct ori_img
{
    dom ds;
    void* imgContent;
} img;

uint/*img**/ img_createDom(uint x, uint y, uint w, uint h, uint/*dom**/ parent);
uint/*img**/ img_changeBgcolor(uint/*img**/ elem, color32 color);
void img_release(uint/*img**/ elem);

uint img_setAttr(uint elem_, int attr, void *val);
uint img_getAttr(uint elem_, int attr, void *des);
