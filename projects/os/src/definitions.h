
// GUI parameters
#define DESK_COLOR CYAN

// App definitions
#define APP_FILES 0
#define APP_IMAGE 1
#define APP_MUSIC 2
#define APP_CHAT  3
#define APP_HOME  4
#define NUM_APPS  4

// Icon definitions
#define ICO_STOP  0
#define ICO_PLAY  1
#define ICO_PREV  2
#define ICO_NEXT  3
#define NUM_ICOS  4

// Media definitions
#if defined(__CBM__) || defined(__LYNX__)
  #define imageExt "img"
  #define musicExt "mus"
  #define textExt  "txt"
#else
  #define imageExt "IMG"
  #define musicExt "MUS"
  #define textExt  "TXT"
#endif

// Version definitions
char version[] = "8BIT-OS 2020/11/15";