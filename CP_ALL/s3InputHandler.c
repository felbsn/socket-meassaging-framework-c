
#include "s3InputHandler.h"
#include <winsock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>


// keyboard states
static s3KeyFlag s3KeyboardState[255];

// maximum instances per program
const  int _s3_MAX_TXBUFFER_INSTANCE = 255;

// buffer for general characters
static unsigned char s3KeyBuffer;

// buffer for state condition characters
static unsigned char s3KeyControlBuffer;

static _s3_RXBUFFER _s3_rxBuffer;

static _s3_TXBUFFER * _s3_txBuffers;


static s3Flag _s3_keyToggle_alt = s3_FALSE;
static s3Flag _s3_keyToggle_control = s3_FALSE;

void  s3KeyGetAltToggleReset()
{
	_s3_keyToggle_alt = s3_FALSE;
}
s3Flag s3KeyGetAltStatus()
{
	if (GetAsyncKeyState(VK_MENU))
		return s3_TRUE;
	else
		return s3_FALSE;

}
s3Flag s3KeyGetControlToggle()
{
	return _s3_keyToggle_control;
}


s3Flag s3SeekKeyDown()
{
	s3KeyboardState[s3KeyBuffer] = s3Key_NotPressed;
	s3KeyBuffer = 0;

	if (_kbhit())
	{
		s3KeyBuffer = _getch();
		s3KeyboardState[s3KeyBuffer] = s3Key_Pressed;


		if (s3KeyBuffer == 8 || s3KeyBuffer == 13)
			return s3_TRUE;
		else
			if ((s3KeyBuffer > 31) && (s3KeyBuffer < 127))

				return s3_TRUE;
			else
				return s3_FALSE;
	}
	else
	{
		return s3_FALSE;
	}


}

s3Flag s3SeekKeyStatus()
{

	s3KeyboardState[s3KeyControlBuffer] = s3Key_NotPressed;
	s3Flag isEvent = s3_FALSE;

	if (GetAsyncKeyState(VK_MENU))
	{
		do
		{
			if (_kbhit())
			{

				s3KeyControlBuffer = _getch();

				s3KeyboardState[s3KeyControlBuffer] = s3Key_Pressed_wAlt;
				isEvent = s3_TRUE;
			}

		} while (GetAsyncKeyState(VK_MENU));
		if (_s3_keyToggle_alt == s3_FALSE)
		{
			_s3_keyToggle_alt = s3_TRUE;
		}
		else
		{
			_s3_keyToggle_alt = s3_FALSE;
		}

	}
	else
		if (GetAsyncKeyState(VK_CONTROL))
		{
			do
			{
				if (_kbhit())
				{

					s3KeyControlBuffer = _getch();

					s3KeyboardState[s3KeyControlBuffer] = s3Key_Pressed_wAlt;
					isEvent = s3_TRUE;
				}

			} while (GetAsyncKeyState(VK_CONTROL));

			_s3_keyToggle_control ? s3_FALSE : s3_TRUE;

		}


	return isEvent;
}

s3Flag s3GetKeyState(unsigned char key, s3KeyFlag flag)
{
	if (s3KeyboardState[key] == flag)

		return s3_TRUE;
	else
		return s3_FALSE;
}


int s3InitRxBuffer()
{
	_s3_rxBuffer.data = (char*)malloc(sizeof(char) * 2048);
	_s3_rxBuffer.Length = 0;
	_s3_rxBuffer.Capacity = 2047;
	return 1;
}



int s3SendBufferTx(SOCKET s)
{
	return send(s, _s3_rxBuffer.data, _s3_rxBuffer.Length, 0);
}

int s3SendBuffer(SOCKET s, char* buffer, int length)
{
	return send(s, buffer, length, 0);
}

int s3RecvBuffer(SOCKET s)
{
	int rec = recv(s, _s3_rxBuffer.data, _s3_rxBuffer.Capacity, 0);


	if (rec > 0)
	{
		_s3_rxBuffer.Length = rec;
		_s3_rxBuffer.data[rec] = 0;
		return s3_OK;
	}

	else
		return rec;
}




int s3SendBufferChannel(SOCKET s, int channel)
{
	_s3_txBuffers[channel].data[_s3_txBuffers[channel].Cur] = 0;
	return send(s, _s3_txBuffers[channel].data, _s3_txBuffers[channel].Cur, 0);

}

