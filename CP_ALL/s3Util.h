
/*
#	S3 General Purpose Utility Library
#
*/


#pragma once
#include "s3DataTypes.h"
#include "s3InputHandler.h"
#include  <winsock2.h>
#include	 <stdio.h>
#include	 <stdlib.h>
#include	 <math.h>
#include  <conio.h>

Token scanPhoneNum();

void setPhoneNoBCD(Token* phoneNo, char* data);

void setPhoneNo(Token* phoneNo, char* data);

void setUserID(Token* userID, char* data);

int s3InitClientList(s3ClientList* destination);

void s3DestroyClientList(s3ClientList* destination);

int clearClientByIndices(s3ClientList* destination, int index);

int removeClientByIndices(s3ClientList* destination, int index);

int eraseClientByIndices(s3ClientList* destination, int index);

Token findIDByPhoneNum(s3ClientList* clist, Token phoneNum);

int findIndexByPhoneNum(s3ClientList* clist, Token phoneNum);

int addClient(s3ClientList* clist, SOCKET s, Token phoneNum , s3Flag* flag);


int s3SendTime(SOCKET s, time_t t);
s3Flag s3RecvTime(SOCKET s, time_t* t);

int s3SendMsg(SOCKET s, int flag);

s3Flag s3RecvMsg(SOCKET s, s3Flag* flagBuffer);


s3Flag s3RecvToken(SOCKET s, Token* data);

int s3SendToken(SOCKET s, Token data);

s3Flag s3StoreServerData(s3ClientList * clist, char* storageFileName);

s3Flag s3Inits3MessageQuene(s3s3MessageQuene* msgQuene);

// !!! only for temporal usage !!!
char* tokenToChar(Token userID);
// !!! only for temporal usage !!!
char* tokenToCharDB(Token userID );

void SetDbPath(const char *DBpath_in);

s3Flag s3AddQueneItem(s3ClientList * clist, Token targetID, Token senderPhone, time_t t, char* s3Message, int s3MessageLen);


int s3ProcessConnection(s3ClientList * clist, Token index, s3Flag flag);

s3Flag s3ProcessMessages(s3ClientProperty* CP);