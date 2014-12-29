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
    uchar isBigData;
    uchar isRepeat;
} img;

uint/*img**/ img_createDom(uint x, uint y, uint w, uint h, uint/*dom**/ parent);
void img_release(uint/*img**/ elem);
void img_setContent(uint/*img**/ elem, void* cont, uchar isBig, uchar isRep);

uint img_setAttr(uint elem_, int attr, void *val);
uint img_getAttr(uint elem_, int attr, void *des);
