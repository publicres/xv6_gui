#include "types.h"
#include "stat.h"
#include "user.h"
#include "colordef.h"
#include "guientity_attrvalue.h"

char buf[512];

void
cat(int fd)
{
  int n;

  while((n = read(fd, buf, sizeof(buf))) > 0)
    write(1, buf, n);
  if(n < 0){
    printf(1, "cat: read error\n");
    exit();
  }
}
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
    uchar *p=malloc(size*4+2);
    uchar *q,*tp,*tq;
    p[0]=w;
    p[1]=h;
    if (picMode==1)
    {
        read(fd1, p+2, size*4);
    }
    else if (picMode==0)
    {
        q=malloc(size*3);
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

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    exit();
  }
//======

    contentStruct pic;
    uint j;
    #define parh(x) (j=x,&j)
    uint huahua;
    uchar *p=readImg("cursor.matrix",1);

        createdom(GUIENT_IMG,0xffffffff,&huahua);
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_X,parh(50));
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_Y,parh(50));
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_WIDTH,parh(320));
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_HEIGHT,parh(320));
        pic.pics=p;
        pic.isRepeat=1;
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_CONTENT,&pic);

//======

  for(i = 1; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "cat: cannot open %s\n", argv[i]);
      exit();
    }
    cat(fd);
    close(fd);
  }
 //====

 //===
  exit();
}
