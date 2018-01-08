#include "s3Util.h"
#pragma warning(disable:4996)
// 10 digit phone number

Token scanPhoneNum()
{
	char phoneChar[11];
	Token num;
		int i;
		for (i = 0; i < 10; i++)
		{
			char ch = 0;
			do {

				ch = _getch();
				if (ch == 8)
				{
					if (i > 0)
					{

						_putch(ch);
						_putch(' ');
						_putch(ch);
						i--;
					}

				}

				// back movement
				if (ch == 27)
					return 0;

			} while (ch > '9' || ch < '0');

			_putch(ch);
			phoneChar[i] = ch;
		}

		setPhoneNoBCD(&num, phoneChar);

		return num;
}


void setPhoneNoBCD(Token* phoneNo, char* data)
{
	*phoneNo = 0;
	for (size_t i = 0; i < 10; i++)
	{
		*phoneNo += (Token)((data[i] - '0')  * pow(10, 9 - i));
	}
}

// !!! only for temporal usage !!!
char* tokenToChar(Token userID)
{
	static char PhoneNum[30];
	sprintf(PhoneNum, "%llu", userID);
	return PhoneNum;
}
// !!! only for temporal usage !!!

static char DBpath[200];
void SetDbPath(const char *DBpath_in)
{
	strcpy(DBpath, DBpath_in);
}


char* tokenToCharDB(Token userID)
{
	static char PhoneNum[200];
	sprintf(PhoneNum, "%s\\%llu.db", DBpath, userID);
	return PhoneNum;
}

void setPhoneNo(Token* phoneNo, char* data)
{
	char* it = (char*)phoneNo;
	it[0] = data[0];
	it[1] = data[1];
	it[2] = data[2];
	it[3] = data[3];
	it[4] = data[4];
	it[5] = data[5];
	it[6] = data[6];
	it[7] = data[7];
}

void setUserID(Token* userID, char* data)
{
	char* it = (char*)userID;

	it[0] = data[0];
	it[1] = data[1];
	it[2] = data[2];
	it[3] = data[3];
	it[4] = data[4];
	it[5] = data[5];
	it[6] = data[6];
	it[7] = data[7];
}

int s3InitClientList(s3ClientList* destination)
{
	destination->IDcount = 0;
	destination->Size = 0;
	destination->Capacity = 2;
	destination->data = (s3ClientProperty*)malloc(sizeof(s3ClientProperty)* destination->Capacity);
	if (destination->data)
		return s3_SUCCESS;
	else
		return s3_FAIL;
}

void s3DestroyClientList(s3ClientList* destination)
{
	if (destination->data)
	{
		free(destination->data);

		destination->data = NULL;
	}

}

int clearClientByIndices(s3ClientList* destination, int index)
{
	if (index < 0 || index >= destination->Size)
		return s3_FAIL;

	destination->data[index].socket = NULL;
	return s3_OK;
}

int removeClientByIndices(s3ClientList* destination, int index)
{
	if (index < 0 || index >= (destination->Size - 1))
	{
		// if index == last element of array
		if (index == destination->Size - 1)
		{
			destination->Size--;
			return s3_OK;
		}
		else
			return s3_NOT_FOUND;
	}


	int i = index;

	while (i < destination->Size - 1)
	{
		destination->data[i] = destination->data[i + 1];
	}
	destination->Size--;

	return s3_OK;
}

int eraseClientByIndices(s3ClientList* destination, int index)
{
	if (index < 0 || index >= destination->Size)
		return s3_FAIL;

	if (destination->data[index].socket)
	{
		printf("X >USER:%d removed from server\n" , index);
		closesocket(destination->data[index].socket);
	}
	destination->data[index].socket = NULL;
	return s3_OK;
}

Token findIDByPhoneNum(s3ClientList* clist, Token phoneNum) //unsigned long phoneNum
{
	int i = 0;
	while ((i < clist->Size) && (clist->data[i].phoneNo != phoneNum))
	{
		i++;
	}
	if (i < clist->Size)
	{
		return clist->data[i].userID;
	}
	else
		return s3_NOT_FOUND;

}

int findIndexByPhoneNum(s3ClientList* clist, Token phoneNum)
{
	int i = 0;
	while ((i < clist->Size) && (clist->data[i].phoneNo != phoneNum))
	{
		i++;
	}
	if (i < clist->Size)
	{
		return i;
	}
	else
		return s3_NOT_FOUND;
}

