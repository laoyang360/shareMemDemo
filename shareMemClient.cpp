// shareMemClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "shareMem.h"

int main()
{
	cout << "shareMemClient is now running!" << endl;
	shareMemory sm("boost", false);
	void * bs = sm.shm_open(NULL,1000,FILE_MAP_READ);
	if(bs == NULL)
	{   cout << "null";
		return -1;
	}
	int b[10] = {0};
	sm.shm_read(bs,b,10*4);

	cout << endl;
	getchar();

	return 0;
}