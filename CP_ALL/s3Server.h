/*
#		s3 Server library
#		included server tools
#
*/
#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "s3DataTypes.h"
#include "s3InputHandler.h"
#include "s3Util.h"
#include  <winsock2.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>

s3Flag s3IsServerInitialised();

SOCKET s3GetMasterSocket();

struct sockaddr_in s3GetServerAddr();


s3Flag s3InitServer(char* serverInfoString, char* path ,int portNumber);//, const char* DBpath);

s3Flag s3DestroyServer();

s3Flag s3RunServer(int timeout);


