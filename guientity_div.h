/*
**  Premise headers:
**      types.h
**      guilayout.h
**      graphbase.h
*/

typedef struct ori_div
{
    dom ds;
    color32 bgColor;
} div;

uint/*div**/ div_createDom(uint x, uint y, uint w, uint h, uint/*dom**/ parent);
uint/*div**/ div_changeBgcolor(uint/*div**/ elem, color32 color);
void div_release(uint/*div**/ elem);

uint div_setAttr(uint elem_, int attr, void *val);
uint div_getAttr(uint elem_, int attr, void *des);