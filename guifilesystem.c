#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"


char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

char*
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  char *result = (char*)malloc(1024 * sizeof(char));
  memset(result, '\0', 512);
  
  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return "";
  }
  
  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return "";
  }
  
  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\n", fmtname(path), st.type, st.ino, st.size);
    strcpy(result + strlen(result), fmtname(path));
    break;
  
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      //printf(1, "%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
      strcpy(result + strlen(result), fmtname(buf));
    }
    break;
  }
  close(fd);
  return result;
}

char** parseAfterls(char* result)
{
    char* tmp = (char*)malloc(15 * sizeof(char));
    memset(tmp, '\0', 15);
    int i, n = strlen(result) / 14, j, k;
    char** r = (char**)malloc(n * sizeof(char*) - 2);
    int ii = 0;
    for (i = 0; i < n; i++)
    {
        j = i * 14;
        for (k = 0; k < 14; k++)
        {
            if (result[j+k] == ' ' || result[j+k] == '\n' || result[j+k] == '\r')
            {
                break;
            }
            else
            {
                tmp[k] = result[j+k];
            }
        }
        if (strlen(tmp) != 0 && tmp[0] != '.')  //has got a file name or a folder name
        {
            r[ii] = tmp;
            ii++;
        }
        tmp = (char*)malloc(15 * sizeof(char));
        memset(tmp, '\0', 15);
    }
    return r;
}

int copyfile(char* srcpath, char* destpath)
{
    int file_src, file_dest;
    if ((file_src = open(srcpath, O_RDONLY)) < 0)
    {
        printf(2, "The file you are going to copy does not exists!!\n");
        return -1;
    }
    if ((file_dest = open(destpath, O_RDONLY)) >= 0)
    {
        printf(2, "The destination you are going to copy the file has already had a file with the same name!!\n");
        close(file_dest);
        return -1;
    }
    if ((file_dest = open(destpath, O_CREATE)) < 0)
    {
        printf(2, "Cannot create a new file!!!\n");
        return -1;
    }
    close(file_dest);
    if ((file_dest = open(destpath, O_WRONLY)) < 0)
    {
        printf(2, "Cannot write to the new file!!!\n");
        return -1;
    }
    char* buff = (char*)malloc(4096 * sizeof(char));
    memset(buff, 0, 4096);
    int size;
    while((size = read(file_src, buff, 4096)) > 0)
        write(file_dest, buff, size);
    if(size < 0)
    {
        printf(2, "copy file error!!!\n");
        return -1;
    }
    close(file_src);
    close(file_dest);
    free(buff);
    return 0;
}

int createnewfile(char* path)
{
    int ff;
    if((ff = open(path, O_CREATE)) < 0)
    {
        printf(2, "cannot create new file\n");
        return -1;
    }
    else
    {
        printf(2, "create file success!!!\n");
        close(ff);
        return 0;
    }
}

int createnewfolder(char *foldername)
{
    if(mkdir(foldername) < 0)
    {
        printf(2, "cannot make directory\n");
        return -1;
    }
    else
    {
        printf(2, "make directory success\n");
        return 0;
    }
}

int changedirectory(char* path)
{
    if(chdir(path) < 0)
    {
        printf(2, "cannot cd\n");
        return -1;
    }
    else
    {
        printf(2, "cd success\n");
        return 0;
    }
}

int removefolderorfile(char* path)
{
    if (unlink(path) < 0)
    {
        printf(2, "cannot remove, the thing you want to remove is being used!!\n");
        return -1;
    }
    else
    {
        printf(2, "remove success!!!\n");
        return 0;
    }
}

int movefile(char* srcpath, char* destpath)
{
    if (copyfile(srcpath, destpath) == -1)
        return -1;
    if (removefolderorfile(srcpath) == -1)
        return -1;
    return 0;
}

int main(int argc, char *argv[])
{
    //=====ls
    /*char* result = ls(".");
    printf(1, "%s\n", result);
    char** parseresult = parseAfterls(result);
    int n = strlen(result) / 14 - 2, i;
    for (i = 0; i < n; i++)
    {
        printf(1, "%s\n", parseresult[i]);
    }*/

    copyfile("README", "B");
    createnewfolder("A");
    movefile("B", "A/B");

    exit();
}