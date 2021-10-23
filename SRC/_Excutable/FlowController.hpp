#pragma once
#include <chrono>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <functional>
#include <sys/time.h>

class FlowThread : public std::thread
{
public:
	FlowThread(std::function<void()> thread);
	FlowThread(std::function<void()> thread, int CPUID);
	FlowThread(std::function<void()> thread, int CPUID, float ClockingHZ);
	FlowThread(std::function<void()> thread, float ClockingHZ);

	void FlowTerminal();
	void FlowTryStop();
	void FlowStopAndWait();
	void FlowWait() { this->join(); };

	void SetFlowError() { FlowLoopError = 0; };
	int GetFlowError() { return FlowLoopError; };
	int GetFlowTimeAva() { return ThreadSpendAva; };

private:
	uint64_t GetTimeStamp()
	{
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return tv.tv_sec * (uint64_t)1000000 + tv.tv_usec;
	}

	uint64_t Time__Max;
	uint64_t TimeStart;
	uint64_t Time__End;
	uint64_t Time_Next;
	uint64_t ThreadSpend = 0;
	float ThreadSpendAva = 0;
	float ThreadSpendAvaTotal = 0;
	float ThreadSpendAVT[5] = {0};
	int FlowLoopError = 0;
	bool IsThreadRunning = false;
	std::function<void()> func = [] {};
};

FlowThread::FlowThread(std::function<void()> thread)
	: std::thread(
		  [&] {
			  usleep(50000);
			  int AvaCount = 0;
			  IsThreadRunning = true;
			  while (IsThreadRunning)
			  {
				  TimeStart = GetTimeStamp();
				  Time_Next = TimeStart - Time__End;
				  //
				  func();
				  //
				  Time__End = GetTimeStamp();
				  ThreadSpend = Time__End - TimeStart;
				  //
				  ThreadSpendAvaTotal -= ThreadSpendAVT[AvaCount];
				  ThreadSpendAVT[AvaCount] = ThreadSpend;
				  ThreadSpendAvaTotal += ThreadSpendAVT[AvaCount];
				  AvaCount++;
				  if (AvaCount >= 5)
				  {
					  ThreadSpendAva = ThreadSpendAvaTotal / 5.f;
					  AvaCount = 0;
				  }
				  // ...
				  //
				  Time__End = GetTimeStamp();
			  }
		  })
{
	func = thread;
	usleep(50000);
};

FlowThread::FlowThread(std::function<void()> thread, int CPUID)
	: std::thread(
		  [&] {
			  usleep(50000);
			  int AvaCount = 0;
			  IsThreadRunning = true;
			  while (IsThreadRunning)
			  {
				  TimeStart = GetTimeStamp();
				  Time_Next = TimeStart - Time__End;
				  //
				  func();
				  //
				  Time__End = GetTimeStamp();
				  ThreadSpend = Time__End - TimeStart;
				  //
				  ThreadSpendAvaTotal -= ThreadSpendAVT[AvaCount];
				  ThreadSpendAVT[AvaCount] = ThreadSpend;
				  ThreadSpendAvaTotal += ThreadSpendAVT[AvaCount];
				  AvaCount++;
				  if (AvaCount >= 5)
				  {
					  ThreadSpendAva = ThreadSpendAvaTotal / 5.f;
					  AvaCount = 0;
				  }
				  // ...
				  //
				  Time__End = GetTimeStamp();
			  }
		  })
{
	func = thread;
	usleep(50000);
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(CPUID, &cpuset);
	int rc = pthread_setaffinity_np(this->native_handle(), sizeof(cpu_set_t), &cpuset);
	usleep(50000);
};

FlowThread::FlowThread(std::function<void()> thread, int CPUID, float ClockingHZ)
	: std::thread(
		  [&] {
			  usleep(50000);
			  int AvaCount = 0;
			  IsThreadRunning = true;
			  while (IsThreadRunning)
			  {
				  TimeStart = GetTimeStamp();
				  Time_Next = TimeStart - Time__End;
				  //
				  func();
				  //
				  Time__End = GetTimeStamp();
				  ThreadSpend = Time__End - TimeStart;
				  //
				  if (ThreadSpend + Time_Next > Time__Max | Time_Next < 0)
					  FlowLoopError++;
				  else
					  usleep(Time__Max - ThreadSpend - Time_Next);
				  //
				  ThreadSpendAvaTotal -= ThreadSpendAVT[AvaCount];
				  ThreadSpendAVT[AvaCount] = ThreadSpend;
				  ThreadSpendAvaTotal += ThreadSpendAVT[AvaCount];
				  AvaCount++;
				  if (AvaCount >= 5)
				  {
					  ThreadSpendAva = ThreadSpendAvaTotal / 5.f;
					  AvaCount = 0;
				  }
				  //
				  Time__End = GetTimeStamp();
			  }
		  })
{
	func = thread;
	Time__Max = (int)((1.f / ClockingHZ) * 1000000.f);
	usleep(50000);
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(CPUID, &cpuset);
	int rc = pthread_setaffinity_np(this->native_handle(), sizeof(cpu_set_t), &cpuset);
	usleep(50000);
};

FlowThread::FlowThread(std::function<void()> thread, float ClockingHZ)
	: std::thread(
		  [&] {
			  usleep(50000);
			  int AvaCount = 0;
			  IsThreadRunning = true;
			  while (IsThreadRunning)
			  {
				  TimeStart = GetTimeStamp();
				  Time_Next = TimeStart - Time__End;
				  //
				  func();
				  //
				  Time__End = GetTimeStamp();
				  ThreadSpend = Time__End - TimeStart;
				  //
				  if (ThreadSpend + Time_Next > Time__Max | Time_Next < 0)
					  FlowLoopError++;
				  else
					  usleep(Time__Max - ThreadSpend - Time_Next);
				  //
				  ThreadSpendAvaTotal -= ThreadSpendAVT[AvaCount];
				  ThreadSpendAVT[AvaCount] = ThreadSpend;
				  ThreadSpendAvaTotal += ThreadSpendAVT[AvaCount];
				  AvaCount++;
				  if (AvaCount >= 5)
				  {
					  ThreadSpendAva = ThreadSpendAvaTotal / 5.f;
					  AvaCount = 0;
				  }
				  //
				  Time__End = GetTimeStamp();
			  }
		  })
{
	func = thread;
	usleep(50000);
	Time__Max = (int)((1.f / ClockingHZ) * 1000000.f);
};

void FlowThread::FlowTerminal(){

};

void FlowThread::FlowTryStop()
{
	IsThreadRunning = false;
};

void FlowThread::FlowStopAndWait()
{
	IsThreadRunning = false;
	if (this->joinable())
		this->join();
};