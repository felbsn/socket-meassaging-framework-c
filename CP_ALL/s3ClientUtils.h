
/*
#		Client utility tools
#		Helps managing contacts and s3Message buffers
#
*/
#pragma once

#include "s3DataTypes.h"
#include "s3InputHandler.h"
#include "s3MessageHandler.h"
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
	s3MessageBuffer msgBuffer;
	int messageCount;
};
typedef struct s3Contact_s s3Contact;

struct s3ContactList_s
{
	s3Contact * contacts;
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

// send buffer typed s3Message to selected contact
s3Flag s3Sends3Message(SOCKET serverSocket, s3Contact* contact, int bufferChannel);

s3Flag s3HandleMessages(s3ContactList* contactList, Token phone);

s3Flag s3GetContactID(SOCKET s_server, s3Contact *contact);