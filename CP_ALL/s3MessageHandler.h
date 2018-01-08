
/*
#		Various functions and macros for screen manipulation
#
*/


#pragma once

#include "s3DataTypes.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>



#define C_GRAY     "\x1b[30m"
#define C_RED     "\x1b[31m"
#define C_GREEN   "\x1b[32m"
#define C_YELLOW  "\x1b[33m"
#define C_BLUE    "\x1b[34m"
#define C_MAGENTA "\x1b[35m"
#define C_CYAN    "\x1b[36m"
#define C_WHITE    "\x1b[37m"
#define C_RESET   "\x1b[00m"

#define CB_BLACK    "\x1b[40m"
#define CB_RED     "\x1b[41m"
#define CB_GREEN   "\x1b[42m"
#define CB_YELLOW  "\x1b[43m"
#define CB_BLUE    "\x1b[44m"
#define CB_MAGENTA "\x1b[45m"
#define CB_CYAN    "\x1b[46m"
#define CB_WHITE   "\x1b[47m"

#define Color_end "\33[0m\0\0]"

#define CH_SHADOW_SIDE 219

// color manipulation macros
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))
#define setColor(x) printf(x)


struct s3Message_s
{
	int Length;
	char* str;
	s3Flag Prop;
	time_t tm;
};
typedef struct s3Message_s  s3Message;

struct s3MessageBuffer_s
{
	s3Message *messages;
	int index;
	int Size;
};
typedef struct s3MessageBuffer_s  s3MessageBuffer;

void s3DeleteBuffer(s3MessageBuffer* msgBuffer);

s3MessageBuffer s3NewMessageBuffer();

void s3DrawFrame(const char* color, int posX, int posY, int width, int height);

void s3InitMessageBuffers(s3MessageBuffer* msgBuffers, int count);

void s3InitMessageHandler(int width, int height);

void s3AddMessage(s3MessageBuffer* MsgBuffer, const char* msgStr, time_t t, s3Flag Prop);

void s3ConsoleLog(const char * lines, s3Flag Prop);

void s3DrawBox(const char* color, int posX, int posY, int width, int height);

int s3PrintMessage(int x, int y, int messageWidth, int horizontalSpace, int *verticalSpace, int *offsetSpace, s3Message* msg);

int s3PrintConsoleMessage(int x, int y, int messageWidth, int horizontalSpace, int verticalSpace, s3Message* msg);

void s3DrawMessageBox(int x, int y, int width, int height, int messageWidth, int messageOffset, s3MessageBuffer msgBuffer);

void s3DrawConsoleBox(int x, int y, int width, int height, int messageWidth);

void s3DrawInfoBar(int isAltPressed);