void s3ResetBufferChannel(int channel)
{
	_s3_txBuffers[channel].Cur = 0;
	_s3_txBuffers[channel].data[0] = 0;
}


int s3InitTxBuffers()
{
	// creater tx buffers
	_s3_txBuffers = (_s3_TXBUFFER*)malloc(sizeof(_s3_TXBUFFER)*_s3_MAX_TXBUFFER_INSTANCE);
	int i;
	for (i = 0; i < _s3_MAX_TXBUFFER_INSTANCE; i++)
	{
		_s3_txBuffers[i].data = (char*)calloc(2, sizeof(char));
		_s3_txBuffers[i].Size = 2;
		_s3_txBuffers[i].Cur = 0;
		_s3_txBuffers[i].data[0] = 0;
	}



	return 1;
}

void s3DestroyBuffers()
{

	// destroy tx buffers
	if (_s3_txBuffers)
	{
		int i;
		for (i = 0; i < _s3_MAX_TXBUFFER_INSTANCE; i++)
		{
			if (_s3_txBuffers[i].data)
				free(_s3_txBuffers[i].data);
		}

		free(_s3_txBuffers);
	}
}

int s3HandleText(int channel)
{
	if (s3KeyBuffer == 13)
	{
		return 0;

	}
	else
		if (s3KeyBuffer == 8)
		{
			if (_s3_txBuffers[channel].Cur > 0)
			{
				_s3_txBuffers[channel].Cur--;
				_s3_txBuffers[channel].data[_s3_txBuffers[channel].Cur] = 0;

				return _s3_txBuffers[channel].Cur;
			}
		}
		else
		{
			if (_s3_txBuffers[channel].Cur >= _s3_txBuffers[channel].Size-1) {
				_s3_txBuffers[channel].Size += 10;
				if ((_s3_txBuffers[channel].data = (char*)realloc(_s3_txBuffers[channel].data, _s3_txBuffers[channel].Size * sizeof(char))) == NULL)
				{

					puts("here we got an error");
					system("pause");
					return 0;
				}
			}
			_s3_txBuffers[channel].data[_s3_txBuffers[channel].Cur] = s3KeyBuffer;
			_s3_txBuffers[channel].Cur++;
			return _s3_txBuffers[channel].Cur;
		}


	return 0;
}

int s3HandleTextInstant(int channel)
{
	if (s3KeyBuffer == 13)
	{
		//system("cls");
		int ret = _s3_txBuffers[channel].Cur;
		_s3_txBuffers[channel].Cur = 0;
		return ret;
	}
	else
		if (s3KeyBuffer == 8)
		{
			if (_s3_txBuffers[channel].Cur > 0)
			{
				_putch(s3KeyBuffer);
				_putch(' ');
				_putch(s3KeyBuffer);
				_s3_txBuffers[channel].Cur--;
			}
		}
		else
		{
			if (_s3_txBuffers[channel].Cur >= _s3_txBuffers[channel].Size) {
				_s3_txBuffers[channel].Size *= 2;
				if ((_s3_txBuffers[channel].data = (char*)realloc(_s3_txBuffers[channel].data, _s3_txBuffers[channel].Size * sizeof(char))) == NULL)
				{

					puts("here we got an error");
					system("pause");
					return 0;
				}
			}
			_putch(s3KeyBuffer);
			_s3_txBuffers[channel].data[_s3_txBuffers[channel].Cur] = s3KeyBuffer;
			_s3_txBuffers[channel].Cur++;
		}


	return 0;
}

int s3PrintBuffer(int color, int channel)
{
	_s3_txBuffers[channel].data[_s3_txBuffers[channel].Cur] = 0;
	printf("%s", _s3_txBuffers[channel].data);
	return 1;
}

int s3PrintRxBuffer()
{
	_s3_rxBuffer.data[_s3_rxBuffer.Length] = 0;
	printf("%s\n", _s3_rxBuffer.data);
	return 1;
}

int	  s3GetBufferChannelLen(int channel)
{
	return  _s3_txBuffers[channel].Cur;
}
char* s3GetBufferChannel(int channel)
{
	_s3_txBuffers[channel].data[_s3_txBuffers[channel].Cur] = 0;
	return _s3_txBuffers[channel].data;
}

char* s3GetRxBuffer()
{
	_s3_rxBuffer.data[_s3_rxBuffer.Length] = 0;
	return _s3_rxBuffer.data;
}

int	s3GetRxBufferLen()
{
	return _s3_rxBuffer.Length;
}
