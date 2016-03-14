#ifndef COMMON_H
#define COMMON_H
#include <cstdlib>
#include <string>
#include <iostream>
#include <windows.h>

using std::cin;		using std::cout;
using std::cerr;	using std::endl;
#ifndef Error(x)
#define Error(x) {\
	std::cerr << "_ERROR_ " << __FILE__ << " Line:" << __LINE__ << std::endl; \
	std::cerr << "        " << x << std::endl; \
	system("Pause"); \
	exit(-1); \
}
#endif
#ifndef Warnning(x)
#define Warnning(x) {\
	std::cerr << "_WARNNING_ " << __FILE__ << " Line:" << __LINE__ << std::endl; \
	std::cerr << "           " << x << std::endl; \
}
#endif
#ifndef pgTimer
static LARGE_INTEGER  pgTimerBegin;
static int pgTimerFreq = 0;
static bool CountTimeStarted = false;

#define pgTimer {\
if (pgTimerFreq == 0)\
{\
	LARGE_INTEGER  tmp; \
	QueryPerformanceFrequency(&tmp); \
	pgTimerFreq = tmp.QuadPart; \
}\
if (CountTimeStarted)\
{\
	LARGE_INTEGER  pgTimerEnd; \
	QueryPerformanceCounter(&pgTimerEnd); \
	long costTime = (long)((pgTimerEnd.QuadPart - pgTimerBegin.QuadPart) * 1000000 / pgTimerFreq); \
	std::cerr << "_TIMING_END_ " << __FILE__ << " Line:" << __LINE__ << " TIME USED: " << costTime << " us" << std::endl; \
	CountTimeStarted = false; \
}\
else \
{\
	std::cerr << "_TIMING_BEGIN_ " << __FILE__ << " Line:" << __LINE__ << std::endl; \
	QueryPerformanceCounter(&pgTimerBegin); \
	CountTimeStarted = true; \
}\
}
#endif
#endif