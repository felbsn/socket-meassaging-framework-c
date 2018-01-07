#include "s3Server.h"

#pragma comment(lib, "ws2_32.lib")


int main()
{
	char db[60] = "DataBase";

	char rePadder[60];
	int i;
	for (i = 0; i < 60; i++)
	{
		rePadder[i] = 8;
	}

	char msg[] = "simple server instance?";
	if (s3InitServer(msg, db, 8888) != s3_SUCCESS)
	{
		puts("cant install server");
		system("pause");
		return 1;
	}


	DWORD time = GetTickCount();
	while (1)
	{
		s3RunServer(60);

		int sizeprint = printf("time passed %.4d", (GetTickCount()-time) / 1000);

		for (size_t i = 0; i < sizeprint*2; i++)
		{
			putchar(8);
		}


	}


	system("pause");





	return 0;
}
