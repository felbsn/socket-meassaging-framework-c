/*
#	s3 simple client library
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

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


SOCKET s3GetServerSocket();

s3Flag s3InitClient(const char* serverAddrString, int portNumber, Token PhoneNumber , Token* userID,int socket_timeout);

s3Flag s3DestroyClient();

Token s3GetLastRecvPhone();

char* s3GetLastRecvMessage();

s3Flag s3Reconnect(Token* userID);

s3Flag s3RunClient(unsigned int TimeoutMicroSecs);