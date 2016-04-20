//common.h
//Lu,Xinzhong @ CPS.udayton.edu
//luxinzhong@msn.com
//Professor Dr. Shen,Ju
//Jan, 2016
#ifndef COMMON_H
#define COMMON_H
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
using std::cin;		using std::cout;
using std::cerr;	using std::endl;
#ifndef MyError(x)
#define MyError(x) {\
	std::cerr << "_ERROR_ " << __FILE__ << " Line:" << __LINE__ << std::endl; \
	std::cerr << "        " << x << std::endl; \
	std::ostringstream  ss; \
	ss << x; \
	throw ss.str(); \
	system("Pause"); \
	exit(-1); \
}
#endif
#ifndef MyWarning(x)
#define MyWarning(x) {\
	std::cerr << "_WARNNING_ " << __FILE__ << " Line:" << __LINE__ << std::endl; \
	std::cerr << "           " << x << std::endl; \
	std::ostringstream  ss; \
	ss << x;\
	throw ss.str(); \
}
#endif
#ifndef MyDebug(x)
static bool MY_DEBUG_INFO = true;
#define MyDebug(x) {\
if (MY_DEBUG_INFO)\
{\
	std::cerr << "_DEBUG_INFO_ " << __FILE__ << " Line:" << __LINE__ << std::endl; \
		std::cerr << "           " << x << std::endl; \
}\
}
#endif
#ifndef MyTimer
static LARGE_INTEGER  pgTimerBegin;
static int pgTimerFreq = 0;
static bool CountTimeStarted = false;
#define MyTimer {\
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