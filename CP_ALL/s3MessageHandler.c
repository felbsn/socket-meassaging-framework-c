



/*
#		Various functions and macros for screen manipulation
#
*/

#pragma warning(disable:4996)

#include "s3MessageHandler.h"



// pre allocated screen buffers
static  char PaddingBuffer[256] = { 32 };
static  char ShadowBuffer[256] = { 223 }; //223  //219
static  char ShadeBuffer[256] = { 177 };
static  int ScreenHeight = 1;
static  int ScreenWidth = 1;

static char* _s3_USER_MSG_COLOR = CB_GREEN;
static char* _s3_CONTACT_MSG_COLOR = CB_BLUE;

// one s3Message buffer needs to be reserved for output console
static s3MessageBuffer consoleBuffer = { 0 };


void s3DeleteBuffer(s3MessageBuffer* msgBuffer)
{
	if (msgBuffer)
	{

	
		int i;
		for (i = 0; i < msgBuffer->index; i++)
		{
			free(msgBuffer->messages[i].str);
			free(msgBuffer->messages);

			msgBuffer->Size = 0;
			msgBuffer->index = 0;
		}
		free(msgBuffer);
	}
}
void s3ClearBuffer(s3MessageBuffer* msgBuffer)
{
	if (msgBuffer)
	{

		int i;
		for (i = 0; i < msgBuffer->index; i++)
		{
			free(msgBuffer->messages[i].str);
			free(msgBuffer->messages);

			msgBuffer->Size = 0;
			msgBuffer->index = 0;
		}
	}
}

s3MessageBuffer s3NewMessageBuffer()
{
	s3MessageBuffer msg;
	msg.Size = 2;
	msg.messages = (s3Message*)malloc(msg.Size * sizeof(s3Message));
	msg.index = 0;
	return msg;
}

void s3InitMessageBuffers(s3MessageBuffer* msgBuffers, int count)
{
	int i;
	for (i = 0; i < count; i++)
	{
		msgBuffers[i].Size = 2;
		msgBuffers[i].messages = (s3Message*)malloc(msgBuffers[i].Size * sizeof(s3Message));
		msgBuffers[i].index = 0;
	}


	// set internal console
	consoleBuffer.Size = 2;
	consoleBuffer.messages = (s3Message*)malloc(consoleBuffer.Size * sizeof(s3Message));
	consoleBuffer.index = 0;

}

void s3InitMessageHandler(int width, int height)
{
	ScreenHeight = height;
	ScreenWidth = width;

	int i;


	consoleBuffer.Size = 2;
	consoleBuffer.messages = (s3Message*)malloc(consoleBuffer.Size * sizeof(s3Message));
	consoleBuffer.index = 0;

	for (i = 0; i < sizeof(PaddingBuffer); i++)
	{
		PaddingBuffer[i] = 32;
		ShadowBuffer[i] = 223;
		ShadeBuffer[i] = 176;
	}
}

void s3AddMessage(s3MessageBuffer* MsgBuffer, const char* msgStr, time_t t, s3Flag Prop)
{

	// check buffer size
	if (MsgBuffer->index >= MsgBuffer->Size)
	{
		MsgBuffer->Size += 10;
		MsgBuffer->messages = (s3Message*)realloc(MsgBuffer->messages, sizeof(s3Message)*MsgBuffer->Size);
		if (!MsgBuffer->messages) { puts("memory allocation error!"); system("pause");  exit(1); }
	}


	int msgLen = strlen(msgStr);



	// copy string to buffer's last element
	MsgBuffer->messages[MsgBuffer->index].str = (char*)malloc( (msgLen+1) * sizeof(char));
	strcpy(MsgBuffer->messages[MsgBuffer->index].str, msgStr);


	MsgBuffer->messages[MsgBuffer->index].Prop = Prop;
	MsgBuffer->messages[MsgBuffer->index].Length = msgLen;
	MsgBuffer->messages[MsgBuffer->index].tm = t;
	MsgBuffer->index++;


}

void s3ConsoleLog(const char * lines, s3Flag Prop)
{
	s3AddMessage(&consoleBuffer, lines,0 ,Prop );
}


void s3DrawFrame(const char* color, int posX, int posY , int width, int height)
{
	setColor(color);


	width--;
	int i = 0;

	gotoxy(posX + i, posY);
	printf("%c", 201);
	int j;
	for ( j = 0; j < width-1; j++)
	{
		printf("%c", 205);
	}
	printf("%c", 187);

	for (i = 1; i < height -1; i++)
	{
		gotoxy(posX + i, posY);
		printf("%c", 186);


		gotoxy(posX + i, posY + width);
		printf("%c", 186);

	}


	gotoxy(posX + i, posY);
	printf("%c", 200);

	for (j = 0; j < width-1; j++)
	{
		printf("%c", 205);
	}

	printf("%c", 188);




	setColor(CB_WHITE);
	setColor(C_GRAY);
	

}


void s3DrawBox(const char* color, int posX, int posY, int width, int height)
{

	setColor(color);

	setColor(C_GRAY);
	int i = 0;

	gotoxy(posX + i, posY);
	printf("%.*s", width, PaddingBuffer);

	for (i = 1; i < height; i++)
	{
		gotoxy(posX + i, posY);
		printf("%.*s", width, PaddingBuffer);

		printf("%c", CH_SHADOW_SIDE);

	}

	gotoxy(posX + i, posY);



	setColor(CB_WHITE);
	setColor(C_GRAY);
	printf(" %.*s", width, ShadowBuffer);

}


