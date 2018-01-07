#include "s3Server.h"
#include <Windows.h>
// s3 server handlers

static WSADATA wsa; // winsock
static SOCKET s3_server;
struct sockaddr_in s3_serverAddr;
static fd_set readfds;

static s3Flag _s3_initStatus = s3_FALSE;
static s3ClientList Clist = { 0 };

// temporal buffer for incoming client messages
static s3Flag msgBuffer[15];
static Token tokenBuffer[15];
static s3MessageQuene msgQuene = { 0 };

static char* s3_serverInfo;
static int s3_serverInfoLen = 0;


s3Flag s3IsServerInitialised()
{
	return _s3_initStatus;
}

SOCKET  s3GetMasterSocket()
{
	return s3_server;
}

struct sockaddr_in s3GetServerAddr()
{
	return s3_serverAddr;
}


void s3ClearDataBases(char* foldername)
{
	char path[50];
	strcpy(path, foldername);

	// search all txt files in 'foldername' folder which was 'kilavuz'
	char searchpath[] = "/*.DB";

	strcat(path, searchpath);





	char command[60];
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {

			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{

				//printf("%s \n", fd.cFileName);
				sprintf(command, "del \"%s\\%s\"", foldername, fd.cFileName);
				system(command);


			}
		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);

	}

	// if folder not exist
	sprintf(command , "mkdir %s" , foldername);
	system(command);

}

s3Flag s3InitServer(char* serverInfoString, char* path, int portNumber)// , const char* DBpath)
{
	if (_s3_initStatus == s3_FALSE)
	{
		s3ClearDataBases(path);

		s3InitRxBuffer();
		s3InitTxBuffers();
		s3InitClientList(&Clist);
		s3InitMessageQuene(&msgQuene);

		SetDbPath(path);

		s3_serverInfoLen = (strlen(serverInfoString) + 1);
		s3_serverInfo = (char*)malloc(s3_serverInfoLen * sizeof(char));//(char*)realloc(serverInfoString, s3_serverInfoLen * sizeof(char));
		strcpy(s3_serverInfo, serverInfoString);


		printf("\nInitialising Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			system("pause");
			exit(EXIT_FAILURE);
		}

		printf("Initialised.\n");

		//Create a socket
		if ((s3_server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d", WSAGetLastError());
			system("pause");
			exit(EXIT_FAILURE);
		}
		printf("Socket created.\n");




		s3_serverAddr.sin_family = AF_INET;
		s3_serverAddr.sin_addr.s_addr = INADDR_ANY;	//inet_addr("172.17.103.200");
		s3_serverAddr.sin_port = htons(portNumber); // 8888 port addr


													//Bind
		if (bind(s3_server, (struct sockaddr *)&s3_serverAddr, sizeof(s3_serverAddr)) == SOCKET_ERROR)
		{
			printf("Bind failed with error code : %d", WSAGetLastError());
			system("pause");
			exit(EXIT_FAILURE);
		}

		puts("Bind done");

		//Listen to incoming connections
		listen(s3_server, 3);

		//Accept and incoming connection
		puts("Waiting for incoming connections...");



		_s3_initStatus = s3_TRUE;
		return s3_SUCCESS;
	}
	else
	{
		puts("s3 already initialised!");
		return s3_FALSE;
	}
}

s3Flag s3DestroyServer()
{
	if (_s3_initStatus == s3_TRUE)
	{
		closesocket(s3_server);
		WSACleanup();
	}
	else
		return s3_FALSE;

}




// run fnc types
s3Flag s3RunServer(int timeout)
{
	//clear the socket fd set
	FD_ZERO(&readfds);

	//add master socket to fd set
	FD_SET(s3_server, &readfds);

	//add child sockets to fd set
	int i;
	for (i = 0; i < Clist.Size; i++)
	{
		SOCKET s = Clist.data[i].socket;

		if (s)
			FD_SET(s, &readfds);
	}

	//wait for an activity on any of the sockets, timeout is NULL , so wait indefinitely
	int activity;
	if (timeout)
	{
		static struct timeval tm = { 0 ,0 };
		tm.tv_usec = timeout;
		activity = select(0, &readfds, NULL, NULL, &tm);
	}
	else
		activity = select(0, &readfds, NULL, NULL, 0);

	if (activity == SOCKET_ERROR)
	{
		printf("select call failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	//If something happened on the master socket , then its an incoming connection
	if (FD_ISSET(s3_server, &readfds))
	{
		SOCKET new_socket;
		struct sockaddr_in address;
		int addrlen = sizeof(struct sockaddr_in);
		if ((new_socket = accept(s3_server, (struct sockaddr *)&address, (int *)&addrlen))<0)
		{
			perror("accept");
			exit(EXIT_FAILURE);
		}

		//inform user of socket number - used in send and receive commands
		printf("New connection , socket fd is %d , ip is : %s , port : %d \n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

		//send server info
		if (send(new_socket, s3_serverInfo, s3_serverInfoLen, 0) != s3_serverInfoLen)
		{
			perror("send failed");
		}


		puts("Welcome message sent successfully");



		// get phone number from new socket
		//valread = recv(new_socket, (char*)tokenBuffer, sizeof(tokenBuffer), 0);
		s3Flag s3Res;
		s3Res = s3RecvToken(new_socket, tokenBuffer);

		if (s3Res == s3_SUCCESS)
		{

			puts("request accepted succesfully");

			Token newID;
			s3Flag result;
			int index = addClient(&Clist, new_socket, tokenBuffer[0], &result);
			s3SendMsg(new_socket, result);
			if (index >= 0)
			{


			s3RecvMsg(new_socket, &s3Res); // incoming user ID request
			newID = index;
			s3SendToken(new_socket, newID);


			printf("socket result code: %d \n", result);

			// send messages to new user
			s3HandleMessages(&Clist, &Clist.data[index]);
			}
		}
		else
		{
			puts("invalid handshake");
			s3SendMsg(new_socket, s3_FAIL);
			closesocket(new_socket);
		}



	}




	//else its some IO operation on some other socket :)
	for (i = 0; i < Clist.Size; i++)
	{
		SOCKET s = Clist.data[i].socket;
		if (!s) continue;


		//if client presend in read sockets
		if (FD_ISSET(s, &readfds))
		{
			//get details of the client
			//getpeername(s, (struct sockaddr*)&address, (int*)&addrlen);

			//Check if it was for closing , and also read the incoming message

			// status reading

			printf("incoming connection\n ");


			int s3Res = s3RecvMsg(s, msgBuffer);

			if (s3Res == SOCKET_ERROR)
			{
				int error_code = WSAGetLastError();
				if (error_code == WSAECONNRESET)
				{
					printf("user disconnect unexpectedly -> Phone:%llu ,UserID:%llu \n", Clist.data[i].phoneNo, Clist.data[i].userID);

					//Close the socket and mark as 0 in list for reuse

					eraseClientByIndices(&Clist, i);
				}
				else
				{
					printf("recv failed with error code : %d", error_code);
				}
			}
			else
				if (s3Res == 0)
				{
					printf("user disconnect -> Phone:%llu ,UserID:%llu \n", Clist.data[i].phoneNo, Clist.data[i].userID);

					eraseClientByIndices(&Clist, i);
				}
				else
				{
					printf("connection came in -> Phone:%llu ,UserID:%llu \n", Clist.data[i].phoneNo, Clist.data[i].userID);

					s3HandleConnection(&Clist, i, msgBuffer[0]);

				}
		}




	}

	return s3_OK;
}

