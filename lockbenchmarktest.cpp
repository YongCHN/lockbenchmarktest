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
	int ElapseTime(){
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
//template<typename T>
class Lock
{
public:
	void lock_shared(){}
	void lock_exclusive(){}
	void release_shared(){}
	void release_exclusive(){}

private:
	//T _lock;
};
void  writer(void * arg);
void  reader(void * arg);

typedef void(*threadentry_t)(void*);

void spoon(threadentry_t function, int count);
void close();
HANDLE * g_handles;
int g_number;
const static int elementToProcess = 100000 * 100;
Lock g_lock;

queue<int> g_container;
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

	spoon(writer, writerCount);
	
	spoon(reader, readercount);

	::WaitForMultipleObjects(readercount + writerCount, g_handles, TRUE, INFINITE);

	
	return 0;
}

void  writer(void * arg)
{
	Timer timer;
	timer.Start();
	for (int i = 0; i < elementToProcess; i++)
	{
		g_lock.lock_shared();
		g_container.push(i);
		g_lock.release_shared();
	}
	timer.Stop();
	printf("use %d miniseconds in writer", timer.ElapseTime());
}

void  reader(void * arg)
{
	Timer timer;
	timer.Start();

	for (int i = 0; i < elementToProcess; i++)
	{
		g_lock.lock_shared();
		g_container.front();
		g_lock.release_shared();		
	}
	timer.Stop();
	printf("use %d miniseconds in reader", timer.ElapseTime());
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