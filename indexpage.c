#include "types.h"
#include "user.h"
#include "colordef.h"
#include "guientity_attrvalue.h"
#include "message.h"
#include "mouse.h"

int parher;
uint superhome;
uint slider;
uint bgc;
uint maincont;
uint starttext;
#define parh(x) (parher=x,&parher)
#define ntile 10

typedef struct ori_tile
{
    uint x;
    uint y;
    uint h;
    uint w;
    color32 bgcolor;

    uint tx;
    uint ty;
    char *text;

    uint px;
    uint py;
    char* picname;
    uint ph;
    uint pw;

    uchar* pic;
    uint tid;
    uint xid;
    uint pid;
} tile;
tile* ts;

uchar *readImg(char *fileName, uchar picMode)   //0:3channel,1:4channel
{
    int fd1 = open(fileName, 0);
    if (fd1 < 0)
    {
        printf(1, "open file error\n");
        return 0;
    }
    uchar w,h;
    read(fd1, &w, 1);
    read(fd1, &h, 1);
    int size=(uint)w*(uint)h,i;
    uchar *p=malloc(size*4+10);
    uchar *q,*tp,*tq;
    p[0]=w;
    p[1]=h;
    if (picMode==1)
    {
        read(fd1, p+2, size*4);
    }
    else if (picMode==0)
    {
        q=malloc(size*3+4);
        read(fd1, q, size*3);
        tp=p+2;
        tq=q;
        for (i=0;i<size;i++)
        {
            *(tp++)=*(q++);
            *(tp++)=*(q++);
            *(tp++)=*(q++);
            *(tp++)=0;
        }
        free(tq);
    }
    close(fd1);

    return p;
}
tile* initTiles()
{
    int i;
    contentStruct tcs;
    uchar ca[3]={0xff,0xff,0xff};
    tile* p=malloc(sizeof(tile)*ntile);
    (p+0)->x=187; (p+0)->y=224; (p+0)->w=210; (p+0)->h=100; (p+0)->bgcolor=rgba(217,81,43,0);
    (p+0)->px=80; (p+0)->py=17; (p+0)->picname="search.matrix";
    (p+0)->text="File";

    (p+1)->x=407; (p+1)->y=224; (p+1)->w=210; (p+1)->h=100; (p+1)->bgcolor=rgba(43,127,237,0);
    (p+1)->px=80; (p+1)->py=17; (p+1)->picname="filetext2.matrix";
    (p+1)->text="Text";

    (p+2)->x=627; (p+2)->y=224; (p+2)->w=210; (p+2)->h=100; (p+2)->bgcolor=rgba(184,28,67,0);
    (p+2)->px=80; (p+2)->py=17; (p+2)->picname="image.matrix";
    (p+2)->text="Image";

    (p+3)->x=187; (p+3)->y=334; (p+3)->w=210; (p+3)->h=100; (p+3)->bgcolor=rgba(95,60,186,0);
    (p+3)->px=80; (p+3)->py=17; (p+3)->picname="bug.matrix";
    (p+3)->text="Minesweeper";

    (p+4)->x=407; (p+4)->y=334; (p+4)->w=210; (p+4)->h=100; (p+4)->bgcolor=rgba(0,156,0,0);
    (p+4)->px=80; (p+4)->py=17; (p+4)->picname="users.matrix";
    (p+4)->text="About";

    (p+5)->x=627; (p+5)->y=334; (p+5)->w=100; (p+5)->h=100; (p+5)->bgcolor=rgba(42,127,237,0);
    (p+5)->px=25; (p+5)->py=17; (p+5)->picname="headphones.matrix";
    (p+5)->text="Music";

    (p+6)->x=737; (p+6)->y=334; (p+6)->w=100; (p+6)->h=100; (p+6)->bgcolor=rgba(95,60,186,0);
    (p+6)->px=25; (p+6)->py=17; (p+6)->picname="cog.matrix";
    (p+6)->text="Set";

    (p+7)->x=187; (p+7)->y=444; (p+7)->w=210; (p+7)->h=100; (p+7)->bgcolor=rgba(43,127,237,0);
    (p+7)->px=80; (p+7)->py=17; (p+7)->picname="bubble2.matrix";
    (p+7)->text="ReadMe";

    (p+8)->x=407; (p+8)->y=444; (p+8)->w=100; (p+8)->h=100; (p+8)->bgcolor=rgba(184,28,67,0);
    (p+8)->px=25; (p+8)->py=17; (p+8)->picname="alarm.matrix";
    (p+8)->text="Clock";

    (p+9)->x=517; (p+9)->y=444; (p+9)->w=100; (p+9)->h=100; (p+9)->bgcolor=rgba(155,37,163,0);
    (p+9)->px=25; (p+9)->py=17; (p+9)->picname="switch.matrix";
    (p+9)->text="Shut";

    for (i=0;i<ntile;i++)
    {
        (p+i)->tx=5;
        (p+i)->ty=70;
        (p+i)->pw=64;
        (p+i)->ph=64;
        (p+i)->pic=readImg((p+i)->picname,1);
    }

    for (i=0;i<ntile;i++)
    {
        createdom(GUIENT_DIV,maincont,&((p+i)->tid));
        setattr(GUIENT_DIV,(p+i)->tid,GUIATTR_DIV_X,&((p+i)->x));
        setattr(GUIENT_DIV,(p+i)->tid,GUIATTR_DIV_Y,&((p+i)->y));
        setattr(GUIENT_DIV,(p+i)->tid,GUIATTR_DIV_WIDTH,&((p+i)->w));
        setattr(GUIENT_DIV,(p+i)->tid,GUIATTR_DIV_HEIGHT,&((p+i)->h));
        setattr(GUIENT_DIV,(p+i)->tid,GUIATTR_DIV_BGCOLOR,&((p+i)->bgcolor));
        setattr(GUIENT_DIV,(p+i)->tid,GUIATTR_DIV_INTEG,0);

        createdom(GUIENT_IMG,(p+i)->tid,&((p+i)->pid));
        setattr(GUIENT_IMG,(p+i)->pid,GUIATTR_IMG_X,&((p+i)->px));
        setattr(GUIENT_IMG,(p+i)->pid,GUIATTR_IMG_Y,&((p+i)->py));
        setattr(GUIENT_IMG,(p+i)->pid,GUIATTR_IMG_WIDTH,&((p+i)->pw));
        setattr(GUIENT_IMG,(p+i)->pid,GUIATTR_IMG_HEIGHT,&((p+i)->ph));
        tcs.isRepeat=0;
        tcs.pics=(p+i)->pic;
        setattr(GUIENT_IMG,(p+i)->pid,GUIATTR_IMG_CONTENT,&tcs);

        createdom(GUIENT_TXT,(p+i)->tid,&((p+i)->xid));
        setattr(GUIENT_TXT,(p+i)->xid,GUIATTR_TXT_X,&((p+i)->tx));
        setattr(GUIENT_TXT,(p+i)->xid,GUIATTR_TXT_Y,&((p+i)->ty));
        setattr(GUIENT_TXT,(p+i)->xid,GUIATTR_TXT_WIDTH,&((p+i)->w));
        setattr(GUIENT_TXT,(p+i)->xid,GUIATTR_TXT_HEIGHT,parh(24));
        setattr(GUIENT_TXT,(p+i)->xid,GUIATTR_TXT_STR,(p+i)->text);
        setattr(GUIENT_TXT,(p+i)->xid,GUIATTR_TXT_COLOR,ca);
    }

    return p;
}
void setupGUI()
{
    color32 cl;
    uchar ca[10];

    createdom(GUIENT_DIV,0xffffffff,&bgc);
    setattr(GUIENT_DIV,bgc,GUIATTR_DIV_X,parh(0));
    setattr(GUIENT_DIV,bgc,GUIATTR_DIV_Y,parh(0));
    setattr(GUIENT_DIV,bgc,GUIATTR_DIV_WIDTH,parh(1024));
    setattr(GUIENT_DIV,bgc,GUIATTR_DIV_HEIGHT,parh(768));
    cl=rgba(1,64,81,0);
    setattr(GUIENT_DIV,bgc,GUIATTR_DIV_BGCOLOR,&cl);

    createdom(GUIENT_DIV,0xffffffff,&superhome);
    setattr(GUIENT_DIV,superhome,GUIATTR_DIV_X,parh(0));
    setattr(GUIENT_DIV,superhome,GUIATTR_DIV_Y,parh(0));
    setattr(GUIENT_DIV,superhome,GUIATTR_DIV_WIDTH,parh(15));
    setattr(GUIENT_DIV,superhome,GUIATTR_DIV_HEIGHT,parh(15));

    createdom(GUIENT_DIV,0xffffffff,&slider);
    setattr(GUIENT_DIV,slider,GUIATTR_DIV_X,parh(-300));
    setattr(GUIENT_DIV,slider,GUIATTR_DIV_Y,parh(0));
    setattr(GUIENT_DIV,slider,GUIATTR_DIV_WIDTH,parh(200));
    setattr(GUIENT_DIV,slider,GUIATTR_DIV_HEIGHT,parh(768));
    cl=rgba(0,0,0,95);
    setattr(GUIENT_DIV,slider,GUIATTR_DIV_BGCOLOR,&cl);


    createdom(GUIENT_DIV,bgc,&maincont);
    setattr(GUIENT_DIV,maincont,GUIATTR_DIV_X,parh(0));
    setattr(GUIENT_DIV,maincont,GUIATTR_DIV_Y,parh(0));
    setattr(GUIENT_DIV,maincont,GUIATTR_DIV_WIDTH,parh(1024));
    setattr(GUIENT_DIV,maincont,GUIATTR_DIV_HEIGHT,parh(768));
    cl=rgba(1,64,81,0);
    setattr(GUIENT_DIV,maincont,GUIATTR_DIV_BGCOLOR,&cl);

    ts=initTiles();

    createdom(GUIENT_TXT,maincont,&starttext);
    setattr(GUIENT_TXT,starttext,GUIATTR_TXT_X,parh(187));
    setattr(GUIENT_TXT,starttext,GUIATTR_TXT_Y,parh(119));
    setattr(GUIENT_TXT,starttext,GUIATTR_TXT_WIDTH,parh(300));
    setattr(GUIENT_TXT,starttext,GUIATTR_TXT_HEIGHT,parh(24));
    setattr(GUIENT_TXT,starttext,GUIATTR_TXT_STR,"Start");
    ca[0]=ca[1]=ca[2]=0xff;
    setattr(GUIENT_TXT,starttext,GUIATTR_TXT_COLOR,ca);
}
void initAll()
{
    initprocessqueue();
    setupGUI();
}
void killAll()
{
    releaseprocessqueue();
    exit();
}
//==================event=========================
//================================================
void OnMouseIn(uint domID)
{
    uint i;
    color24 p;
    for (i=0;i<ntile;i++)
        if ((ts+i)->tid==domID)
        {
            p=((ts+i)->bgcolor.c);
            p.r=p.r<225?p.r+30:0xff;
            p.g=p.g<225?p.g+30:0xff;
            p.b=p.b<225?p.b+30:0xff;
            setattr(GUIENT_DIV,(ts+i)->tid,GUIATTR_DIV_BGCOLOR,&p);
            return;
        }
}
void OnMouseOut(uint domID)
{
    uint i;
    for (i=0;i<ntile;i++)
        if ((ts+i)->tid==domID)
        {
            setattr(GUIENT_DIV,(ts+i)->tid,GUIATTR_DIV_BGCOLOR,&((ts+i)->bgcolor));
            return;
        }
}
//==================main=========================
//================================================
int main(int argc, char *argv[])
{
    initAll();
    int *msg = (int*)malloc(100);
    MouseMsg* km;
    while (1)
    {
        getmsgfromqueue(msg);
        if (*msg==MOUSE_MESSAGE)
        {
            km=(MouseMsg*)msg;
            if (km->enter_or_leave==MOUSE_ENTER)
                OnMouseIn(km->dom_id);
            else if (km->enter_or_leave==MOUSE_LEAVE)
                OnMouseOut(km->dom_id);
        }
    }
}