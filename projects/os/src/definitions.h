
#include "unity.h"

// GUI parameters
#define DESK_COLOR CYAN

// App definitions
#define APP_FILES  0
#define APP_IMAGE  1
#define APP_MUSIC  2
#define APP_CHAT   3
#define NUM_APPS   4

// Icon definitions
#define ICO_STOP   4
#define ICO_PLAY   5
#define ICO_PREV   6
#define ICO_NEXT   7
#define NUM_ICONS  4

#define NUM_CHUNKS 9

// Media definitions
#if defined(__CBM__) || defined(__LYNX__) || defined(__NES__)
  #define imageExt "img"
  #define musicExt "mus"
  #define textExt  "txt"
#else
  #define imageExt "IMG"
  #define musicExt "MUS"
  #define textExt  "TXT"
#endif

// General Functions
void ClearScreen(void);
void DrawTaskBar(void);
void HomeScreen(void);
void InitMouse(void);
void ProcessMouse(void);

// Chat Functions
void ChatPage(void);
void ChatRefresh(void);
void ChatSend(void);
void ChatLogin(void);
void ChatMessage(unsigned char index, unsigned char* packet);
void ChatScreen(void);
void ChatPacket(unsigned char *packet);
void ChatCallback(callback* call);

// Browser Functions
void GetFileList(void);
void SelectFile(char dir, unsigned char* extension, char* fileSel);
void PreviewText(void);
void FilesScreen(void);
void FileCallback(callback* call);

// Slideshow Functions
void ImageDisplay(void);
void ImageScreen(void);
void ImageCallback(callback* call);

// Music Functions
void PlayTrack(char *fname);
void PauseTrack(void);
void UnpauseTrack(void);
void StopTrack(void);
void MusicTitle(void);
void MusicScreen(void);
void MusicCallback(callback* call);

// Network Functions
void ServerConnect(void);
