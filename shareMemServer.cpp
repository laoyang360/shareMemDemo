// shareMemServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "shareMem.h"


int main()
{
	cout << "The Server is now running!" << endl;

	shareMemory sm("boost", false);
	void * bs = sm.shm_open(NULL, 1000*4, FILE_MAP_WRITE);
	if(bs==NULL)
	{
		return -1;
	}
	int a[10] = {0};
	for(int i=0; i<10; ++i)
	{
		a[i] = i;
	}
	sm.shm_write(bs,a,10*sizeof(int));


	Sleep(100000);
}
