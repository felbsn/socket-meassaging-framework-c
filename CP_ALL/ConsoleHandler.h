
/*
#		Various functions and macros for screen manipulation
#
*/


#pragma once

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

#pragma warning(disable:4996)



struct Message_s
{
	int Length;
	char* str;
	s3Flag Prop;
	time_t tm;
};
typedef struct Message_s  Message;

struct MessageBuffer_s
{
	Message *data;
	int index;
	int Size;
};
typedef struct MessageBuffer_s  MessageBuffer;

MessageBuffer NewMessageBuffer()
{
	MessageBuffer lb;
	lb.index = 0;
	lb.Size = 2;
	lb.data = (Message*)malloc(sizeof(Message)*lb.Size);
	return lb;
}





const char ColorsBG[8][6] = {
	CB_BLACK,
	CB_RED,
	CB_GREEN,
	CB_YELLOW,
	CB_BLUE,
	CB_MAGENTA,
	CB_CYAN,
	CB_WHITE
};

const char Colors[8][6] = {
	C_GRAY	  ,
	C_RED	  ,
	C_GREEN	  ,
	C_YELLOW  ,
	C_BLUE	  ,
	C_MAGENTA ,
	C_CYAN	  ,
	C_WHITE
};

static  char PaddingBuffer[2048] = { 32 };
static  char ShadowBuffer[2048] = { 223 }; //223  //219
static  char ShadeBuffer[2048] = { 177 };
static  int ScreenHeight = 1;
static  int ScreenWidth = 1;

// one message buffer needs to be reserved for output console

static MessageBuffer consoleBuffer = { 0 };


void InitMessageBuffers(MessageBuffer* msgBuffers, int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		msgBuffers[i].Size = 2;
		msgBuffers[i].data = (Message*)malloc(msgBuffers[i].Size * sizeof(Message));
		msgBuffers[i].index = 0;
	}
}

void SetupConsolehandler(int width, int height)
{
	ScreenHeight = height;
	ScreenWidth = width;

	int i;


	consoleBuffer.Size = 2;
	consoleBuffer.data = (Message*)malloc(consoleBuffer.Size* sizeof(Message));
	consoleBuffer.index = 0;

	for (i = 0; i < sizeof(PaddingBuffer); i++)
	{
		PaddingBuffer[i] = 32;
		ShadowBuffer[i] = 223;
		ShadeBuffer[i] = 176;
	}
}

void AddMessageByLine(MessageBuffer* MsgBuffer, Message line)
{
	if (MsgBuffer->index >= MsgBuffer->Size)
	{
		MsgBuffer->Size += 10;
		MsgBuffer = (MessageBuffer*)realloc(MsgBuffer, sizeof(MessageBuffer)*MsgBuffer->Size);
		if (!MsgBuffer) { puts("memory allocation error!"); system("pause");  exit(1); }
	}

	MsgBuffer->data[MsgBuffer->index] = line;
	MsgBuffer->index++;

}

char** LineParser(char* Line, int* OutputLineCount, int boxLengt)
{
	char * pch;

	char ** liner;

	int i;
	int maxLines = 20;
	liner = (char**)malloc(maxLines * sizeof(char*));
	for (i = 0; i < 20; i++)
	{
		liner[i] = (char*)calloc((boxLengt+1), sizeof(char));
	}


	int lineCount = 0;
	int lineLength = 1;


	pch = strtok(Line, " ");
	while (pch != NULL)
	{
		int wordlen = strlen(pch);


		if (lineLength + wordlen + 1 >= boxLengt)
		{
			if (wordlen <= boxLengt)
			{
				liner[lineCount][boxLengt] = '\0';
				lineCount++;
				strncpy(liner[lineCount], pch, wordlen);
				strncat(liner[lineCount], " ", 2);
				lineLength = wordlen +1;
			}
			else
			{
				liner[lineCount][lineLength] = '\0';

				lineLength = 0;

				int rep = 0;
				while (wordlen >= boxLengt)
				{
					lineCount++;
					strncpy(liner[lineCount], pch + rep * boxLengt, boxLengt);
					liner[lineCount][boxLengt] = '\0';
					rep++;
					wordlen -= boxLengt;
				}
				if (wordlen > 0)
				{
					lineCount++;
					strncpy(liner[lineCount], pch + rep * boxLengt, wordlen);
					lineLength += wordlen;
				}


			}
		}
		else
		{
			
			strncat(liner[lineCount], pch, wordlen);
			strncat(liner[lineCount], " ", 2);
			lineLength += wordlen;
			lineLength++;
		}



		pch = strtok(NULL, " ");
	}


	for ( i = lineCount + 1; i < maxLines; i++)
	{
		// freeing unused lines
		free(liner[i]);
	}


	*OutputLineCount = (lineCount + 1);
	return liner;
}

