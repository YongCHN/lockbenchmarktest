// lockbenchmarktest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include "process.h"

void  writer(void * arg);
void  reader(void * arg);

typedef void(*threadentry_t)(void*);

void spoon(threadentry_t function, int count);
void close();
HANDLE * g_handles;
int g_number;
int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		printf("please give a reader number and writer number");
		return 1;
	}

	int readercount = atoi(argv[1]);

	int writerCount = atoi(argv[2]);

	g_handles = new HANDLE[readercount + writerCount];

	spoon(writer, writerCount);
	
	spoon(reader, readercount);

	::WaitForMultipleObjects(readercount + writerCount, g_handles, TRUE, INFINITE);

	
	return 0;
}

void  writer(void * arg)
{
	printf("writer\n");
	::Sleep(1000);
}

void  reader(void * arg)
{
	printf("reader\n");
	::Sleep(1000);
}

void spoon(threadentry_t function, int count)
{
	for (int i = 0; i < count; i++)
	{
		g_handles[g_number++] = (HANDLE)_beginthread(function, 0, nullptr);
	}
}

void close()
{
	for (int i = 0; i < g_number; i++)
		::CloseHandle(g_handles[i]);
}