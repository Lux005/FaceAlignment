#ifndef CFACE_LANDMARKER_H
#define CFACE_LANDMARKER_H
#include "common.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <math.h>
#include "fstream"
using namespace std;
using namespace cv;
class CFace
{
public:
#define PI 3.14159265
#define degToRad(x) {x=x*PI/180;}
	typedef Point2f lmPoint;
	typedef vector<lmPoint> Landmarkers;
	typedef Landmarkers::iterator lmiter;
	typedef Landmarkers::const_iterator lmiterc;
	Landmarkers lmarkers;
	lmPoint center;
	double theta;
	double scale;
	double diff;
	CFace() :center(lmPoint(0, 0)), theta(0.0), scale(0), diff(9999999.9){}
	~CFace(){};
	int size(){ return lmarkers.size(); }
	int size()const{ return lmarkers.size(); }
	void normalize_to_center()
	{
		lmPoint total;
		for (lmiter i = lmarkers.begin(); i != lmarkers.end(); i++)
			total += *i;
		center = avg(total, lmarkers.size());
		for (lmiter i = lmarkers.begin(); i != lmarkers.end(); i++)
			*i = *i - center;
	}
	double calcDiff(const CFace& f)
	{
		if (f.size() == 0 || this->size() != f.size())
			Error("Empty faceLandmark or sizes of two landmarks didn't match!");
		double _diff = 0;
		lmiterc iter1 = this->lmarkers.begin(), iter2 = f.lmarkers.begin();
		while (iter1 != this->lmarkers.end() && iter2 != f.lmarkers.end())
		{
			_diff += (iter1->x - iter2->x)*(iter1->x - iter2->x);
			_diff += (iter1->y - iter2->y)*(iter1->y - iter2->y);
			iter1++;
			iter2++;
		}
		this->diff = _diff;
		return _diff;
	}
	void transform(const double _theta, const double _scale=1.0)
	{
		if (_theta<0 || _theta>360)
			Error("Invalid degree for rotation");
		if (_scale<0.1 || _scale>10)
			Error("Invalid param for scaling")
		theta = _theta;
		scale = _scale;
		double rad = _theta;
		degToRad(rad);
		Mat tMatC = (Mat_<double>(3, 3) <<
			1, 0, -1,
			0, 1, -1,
			0, 0, 1);
		Mat tMatB = (Mat_<double>(3, 3) <<
			1, 0, 1,
			0, 1, 1,
			0, 0, 1);
		Mat rMat = (Mat_<double>(3, 3) <<
			cos(rad), sin(rad), 0,
			-sin(rad), cos(rad), 0,
			0, 0, 1);
		Mat sMat = (Mat_<double>(3, 3) <<
			_scale, 0, 0,
			0, _scale, 0,
			0, 0, 1);
		Mat	pMat = Mat_<double>(3,lmarkers.size());
		
		for (lmiter i = lmarkers.begin(); i != lmarkers.end(); i++)
		{
			int x = i - lmarkers.begin();
			pMat.at<double>(0,x) = i->x;
			pMat.at<double>(1,x) = i->y;
			pMat.at<double>(2,x) = 1;
		}
		pMat = rMat*sMat*pMat;
	 for (lmiter i = lmarkers.begin(); i != lmarkers.end(); i++)
	 {
		 int x = i - lmarkers.begin();
		 i->x = pMat.at<double>(0, x);
		 i->y= pMat.at<double>(1, x);
	 }
	}
	void loadPoints(string fileName)
	{
		lmarkers.clear();
		FILE* lf;
		fopen_s(&lf, fileName.c_str(), "r");
		Point2f tmp;
		if (lf == NULL)return;
		int id;
		 fscanf_s(lf, "%d\n", &id);
		while (!feof(lf)){
			double x, y;
			int re;
			re = fscanf_s(lf, "%lf %lf \n", &x, &y);
			if (re == 2)
			{
				tmp.x = x;
				tmp.y = y;
				lmarkers.push_back(tmp);
			}
		}
		fclose(lf);
		cout << "done " << endl;
	}
	void drawFace(string windowName)
	{
		Mat x(1000, 1000, CV_8UC3,Scalar(255,255,255));
		Point t;
		circle(x, Point(500,500), 10, Scalar(255, 0, 255), -1);
		for (lmiter i = lmarkers.begin(); i != lmarkers.end(); i++)
		{
			t.x = i->x * 2000 + 500;
			t.y = i->y * 2000 + 500;
			circle(x, t, 1, Scalar(0, 0, 255), -1);
			putText(x, to_string(i - lmarkers.begin() + 1), t, 1, 1.0, Scalar(0, 0, 0), 1, 8, false);
		}
		cv::namedWindow(windowName);
		imshow(windowName, x);
		waitKey(100);

	}
private:
	lmPoint avg(lmPoint& p1, int n){
		return lmPoint(p1.x / n, p1.y / n);
	}

};
#endif