// extend char** printer
int printParsedEx(char** Line,int x ,int y , int rows , int limit , int isUpper)
{

	int i;
	int printedlines = 0;
	int linesDraw = min(limit, rows);
	if (isUpper)
	{
		x += linesDraw -1;
		for (i = linesDraw-1; i >= 0 ; i--)
		{
			gotoxy(x - i, y);
			printf("%s", Line[i]);
			printedlines++;
		}
	}
	else
	{
		x -= linesDraw;
		int lineX = 0;
		int lineIndex = rows - linesDraw;
		for (i = 0; i < linesDraw; i++)
		{
			gotoxy(x+i, y);
			printf("%s", Line[lineIndex]);
			lineIndex++;
			printedlines++;
		}
	}

	return printedlines;

}

// char* printer
void printParsed(char* buffer, int x, int y, int limitV ,int limitH, int isUpper)
{
	int bufflen = strlen(buffer);
	int lines;
	char* strHolder = (char*)malloc(bufflen * sizeof(char));
	strHolder[bufflen] = 0;
	strcpy(strHolder, buffer);
	char** liner = LineParser(strHolder, &lines, limitH);

	printParsedEx(liner, x, y, lines, limitV, isUpper);


}


void AddMessage(MessageBuffer* MsgBuffer,const char* msgStr , s3Flag Prop)
{

	// check buffer size
	if (MsgBuffer->index >= MsgBuffer->Size)
	{
		MsgBuffer->Size += 10;
		MsgBuffer->data = (Message*)realloc(MsgBuffer->data,sizeof(Message)*MsgBuffer->Size);
		if (!MsgBuffer->data) { puts("memory allocation error!"); system("pause");  exit(1); }
	}


	int msgLen = strlen(msgStr);



	// copy string to buffer's last element
	MsgBuffer->data[MsgBuffer->index].str = (char*)malloc(msgLen * sizeof(char));
	strcpy(MsgBuffer->data[MsgBuffer->index].str, msgStr);


	MsgBuffer->data[MsgBuffer->index].Prop = Prop;
	MsgBuffer->data[MsgBuffer->index].Length = msgLen;
	MsgBuffer->data[MsgBuffer->index].tm = time(NULL);
	MsgBuffer->index++;


}

void consoleLog(const char * lines , s3Flag Prop)
{
	AddMessage(&consoleBuffer, lines, Prop);
}


void DrawBox(const char* color, int posX, int posY, int RowLen, int ColumnLen)
{
	setColor(color);

	setColor(C_GRAY);
	int i = 0;

	gotoxy(posX + i, posY);
	printf("%.*s", ColumnLen, PaddingBuffer);

	for (i = 1; i < RowLen; i++)
	{
		gotoxy(posX + i, posY);
		printf("%.*s", ColumnLen, PaddingBuffer);

		printf("%c", CH_SHADOW_SIDE);

	}

	gotoxy(posX + i, posY);



	setColor(CB_WHITE);
	setColor(C_GRAY);
	printf(" %.*s", ColumnLen, ShadowBuffer);

}


