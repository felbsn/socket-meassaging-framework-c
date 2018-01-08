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


	Token UserPhoneNumber =-1;
	Token UserID = -1;

	// create s3Message buffers for each channel / person



	const int ScreenWidth = 100;
	const int ScreenHeight = 44;
	char command[30];

	// setup inital screen 
	sprintf(command, "mode %d,%d", ScreenWidth, ScreenHeight);
	system(command);
	system("cls");
	system("color 70");


	s3InitMessageHandler(ScreenWidth , ScreenHeight);


	s3DrawBox(CB_CYAN, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 36, 6);
	setColor(CB_CYAN);
	gotoxy(ScreenHeight / 2-2 , ScreenWidth / 2 - 17);
	printf("Enter Client phone number");
	gotoxy(ScreenHeight / 2 -1, ScreenWidth / 2 - 17);
	printf("Phone:");
	UserPhoneNumber = scanPhoneNum();
	if (!UserPhoneNumber)
	{
		puts("invalid phone number");
		_getch();
		exit(1);
	}
	setColor(CB_WHITE);





	s3ContactList contactList = { 0 };

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

		s_server = 0;
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
	float offset = 0;

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
			s3Flag result = s3RunClient( &contactList,20);


			switch (result)
			{
			case s3_FAIL:
				s3ConsoleLog("server error", s3_ERROR);
				break;
			case s3_INCOMING_MSG:
				screenChange = s3_TRUE;
				s3ConsoleLog("incoming message", s3_DEFAULT);
				offset = 0;
				break;
			case s3_SERVER_ERROR:
				s3DrawBox(CB_MAGENTA, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 36, 6);
				setColor(CB_MAGENTA);
				gotoxy(ScreenHeight / 2 - 2, ScreenWidth / 2 - 17);
				printf("Server Error");
				gotoxy(ScreenHeight / 2 - 1, ScreenWidth / 2 - 17);
				printf("press any key to continue");
				_getch();
				setColor(CB_WHITE);
				setColor(C_GRAY);
				s3ConsoleLog("connection lost", s3_ERROR);
				screenChange = s3_TRUE;
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


		if (AltStatus)
		{
			if (s3GetKeyState('+', s3Key_Pressed))
			{
				offset = 0;
				channel++;
				if (channel >= contactList.Size) channel--;
				screenChange = s3_TRUE;
			}
			if (s3GetKeyState('-', s3Key_Pressed))
			{
				offset = 0;
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
				s3DrawBox(CB_YELLOW, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 36, 6);
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
						s3DrawBox(CB_YELLOW, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 36, 6);

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
						s3ConsoleLog("already added" ,s3_ERROR);
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
				s3DrawBox(CB_YELLOW, ScreenHeight / 2 - 3, ScreenWidth / 2 - 18, 36, 6);

				setColor(CB_YELLOW);
				gotoxy(ScreenHeight / 2 - 2, ScreenWidth / 2 - 17);
				printf("enter user name:");
				gotoxy(ScreenHeight / 2 - 1, ScreenWidth / 2 - 17);
				char buffer[20];
				fgets(buffer, 20, stdin);
				buffer[strlen(buffer) - 1] = 0;
				setColor(CB_WHITE);

				strcpy(contactList.contacts[channel].info, buffer);
				screenChange = s3_TRUE;
			}



			if (s3GetKeyState('d', s3Key_Pressed) || s3GetKeyState('D', s3Key_Pressed))
			{
				if (channel < contactList.Size)
				{

					if (channel < contactList.Size -1)
					{
						s3ClearContact(&contactList.contacts[channel]);
						contactList.contacts[channel] = contactList.contacts[contactList.Size - 1];
						contactList.Size--;
						s3ResetBufferChannel(channel);

					}
					else
					{

						
						int delIndex =  contactList.Size--;
						s3ClearContact(&contactList.contacts[delIndex]);
						s3ResetBufferChannel(channel);
					}
					screenChange = s3_TRUE;

				}
				
			}

			if (s3GetKeyState('r', s3Key_Pressed) || s3GetKeyState('R', s3Key_Pressed))
			{
				s3ConsoleLog("Trying to reconnect" , s3_DEFAULT);
				if (s3Reconnect(&UserID , &s_server) == s3_SUCCESS)
					s3ConsoleLog("Done!" ,s3_DONE);
				else
					s3ConsoleLog("Failed!" , s3_FAIL);
			}

			s3SeekKeyDown();
		}
		else
			if (s3SeekKeyDown())
			{	
					if (GetAsyncKeyState(VK_UP))
					{
						offset += 0.03f * max(GetTickCount() - ElapsedTime, 33);
						screenChange = s3_TRUE;
							
					}else
					if (GetAsyncKeyState(VK_DOWN))
					{
						offset -= 0.03f * max(GetTickCount() - ElapsedTime, 33);
						if (offset < 0) offset = 0;
						screenChange = s3_TRUE;
					}
					else
					if (s3GetKeyState(VK_RETURN, s3Key_Pressed))
					{
						if (contactList.Size > channel && (s3GetBufferChannelLen(channel) > 0))
						{
							s3Flag result;


								result = s3Sends3Message(s_server, &contactList.contacts[channel], channel);

								switch (result)
								{
								case s3_SUCCESS:
									s3AddMessage(&contactList.contacts[channel].msgBuffer, s3GetBufferChannel(channel), time(NULL) ,s3_FROM_OWN);
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
									break;
								case s3_FAIL:
									s3ConsoleLog("fail on send", s3_FAIL);
									break;
								default:
									break;
								}



						}
						else
						{
							s3ConsoleLog("Invalid entry", s3_FAIL);
						}

						screenChange = s3_TRUE;
					}
					else
					{
					gotoxy(30, 0);
					s3HandleText(channel);
					// input box draw
					s3DrawBox(CB_YELLOW, ScreenHeight - 6, 23, 56, 6);
					setColor(CB_YELLOW);
					char* channelData = s3GetBufferChannel(channel);
					int len = s3GetBufferChannelLen(channel);
					int lenPrint = 0;
					int xPos = ScreenHeight - 5;
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
							printf("%s", channelData + lenPrint);
							len = 0;
							}
						}
						setColor(CB_WHITE);
						setColor(C_GRAY);
					}



				//screenChange = s3_TRUE;
			}

		if (screenChange)
		{
			clear();


			// draw contacts
			s3DrawContactList(3, 1, 20, ScreenHeight - 12, &contactList, channel);



			// draw s3Message boxes
			setColor(CB_WHITE);
			s3DrawFrame(C_CYAN, 2, 24, 75, ScreenHeight - 8);
			if(contactList.Size > 0)
			s3DrawMessageBox(3, 25,  74, ScreenHeight - 10, 30, offset, contactList.contacts[channel].msgBuffer);


			// current state box
			s3DrawBox(CB_BLUE, ScreenHeight - 6 , 2 , 20, 6);
			setColor(CB_BLUE);
			gotoxy(ScreenHeight - 5, 3);
			printf("Channel: %d", channel);
			gotoxy(ScreenHeight - 4, 3);
			printf("Phone: %lld\n", UserPhoneNumber);
			gotoxy(ScreenHeight - 3, 3);
			printf("UserID:%lld\n",UserID);
			setColor(CB_WHITE);


			// information console
			s3DrawBox(CB_BLUE, ScreenHeight - 6, ScreenWidth - 20, 20, 6);
			setColor(CB_BLUE);
			s3DrawConsoleBox(ScreenHeight - 6, ScreenWidth - 20, 20, 6, 20);
			setColor(CB_WHITE);




			// input box draw
			s3DrawBox(CB_YELLOW, ScreenHeight-6, 23,56, 6);
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



	}


	return 0;
}

#endif // CLIENT
