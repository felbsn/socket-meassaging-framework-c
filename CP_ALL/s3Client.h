/*
#	s3 simple client library
#
*/

#pragma once

#include "s3DataTypes.h"
#include "s3InputHandler.h"
#include "s3ClientUtils.h"
#include "s3MessageHandler.h"
#include "s3Util.h"
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


SOCKET s3GetServerSocket();

s3Flag s3InitClient(const char* serverAddrString, int portNumber, Token PhoneNumber , Token* userID,int socket_timeout);

s3Flag s3DestroyClient();

Token s3GetLastRecvPhone();

char* s3GetLastRecvs3Message();

s3Flag s3Reconnect(Token* userID , SOCKET* serverSocket);

s3Flag s3RunClient(s3ContactList *contactList, unsigned int TimeoutMicroSecs);

void s3DeleteContact(s3Contact* contact);

void s3ClearContact(s3Contact* contact);