int PrintMessage(int x_end, int y,int messageWide, int BoxWide, int verticalSpace , Message* msg)
{
	int len = msg->Length;
	int lineCount = ceil((float)len / messageWide);
	int userMessageOffset = y + BoxWide - messageWide+1;

	
	int i;
	if (lineCount+2 < verticalSpace)
	{
		i = 0;
		x_end -= lineCount;
	}
	else
	{
		i = verticalSpace;
		x_end -= verticalSpace;
	}

	char isFromUser = msg->Prop == s3_FROM_OWN;
	
	DrawBox(CB_GREEN, x_end , isFromUser ? y + BoxWide - messageWide : y , lineCount+1 , messageWide + 2);
	y++;
	setColor(CB_GREEN);

		for (; i < lineCount; i++)
		{
		
			if (isFromUser)
			{
				gotoxy(x_end, userMessageOffset);
				int lineLen = min(messageWide, len);
				printf("%.*s%.*s", messageWide - lineLen, PaddingBuffer , lineLen, msg->str + messageWide * i);
				len -= messageWide;
				x_end++;
				verticalSpace--;
			}
			else
			{
				gotoxy(x_end, y);
				printf("%.*s", min(messageWide, len), msg->str + messageWide * i);
				len -= messageWide;
				x_end++;
				verticalSpace--;
			}
		}
		if (isFromUser)
		{
			gotoxy(x_end, userMessageOffset);
			printf("%.*s~%.*s", messageWide - 9 , PaddingBuffer, 8, ctime(&msg->tm) + 11);
		}
		else
		{
			gotoxy(x_end, y);
			printf("~%.*s", 8, ctime(&msg->tm) + 11);
		}


		setColor(CB_WHITE);
		setColor(C_GRAY);

		return verticalSpace-2;
}

int PrintConsoleMessage(int x_end, int y, int messageWide, int BoxWide, int verticalSpace, Message* msg)
{
	int len = msg->Length;
	int lineCount = ceil((float)len / messageWide);
	int userMessageOffset = y + BoxWide - messageWide + 1;


	int i;
	if (lineCount <= verticalSpace)
	{
		i = 0;
		x_end -= lineCount;
	}
	else
	{
		i = verticalSpace;
		x_end -= verticalSpace;
	}


	//DrawBox(CB_GREEN, x_end, msg->isFromUser ? y + BoxWide - messageWide : y, lineCount + 1, messageWide + 2);
	//y++;
	setColor(CB_BLUE);

	switch (msg->Prop)
	{
	case s3_SUCCESS:
	case s3_DONE:
		setColor(C_GREEN);
		break;
	case s3_FAIL:
	case s3_ERROR:
	case s3_OVERRIDE_ERROR:
		setColor(C_RED);
		break;
	default:
		setColor(C_WHITE);
		break;
	}

	

	for (; i < lineCount; i++)
	{

		//if (msg->isFromUser)
		//{
		//	gotoxy(x_end, userMessageOffset);
		//	int lineLen = min(messageWide, len);
		//	printf("%.*s%.*s", messageWide - lineLen, PaddingBuffer, lineLen, msg->str + messageWide * i);
		//	len -= messageWide;
		//	x_end++;
		//	verticalSpace--;
		//}
		//else
		//{
			gotoxy(x_end, y);
			printf("%.*s", min(messageWide, len), msg->str + messageWide * i);
			len -= messageWide;
			x_end++;
			verticalSpace--;
		//}
	}
	//if (msg->isFromUser)
	//{
	//	gotoxy(x_end, userMessageOffset);
	//	printf("%.*s~%.*s", messageWide - 9, PaddingBuffer, 8, ctime(&msg->tm) + 11);
	//}
	//else
	//{
	//	gotoxy(x_end, y);
	//	printf("~%.*s", 8, ctime(&msg->tm) + 11);
	//}


	setColor(CB_WHITE);
	setColor(C_GRAY);

	return verticalSpace;
}


void DrawMessageBox(int x, int y, int rowLen, int MessageWide, int BoxWide, MessageBuffer msgBuffer)
{
	int screenSpace = rowLen;
	int i = msgBuffer.index-1;
	if (i < 0) return;


	while ((i >= 0)  && (screenSpace > 0))
	{
		screenSpace = PrintMessage(x+screenSpace, y, MessageWide, BoxWide, screenSpace, &msgBuffer.data[i]);
		i--;
	}

}


void DrawConsoleBox(int x, int y, int rowLen, int MessageWide, int BoxWide)
{
	int screenSpace = rowLen;
	int i = consoleBuffer.index - 1;
	if (i < 0) return;


	while ((i >= 0) && (screenSpace > 0))
	{
		screenSpace = PrintConsoleMessage(x + screenSpace, y, MessageWide, BoxWide, screenSpace, &consoleBuffer.data[i]);
		i--;
	}
}



