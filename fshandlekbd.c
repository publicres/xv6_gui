#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "colordef.h"
#include "guientity_attrvalue.h"
#include "message.h"
#include "mouse.h"

#define MAX_DIRECTORY_LEN 512
#define FOLDERTYPE 1
#define NOTYPE 0
#define FILETYPE 2
#define FILENUMINAROW 7
#define SCROLL_DISTANCE 50
#define RIGHT_CLICK_MENU_Y_POS 610

typedef struct lsresult
{
    char* filenames;
    char** parseresult;
    int* fileinfo;
}LSResult;

typedef struct guifilenodes
{
    uint* nodes;
    uint* icons;
    uint* titles;
    int num;
}FileNodes;

uint windowparent = 0xffffffff;
FileNodes filenodes;
int last_right_clicked_fileno = -1;
char* current_path;
char* last_copied_path;
char* last_moved_path;
LSResult listresult;
int file_type_to_be_operate = NOTYPE;
contentStruct folder_type_content;
contentStruct bitmap_type_content;
contentStruct file_type_content;
contentStruct text_type_content;
contentStruct close_icon_content;

uint j;
#define parh(x) (j=x,&j)

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


LSResult ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  LSResult r;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\r\n", path);
    r.filenames = (char*)malloc(2 * sizeof(char));
    memset(r.filenames, '\0', 3);
    r.fileinfo = (int*)malloc(2 * sizeof(int));
    r.parseresult = (char**)0;
    return r;
  }
  
  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\r\n", path);
    r.filenames = (char*)malloc(2 * sizeof(char));
    memset(r.filenames, '\0', 3);
    r.fileinfo = (int*)malloc(2 * sizeof(int));
    r.parseresult = (char**)0;
    close(fd);
    return r;
  }

  char *result = (char*)malloc(1400 * sizeof(char));
  memset(result, '\0', 1400);
  int* info = (int*)malloc(100 * sizeof(int));
  memset(info, 0, 100 * sizeof(int));
  int n = 0;
  
  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d\r\n", fmtname(path), st.type, st.ino, st.size);
    strcpy(result + strlen(result), fmtname(path));
    info[n++] = st.type;
    break;
  
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\r\n");
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
        printf(1, "ls: cannot stat %s\r\n", buf);
        continue;
      }
      //printf(1, "%s %d %d %d\r\n", fmtname(buf), st.type, st.ino, st.size);
      strcpy(result + strlen(result), fmtname(buf));
      info[n++] = st.type;
    }
    break;
  }
  close(fd);
  r.filenames = result;
  r.fileinfo = info;
  r.parseresult = parseAfterls(r.filenames);
  return r;
}


