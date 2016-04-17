//Face Alignment by Explicit Shape Regression C++ Implementation
//Lu, Xinzhong @ CPS.udayton.edu
//luxinzhong@msn.com
//Professor Dr. Shen,Ju
//Jan, 2016
#ifndef _FACE_ALIGNMENT_BASIC_TYPES_H_
#define _FACE_ALIGNMENT_BASIC_TYPES_H_
#include "common.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include <vector>
#include <string>
namespace fa
{
	//---------	using


	using cv::Mat;
	using cv::Mat_;
	using cv::Point2f;
	using cv::Scalar;
	
	using std::vector;
	using std::pair;
	using std::ostream;
	using std::string;








	//---------	tyedef





	//Nfp * 1 
	//Matrix= (x0,y0,x1,y1.....xNfp-1,yNfp-1)T
	//CV_32FC1
	typedef Mat Shape;



	//Height* Width 
	//Matrix
	//CV_32FC1
	typedef Mat Image;




	//N * 2Nfp 
	//Matrix: Regression Targets Y
	//CV_32FC1
	typedef Mat RegressionTargets;




	//N * P 
	//Matrix
	//CV_32FC1
	typedef Mat ShapeIndexedPixels;


	//N * P^2 
	//Matrix: Pairwise Difference Cov(p)
	//CV_32FC1
	typedef Mat PixelPixelCovariance;



	//pair->first  = Image, CV_32FC1, 
	//pair->second = Shape, CV_32FC1
	typedef pair<Image, Shape> ImageAndShape;


	//pair->first->first= Image, 32FC1,
	//pair->first->second= Shape, 32FC1,
	//pair->second= Shape, 32FC1
	typedef pair<ImageAndShape, Shape> ImageShapeAndShape;

	//3 * 3
	//Matrix
	//32FC1
	typedef Mat TransMat2D;


	//---------	class Declarations



	class ExplicitShapeRegression;
	class LearnStageRegressor;
	class ShapeNormalziation;
	class CorrelationBasedFeatureSelection;







	//---------	Structure Declarations



	
	struct LocalCoordinate;
	struct InternalRegressor;
	struct StageRegressor;
	




	//---------	Sturcture Definitions




	//Internal Regressor
	struct InternalRegressor
	{
		//mn[0,F)
		//pair of feature points' indices.
		//pair<int m,int n>
		vector<pair<int, int>> mn; 
		//theta: 1 * F 
		//Matrix, thresholds for F fern features.
		//CV_32FC1
		Mat theta; 
		//yb: 2^F * 2Nfp 
		//Matrix,prediction y of the bin
		//CV_32FC1
		Mat yb; 
	};






	struct StageRegressor
	{
		//Internal Regressor r[0,K)
		vector<InternalRegressor> r;
		//Local Coordinate lc[0,P)
		vector<LocalCoordinate> lc;
	};







	










	struct LocalCoordinate
	{
		//Feature Point's index [0,p) 
		int la;
		//Local Coordinate [-k,k]
		Point2f dl;
		LocalCoordinate() :la(0), dl(Point2f(0, 0)){};
		LocalCoordinate(int _l, Point2f _dl) :la(_l), dl(_dl){};
		friend ostream& operator << (ostream& os, const LocalCoordinate& lc)
		{
			os << "landmark:" << lc.l << " local coordinate:(" << lc.dl.x << "," << lc.dl.y << ")";
		}
	};









	struct ShapeNormalizer
	{
		float theta;
		float scale;
		TransMat2D transMat;
		ShapeNormalizer(float _theta = 0, float _scale = 0) :theta(_theta), scale(_scale)
		{
			transMat = Mat::zeros(3, 3, CV_32FC1);
			Mat tm = getRotationMatrix2D(Point2f(0, 0), theta, scale);
			transMat.row(0) = tm.row(0);
			transMat.row(1) = tm.row(1);
			transMat.at<float>(2, 2) = 1.0f;
		}
		Point2f transform(const Point2f &p)
		{
			Mat pm = (Mat_<float>(3,1)<<p.x,p.y,1.0f);
			Mat pmT = transMat*pm;
			return(Point2f(pmT.at<float>(0, 0), pmT.at<float>(1, 0)));
		}
		Point2f invTransform(const Point2f&p)
		{
			Mat pm = (Mat_<float>(3, 1) << p.x, p.y, 1.0f);
			Mat pmT = transMat.inv()*pm;
			return(Point2f(pmT.at<float>(0, 0), pmT.at<float>(1, 0)));
		}
		Shape invTransform(const Shape&s)
		{
			Mat pm = (Mat_<float>(3, 1) << p.x, p.y, 1.0f);
			Mat pmT = transMat.inv()*pm;
			return(Point2f(pmT.at<float>(0, 0), pmT.at<float>(1, 0)));
		}
	};

	






	//---------- stataic Functions



	

}



#endif