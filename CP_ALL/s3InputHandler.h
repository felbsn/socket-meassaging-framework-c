#pragma once
#include "s3DataTypes.h"
#include<winsock2.h>



enum s3KeyFlag_e
{
	s3Key_NotPressed,
	s3Key_Pressed_wControl,
	s3Key_Pressed_wAlt,
	s3Key_Pressed,
};
typedef enum s3KeyFlag_e s3KeyFlag;


s3Flag s3SeekKeyDown();
void  s3KeyGetAltToggleReset();
s3Flag s3KeyGetAltStatus();
s3Flag s3KeyGetControlToggle();

s3Flag s3SeekKeyStatus();

s3Flag s3GetKeyState(unsigned char key, s3KeyFlag flag);


int s3InitRxBuffer();

int s3SendBufferTx(SOCKET s);

int s3RecvBuffer(SOCKET s);

int s3SendBuffer(SOCKET s, char* buffer, int length);

int s3SendBufferChannel(SOCKET s, int channel);

int s3InitTxBuffers();

void s3DestroyBuffers();

char* s3GetBufferChannel(int channel);
int	  s3GetBufferChannelLen(int channel);

void s3ResetBufferChannel(int channel);

int s3HandleText(int channel);

int s3HandleTextInstant(int channel);


int s3PrintBuffer(int color, int channel);

int s3PrintRxBuffer();

char* s3GetRxBuffer();

int	s3GetRxBufferLen();