int copyfile(char* srcpath, char* destpath)
{
    int file_src, file_dest;
    if ((file_src = open(srcpath, O_RDONLY)) < 0)
    {
        printf(2, "The file you are going to copy does not exists!!\r\n");
        return -1;
    }
    if ((file_dest = open(destpath, O_RDONLY)) >= 0)
    {
        printf(2, "The destination you are going to copy the file has already had a file with the same name!!\r\n");
        close(file_dest);
        return -1;
    }
    if ((file_dest = open(destpath, O_CREATE)) < 0)
    {
        printf(2, "Cannot create a new file!!!\r\n");
        return -1;
    }
    close(file_dest);
    if ((file_dest = open(destpath, O_WRONLY)) < 0)
    {
        printf(2, "Cannot write to the new file!!!\r\n");
        return -1;
    }
    char* buff = (char*)malloc(4096 * sizeof(char));
    memset(buff, 0, 4096);
    int size;
    while((size = read(file_src, buff, 4096)) > 0)
        write(file_dest, buff, size);
    if(size < 0)
    {
        printf(2, "copy file error!!!\r\n");
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
    if (((ff = open(path, O_RDONLY)) >= 0))
    {
        printf(2, "cannot create new file, the file has existed!!!\r\n");
        close(ff);
        return -1;
    }
    if ((ff = open(path, O_CREATE)) < 0)
    {
        printf(2, "cannot create new file\r\n");
        return -1;
    }
    else
    {
        printf(2, "create file success!!!\r\n");
        close(ff);
        return 0;
    }
}

int createnewfolder(char *foldername)
{
    if(mkdir(foldername) < 0)
    {
        printf(2, "cannot make directory\r\n");
        return -1;
    }
    else
    {
        printf(2, "make directory success\r\n");
        return 0;
    }
}

//the path passed in must ensure that it is the pure name of a child of the current directory
int changedirectory(char* path) 
{
    if(chdir(path) < 0)
    {
        printf(2, "cannot cd\r\n");
        return -1;
    }
    else
    {
        printf(2, "cd success\r\n");
        int n = strlen(path);
        int i;
        if (n == 2 && path[0] == '.' && path[1] == '.')
        {
            if (!(strlen(current_path) == 1 && current_path[0] == '/'))
            {
                int tmpn = strlen(current_path);
                current_path[tmpn - 1] = '\0';
                for (i = tmpn - 2; i > 0; i--)
                {
                    if (current_path[i] != '/')
                        current_path[i] = '\0';
                    else
                        break;
                }
            }
        }
        else
        {
            int tmpn = strlen(current_path);
            strcpy(current_path + tmpn, path);
            tmpn = strlen(current_path);
            current_path[tmpn] = '/';
            current_path[tmpn + 1] = '\0';
        }
        return 0;
    }
}

int removefile(char* path)
{
    if (unlink(path) < 0)
    {
        printf(2, "cannot remove, the thing you want to remove is being used!!  %s\r\n", path);
        return -1;
    }
    else
    {
        printf(2, "remove file success!!!\r\n");
        return 0;
    }
}

int removefolder(char* path)
{
    LSResult lsr = ls(path);
    int n = strlen(lsr.filenames) / 14 - 2;
    int r;
    int pathlen = strlen(path);
    char* savedpath = (char*)malloc((pathlen + 2) * sizeof(char));
    memset(savedpath, '\0', pathlen + 2);
    strcpy(savedpath, path);
    savedpath[pathlen] = '/';
    savedpath[pathlen + 1] = '\0';
    char* tmppath = (char*)malloc(MAX_DIRECTORY_LEN);
    if (n > 0)  //there are files or folders in this folder
    {
        int i;
        for (i = 0; i < n; i++)
        {
            memset(tmppath, '\0', MAX_DIRECTORY_LEN);
            strcpy(tmppath, savedpath);
            strcpy(tmppath + strlen(tmppath), lsr.parseresult[i]);
            if (lsr.fileinfo[i+2] == FOLDERTYPE)
            {
                r = removefolder(tmppath);
                if (r == -1)
                {
                    printf(2, "There are something wrong when recursively remove folder!!\r\n");
                    free(tmppath);
                    free(savedpath);
                    return -1;
                }
            }
            else
            {
                r = removefile(tmppath);
                if (r == -1)
                {
                    printf(2, "There are something wrong when recursively remove file!!\r\n");
                    free(tmppath);
                    free(savedpath);
                    return -1;
                }
            }
        }
    }
    if (unlink(path) < 0)
    {
        printf(2, "There are something wrong when delete the empty folder!!!\r\n");
        free(tmppath);
        free(savedpath);
        return -1;
    }
    else
    {
        printf(2, "remove folder success!!!\r\n");
        free(tmppath);
        free(savedpath);
        return 0;
    }
}

int movefile(char* srcpath, char* destpath)
{
    if (copyfile(srcpath, destpath) == -1)
        return -1;
    if (removefile(srcpath) == -1)
        return -1;
    return 0;
}

//copyfolder("/A/B", "/C/D")   -> /C/D(B->D)
int copyfolder(char* srcpath, char* destpath)
{
    int fds;
    if((fds = open(srcpath, O_RDONLY)) < 0)
    {
        printf(2, "The folder you want to copy does not exist!!!!\r\n");
        return -1;
    }
    close(fds);
    int fdd;
    if ((fdd = open(destpath, O_RDONLY)) >= 0)
    {
        printf(2, "The destination has already had a folder with the same name!!!\r\n");
        close(fdd);
        return -1;
    }
    if (createnewfolder(destpath) < 0)
    {
        printf(2, "copyfolder: cannot create the new folder!!!!\r\n");
        return -1;
    }
    LSResult srccontent = ls(srcpath);
    int n = strlen(srccontent.filenames) / 14 - 2;
    int i;
    char* tmpsrc, *tmpdest;
    tmpsrc = (char*)malloc(MAX_DIRECTORY_LEN);
    tmpdest = (char*)malloc(MAX_DIRECTORY_LEN);
    int srclen = strlen(srcpath);
    char* savedsrc = (char*)malloc((srclen + 2) * sizeof(char));
    memset(savedsrc, '\0', srclen + 2);
    strcpy(savedsrc, srcpath);
    savedsrc[srclen] = '/';
    savedsrc[srclen+1] = '\0';
    int destlen = strlen(destpath);
    char* saveddest = (char*)malloc((destlen + 2) * sizeof(char));
    memset(saveddest, '\0', destlen + 2);
    strcpy(saveddest, destpath);
    saveddest[destlen] = '/';
    saveddest[destlen+1] = '\0';
    for (i = 0; i < n; i++)
    {
        memset(tmpsrc, '\0', MAX_DIRECTORY_LEN);
        memset(tmpdest, '\0', MAX_DIRECTORY_LEN);
        strcpy(tmpsrc, savedsrc);
        strcpy(tmpdest, saveddest);
        strcpy(tmpsrc + strlen(tmpsrc), srccontent.parseresult[i]);
        strcpy(tmpdest + strlen(tmpdest), srccontent.parseresult[i]);
        if (srccontent.fileinfo[i+2] == FOLDERTYPE)
        {
            if (copyfolder(tmpsrc, tmpdest) < 0)
            {
                printf(2, "copyfolder: We meet some problem when recursively copy folder!!!\r\n");
                free(tmpsrc);
                free(tmpdest);
                free(savedsrc);
                free(saveddest);
                return -1;
            }
        }
        else
        {
            if (copyfile(tmpsrc, tmpdest) < 0)
            {
                printf(2, "copyfolder: We meet some problem when recursively copy file!!!\r\n");
                free(tmpsrc);
                free(tmpdest);
                free(savedsrc);
                free(saveddest);
                return -1;
            }
        }
    }
    free(tmpsrc);
    free(tmpdest);
    free(savedsrc);
    free(saveddest);
    printf(2, "copyfolder: copy folder success!!!\r\n");
    return 0;
}

int movefolder(char* srcpath, char* destpath)
{
    if (copyfolder(srcpath, destpath) == -1)
    {
        printf(2, "movefolder: There are something wrong when we copyfolder folder!!!\r\n");
        return -1;
    }
    if (removefolder(srcpath) == -1)
    {
        printf(2, "movefolder: There are something wrong when we delete the folder!!!\r\n");
        return -1;
    }
    printf(2, "movefolder success!!!\r\n");
    return 0;
}

//in this case, the parameter passed to here is pure filename, it don't need the concrete path
int renamefile(char* originname, char* nowname)
{
    int r = movefile(originname, nowname);
    if (r < 0)
        printf(2, "rename fail!!!\r\n");
    else
        printf(2, "rename success!!!\r\n");
    return r;
}

char* createInputNameDialog(char* defaultstring, uint parent)
{
    uint dialog;
        uint dialog_titlebar;
            uint dialog_title_textview;
            uint dialog_closebutton;
                uint dialog_closeicon;
        uint warning_textview;
        uint okbutton;
            uint oktext;
        uint cancelbutton;
            uint canceltext;


    color32 dialog_background = rgba(255, 255, 255, 0);
    color32 dialog_titlebar_color = rgba(210, 174, 142, 0);
    color32 warning_textcolor = rgba(0, 0, 0, 0);
    color32 okbutton_color = rgba(0, 255, 0, 0);
    color32 cancelbutton_color = rgba(255, 0, 0, 0);
    color32 dialog_closebutton_color = rgba(200, 80, 81, 0);

    char* ok = "OK";
    char* title_text = "input name:";
    char* cancel = "Cancel";


    createdom(GUIENT_DIV, parent, &dialog);
    setattr(GUIENT_DIV, dialog, GUIATTR_DIV_X, parh(300));
    setattr(GUIENT_DIV, dialog, GUIATTR_DIV_Y, parh(250));
    setattr(GUIENT_DIV, dialog, GUIATTR_DIV_WIDTH, parh(500));
    setattr(GUIENT_DIV, dialog, GUIATTR_DIV_HEIGHT, parh(155));
    setattr(GUIENT_DIV, dialog, GUIATTR_DIV_BGCOLOR, &dialog_background);

        createdom(GUIENT_DIV, dialog, &dialog_titlebar);
        setattr(GUIENT_DIV, dialog_titlebar, GUIATTR_DIV_X, parh(0));
        setattr(GUIENT_DIV, dialog_titlebar, GUIATTR_DIV_Y, parh(0));
        setattr(GUIENT_DIV, dialog_titlebar, GUIATTR_DIV_WIDTH, parh(500));
        setattr(GUIENT_DIV, dialog_titlebar, GUIATTR_DIV_HEIGHT, parh(35));
        setattr(GUIENT_DIV, dialog_titlebar, GUIATTR_DIV_BGCOLOR, &dialog_titlebar_color);

            createdom(GUIENT_TXT, dialog_titlebar, &dialog_title_textview);
            setattr(GUIENT_TXT, dialog_title_textview, GUIATTR_TXT_X, parh(167));
            setattr(GUIENT_TXT, dialog_title_textview, GUIATTR_TXT_Y, parh(5));
            setattr(GUIENT_TXT, dialog_title_textview, GUIATTR_TXT_WIDTH, parh(165));
            setattr(GUIENT_TXT, dialog_title_textview, GUIATTR_TXT_HEIGHT, parh(24));
            setattr(GUIENT_TXT, dialog_title_textview, GUIATTR_TXT_STR, title_text);
            setattr(GUIENT_TXT, dialog_title_textview, GUIATTR_TXT_COLOR, &warning_textcolor);

            createdom(GUIENT_DIV, dialog_titlebar, &dialog_closebutton);
            setattr(GUIENT_DIV, dialog_closebutton, GUIATTR_DIV_X, parh(449));
            setattr(GUIENT_DIV, dialog_closebutton, GUIATTR_DIV_Y, parh(0));
            setattr(GUIENT_DIV, dialog_closebutton, GUIATTR_DIV_WIDTH, parh(51));
            setattr(GUIENT_DIV, dialog_closebutton, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, dialog_closebutton, GUIATTR_DIV_BGCOLOR, &dialog_closebutton_color);
            setattr(GUIENT_DIV, dialog_closebutton, GUIATTR_DIV_INTEG, parh(1));

                createdom(GUIENT_IMG, dialog_closebutton, &dialog_closeicon);
                setattr(GUIENT_IMG, dialog_closeicon, GUIATTR_IMG_X, parh(18));
                setattr(GUIENT_IMG, dialog_closeicon, GUIATTR_IMG_Y, parh(7));
                setattr(GUIENT_IMG, dialog_closeicon, GUIATTR_IMG_WIDTH, parh(17));
                setattr(GUIENT_IMG, dialog_closeicon, GUIATTR_IMG_HEIGHT, parh(17));
                setattr(GUIENT_IMG, dialog_closeicon, GUIATTR_IMG_CONTENT, &close_icon_content);

        createdom(GUIENT_TXT, dialog, &warning_textview);
        setattr(GUIENT_TXT, warning_textview, GUIATTR_TXT_X, parh(10));
        setattr(GUIENT_TXT, warning_textview, GUIATTR_TXT_Y, parh(45));
        setattr(GUIENT_TXT, warning_textview, GUIATTR_TXT_WIDTH, parh(480));
        setattr(GUIENT_TXT, warning_textview, GUIATTR_TXT_HEIGHT, parh(72));
        setattr(GUIENT_TXT, warning_textview, GUIATTR_TXT_STR, defaultstring);
        setattr(GUIENT_TXT, warning_textview, GUIATTR_TXT_COLOR, &warning_textcolor);

        createdom(GUIENT_DIV, dialog, &okbutton);
        setattr(GUIENT_DIV, okbutton, GUIATTR_DIV_X, parh(120));
        setattr(GUIENT_DIV, okbutton, GUIATTR_DIV_Y, parh(125));
        setattr(GUIENT_DIV, okbutton, GUIATTR_DIV_WIDTH, parh(70));
        setattr(GUIENT_DIV, okbutton, GUIATTR_DIV_HEIGHT, parh(24));
        setattr(GUIENT_DIV, okbutton, GUIATTR_DIV_BGCOLOR, &okbutton_color);
        setattr(GUIENT_DIV, okbutton, GUIATTR_DIV_INTEG, parh(1));

            createdom(GUIENT_TXT, okbutton, &oktext);
            setattr(GUIENT_TXT, oktext, GUIATTR_TXT_X, parh(20));
            setattr(GUIENT_TXT, oktext, GUIATTR_TXT_Y, parh(0));
            setattr(GUIENT_TXT, oktext, GUIATTR_TXT_WIDTH, parh(30));
            setattr(GUIENT_TXT, oktext, GUIATTR_TXT_HEIGHT, parh(24));
            setattr(GUIENT_TXT, oktext, GUIATTR_TXT_STR, ok);
            setattr(GUIENT_TXT, oktext, GUIATTR_TXT_COLOR, &warning_textcolor);

        createdom(GUIENT_DIV, dialog, &cancelbutton);
        setattr(GUIENT_DIV, cancelbutton, GUIATTR_DIV_X, parh(310));
        setattr(GUIENT_DIV, cancelbutton, GUIATTR_DIV_Y, parh(125));
        setattr(GUIENT_DIV, cancelbutton, GUIATTR_DIV_WIDTH, parh(70));
        setattr(GUIENT_DIV, cancelbutton, GUIATTR_DIV_HEIGHT, parh(24));
        setattr(GUIENT_DIV, cancelbutton, GUIATTR_DIV_BGCOLOR, &cancelbutton_color);
        setattr(GUIENT_DIV, cancelbutton, GUIATTR_DIV_INTEG, parh(1));

            createdom(GUIENT_TXT, cancelbutton, &canceltext);
            setattr(GUIENT_TXT, canceltext, GUIATTR_TXT_X, parh(20));
            setattr(GUIENT_TXT, canceltext, GUIATTR_TXT_Y, parh(0));
            setattr(GUIENT_TXT, canceltext, GUIATTR_TXT_WIDTH, parh(30));
            setattr(GUIENT_TXT, canceltext, GUIATTR_TXT_HEIGHT, parh(24));
            setattr(GUIENT_TXT, canceltext, GUIATTR_TXT_STR, cancel);
            setattr(GUIENT_TXT, canceltext, GUIATTR_TXT_COLOR, &warning_textcolor);

    //message handler
    int *dmsg = (int*)malloc(100);
    MouseMsg* dmm;
    while(1)
    {
        getmsgfromqueue(dmsg);
        if (*dmsg == MOUSE_MESSAGE)
        {
            dmm = (MouseMsg*)dmsg;
            if ((dmm->mouse_event_type & LEFT_BTN_UP) != 0)
            {
                if (dmm->dom_id == dialog_closebutton || dmm->dom_id == okbutton)
                    break;
            }
        }
    }

    //============release
    releasedom(GUIENT_IMG, dialog_closeicon);
    releasedom(GUIENT_TXT, warning_textview);
    releasedom(GUIENT_TXT, oktext);
    releasedom(GUIENT_DIV, dialog);

    free(dmsg);
    return "";
}

//parameter: default name, current folder
int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf(2, "parameter num wrong!!!\r\n");
        exit();
    }
    else
    {
        renamefile(argv[1], "HH");
    }
    exit();
}