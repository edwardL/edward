#ifndef TIMER_H
#define TIMER_H

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


#include<time.h>
class Timer
{
private:
	clock_t mZeroClock;
	DWORD mStartTick;
	LONGLONG mLastTime;
	LARGE_INTEGER 

};



#else


#endif


#endif