int s3PrintMessage(int x, int y, int messageWidth , int horizontalSpace ,int *verticalSpace , int *offsetSpace, s3Message* msg)
{


	int space = *verticalSpace;
	int offset = *offsetSpace;

	/*if (offset == 6)
	{
		printf("wow");
	}*/

	int x_end = x + space;
	int len = msg->Length;
	int lineCount =(int) ceil((float)len / messageWidth);
	int users3MessageOffset = y + horizontalSpace - messageWidth-2;
	int boxHeight;

	int i;



	// if there is an offset
	if (offset > 0)
	{
		offset = offset-( lineCount + 1);
		if (offset >= 0)
		{
			// do not print any line
			*offsetSpace = offset;
			return lineCount;
		}
		else
		{
			// if offset exceeded
			i = 0;
			
			lineCount =  abs(offset);
			boxHeight = lineCount ;
			x_end -= (lineCount );

			
		}
	}
	else
	{
		if (space > lineCount + 2)
		{
			i = 0;
			x_end -= (lineCount + 2);
			boxHeight = lineCount + 1;
		}
		else
		{
			i = lineCount - space + 2;
			x_end -= space;
			boxHeight = lineCount - i+1;
		}



	}

	

	char isFromUser = msg->Prop == s3_FROM_OWN;
	if (isFromUser)
	{
		// draw colored rectangle
		s3DrawBox(_s3_USER_MSG_COLOR, x_end, users3MessageOffset -1,  messageWidth+1  , boxHeight);
		setColor(_s3_USER_MSG_COLOR);

		// print message lines
		for (; i < lineCount; i++)
		{
			gotoxy(x_end, users3MessageOffset);
			int lineLen = min(messageWidth, len);
			printf("%.*s%.*s", messageWidth - lineLen, PaddingBuffer, lineLen, msg->str + messageWidth * i);
			len -= messageWidth;
			x_end++;
			space--;
		}

		// print time
		if (!offset)
		{
			gotoxy(x_end, users3MessageOffset);
			printf("%.*s~%.*s", messageWidth - 9, PaddingBuffer, 8, ctime(&msg->tm) + 11);
		}

	}
	else
	{
		// draw colored rectangle
		s3DrawBox(_s3_CONTACT_MSG_COLOR, x_end, y, messageWidth+1 , boxHeight);
		setColor(_s3_CONTACT_MSG_COLOR);

		// add 1 unit space at beginning
		y++; 

		// print message lines
		for (; i < lineCount; i++)
		{
			gotoxy(x_end, y);
			printf("%.*s", min(messageWidth, len), msg->str + messageWidth * i);
			len -= messageWidth;
			x_end++;
			space--;
		}

		//print time
		if (!offset)
		{ 
			gotoxy(x_end, y);
			printf("~%.*s", 8, ctime(&msg->tm) + 11);
		}
	}


	// reset colors
	setColor(CB_WHITE);
	setColor(C_GRAY);

	
	if (offset)
	{
		*offsetSpace = 0;
		*verticalSpace = space -1;
	}
	else
	{
		*verticalSpace = space - 2;
	}
	


	// 2 unit of padding
	return space - 2;
}

int s3PrintConsoleMessage(int x, int y, int messageWidth, int horizontalSpace, int verticalSpace, s3Message* msg)
{
	int x_end = x + verticalSpace;
	int len = msg->Length;
	int lineCount =(int) ceil((float)len / messageWidth);
	int users3MessageOffset = y + horizontalSpace - messageWidth ;


	int i;
	if (verticalSpace >= lineCount)
	{
		i = 0;
		x_end -= lineCount;
	}
	else
	{
		i = lineCount - verticalSpace;
		x_end -= verticalSpace;
	}

	// set box color 
	setColor(CB_BLUE);


	// select warn color according to prop
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

	// print lines
	for (; i < lineCount; i++)
	{

		gotoxy(x_end, y);
		printf("%.*s", min(messageWidth, len), msg->str + messageWidth * i);
		len -= messageWidth;
		x_end++;
		verticalSpace--;

	}


	// reset color
	setColor(CB_WHITE);
	setColor(C_GRAY);

	return verticalSpace;
}

void s3DrawMessageBox(int x, int y,  int width, int height, int messageWidth,int messageOffset , s3MessageBuffer msgBuffer)
{

	int screenSpace = height;
	int offset = messageOffset;

	int i = msgBuffer.index - 1;
	if (i < 0) return;


	while ((i >= 0) && (screenSpace > 0))
	{
		s3PrintMessage(x, y, messageWidth, width, &screenSpace, &offset, &msgBuffer.messages[i]);
		i--;

		
	}

}

void s3DrawConsoleBox(int x, int y, int width, int height, int messageWidth)
{
	int i = consoleBuffer.index - 1;
	if (i < 0) return;


	while ((i >= 0) && (height > 0))
	{
		height = s3PrintConsoleMessage(x,y ,messageWidth ,  width, height,  &consoleBuffer.messages[i]);
		i--;
	}
}

void s3DrawInfoBar(int isAltPressed)
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

		printf(passiveStr, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, C_YELLOW, C_WHITE, ScreenWidth - sizeof(passiveStr) + 29, PaddingBuffer);
	}

	setColor(C_GRAY);
	setColor(CB_WHITE);
}

