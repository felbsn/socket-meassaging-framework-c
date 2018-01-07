
#pragma once
#include<winsock2.h>

typedef  long long  Token;

enum s3Flag
{
	// behavior flags
	s3_ALREADY_REGISTERED = -3,
	s3_NOT_FOUND = -2,
	s3_ERROR,
	s3_FALSE,
	s3_FAIL = s3_FALSE,
	s3_CONNECTION_LOST = s3_FALSE,
	s3_OK,
	s3_TRUE = s3_OK,
	s3_SUCCESS = s3_OK,
	s3_FOUND = s3_OK,
	s3_ACCEPT,
	s3_RESIGNED,
	s3_SERVER_ERROR,
	s3_DONE,
	s3_CONTINUE,
	s3_DEFAULT,

	//request flags
	s3_REQ_USERID_wPHONENO,
	s3_REQ_USERID_OWN,
	s3_REQ_SERVER_INFO,


	// communication flags
	s3_INCOMING_MSG,
	s3_SEND_MSG_wUSERID,
	s3_SEND_MSG_wPHONENO,


	// structure definitions
	s3_SERVER,
	s3_CLIENT,
	s3_FROM_OWN,
	s3_FROM_CONTACT,
	S3_FROM_SERVER,



	// server manipulation flags

	s3_EXIT_FROM_SERVER,




	//user state ERROR definitions
	s3_USER_OFFLINE = -100,
	s3_USER_NOT_FOUND,
	s3_USER_ID_INVALID,
	s3_OVERRIDE_ERROR,
	s3_TIMEOUT

};
typedef enum s3Flag s3Flag;

struct _s3_RXBUFFER_s
{
	int Length;
	int Capacity;
	char* data;
};

typedef struct _s3_RXBUFFER_s _s3_RXBUFFER;


struct _s3_TXBUFFER_s
{
	int Size;
	int Cur;
	char* data;
};
typedef struct _s3_TXBUFFER_s _s3_TXBUFFER;

struct s3ClientProperty_s
{
	// unique id that stays same
	Token userID;
	int msgCounter;
	Token phoneNo;
	SOCKET socket;
};
typedef struct s3ClientProperty_s s3ClientProperty;


struct s3ClientList_s
{
	Token IDcount;
	int Size;
	int Capacity;
	s3ClientProperty *data;
};

typedef struct s3ClientList_s s3ClientList;


struct s3MessageData_s
{
	Token UserID;
	int Count;
};
typedef struct s3MessageData_s s3MessageData;


struct s3MessageQuene_s
{
	int Size;
	int Capacity;
	s3MessageData* msgData;

};
typedef struct s3MessageQuene_s s3MessageQuene;
