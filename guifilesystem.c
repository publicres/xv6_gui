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
#define FILETYPE 1
#define FILENUMINAROW 10
#define SCROLL_DISTANCE 20

typedef struct lsresult
{
    char* filenames;
    char** parseresult;
    int* fileinfo;
}LSResult;

typedef struct guifilenodes
{
    uint* nodes;
    int num;
}FileNodes;

uint windowparent = 0xffffffff;
FileNodes filenodes;
int last_right_clicked_fileno = -1;
char* current_path;
char* last_copied_path;
char* last_moved_path;
LSResult listresult;

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
                current_path[n - 1] = '\0';
                for (i = n - 2; i > 0; i--)
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
            strcpy(current_path + n, path);
            current_path[strlen(current_path)] = '/';
        }
        return 0;
    }
}

int removefolderorfile(char* path)
{
    if (unlink(path) < 0)
    {
        printf(2, "cannot remove, the thing you want to remove is being used!!\r\n");
        return -1;
    }
    else
    {
        printf(2, "remove success!!!\r\n");
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

void render(char** names, int num, uint parent, int* fileinfo)
{
    int i;
    filenodes.nodes = (uint*)malloc(num * sizeof(uint));
    filenodes.num = num;
    color32 folder_color = rgba(0, 255, 255, 0);
    color32 file_color = rgba(255, 0, 255, 0);
    memset(filenodes.nodes, 0, num * sizeof(uint));
    for (i = 0; i < num; i++)
    {
        //printf(1, "%s\r\n", names[i]);
        int row = i / 10;
        int col = i % 10;
        createdom(GUIENT_DIV, parent, &filenodes.nodes[i]);
        setattr(GUIENT_DIV, filenodes.nodes[i], GUIATTR_DIV_X, parh(17 + 100 * col)); //(90+10)
        setattr(GUIENT_DIV, filenodes.nodes[i], GUIATTR_DIV_Y, parh(10 + 125 * row)); //(115+10)
        setattr(GUIENT_DIV, filenodes.nodes[i], GUIATTR_DIV_WIDTH, parh(90));
        setattr(GUIENT_DIV, filenodes.nodes[i], GUIATTR_DIV_HEIGHT, parh(115));
        if (fileinfo[i] == FILETYPE) //folder
            setattr(GUIENT_DIV, filenodes.nodes[i], GUIATTR_DIV_BGCOLOR, &folder_color);
        else    //file
            setattr(GUIENT_DIV, filenodes.nodes[i], GUIATTR_DIV_BGCOLOR, &file_color);

    }
}

int FindRightClickOnFile(uint domid, int num)
{
    int i;
    for (i = 0; i < num; i++)
    {
        if (filenodes.nodes[i] == domid)
            return i;
    }
    return -1;
}

void deletebutton_onclick()
{
    //we can add something like a dialog to remind user that their operation did not succeed later
    //printf(1, "%d\r\n", last_right_clicked_fileno);
    removefolderorfile(listresult.parseresult[last_right_clicked_fileno]);
    last_right_clicked_fileno = -1;
}

void copybutton_onclick()
{
    memset(last_copied_path, '\0', MAX_DIRECTORY_LEN);
    memset(last_moved_path, '\0', MAX_DIRECTORY_LEN);
    strcpy(last_copied_path, current_path);
    strcpy(last_copied_path + strlen(last_copied_path), listresult.parseresult[last_right_clicked_fileno]);
    last_right_clicked_fileno = -1;
}

void movebutton_onclick()
{
    memset(last_copied_path, '\0', MAX_DIRECTORY_LEN);
    memset(last_moved_path, '\0', MAX_DIRECTORY_LEN);
    strcpy(last_moved_path, current_path);
    strcpy(last_moved_path + strlen(last_moved_path), listresult.parseresult[last_right_clicked_fileno]);
    last_right_clicked_fileno = -1;
}

int handlepaste()
{
    int lcp = strlen(last_copied_path);
    int lmp = strlen(last_moved_path);
    if (lcp == 0 && lmp == 0)
    {
        return -1;
    }
    else if (lcp != 0)
    {
        char* tmp = (char*)malloc(MAX_DIRECTORY_LEN);
        memset(tmp, '\0', MAX_DIRECTORY_LEN);
        int i, k, ii;
        for (i = lcp - 1, k = 0; i > 0; i--)
        {
            if (last_copied_path[i] != '/')
                tmp[k++] = last_copied_path[i];
            else
                break;
        }
        char* tmp2 = (char*)malloc(k+1);
        memset(tmp2, '\0', k+1);
        for (i = 0, ii = k - 1; i < k; i++, ii--)
        {
            tmp2[i] = tmp[ii];
        }
        memset(tmp, '\0', MAX_DIRECTORY_LEN);
        strcpy(tmp, current_path);
        strcpy(tmp + strlen(tmp), tmp2);
        return copyfile(last_copied_path, tmp);
    }
    else if (lmp != 0)
    {
        char* tmp = (char*)malloc(MAX_DIRECTORY_LEN);
        memset(tmp, '\0', MAX_DIRECTORY_LEN);
        int i, k, ii;
        for (i = lmp - 1, k = 0; i > 0; i--)
        {
            if (last_moved_path[i] != '/')
                tmp[k++] = last_moved_path[i];
            else
                break;
        }
        char* tmp2 = (char*)malloc(k+1);
        memset(tmp2, '\0', k+1);
        for (i = 0, ii = k - 1; i < k; i++, ii--)
        {
            tmp2[i] = tmp[ii];
        }
        memset(tmp, '\0', MAX_DIRECTORY_LEN);
        strcpy(tmp, current_path);
        strcpy(tmp + strlen(tmp), tmp2);
        printf(1, "%s\r\n", tmp);
        return movefile(last_moved_path, tmp);
    }
    return -1;
}

void pastebutton_onclick()
{
    if (handlepaste() >= 0)
    {
        memset(last_moved_path, '\0', MAX_DIRECTORY_LEN);
        memset(last_copied_path, '\0', MAX_DIRECTORY_LEN);
    }
    else
    {
        //come up with a dialog reminding the failure
    }
}

void invalidate(uint parent)
{
    int i;
    free(listresult.filenames);
    for (i = 0; i < filenodes.num; i++)
        free(listresult.parseresult[i]);
    free(listresult.parseresult);
    free(listresult.fileinfo);

    for (i = 0; i < filenodes.num; i++)
    {
        releasedom(GUIENT_DIV, filenodes.nodes[i]);
    }
    free(filenodes.nodes);
    filenodes.num = 0;

    listresult = ls(".");
    int n = strlen(listresult.filenames) / 14 - 2;
    render(listresult.parseresult, n, parent, listresult.fileinfo + 2); //the first two are . and ..
}

int main(int argc, char *argv[])
{
    initprocessqueue();
    
    //======init path variable
    current_path = (char*)malloc(MAX_DIRECTORY_LEN * sizeof(char));
    memset(current_path, '\0', MAX_DIRECTORY_LEN);
    last_copied_path = (char*)malloc(MAX_DIRECTORY_LEN * sizeof(char));
    memset(last_copied_path, '\0', MAX_DIRECTORY_LEN);
    last_moved_path = (char*)malloc(MAX_DIRECTORY_LEN * sizeof(char));
    memset(last_moved_path, '\0', MAX_DIRECTORY_LEN);
    current_path[0] = '/';

    //===========create window
    uint window;
        uint titlebar;
            uint closebutton;
        uint toolbar;
            uint gobackbutton;
            uint upbutton;
            uint downbutton;
        uint scrollview;
        uint right_click_onfile;
            uint delete_button;
            uint copy_button;
            uint move_button;
        uint right_click_onwindow;
            uint new_folder_button;
            uint new_file_button;
            uint paste_button;
    color32 window_background = rgba(255, 255, 255, 0);
    color32 titlebar_color = rgba(0, 0, 255, 100);
    color32 closebutton_color = rgba(255, 0, 0, 0);
    color32 right_click_onfile_color = rgba(170, 170, 170, 50);
    color32 right_click_onwindow_color = rgba(250, 250, 0, 50);
    color32 button_color = rgba(0, 0, 0, 0);
    color32 toolbar_color = rgba(32, 125, 234, 50);
    

    createdom(GUIENT_DIV, windowparent, &window);
    setattr(GUIENT_DIV, window, GUIATTR_DIV_X, parh(0));
    setattr(GUIENT_DIV, window, GUIATTR_DIV_Y, parh(0));
    setattr(GUIENT_DIV, window, GUIATTR_DIV_WIDTH, parh(1024));
    setattr(GUIENT_DIV, window, GUIATTR_DIV_HEIGHT, parh(768));
    setattr(GUIENT_DIV, window, GUIATTR_DIV_BGCOLOR, &window_background);

        createdom(GUIENT_DIV, window, &titlebar);
        setattr(GUIENT_DIV, titlebar, GUIATTR_DIV_X, parh(0));
        setattr(GUIENT_DIV, titlebar, GUIATTR_DIV_Y, parh(0));
        setattr(GUIENT_DIV, titlebar, GUIATTR_DIV_WIDTH, parh(1024));
        setattr(GUIENT_DIV, titlebar, GUIATTR_DIV_HEIGHT, parh(20));
        setattr(GUIENT_DIV, titlebar, GUIATTR_DIV_BGCOLOR, &titlebar_color);

            createdom(GUIENT_DIV, titlebar, &closebutton);
            setattr(GUIENT_DIV, closebutton, GUIATTR_DIV_X, parh(1004));
            setattr(GUIENT_DIV, closebutton, GUIATTR_DIV_Y, parh(0));
            setattr(GUIENT_DIV, closebutton, GUIATTR_DIV_WIDTH, parh(20));
            setattr(GUIENT_DIV, closebutton, GUIATTR_DIV_HEIGHT, parh(20));
            setattr(GUIENT_DIV, closebutton, GUIATTR_DIV_BGCOLOR, &closebutton_color);

        createdom(GUIENT_DIV, window, &toolbar);
        setattr(GUIENT_DIV, toolbar, GUIATTR_DIV_X, parh(0));
        setattr(GUIENT_DIV, toolbar, GUIATTR_DIV_Y, parh(20));
        setattr(GUIENT_DIV, toolbar, GUIATTR_DIV_WIDTH, parh(1024));
        setattr(GUIENT_DIV, toolbar, GUIATTR_DIV_HEIGHT, parh(40));
        setattr(GUIENT_DIV, toolbar, GUIATTR_DIV_BGCOLOR, &toolbar_color);

            createdom(GUIENT_DIV, toolbar, &gobackbutton);
            setattr(GUIENT_DIV, gobackbutton, GUIATTR_DIV_X, parh(5));
            setattr(GUIENT_DIV, gobackbutton, GUIATTR_DIV_Y, parh(5));
            setattr(GUIENT_DIV, gobackbutton, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, gobackbutton, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, gobackbutton, GUIATTR_DIV_BGCOLOR, &button_color);

            createdom(GUIENT_DIV, toolbar, &upbutton);
            setattr(GUIENT_DIV, upbutton, GUIATTR_DIV_X, parh(75));
            setattr(GUIENT_DIV, upbutton, GUIATTR_DIV_Y, parh(5));
            setattr(GUIENT_DIV, upbutton, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, upbutton, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, upbutton, GUIATTR_DIV_BGCOLOR, &button_color);

            createdom(GUIENT_DIV, toolbar, &downbutton);
            setattr(GUIENT_DIV, downbutton, GUIATTR_DIV_X, parh(145));
            setattr(GUIENT_DIV, downbutton, GUIATTR_DIV_Y, parh(5));
            setattr(GUIENT_DIV, downbutton, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, downbutton, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, downbutton, GUIATTR_DIV_BGCOLOR, &button_color);

        createdom(GUIENT_DIV, window, &right_click_onfile);
        setattr(GUIENT_DIV, right_click_onfile, GUIATTR_DIV_X, parh(0));
        setattr(GUIENT_DIV, right_click_onfile, GUIATTR_DIV_Y, parh(768));  //change between 668 and 768
        setattr(GUIENT_DIV, right_click_onfile, GUIATTR_DIV_WIDTH, parh(1024));
        setattr(GUIENT_DIV, right_click_onfile, GUIATTR_DIV_HEIGHT, parh(100));
        setattr(GUIENT_DIV, right_click_onfile, GUIATTR_DIV_BGCOLOR, &right_click_onfile_color);

            createdom(GUIENT_DIV, right_click_onfile, &delete_button);
            setattr(GUIENT_DIV, delete_button, GUIATTR_DIV_X, parh(10));
            setattr(GUIENT_DIV, delete_button, GUIATTR_DIV_Y, parh(10));
            setattr(GUIENT_DIV, delete_button, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, delete_button, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, delete_button, GUIATTR_DIV_BGCOLOR, &button_color);

            createdom(GUIENT_DIV, right_click_onfile, &copy_button);
            setattr(GUIENT_DIV, copy_button, GUIATTR_DIV_X, parh(80));
            setattr(GUIENT_DIV, copy_button, GUIATTR_DIV_Y, parh(10));
            setattr(GUIENT_DIV, copy_button, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, copy_button, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, copy_button, GUIATTR_DIV_BGCOLOR, &button_color);

            createdom(GUIENT_DIV, right_click_onfile, &move_button);
            setattr(GUIENT_DIV, move_button, GUIATTR_DIV_X, parh(150));
            setattr(GUIENT_DIV, move_button, GUIATTR_DIV_Y, parh(10));
            setattr(GUIENT_DIV, move_button, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, move_button, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, move_button, GUIATTR_DIV_BGCOLOR, &button_color);

        createdom(GUIENT_DIV, window, &right_click_onwindow);
        setattr(GUIENT_DIV, right_click_onwindow, GUIATTR_DIV_X, parh(0));
        setattr(GUIENT_DIV, right_click_onwindow, GUIATTR_DIV_Y, parh(768));  //change between 668 and 768
        setattr(GUIENT_DIV, right_click_onwindow, GUIATTR_DIV_WIDTH, parh(1024));
        setattr(GUIENT_DIV, right_click_onwindow, GUIATTR_DIV_HEIGHT, parh(100));
        setattr(GUIENT_DIV, right_click_onwindow, GUIATTR_DIV_BGCOLOR, &right_click_onwindow_color);

            createdom(GUIENT_DIV, right_click_onwindow, &new_folder_button);
            setattr(GUIENT_DIV, new_folder_button, GUIATTR_DIV_X, parh(10));
            setattr(GUIENT_DIV, new_folder_button, GUIATTR_DIV_Y, parh(10));
            setattr(GUIENT_DIV, new_folder_button, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, new_folder_button, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, new_folder_button, GUIATTR_DIV_BGCOLOR, &button_color);

            createdom(GUIENT_DIV, right_click_onwindow, &new_file_button);
            setattr(GUIENT_DIV, new_file_button, GUIATTR_DIV_X, parh(80));
            setattr(GUIENT_DIV, new_file_button, GUIATTR_DIV_Y, parh(10));
            setattr(GUIENT_DIV, new_file_button, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, new_file_button, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, new_file_button, GUIATTR_DIV_BGCOLOR, &button_color);

            createdom(GUIENT_DIV, right_click_onwindow, &paste_button);
            setattr(GUIENT_DIV, paste_button, GUIATTR_DIV_X, parh(150));
            setattr(GUIENT_DIV, paste_button, GUIATTR_DIV_Y, parh(10));
            setattr(GUIENT_DIV, paste_button, GUIATTR_DIV_WIDTH, parh(50));
            setattr(GUIENT_DIV, paste_button, GUIATTR_DIV_HEIGHT, parh(30));
            setattr(GUIENT_DIV, paste_button, GUIATTR_DIV_BGCOLOR, &button_color);

        //=====ls and render the whole directory
        listresult = ls(".");
        int n = strlen(listresult.filenames) / 14 - 2;
        int rowno = n / 10 + 1;
        createdom(GUIENT_DIV, window, &scrollview);
        setattr(GUIENT_DIV, scrollview, GUIATTR_DIV_X, parh(0));
        setattr(GUIENT_DIV, scrollview, GUIATTR_DIV_Y, parh(60));
        setattr(GUIENT_DIV, scrollview, GUIATTR_DIV_WIDTH, parh(1024));
        setattr(GUIENT_DIV, scrollview, GUIATTR_DIV_HEIGHT, parh(125 * rowno + 10));
        setattr(GUIENT_DIV, scrollview, GUIATTR_DIV_BGCOLOR, &window_background);
        render(listresult.parseresult, n, scrollview, listresult.fileinfo + 2); //the first two are . and ..

    //==========receive events
    int *msg = (int*)malloc(100);
    MouseMsg* mm;
    while(1)
    {
        getmsgfromqueue(msg);
        if (*msg == MOUSE_MESSAGE)
        {
            mm = (MouseMsg*)msg;
            if ((mm->mouse_event_type & RIGHT_BTN_UP) != 0) //right button up
            {
                if (mm->dom_id == window || mm->dom_id == scrollview)   //right button up on window
                {
                    setattr(GUIENT_DIV, right_click_onwindow, GUIATTR_DIV_Y, parh(668));
                    //printf(1, "I'm in!!!!!!!!\r\n");
                }
                else
                {
                    last_right_clicked_fileno = FindRightClickOnFile(mm->dom_id, filenodes.num);
                    if (last_right_clicked_fileno != -1)
                        setattr(GUIENT_DIV, right_click_onfile, GUIATTR_DIV_Y, parh(668));
                }
            }
            else if (mm->mouse_event_type != 0)
            {
                if ((mm->mouse_event_type & LEFT_BTN_UP) != 0)
                {
                    if (mm->dom_id == closebutton)  //close the application
                    {
                        break;
                    }
                    else if (mm->dom_id == delete_button) //here I used switch but failed
                    {
                        deletebutton_onclick();
                        invalidate(scrollview);
                    }
                    else if (mm->dom_id == copy_button)
                    {
                        copybutton_onclick();
                    }
                    else if (mm->dom_id == move_button)
                    {
                        movebutton_onclick();
                    }
                    else if (mm->dom_id == paste_button)
                    {
                        pastebutton_onclick();
                        invalidate(scrollview);
                    }
                    else if (mm->dom_id == gobackbutton)
                    {
                        changedirectory("..");
                        invalidate(scrollview);
                    }
                    else if (mm->dom_id == upbutton)
                    {
                        int y;
                        getattr(GUIENT_DIV, scrollview, GUIATTR_DIV_Y, &y);
                        y = y - 20;
                        setattr(GUIENT_DIV, scrollview, GUIATTR_DIV_Y, &y);
                    }
                    else if (mm->dom_id == downbutton)
                    {
                        int y;
                        getattr(GUIENT_DIV, scrollview, GUIATTR_DIV_Y, &y);
                        y = y + 20;
                        setattr(GUIENT_DIV, scrollview, GUIATTR_DIV_Y, &y);
                    }
                    else    //open folder or file
                    {
                        int nodeno = FindRightClickOnFile(mm->dom_id, filenodes.num);
                        if (nodeno != -1 && listresult.fileinfo[nodeno + 2] == FILETYPE)
                        {
                            changedirectory(listresult.parseresult[nodeno]);
                            printf(1, "refresh!!!\r\n");
                            invalidate(scrollview);
                        }
                    }
                }
                setattr(GUIENT_DIV, right_click_onwindow, GUIATTR_DIV_Y, parh(768));
                setattr(GUIENT_DIV, right_click_onfile, GUIATTR_DIV_Y, parh(768));
            }
        }
    }


    //=======release window
    releasedom(GUIENT_DIV, window);

    releaseprocessqueue();
    exit();
}