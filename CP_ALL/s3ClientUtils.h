
/*
#		Client utility tools
#		Helps managing contacts and message buffers
#
*/
#pragma once

#include "s3DataTypes.h"
#include "s3InputHandler.h"
#include "s3Util.h"
#include  <winsock2.h>
#include	 <stdio.h>
#include	 <stdlib.h>
#include	 <math.h>

#pragma warning(disable:4996)

struct s3Contact_s
{
	char info[20];
	Token id;
	Token phoneNo;
	//reserved index for later usage
	int ClientIndex;
	int messageCount;
};
typedef struct s3Contact_s s3Contact;

struct s3ContactList_s
{
	s3Contact * data;
	int Capacity;
	int Size;
	int ContactCount;
};
typedef struct s3ContactList_s  s3ContactList;

// later design phone finder
int BinarySearchPhone(s3Contact arr[], int l, int r, Token phoneNum);

// contact finder
int s3FindContactIndexByPhone(s3ContactList* contactList, Token phoneNo);

//simple contatct maker function
s3Contact NewContactProtoype(const char* info, Token phoneNo);

// inital allocation for contactlist
s3Flag s3InitContactList(s3ContactList* contactList);

// contact adder , initaliser
int s3AddContact(SOCKET serverSocket, s3ContactList* contactList, char* infoStr, Token phoneNo);

// send buffer typed message to selected contact
s3Flag s3SendMessage(SOCKET serverSocket, s3Contact* contact, int bufferChannel);