int addClient(s3ClientList* clist, SOCKET s, Token phoneNum , s3Flag* flag)
{
	int index = findIndexByPhoneNum(clist, phoneNum);
	if (index < 0)
	{
		if (clist->Size >= clist->Capacity)
		{
			clist->Capacity = (int)(clist->Capacity * 1.5f);
			clist->data = (s3ClientProperty*)realloc(clist->data, sizeof(s3ClientProperty)*clist->Capacity);
			if (!clist->data) { return s3_FAIL; }
		}


		// set timeout value for new socket
			//DWORD timeout = 100;
			//setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

		clist->Size++;
		clist->data[clist->Size - 1].socket = s;
		clist->data[clist->Size - 1].phoneNo = phoneNum;
		clist->data[clist->Size - 1].userID = clist->IDcount;
		clist->data[clist->Size - 1].msgCounter = 0;
		clist->IDcount++;

		*flag  = s3_OK;
		return clist->Size - 1;
	}
	else
	{
		if (clist->data[index].socket == NULL)
		{
			clist->data[index].socket = s;
			//		DWORD timeout = 1000 * 2;
			//		setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
			*flag = s3_RESIGNED;
			return index;
		}
		else
		{
			*flag = s3_ALREADY_REGISTERED;
			return -99;
		}
	}
}


int s3SendTime(SOCKET s, time_t t)
{
	return send(s, (char*)&t, sizeof(time_t), 0);
}
s3Flag s3RecvTime(SOCKET s, time_t* t)
{

	int rec = recv(s, (char*)t, sizeof(time_t), 0);

	if (rec > 0)
	{
		if (rec == sizeof(Token))
		{
			return s3_SUCCESS;
		}
		else
		{
			return s3_FALSE;
		}
	}
	else
		return s3_FALSE;
}


int s3SendMsg(SOCKET s, int flag)
{
	return send(s, (char*)&flag, sizeof(s3Flag), 0);
}

s3Flag s3RecvMsg(SOCKET s, s3Flag* flagBuffer)
{
	int rec = recv(s, (char*)flagBuffer, sizeof(s3Flag), 0);

	if (rec > 0)
		if (rec == sizeof(s3Flag))
		{
			return s3_SUCCESS;
		}
		else
		{
			return s3_FAIL;
		}
	else
		return s3_FAIL;
}


s3Flag s3RecvToken(SOCKET s, Token* data)
{

	int rec = recv(s, (char*)data, sizeof(Token), 0);

	if (rec > 0)
	{
		if (rec == sizeof(Token))
		{
			return s3_SUCCESS;
		}
		else
		{
			return s3_FALSE;
		}
	}
	else
		return s3_FALSE;
}

int s3SendToken(SOCKET s, Token data)
{
	return send(s, (char*)&data, sizeof(Token), 0);
}


s3Flag s3AddQueneItem(s3ClientList * clist, Token targetID, Token senderPhone,time_t t , char* s3Message, int s3MessageLen)
{

	FILE *fa = fopen(tokenToCharDB(targetID), "ab");
	fwrite(&senderPhone, sizeof(Token), 1, fa);
	fwrite(&t, sizeof(time_t), 1, fa);
	fwrite(&s3MessageLen, sizeof(int), 1, fa);
	fwrite(s3Message, s3MessageLen, 1, fa);


	clist->data[targetID].msgCounter++;

	fclose(fa);
	return s3_SUCCESS;
}


s3Flag s3ProcessMessages( s3ClientProperty* CP)
{
	static char buffer[2048];
	if (!CP->msgCounter)
		return s3_FALSE;

	int i;
	Token targetID = CP->userID;

	char* fileName = tokenToCharDB(targetID);
	FILE * fi = fopen(fileName, "rb");


	    s3SendMsg(CP->socket, s3_INCOMING_MSG);

		for (i = 0; i < CP->msgCounter; i++)
		{
				Token senderPhone;
				fread(&senderPhone, sizeof(Token), 1, fi);

				time_t t;
				fread(&t, sizeof(time_t), 1, fi);

				int bufferSize;
				fread(&bufferSize, sizeof(int), 1, fi);

				fread(buffer, bufferSize, 1, fi);

				s3Flag  msg;
				

				s3RecvMsg(CP->socket, &msg);//s3_ACCEPT

				s3SendToken(CP->socket, senderPhone);

				s3RecvMsg(CP->socket, &msg);// achk

				s3SendTime(CP->socket, t);

				s3RecvMsg(CP->socket, &msg);// achk

				s3SendBuffer(CP->socket, buffer, bufferSize);

				s3RecvMsg(CP->socket, &msg);// achk

				if(i+1 < CP->msgCounter)
				s3SendMsg(CP->socket, s3_CONTINUE);
				else
				s3SendMsg(CP->socket, s3_DONE);



		}

		CP->msgCounter = 0;

	fclose(fi);
	// reset file
	fi = fopen(fileName, "w");
	fclose(fi);

	return s3_SUCCESS;
}


