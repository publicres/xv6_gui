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
    #define parh(x) (j=x,&j)
    uint huahua,j,p2;
    createdom(GUIENT_DIV,0xffffffff,&huahua);
    setattr(GUIENT_DIV,huahua,GUIATTR_DIV_X,parh(20));
    setattr(GUIENT_DIV,huahua,GUIATTR_DIV_Y,parh(90));
    setattr(GUIENT_DIV,huahua,GUIATTR_DIV_WIDTH,parh(190));
    setattr(GUIENT_DIV,huahua,GUIATTR_DIV_HEIGHT,parh(290));
    setattr(GUIENT_DIV,huahua,GUIATTR_DIV_HEIGHT,parh(290));
    color32 c=rgba(233,100,20,100);
    setattr(GUIENT_DIV,huahua,GUIATTR_DIV_BGCOLOR,&c);

    createdom(GUIENT_DIV,huahua,&p2);
    setattr(GUIENT_DIV,p2,GUIATTR_DIV_WIDTH,parh(100));
    setattr(GUIENT_DIV,p2,GUIATTR_DIV_HEIGHT,parh(1000));
    c.a=0;
    c.c.r=200;
    setattr(GUIENT_DIV,p2,GUIATTR_DIV_BGCOLOR,&c);

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
    printf(1,"%d",j);
    setattr(GUIENT_DIV,huahua,GUIATTR_DIV_Y,parh(290));
 //===
  exit();
}
