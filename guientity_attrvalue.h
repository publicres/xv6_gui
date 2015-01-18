#define GUIENT_DIV          0
#define GUIENT_IMG          1

#define GUIATTR_DIV_X       0
#define GUIATTR_DIV_Y       1
#define GUIATTR_DIV_WIDTH   2
#define GUIATTR_DIV_HEIGHT  3
#define GUIATTR_DIV_BGCOLOR 4
#define GUIATTR_DIV_REFRESH 5

#define GUIATTR_IMG_X       0
#define GUIATTR_IMG_Y       1
#define GUIATTR_IMG_WIDTH   2
#define GUIATTR_IMG_HEIGHT  3
#define GUIATTR_IMG_CONTENT 4
#define GUIATTR_IMG_REFRESH 5

#define GUIATTR_CHA_X       0
#define GUIATTR_CHA_Y       1
#define GUIATTR_CHA_WIDTH   2
#define GUIATTR_CHA_HEIGHT  3
#define GUIATTR_CHA_CH      4
#define GUIATTR_CHA_COLOR   5
#define GUIATTR_CHA_REFRESH 6
typedef struct ori_contentStruct
{
    void* pics;
    uchar isRepeat;
} contentStruct;
