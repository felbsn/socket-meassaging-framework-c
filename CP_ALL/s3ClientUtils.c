#include "s3ClientUtils.h"



int BinarySearchPhone(s3Contact arr[], int l, int r, Token phoneNum)
{
	if (r >= l)
	{
		int mid = l + (r - l) / 2;

		// If the element is present at the middle itself
		if (arr[mid].phoneNo == phoneNum)  return mid;

		// If element is smaller than mid, then it can only be present
		// in left subarray
		if (arr[mid].phoneNo > phoneNum) return BinarySearchPhone(arr, l, mid - 1, phoneNum);

		// Else the element can only be present in right subarray
		return BinarySearchPhone(arr, mid + 1, r, phoneNum);
	}

	// We reach here when element is not present in array
	return -1;
}

int s3FindContactIndexByPhone(s3ContactList* contactList, Token phoneNo)
{
	int i = 0;
	while (i < contactList->Size && contactList->data[i].phoneNo != phoneNo)
	{
		i++;
	}
	if (i < contactList->Size)
	{

		return i;
	}
	else
	{
		return -1;
	}

}

s3Contact NewContactProtoype(const char* info, Token phoneNo)
{
	s3Contact contact;
	contact.id = -1;
	contact.messageCount = 1;
	contact.ClientIndex = 0;
	int infoLen = strlen(info);
	strncpy(contact.info, info, infoLen);

	return contact;
}

s3Flag s3InitContactList(s3ContactList* contactList)
{
	contactList->Capacity = 2;
	contactList->Size = 0;
	contactList->ContactCount = 0;
	contactList->data = (s3Contact*)calloc(contactList->Capacity, sizeof(s3Contact));

	if (contactList->data)
		return s3_SUCCESS;
	else
		return s3_FAIL;
}


// insertion adder
int s3AddContactEx(s3ContactList* contactList, char* infoStr, Token phoneNo, Token id)
{
	if (contactList->Size >= contactList->Capacity)
	{
		contactList->Capacity += 10;
		contactList->data = (s3Contact*)realloc(contactList->data, contactList->Capacity * sizeof(s3Contact));
		if (!contactList->data) { return s3_FAIL; }
		int j = 0;
		for (j = contactList->Capacity - 1; j <= contactList->Capacity - 10; j++)
		{
			contactList->data[j].ClientIndex = -1;
		}
	}

	// perform an insertion algorithm
	int i = 0;

	while (i < contactList->Size && phoneNo > contactList->data[i].phoneNo)
	{
		i++;
	}

	int index = i;

	if (i < contactList->Size)
	{
		while (i >= index)
		{
			contactList->data[i + 1] = contactList->data[i];
			i--;
		}
	}




	contactList->Size++;




	contactList->data[index].id = id;
	contactList->data[index].messageCount = 0;
	contactList->data[index].phoneNo = phoneNo;
	strcpy(contactList->data[index].info, infoStr);

	i = 0;
	while (contactList->data[i].ClientIndex >= 0)
	{
		i++;
	}
	contactList->data[index].ClientIndex = i;

	contactList->ContactCount++;


	return  index;
}

// last element adder
int s3AddContactRx(s3ContactList* contactList, char* infoStr, Token phoneNo, Token id)
{
	if (contactList->Size >= contactList->Capacity)
	{
		contactList->Capacity += 10;
		contactList->data = (s3Contact*)realloc(contactList->data, contactList->Capacity * sizeof(s3Contact));
		if (!contactList->data) { return s3_ERROR; }
	}

	// add to last

	contactList->data[contactList->Size].id = id;
	contactList->data[contactList->Size].messageCount = 0;
	contactList->data[contactList->Size].phoneNo = phoneNo;
	strcpy(contactList->data[contactList->Size].info, infoStr);
	contactList->Size++;


	return  (contactList->Size - 1);
}

int s3AddContact(SOCKET serverSocket, s3ContactList* contactList, char* infoStr, Token phoneNo)
{
	if (serverSocket)
	{
		s3Flag result;
		Token  id;
		s3SendMsg(serverSocket, s3_REQ_USERID_wPHONENO);
		s3RecvMsg(serverSocket, &result);

		s3SendToken(serverSocket, phoneNo);

		if (s3RecvMsg(serverSocket, &result) == s3_SUCCESS)
		{
			if (result == s3_FOUND)
			{
				s3SendMsg(serverSocket, s3_OK);
				s3RecvToken(serverSocket, &id);

				return s3AddContactRx(contactList, infoStr, phoneNo, id);
			}
			else
				if (result == s3_NOT_FOUND)
				{
					return s3_USER_NOT_FOUND;
				}
		}

	}
	else
	{
		return s3_ERROR;
	}
}

s3Flag s3SendMessage(SOCKET serverSocket, s3Contact* contact, int bufferChannel)
{
	s3Flag msg;
	int res;
	Token id = contact->id;
	res = s3SendMsg(serverSocket, s3_SEND_MSG_wUSERID);
	if (!res) return s3_TIMEOUT;

	res = s3RecvMsg(serverSocket, &msg);
	if (!res) return s3_TIMEOUT;
	if (msg == s3_FAIL) return s3_FAIL;

	res = s3SendToken(serverSocket, id);
	if (!res) return s3_TIMEOUT;

	res = s3RecvMsg(serverSocket, &msg);
	if (!res) return s3_TIMEOUT;
	if (msg == s3_FAIL) return s3_FAIL;

	switch (msg)
	{
	case s3_NOT_FOUND:
		//puts("user not found!");
		break;
	case s3_FOUND:
		//puts("sending msg!");
		res = s3SendBufferChannel(serverSocket, bufferChannel);
		if (!res) return s3_TIMEOUT;
		res = s3RecvMsg(serverSocket, &msg);
		if (!res) return s3_TIMEOUT;
		if (msg == s3_SUCCESS)
		{
			//	puts("succesfull send");
		}
		break;
	case s3_USER_OFFLINE:
		//puts("user offline !");
		break;
	case s3_USER_NOT_FOUND:
		//puts("user not found!");
		break;
	case s3_USER_ID_INVALID:
		//puts("user id invalid!");
		break;
	case s3_FAIL:
		// s3 fail
		break;
	default:
		break;
	}

	return msg;
}