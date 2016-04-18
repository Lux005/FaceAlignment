//Face Alignment by Explicit Shape Regression C++ Implementation
//Lu, Xinzhong @ CPS.udayton.edu
//luxinzhong@msn.com
//Professor Dr. Shen,Ju
//Jan, 2016
#ifndef _FA_INITIALIZER_H_
#define _FA_INITIALIZER_H_
#include "fa.h"
class fa::Initializer
{
public:
	static void loadShapes();
	static void loadImages();
	static void setMeanShape();
};
#endif