#ifndef CALCULATE_RS_H
#define CALCULATE_RS_H
#include "common.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "CFace.h"
using namespace cv;
using namespace std;
class CcalculateRS
{

typedef CFace Face;
public:
	
	CcalculateRS(){};
	CcalculateRS(const CcalculateRS&){}
	CcalculateRS& operator=(const CcalculateRS){}
	~CcalculateRS(){};

	static void calc(Face& f1, Face& f2, double Rotaion_Step = 0.5, double Scale_Step = 0.01, double Scale_Range=0.5)
	{

		if (f1.size() == 0 || f1.size() != f2.size())
			Error("Empty faceLandmark or sizes of two landmarks didn't match!");
		Face faces[2];
		faces[0] = f1;
		faces[1] = f2;
		faces[0].normalize_to_center();
		faces[1].normalize_to_center();
		const Face target = faces[1];
		Face bestFace = faces[0];
		for (double r = 80; r <= 100; r += Rotaion_Step)
		{
			for (double s = 1 - Scale_Range; s <= 1 + Scale_Range; s += Scale_Step)
			{
				Face temp = faces[0];
				temp.transform(r, s);
				temp.calcDiff(target);
				bestFace = (bestFace.diff <= temp.diff) ? bestFace : temp;
			}
		}
		cout << "Best: " << bestFace.theta << " " << bestFace.scale << " " << bestFace.diff << endl;;
	}
private:
	
	

};
#endif