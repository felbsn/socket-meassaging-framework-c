#include "option.h"


#ifdef CLIENT


#include "s3MessageHandler.h"
#include "s3ClientUtils.h"
#include "s3Client.h"



#include <stdlib.h>
#include <stdio.h>

#include <conio.h>
#pragma comment(lib, "ws2_32.lib")




int main(int argc , char* argv[])
{
	system("cls");



	//s3DrawFrame(C_CYAN, 1, 1, 13, 13);

	//s3DrawBox(CB_BLUE, 2, 2, 10, 10);

	//_getch();

	/*

	system("mode 100,60");
	s3InitMessageHandler(1024, 768);
	system("color 70");

	s3MessageBuffer mybuff;
	s3InitMessageBuffers(&mybuff, 1);


	s3AddMessage(&mybuff, "heyy o are you rady you mother what if there was any of them there are if", s3_FROM_CONTACT);
	s3AddMessage(&mybuff, "heyy o are you rady you mother what if there was any of them there are if", s3_FROM_OWN);
	s3AddMessage(&mybuff, "11111111111111111111111111111111111111111111111", s3_FROM_OWN);
	s3AddMessage(&mybuff, "2222222222222222222222222222222222222222222222222222222222222", s3_FROM_CONTACT);
	s3AddMessage(&mybuff, "heyy o are you rady you mother what if there was any of them there are if", s3_FROM_OWN);

	
	int space = 18;

	static char rebuff[4096];
	for (size_t i = 0; i < sizeof(rebuff)-1; i++)
	{
		rebuff[i] = ' ';
	}
	rebuff[4095] = 0;


	_getch();
	int cahnger = 0;
	float offset = 0;

	DWORD delay = 0;
	DWORD timerOld  = GetTickCount();
	DWORD timerNew;
	do
	{
		timerNew = GetTickCount();
		delay = timerNew - timerOld;
		timerOld = timerNew;

		if (delay < 30) delay = 30 ;

		if (GetAsyncKeyState(VK_UP))
		{
			offset += delay * 0.005f;

			cahnger = 1;
		}

		if (GetAsyncKeyState(VK_RIGHT))
		{

			cahnger = 1;
		}
		
		if (GetAsyncKeyState(VK_DOWN))
		{
			offset -= delay * 0.005f;
			offset < 0 ? offset = 0 : 0;
			cahnger = 1;
		}

		if (cahnger)
		{
			gotoxy(0, 0);
			printf(rebuff);

		
		s3DrawMessageBox(2, 2, 40, 20, 18, offset, mybuff);
		setColor(CB_CYAN);
		s3DrawFrame(C_CYAN, 1, 1, 42, 22);

		gotoxy(21, 2);
		printf("%f", offset);


		cahnger = 0;

		}


	} while (1);

	*/
	//system("mode 60,40");
	//system("cls");
	//system("color 70");
	//char* merabadunya = "ahg\0dfsdfsdf";
	////merabadunya[3] = 0;
	//printf("%.*s\n", 20, merabadunya);
	//
	//SetupConsolehandler(1024, 768);
	//
	//s3MessageBuffer mbuf;
	//Inits3MessageBuffers(&mbuf, 1);
	//
	//Adds3Message(&mbuf, "merhaba kardesim ben agzerbeycandan gelmisem sana bakirem ", 0);
	//Adds3Message(&mbuf, "ooo hosgelmissen aybalam ben de seni ozlirem aa", 1);
	//Adds3Message(&mbuf, "ooo hosgelmissen aybalam ben de seni ozlirem aa", 1);
	//Adds3Message(&mbuf, "ooo hosgelmissen aybalam ben de seni ozlirem aa", 1);
	//
	//Draws3MessageBox(2, 2, 30, 10, 50, mbuf);
	//
	//
	//_getch();
	//system("pause");

	if (argc == 1)
	{
		printf("press c for run with compatibility mode\n");
		printf("press any key to continue ... \n");
			char cmpMode;
			cmpMode = _getch();
				if (cmpMode == 'c' || cmpMode == 'C')
				{
					char command[60];
					sprintf(command, "ansicon %s %s", argv[0], "c");
					int result  = system(command);
					if (result)
					{
						printf("system cant find ansicon.exe\n");
						system("pause");
					}
					return 0;
				}
	}

	system("cls");


	const int ScreenWidth = 100;
	const int ScreenHeight = 44;
	char command[30];

	Token UserPhoneNumber;
	Token UserID;


	// setup inital screen 
	sprintf(command, "mode %d,%d", ScreenWidth, ScreenHeight);
	system(command);
	system("cls");
	system("color 70");

	// create s3Message buffers for each channel / person
	s3MessageBuffer msgBuffers[256];


	// bind 
	s3InitMessageHandler(ScreenWidth , ScreenHeight);

	s3InitMessageBuffers(msgBuffers, 256);


	s3DrawBox(CB_CYAN, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 6, 36);
	setColor(CB_CYAN);
	gotoxy(ScreenHeight / 2-2 , ScreenWidth / 2 - 17);
	printf("Enter Client phone number");
	gotoxy(ScreenHeight / 2 -1, ScreenWidth / 2 - 17);
	printf("Phone:");
	UserPhoneNumber = scanPhoneNum();
	if (!UserPhoneNumber)
	{
		puts("invalid phone number");
		system("pause");
		exit(1);
	}
	setColor(CB_WHITE);





	static s3ContactList contactList = { 0 };

	s3InitContactList(&contactList);
	SOCKET s_server;
	
	if (s3InitClient("localhost", 8888, UserPhoneNumber, &UserID, 0) != s3_SUCCESS)
	{
		gotoxy(ScreenHeight / 2 , ScreenWidth / 2 - 17);
		setColor(CB_CYAN);
		setColor(C_RED);
		printf("Cant initialised client!");
		gotoxy(ScreenHeight / 2 + 1, ScreenWidth / 2 - 17);
		printf("Enter to continue");
		char ch = 0;
		while (ch != 13) ch = _getch();
		setColor(CB_WHITE);
		setColor(C_GRAY);

		s_server = NULL;
	}
	else
	{
		s_server = s3GetServerSocket();


		gotoxy(ScreenHeight / 2, ScreenWidth / 2 - 17);
		setColor(CB_CYAN);
		setColor(C_GREEN);
		s3PrintRxBuffer();
		_getch();
		gotoxy(ScreenHeight / 2 + 1, ScreenWidth / 2 - 17);
		printf("Enter to continue");
		setColor(CB_WHITE);
		setColor(C_GRAY);
	}



	s3Flag isRun = s3_OK;
	static s3Flag AltStatus = s3_FALSE;

	int channel = 0;
	int UserChannel = 0;

	s3Flag screenChange = s3_TRUE;



	DWORD ElapsedTime = 0;
	DWORD LastTime = GetTickCount();
	DWORD TimeRef = LastTime;
	while (isRun)
	{

		// check every 1 second incoming s3Messages
		ElapsedTime = GetTickCount();
		if (ElapsedTime - LastTime > 1000)
		{

		// wait 20 microsec for any incoming event
		s3Flag result = s3RunClient(20);

		switch (result)
		{
		case s3_ERROR:
			break;
		case s3_FAIL:
			break;
		case s3_INCOMING_MSG:
		{
			Token lphone = s3GetLastRecvPhone();
			int ContactIndex = s3FindContactIndexByPhone(&contactList, lphone);
			if (ContactIndex >= 0)
			{
				s3AddMessage(&msgBuffers[ContactIndex], s3GetLastRecvs3Message(), 0);
				contactList.data[ContactIndex].s3MessageCount++;
			}
			else
			{
				char buffer[30];
				sprintf(buffer, "%llu", lphone);
				int index = s3AddContact(s_server, &contactList, buffer, lphone);

				contactList.data[index].s3MessageCount++;
				UserChannel++;
				// whic last added contact
				s3AddMessage(&msgBuffers[index], s3GetLastRecvs3Message(), 0);
			}

			s3ConsoleLog( "incoming s3Message" , s3_DEFAULT);
			screenChange = s3_TRUE;

		}
		break;
		case s3_SERVER_ERROR:
		{	s3ConsoleLog("SERVER_ERROR" ,s3_ERROR);
			s3KeyGetAltToggleReset();

			s3DrawBox(CB_RED, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 6, 36);
			setColor(CB_RED);
			gotoxy(ScreenHeight / 2 - 2, ScreenWidth / 2 - 17);
			printf("SERVER_ERROR");
			gotoxy(ScreenHeight / 2 - 1, ScreenWidth / 2 - 17);
			printf("you can try to reconnect");
			setColor(CB_WHITE );

		}break;
		case s3_OVERRIDE_ERROR:
			{
			s3DrawBox(CB_RED, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 6, 36);
			setColor(CB_RED);
			gotoxy(ScreenHeight / 2 - 2, ScreenWidth / 2 - 17);
			printf("OVERRIDE_ERROR");
			gotoxy(ScreenHeight / 2 - 1, ScreenWidth / 2 - 17);
			printf("press any key to contiune");
			setColor(CB_WHITE);
			}break;
		case s3_USER_OFFLINE:
			s3ConsoleLog("user offline" , s3_DEFAULT);
			break;
		case s3_USER_NOT_FOUND:
			s3ConsoleLog("user not found" ,s3_ERROR);
			break;
		case s3_USER_ID_INVALID:
			s3ConsoleLog("user id invalid" ,s3_ERROR);
			break;
		case s3_TIMEOUT:
			s3ConsoleLog("timeout on receive",s3_ERROR);
			break;
		default:
			break;
		}

		LastTime = ElapsedTime;

		}


		s3Flag currentAltStatus = s3KeyGetAltStatus();
		if (AltStatus != currentAltStatus)
		{
			AltStatus = currentAltStatus;
			screenChange = s3_TRUE;
		}

		

		s3KeyGetAltStatus();

		if (AltStatus)
		{
			if (s3GetKeyState('+', s3Key_Pressed))
			{

				channel++;
				if (channel >= UserChannel) channel = max((UserChannel-1) , 0 );
				screenChange = s3_TRUE;
			}
			if (s3GetKeyState('-', s3Key_Pressed))
			{

				channel--;
				if (channel < 0) channel = 0;
				screenChange = s3_TRUE;
			}
			if (s3GetKeyState('q', s3Key_Pressed))
			{

				isRun = s3_FALSE;
				s3SendMsg(s_server, s3_EXIT_FROM_SERVER);
			}

			if (s3GetKeyState('a', s3Key_Pressed) || s3GetKeyState('A', s3Key_Pressed))
			{
				s3KeyGetAltToggleReset();
				s3DrawBox(CB_YELLOW, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 6, 36);
				setColor(CB_YELLOW);
				gotoxy(ScreenHeight / 2 - 2, ScreenWidth / 2 - 17);
				printf("Enter phone number");
				gotoxy(ScreenHeight / 2 - 1, ScreenWidth / 2 - 17);
				printf("Phone:");
				Token phone = scanPhoneNum();
				setColor(CB_WHITE);

				// escape command
				if (!phone) continue;

				if (phone != UserPhoneNumber)
				{
					int ContactIndex = s3FindContactIndexByPhone(&contactList, phone);
					if (ContactIndex < 0)
					{
						s3DrawBox(CB_YELLOW, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 6, 36);

						setColor(CB_YELLOW);
						gotoxy(ScreenHeight / 2 - 2, ScreenWidth / 2 - 17);
						printf("enter user name:");
						gotoxy(ScreenHeight / 2 - 1, ScreenWidth / 2 - 17);
						char buffer[20];
						fgets(buffer, 20, stdin);
						buffer[strlen(buffer) - 1] = 0;
						setColor(CB_WHITE);
						int res = s3AddContact(s_server, &contactList, buffer, phone);
						if (res >= 0)
						{

							s3ConsoleLog("user added succesffully",s3_SUCCESS);
							UserChannel++;
						}
						else
						{
							if(res == s3_ERROR)
							s3ConsoleLog("Server Offline" , s3_ERROR);
							else
								if (res == s3_USER_NOT_FOUND)
								{
									s3ConsoleLog("User not found on server", s3_ERROR);
								}
								else
								{
									s3ConsoleLog("Undefined Error" , s3_ERROR);
								}
						}

					}
					else
					{
						s3ConsoleLog("allready added" ,s3_ERROR);
					}


				}
				else
				{
					s3ConsoleLog("PhoneNo corresponds with client" , s3_ERROR);
				}

				screenChange = s3_TRUE;
			}

			if (s3GetKeyState('e', s3Key_Pressed) || s3GetKeyState('E', s3Key_Pressed))
			{
				s3KeyGetAltToggleReset();
				s3DrawBox(CB_YELLOW, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 6, 36);

				setColor(CB_YELLOW);
				gotoxy(ScreenHeight / 2 - 2, ScreenWidth / 2 - 17);
				printf("enter user name:");
				gotoxy(ScreenHeight / 2 - 1, ScreenWidth / 2 - 17);
				char buffer[20];
				fgets(buffer, 20, stdin);
				buffer[strlen(buffer) - 1] = 0;
				setColor(CB_WHITE);

				strcpy(contactList.data[channel].info, buffer);
				screenChange = s3_TRUE;
			}



			if (s3GetKeyState('d', s3Key_Pressed), s3GetKeyState('D', s3Key_Pressed))
			{

				if (UserChannel >= 0)
				{
					if (channel != UserChannel - 1)
					{

						msgBuffers[channel].index = 0;

						s3MessageBuffer tmpBuf = msgBuffers[channel];

						msgBuffers[channel] = msgBuffers[UserChannel - 1];

						msgBuffers[UserChannel - 1] = tmpBuf;

						contactList.data[channel] = contactList.data[UserChannel - 1];

						contactList.Size--;
						UserChannel--;
					}
					else
					{

						msgBuffers[channel].index = 0;
						contactList.Size--;
						UserChannel--;
					}
					screenChange = s3_TRUE;
				}
			}

			if (s3GetKeyState('r', s3Key_Pressed) || s3GetKeyState('R', s3Key_Pressed))
			{
				s3ConsoleLog("Trying to reconnect" , s3_DEFAULT);
				if (s3Reconnect(&UserID) == s3_SUCCESS)
					s3ConsoleLog("Done!" ,s3_DONE);
				else
					s3ConsoleLog("Failed!" , s3_FAIL);
			}

			s3SeekKeyDown();
		}
		else
			if (s3SeekKeyDown())
			{

					if (s3GetKeyState(VK_RETURN, s3Key_Pressed))
					{
						if (UserChannel > channel && (s3GetBufferChannelLen(channel) > 0))
						{
							s3Flag result;


								result = s3Sends3Message(s_server, &contactList.data[channel], channel);

								switch (result)
								{
								case s3_SUCCESS:
									s3AddMessage(&msgBuffers[channel], s3GetBufferChannel(channel), s3_FROM_OWN);
									s3ResetBufferChannel(channel);
									s3ConsoleLog("succesfully send", s3_SUCCESS);
									break;
								case s3_NOT_FOUND:
									s3ConsoleLog("user not found" , s3_FAIL);
									break;
								case s3_USER_OFFLINE:
									s3ConsoleLog("user offline", s3_FAIL);
									break;
								case s3_USER_NOT_FOUND:
									s3ConsoleLog("user not found", s3_FAIL);
									break;
								case s3_USER_ID_INVALID:
									s3ConsoleLog("user id invalid", s3_FAIL);
									break;
								case s3_TIMEOUT:
									s3ConsoleLog("timeout on send", s3_FAIL);
									//s3SendMsg(s_server, s3_TIMEOUT);
									break;
								case s3_FAIL:
									s3ConsoleLog("fail on send", s3_FAIL);
									//s3SendMsg(s_server, s3_TIMEOUT);
									break;
								default:
									break;
								}



						}
						else
						{
							s3ConsoleLog("Invalid entry", s3_FAIL);
						}
					}
					else
					{
					gotoxy(30, 0);
					s3HandleText(channel);
					}



				screenChange = s3_TRUE;
			}

		if (screenChange)
		{
			clear();


			/// draw s3Message boxes
			setColor(CB_RED);
			s3DrawFrame(C_CYAN, 2, 19, ScreenHeight - 10, 70);
			if(UserChannel > 0)
			s3DrawMessageBox(3, 20, ScreenHeight - 10, 30,70, 0, msgBuffers[channel]);


			// current state box

			s3DrawBox(CB_BLUE, ScreenHeight - 6 , 2 , 6, 20);
			setColor(CB_BLUE);
			gotoxy(ScreenHeight - 5, 3);
			printf("Channel: %d", channel);
			gotoxy(ScreenHeight - 4, 3);
			printf("Phone:%llu\n", ( unsigned long long)UserPhoneNumber);
			gotoxy(ScreenHeight - 3, 3);
			printf("UserID:%llu\n",( unsigned long long) UserID);
			setColor(CB_WHITE);


			// information console
			s3DrawBox(CB_BLUE, ScreenHeight - 6, ScreenWidth - 20, 6, 20);
			setColor(CB_BLUE);
			int maxConsoleLines = max(msgBuffers[255].index - 6, 0);
			int consoleLineLimit = 6;

			s3DrawConsoleBox(ScreenHeight - 6, ScreenWidth - 20, 6, 20, 20);


			setColor(CB_WHITE);




			// input box draw
			s3DrawBox(CB_YELLOW, ScreenHeight-6, 23, 6, 56);
			setColor(CB_YELLOW);
			char* channelData = s3GetBufferChannel(channel);
			int len = s3GetBufferChannelLen(channel);
			int lenPrint = 0;
			int xPos = ScreenHeight -5;
			int yPos = 24;
			int LinePadding = 54;
			while (len > 0)
			{
				if (len > LinePadding)
				{
					gotoxy(xPos, yPos);
					printf("%.*s", LinePadding, channelData + lenPrint);
					lenPrint += LinePadding;
					len -= LinePadding;
					xPos++;
				}
				else
				{
					gotoxy(xPos, yPos);
					printf("%s", channelData+ lenPrint);
					len = 0;
				}
			}

			//draw info bar
			gotoxy(0, 0);

			s3DrawInfoBar(AltStatus);

			setColor(CB_WHITE);
			screenChange = s3_FALSE;
		}


		



		gotoxy(ScreenHeight - 2, 3);
		int printlen = printf("time:%d", (ElapsedTime - TimeRef) / 1000);
		int i;


	}


	system("pause");
	return 0;
}

#endif // CLIENT