void DrawBoxShaded(int color, int posX, int posY, int RowLen, int ColumnLen)
{
	setColor(ColorsBG[color]);

	setColor(C_GRAY);
	int i = 0;

	gotoxy(posX + i, posY);
	printf("%.*s", ColumnLen, PaddingBuffer);

	for (i = 1; i < RowLen; i++)
	{
		setColor(ColorsBG[color]);
		gotoxy(posX + i, posY);
		printf("%.*s", ColumnLen, PaddingBuffer);
		setColor(CB_WHITE);
		printf("%c", ShadeBuffer[0]);

	}

	gotoxy(posX + i, posY);



	setColor(CB_WHITE);
	setColor(C_GRAY);
	printf(" %.*s", ColumnLen, ShadeBuffer);

}

void DrawBoxShadeless(int color, int posX, int posY, int RowLen, int ColumnLen)
{
	setColor(ColorsBG[color]);

	setColor(C_GRAY);
	int i = 0;

	gotoxy(posX + i, posY);
	printf("%.*s", ColumnLen, PaddingBuffer);

	for (i = 1; i < RowLen; i++)
	{
		gotoxy(posX + i, posY);
		printf("%.*s", ColumnLen, PaddingBuffer);
	}

	setColor(CB_WHITE);
	setColor(C_GRAY);

}

void DrawBoxShadowless(int color, int posX, int posY, int RowLen, int ColumnLen)
{
	setColor(ColorsBG[color]);

	setColor(C_GRAY);
	int i = 0;



	for (i = 0; i < RowLen; i++)
	{
		gotoxy(posX + i, posY+1);
		printf("%.*s", ColumnLen, PaddingBuffer);

	}
	setColor(CB_WHITE);
	setColor(C_GRAY);
}

void DrawBoxedStr(int color , char* str, Token phoneNo, int num, int x , int y)
{
	DrawBox(color, x, y, 3, 20);

	setColor(ColorsBG[color]);
	gotoxy(x +1, y+1);
	char numBuf[4];
	if (num != 0)
	{
		if (num < 100)
			sprintf(numBuf, "%d", num);
		else
			strcpy(numBuf, "99+");

		printf("(%s) %.*s%s", numBuf, 17, str , strlen(str) >16 ? "..." :" ");

	}
	else
	{
		printf("%.*s%s", 19, str, strlen(str) >16 ? "..." : " ");
	}
	
	gotoxy(x + 2, y);
	printf(" Phone:%llu", phoneNo);

	setColor(C_GRAY);
	setColor(CB_WHITE);

}

void DrawBoxedStrSel(int color, int color2, char* str, Token phoneNo, int num, int x, int y)
{



	y++;

	DrawBox(color, x, y, 3, 20);

	setColor(ColorsBG[color]);
	gotoxy(x + 1, y+1);
	char numBuf[4];
	if (num != 0)
	{
		if (num < 100)
			sprintf(numBuf, "%d", num);
		else
			strcpy(numBuf, "99+");

		printf("(%s) %.*s%s", numBuf, 17, str, strlen(str) >16 ? "..." : " ");

	}
	else
	{
		printf("%.*s%s", 19, str, strlen(str) >16 ? "..." : " ");
	}
	gotoxy(x + 2, y);
	printf(" Phone:%llu", phoneNo);

	setColor(CB_WHITE);
	setColor(Colors[color2]);

	gotoxy(x, y - 1);
	putchar(CH_SHADOW_SIDE);
	gotoxy(x + 1, y - 1);
	putchar(CH_SHADOW_SIDE);
	gotoxy(x + 2, y - 1);
	putchar(CH_SHADOW_SIDE);
	gotoxy(x + 3, y);

	setColor(C_GRAY);
	putchar(ShadowBuffer[0]);


}

void DrawInfoBar(int isAltPressed)
{
	// infor bar definintion
	static const char passiveStr[] = "  %sA%sdd Contact , %sE%sdit Contact ,%sD%selete Contact , %s'+'/'-'%s Change Channel ,%sR%seconnect, %sQ%suit %.*s";
	setColor(CB_BLUE);
	setColor(C_WHITE);
	if (isAltPressed)
	{
		printf(passiveStr, CB_RED, CB_BLUE, CB_RED, CB_BLUE, CB_RED, CB_BLUE, CB_RED, CB_BLUE, CB_RED, CB_BLUE, CB_RED, CB_BLUE, ScreenWidth - sizeof(passiveStr) + 29, PaddingBuffer);
	}
	else
	{

		printf(passiveStr, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, ScreenWidth - sizeof(passiveStr) +29, PaddingBuffer);
	}

	setColor(C_GRAY);
	setColor(CB_WHITE);
}

