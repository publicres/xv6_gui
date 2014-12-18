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

uchar drawDiv(dom* elem, uint x, uint y, uint w, uint h);
div* div_createDom(uint _id, uint x, uint y, uint w, uint h, dom* parent);
div* div_changeBgcolor(div* elem, color32 color);