int s3ProcessConnection(s3ClientList * clist,Token index, s3Flag flag)
{
	static s3Flag result;
	static s3Flag msg;
	s3ClientProperty CP = clist->data[index];


	s3SendMsg(CP.socket, s3_OK);
	switch (flag)
	{
	case s3_REQ_USERID_OWN:
	{
		puts("s3_REQ_USERID_OWN");
		s3SendToken(CP.socket, CP.userID);
	}
	break;


	case s3_REQ_USERID_wPHONENO:
	{
		puts("s3_REQ_USERID_wPHONENO");
		Token no;
		result = s3RecvToken(CP.socket, &no);
		if (!result) {
			puts("#ERROR on receive token");
			eraseClientByIndices(clist, index);
		}
		int id = findIndexByPhoneNum(clist, no);
		if (id != s3_NOT_FOUND)
		{
			result = (s3Flag)s3SendMsg(CP.socket, s3_FOUND);
			if (!result) {
				puts(" #ERROR sending FOUND msg");
				eraseClientByIndices(clist, index);
			}
			result = s3RecvMsg(CP.socket, &result);
			if (!result) {
				puts(" #ERROR receiving FOUND msg");
				eraseClientByIndices(clist, index);
			}
			if (result == s3_OK)
				result = (s3Flag)s3SendToken(CP.socket, clist->data[id].userID);
			if (!result) {
				puts(" #ERROR sending final token");
				eraseClientByIndices(clist, index);
			}
		}
		else
		{
			puts(" sending s3_NOT_FOUND");
			s3SendMsg(CP.socket, s3_NOT_FOUND);

		}
	}
	break;

	case s3_SEND_MSG_wPHONENO:
		//************************************************
		//
		// Currently not supported / incompleted feature
		//
		//************************************************
		puts("s3_SEND_MSG_wPHONENO");
		{
			Token no;
			s3RecvToken(CP.socket, &no);
			int id = findIndexByPhoneNum(clist, no);
			if (id != s3_NOT_FOUND)
			{
				if (clist->data[id].socket != NULL)
				{
					// if user found
					// send achnowledge to peer

					// send data to peer
					s3SendMsg(clist->data[id].socket, s3_INCOMING_MSG);
					result = s3RecvMsg(clist->data[id].socket, &msg);
					if((msg == s3_OK) && (result > 0))
					{
						s3SendMsg(CP.socket, s3_FOUND);

						//receive data from requester
						s3RecvBuffer(CP.socket);



						printf("< %d \n", msg);

						s3SendToken(clist->data[id].socket, CP.phoneNo);
						result = s3RecvMsg(clist->data[id].socket, &msg);


						if (s3SendBufferTx(clist->data[id].socket))
						{
							//send second status to peer
							s3SendMsg(CP.socket, s3_SUCCESS);
						}
						else
						{
							//send second status to peer
							s3SendMsg(CP.socket, s3_FAIL);
						}
					}
					else
					{
						s3SendMsg(CP.socket, s3_OVERRIDE_ERROR);
					}

				}
				else
				{
					s3SendMsg(CP.socket, s3_USER_OFFLINE);
				}

				s3RecvBuffer(CP.socket);
				if (result == s3_OK)
					s3SendToken(CP.socket, clist->data[id].userID);
			}
			else
			{
				s3SendMsg(CP.socket, s3_USER_NOT_FOUND);
			}
			//to do some methods
		}
		break;
	case s3_SEND_MSG_wUSERID:
		puts("s3_SEND_MSG_wUSERID");
		{
			Token id;
			s3RecvToken(CP.socket, &id);
			printf("Peer try to connect user: >>>>>>>>>>>  %llu  \n", id);
			if (id == CP.userID)
			{
				s3SendMsg(CP.socket, s3_USER_ID_INVALID);
			}
			else
				if ((id >= 0) && (id < clist->Size))
				{


						puts("writing to database");
						s3SendMsg(CP.socket, s3_FOUND); //send acknowledge

						s3RecvBuffer(CP.socket); // recv str buffer
						
						s3AddQueneItem(clist, id, CP.phoneNo, time(NULL), s3GetRxBuffer(),  s3GetRxBufferLen());

						s3SendMsg(CP.socket, s3_SUCCESS);//send acknowledge
					
				}
				else
				{
					s3SendMsg(CP.socket, s3_USER_ID_INVALID);
				}

		}break;

	case s3_EXIT_FROM_SERVER:
		puts("s3_EXIT_FROM_SERVER");
		printf("user disconnected successfully id: %llu ,phoneNo: %llu \n", CP.userID, CP.phoneNo);
		closesocket(clist->data[index].socket);
		eraseClientByIndices(clist, index);
		break;
	default:
		puts("NOT_DEFINED");
		printf("Msg Code: '%d' \n", flag);
		break;
	}

	puts("----------------------------->process end");

	return s3_OK;
}

s3Flag s3StoreServerData(s3ClientList * clist, char* storageFileName)
{
	//FILE * fo = fopen_s(storageFileName, "wb");


	return s3_SUCCESS;
}

s3Flag s3Inits3MessageQuene(s3s3MessageQuene* msgQuene)
{
	msgQuene->Capacity = 2;
	msgQuene->Size = 0;
	msgQuene->msgData = (s3s3MessageData*)malloc(msgQuene->Capacity * sizeof(s3s3MessageData));


	// be sure that count
	msgQuene->msgData[0].Count = 0;
	msgQuene->msgData[1].Count = 0;
	return s3_SUCCESS;
}
