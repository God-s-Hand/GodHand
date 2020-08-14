#ifndef __COMMON_HEADER__
#define __COMMON_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>  // for ioctl
#include <sys/mman.h>
#include <linux/input.h>
#include <linux/fb.h>   // for fb_var_screeninfo, FBIOGET_VSCREENINFO
#include <errno.h>
#include <sys/poll.h>
#include <linux/videodev2.h>
#include "bitmap.h"

void* SerialData();
void* show_mled();
int Busled();

int IsBusy(void);
int writeCmd(unsigned short cmd);
int setDDRAMAddr(int x , int y);
int displayMode(int bCursor, int bCursorblink, int blcd);
int writeCh(unsigned short ch);
int setCursorMode(int bMove , int bRightDir);
int functionSet(void);
int writeStr(char* str);
int clearScreen(int nline);
int kbhit(void);
int fndDisp(int driverfile, int num , int dotflag,int durationSec);

int tlcd(char mode,int line,char* message);
void show_currenttime(void);
int count_dot_matrix(int co);
int buzzer(int sound);
void keypress();
void dipswitch();

unsigned long simple_strtoul(char *cp, char **endp,unsigned int base);
unsigned long read_hex(const char* str);
int owriteCmd(int size , unsigned short* cmdArr);
int reset(void);
int writeData(int size , unsigned char* dataArr);
int readData(int size , unsigned short* dataArr);
int setAddressDefalut(void);
int setCmdLock(int bLock);
int imageLoading(char* fileName);
int oled(char* image_file_name);
int Init();
void display(char* filename);
void touch();
void drawRect(unsigned char *fb_mem , int sx , int sy, int ex, int ey,  unsigned long color);
void drawCoordinate(unsigned char *fb_mem , int cx , int cy, int prex , int prey);

void readFirstCoordinate(int fd, int* cx , int* cy);
int capture_img();


// global variable
int Mode;
int oledfd;
int tlcdfd;
float voltage;
int STOP; 
int num;
int flag;
int other_device_power[16];
int timebuffer;
char* ipbuf;
int capturetime;
int captureflag;
int powerup;

int		screen_width;
int		screen_height;
int		bits_per_pixel;
int		line_length;
int clnt_socket;//this is added
int clnt_img_socket;
#endif
