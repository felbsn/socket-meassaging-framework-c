#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "s3Client.h"



static WSADATA wsa;
static SOCKET s_server = NULL;
static struct sockaddr_in server_addr;
static int addrlen;

static Token _phoneNum;
static fd_set readfds;

static s3Flag _s3_initStatus = s3_FALSE;
static Token _lastReceivedPhoneNo;
static DWORD _ConnectionTimeout = 0;


void s3DeleteContact(s3Contact* contact)
{
	if (contact)
	{
		s3ClearBuffer(&contact->msgBuffer);
		free(contact);
	}
}

void s3ClearContact(s3Contact* contact)
{
	if (contact)
	{
		s3ClearBuffer(&contact->msgBuffer);
		
	}
}


Token s3GetLastRecvPhone()
{
	return _lastReceivedPhoneNo;
}

char* s3GetLastRecvs3Message()
{
	return s3GetRxBuffer();
}

SOCKET s3GetServerSocket()
{
	return s_server;
}


s3Flag s3InitClient(const char* HostNameStr, int portNumber, Token PhoneNumber , Token* userID,int socket_timeout)
{
	if (_s3_initStatus == s3_FALSE)
	{


		_phoneNum = PhoneNumber;
		s3InitRxBuffer();
		s3InitTxBuffers();
		//s3InitContactList(&contactList);




		//printf("\nInitialising Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			//printf("Failed. Error Code : %d", WSAGetLastError());
			//system("pause");
			return s3_FAIL;
		}

		//printf("Initialised.\n");

		if ((s_server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			//printf("Could not create socket : %d", WSAGetLastError());
			return s3_FAIL;
		}

		struct hostent *he;

		if ((he = gethostbyname(HostNameStr)) == NULL)
		{
			//gethostbyname failed
			//printf("gethostbyname failed : %d", WSAGetLastError());
			//system("pause");
			return s3_FAIL;
		}

		struct in_addr ** addr_list = (struct in_addr **) he->h_addr_list;
		int i;
		char ip[100];
		for (i = 0; addr_list[i] != NULL; i++)
		{
			//Return the first one;
			strcpy(ip, inet_ntoa(*addr_list[i]));
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_addr = *addr_list[0];
		server_addr.sin_port = htons(portNumber);

		if (connect(s_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		{
			//printf("connect failed %d \n", WSAGetLastError());
			//system("pause");
			return s3_FAIL;
		}

		//puts("connect done");

		listen(s_server, 3);
		addrlen = sizeof(struct sockaddr_in);

		s3RecvBuffer(s_server);

		// begin hand shake process
		s3SendToken(s_server, PhoneNumber);
		s3Flag msg;

		s3RecvMsg(s_server, &msg);

		s3SendMsg(s_server, s3_REQ_USERID_OWN);

		s3RecvToken(s_server, userID);


		// after registering server set timeout value
		if (socket_timeout)
		{
			_ConnectionTimeout = socket_timeout;
		setsockopt(s_server, SOL_SOCKET, SO_RCVTIMEO, (char*)&_ConnectionTimeout, sizeof(_ConnectionTimeout));
		}
		if (msg == s3_ALREADY_REGISTERED)
		{
			//puts("s3 failed ,aready resigned in!");
			s3DestroyClient();
			return s3_FAIL;
		}


		_s3_initStatus = s3_TRUE;
		return s3_SUCCESS;
	}
	else
	{
		//puts("s3 already initialised!");
		return s3_FALSE;
	}


	return s3_SUCCESS;
}

s3Flag s3Reconnect(Token* userID ,SOCKET* serverSocket)
{
	closesocket(s_server);
	if ((s_server = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		return s3_FAIL;
	};


	if (connect(s_server, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{

		return s3_FAIL;
	}

	listen(s_server, 3);
	addrlen = sizeof(struct sockaddr_in);


	s3Flag msg;
	////
	// begin hand shake process
	s3RecvBuffer(s_server);

	s3SendToken(s_server, _phoneNum);
	s3RecvMsg(s_server, &msg);

	if (msg <= 0)
	{
		return s3_FAIL;
	}


	// request user owned user id from server
	s3SendMsg(s_server, s3_REQ_USERID_OWN);
	s3RecvToken(s_server, userID);

	////

	// after registering server set timeout value
	if(_ConnectionTimeout)
	setsockopt(s_server, SOL_SOCKET, SO_RCVTIMEO, (char*)&_ConnectionTimeout, sizeof(_ConnectionTimeout));

	if (msg == s3_SUCCESS)
	{
		//s3 succesfully registered!

	}else
	if (msg == s3_RESIGNED)
	{
		//s3 succesfully resigned!
	}

	*serverSocket = s_server;
	return s3_SUCCESS;
}

s3Flag s3DestroyClient()
{
	//to do some destruction stuff

	return s3_SUCCESS;
}

s3Flag s3RunClient(s3ContactList *contactList ,unsigned int TimeoutMicroSecs)
{
	

	FD_ZERO(&readfds);

	//add master socket to fd set
	FD_SET(s_server, &readfds);

	int activity;

	if (TimeoutMicroSecs)
	{
		struct  timeval tm;
		tm.tv_usec = TimeoutMicroSecs;
		activity = select(0, &readfds, NULL, NULL, &tm);
	}
	else
		activity = select(0, &readfds, NULL, NULL, 0);

	if (FD_ISSET(s_server, &readfds))
	{


		s3Flag msg;

		int res = s3RecvMsg(s_server, &msg);

		if (!res) return s3_SERVER_ERROR;

		if (msg == s3_INCOMING_MSG)
		{
			do
			{
				s3SendMsg(s_server, s3_ACCEPT);
				s3Flag Res = s3RecvToken(s_server, &_lastReceivedPhoneNo);
				s3SendMsg(s_server, s3_OK);

				time_t timeRec;
				 Res = s3RecvTime(s_server, &timeRec);
				s3SendMsg(s_server, s3_OK);


				s3RecvBuffer(s_server);

				s3HandleMessages(contactList , _lastReceivedPhoneNo , timeRec);

				s3SendMsg(s_server, s3_OK);

				int res = s3RecvMsg(s_server, &msg);

			} while (msg == s3_CONTINUE);

			if (msg == s3_DONE)
			{
				return s3_INCOMING_MSG;
			}
			else
			{
				return s3_FAIL;
			}
				
		}
		else
		{

			return msg;

		}


		
	}
	else
	{
		return s3_DEFAULT;
	}


	
}
