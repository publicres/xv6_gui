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

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    cat(0);
    exit();
  }
//======
    uchar* buff=(uchar*)malloc(3333 * sizeof(uchar));
    uchar* buff2=(uchar*)malloc(4444 * sizeof(uchar));
    uint huahua,j;
    int fd1, n, i1, j1;
    contentStruct pic;
    #define parh(x) (j=x,&j)

    fd1 = open("A", 0);
    if (fd1 < 0)
    {
        printf(1, "open file error\n");
    }
    else
    {
        n = read(fd1, buff, 3100);
        buff2[0]=buff2[1]=32;
        j1=2;
        for (i1=0;i1<n;i1+=3)
        {
            buff2[j1++]=buff[i1+2];
            buff2[j1++]=buff[i1+1];
            buff2[j1++]=buff[i1];
            buff2[j1++]=129;
        }

        createdom(GUIENT_IMG,0xffffffff,&huahua);
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_X,parh(50));
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_Y,parh(50));
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_WIDTH,parh(320));
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_HEIGHT,parh(320));
        pic.pics=buff2;
        pic.isRepeat=1;
        setattr(GUIENT_IMG,huahua,GUIATTR_IMG_CONTENT,&pic);

        uint qq;
        createdom(GUIENT_DIV,huahua,&qq);
        setattr(GUIENT_DIV,huahua,GUIATTR_DIV_X,parh(50));
        setattr(GUIENT_DIV,huahua,GUIATTR_DIV_X,parh(50));
    }
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
