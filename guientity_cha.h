/*
**  Premise headers:
**      types.h
**      guilayout.h
**      graphbase.h
*/

typedef struct ori_cha
{
    dom ds;
    void* chaContent;
    color24 chColor;
    uchar ch;
} cha;

uint/*cha**/ cha_createDom(cha* chaPtr, uint x, uint y, uint w, uint h, uint/*dom**/ parent);
uint/*cha**/ cha_setColor(uint/*cha**/ elem, color24 color);
void cha_release(uint/*cha**/ elem);
void cha_setContent(uint/*cha**/ elem, void* cont, uchar ch);

uint cha_setAttr(uint elem_, int attr, void *val);
uint cha_getAttr(uint elem_, int attr, void *des);
