// lockbenchmarktest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "windows.h"
#include "process.h"
#include <queue>
using namespace std;

class Timer
{
public:
	Timer() :m_total(0){
		QueryPerformanceFrequency(&m_liPerfFreq);
	}
	void Start();
	void Stop();
	void Pause();
	void Resume();
	long long ElapseTime(){
		return m_total;
	}
private:
	void Sum();
private:
	LARGE_INTEGER m_liPerfFreq;
	LARGE_INTEGER m_liPerfLast;
	LARGE_INTEGER m_liPerfCurrent;

	long long m_total;
};

void Timer::Start()
{
	QueryPerformanceCounter(&m_liPerfLast);
}

void Timer::Stop()
{
	QueryPerformanceCounter(&m_liPerfCurrent);
	Sum();
}

void Timer::Pause(){
	QueryPerformanceCounter(&m_liPerfCurrent);
}

void Timer::Resume()
{
	QueryPerformanceCounter(&m_liPerfLast);
	Sum();
}

void Timer::Sum()
{
	m_total += (((m_liPerfCurrent.QuadPart - m_liPerfLast.QuadPart) * 1000) / m_liPerfFreq.QuadPart);
}
template<typename T>
class Lock
{
public:
	void lock_shared(){ _lock.AcquireReadLock(); }
	void lock_exclusive(){ _lock.AcquireWriteLock(); }
	void release_shared(){ _lock.ReleaseReadLock(); }
	void release_exclusive(){ _lock.ReleaseWriteLock(); }

private:
	T _lock;
};
void  writer(void * arg);
void  reader(void * arg);

typedef void(*threadentry_t)(void*);

void spoon(threadentry_t function, int count);
void close();
HANDLE * g_handles;
int g_number;
const static int elementToProcess = 100000 * 10;

class CLock
{

};
Lock<CLock> g_lock;

queue<int> g_container;
int * g_buffer = new int[1024];

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		printf("please give a reader number and writer number. \n");
		return 1;
	}

	int readercount = atoi(argv[1]);

	int writerCount = atoi(argv[2]);

	g_handles = new HANDLE[readercount + writerCount];

	Timer timer;
	timer.Start();
	spoon(writer, writerCount);
	
	spoon(reader, readercount);

	::WaitForMultipleObjects(readercount + writerCount, g_handles, TRUE, INFINITE);
	timer.Stop();
	printf("totally;%d;\r\n", timer.ElapseTime());
	Close();
	return 0;
}

void  writer(void * arg)
{
	int threadindex = (int)*(int*)arg;
	
	for (int i = 0; i < elementToProcess; i++)
	{
		g_lock.lock_exclusive();
		g_buffer[0] = i;
		g_lock.release_exclusive();
	}
}

void  reader(void * arg)
{
	int threadindex = (int)*(int*)arg;
	
	int localNumber;
	for (int i = 0; i < elementToProcess; i++)
	{
		g_lock.lock_shared();
		localNumber = g_buffer[0];
		g_lock.release_shared();
	}
	localNumber++;
}

void spoon(threadentry_t function, int count)
{
	for (int i = 0; i < count; i++)
	{
		int threadId = g_number;
		g_handles[g_number] = (HANDLE)_beginthread(function, 0, (void*)&threadId);
		g_number++;
	}
}

void close()
{
	for (int i = 0; i < g_number; i++)
		::CloseHandle(g_handles[i]);
}