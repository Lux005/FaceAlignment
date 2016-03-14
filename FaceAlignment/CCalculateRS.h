#ifndef CALCULATE_RS_H
#define CALCULATE_RS_H
#include <omp.h>
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

	static Face calc(const Face& f1, const Face& f2, double Rotaion_Step = 1.0, int Rotation_Range = 45, double Scale_Step = 0.05, double Scale_Range = 0.2)
	{
		if (f1.size() == 0 || f1.size() != f2.size())
			Error("Empty faceLandmark or sizes of two landmarks didn't match!");
		cout << "Rotation Step Size:" << Rotaion_Step << " Rotation Range: +-" << Rotation_Range << "  Scale Step Size: " << Scale_Step << "  Scale Range: +-" << Scale_Range * 100 << "%" << endl;
		const Face modify = f1;
		const Face target = f2;
		Face bestFace = modify;
		int rec = 0;
		static const int rangeI = Rotation_Range * 10;
#pragma omp parallel for
		for (int ri = -rangeI; ri <= rangeI; ri += Rotaion_Step * 10)
		{
			double r = ri;
			r /= 10;
			for (double sx = 1 - Scale_Range; sx <= 1 + Scale_Range; sx += Scale_Step)
			{

				for (double sy = 1 - Scale_Range; sy <= 1 + Scale_Range; sy += Scale_Step)
				{
					//pgTimer;
					Face temp = modify;
					temp.transform(r, Face::lmPoint(sx, sy));
					temp.calcDiff(target);
					bestFace = (bestFace.diff <= temp.diff) ? bestFace : temp;
					//pgTimer;
				}

			}
//#pragma omp critical
//			{
//				rec++;
//				cout << "\rprogress: " << rec << "/" << Rotation_Range * 2 / Rotaion_Step + 1 << std::flush;
//			}

		}
		cout << endl << "Best: " << bestFace.theta << " " << bestFace.scale << " " << bestFace.diff << endl;;
		return bestFace;
	}



	static Face calc2(const Face& f1, const Face& f2, double Rotaion_Step = 1.0, int Rotation_Range = 45, double Scale_Step = 0.02, double Scale_Range = 0.2, bool recu = false)
	{
		static double rotationStep = 1;
		const Face modify = f1;
		const Face target = f2;
		Face bestFace = modify;
		for (int r = -Rotation_Range; r <= Rotation_Range; r += Rotation_Range / 5)
		{
			for (double sx = 1 - Scale_Range; sx <= 1 + Scale_Range; sx += Scale_Range / 5)
			{
				for (double sy = 1 - Scale_Range; sy <= 1 + Scale_Range; sy += Scale_Range / 5)
				{

					Face temp = modify;
					temp.transform(r, Face::lmPoint(sx, sy));
					temp.calcDiff(target);
					bestFace = (bestFace.diff <= temp.diff) ? bestFace : temp;
				}
			}
		}
		cout << endl << "Best: " << bestFace.theta << " " << bestFace.scale << " " << bestFace.diff << endl;
		return bestFace;
	}
private:



};
#endif