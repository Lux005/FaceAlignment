//Face Alignment by Explicit Shape Regression C++ Implementation
//Lu, Xinzhong @ CPS.udayton.edu
//luxinzhong@msn.com
//Professor Dr. Shen,Ju
//Jan, 2016
#ifndef _LEARNSTAGEREGRESSOR_H_
#define _LEARNSTAGEREGRESSOR_H_
#include "common.h"
#include <ctime>
#include "omp.h"
#include "fa.h"
#include "faShapeNormalization.h"
#include "faCorrelationBasedFeatureSelection.h"
class fa::LearnStageRegressor
{
public:
	//[0,P)
	typedef vector<LocalCoordinate> Vec_LocalCoordinate;
	//{Ii,Si}, [0,N)
	typedef vector<ImageAndShape> Vec_ImageAndShape;

	struct TrainParams;

	struct TrainParams
	{
		//Number of Feature Points(face landmarks)
		//default 29
		int Nfp = 29;
		//Number of Local Coordinates to be Generated
		//default 400
		int P = 400;
		//Lowercase k = 0.3 times of the distance between two pupils on the mean shape
		//default 10
		int k = 10;
		//Number of Ferns
		//default 5
		int F = 5;
		//int
		//Number of internal regressors
		//default 500
		int K = 500;
	};



	//	static void GenerateLocalCoordinates(int _Nfp, int _P, int _k, LocCoordV& _locCoordV);
	//static void LearnStageRegressor(const Mat _Y, const ISpairV& _iSpairV, const TrainParams& _trainP);



	//void GenLocalCoordinates(
	//  Vec_LocalCoordinate&, <--output
	//  const int Nfp,
	//  const int P,
	//  const int k,
	//  )
	static void GenLocalCoordinates(Vec_LocalCoordinate& _locCoordV, const int _Nfp, const int _P, const int _k)
	{
		Vec_LocalCoordinate & veclc = _locCoordV;
		veclc.clear();
		veclc.resize(_P);
		srand((unsigned)time(NULL));
#pragma omp parallel for
		for (int alpha = 0; alpha < _P; alpha++)
		{
			LocalCoordinate lc;
			lc.la = (rand() % (_Nfp));
			lc.dl.x = (rand() % (2 * _k + 1)) - _k;
			lc.dl.y = (rand() % (2 * _k + 1)) - _k;
			veclc[alpha] = lc;
		//	cout << lc;
		}
		
	}








	//void ExtractShapeIndexedPixels(
	//  ShapeIndexedPixels& _matP, <--output N * P, 32FC1
	//  Vec_ImageAndShape& , 
	//  const Vec_LocalCoordinate& _vecLocCoord
	//  )
	static void ExtractShapeIndexedPixels(ShapeIndexedPixels& matP, const Vec_ImageAndShape& vecISpair, const Vec_LocalCoordinate& vecLocCoord)
	{
		const int P = vecLocCoord.size();
		const int N = vecISpair.size();
		if (P == 0 || N==0)
			MyError("Invalid size");
		matP = Mat::zeros(N, P, CV_32FC1);
#pragma omp parallel for
		for (int i = 0; i < N; i++)
		{
			for (int a = 0; a < P; a++)
			{
				const Image & image = vecISpair[i].first;
				const Shape & shape = vecISpair[i].second;
				const LocalCoordinate & lc = vecLocCoord[a];
				Point2f m_dl = ShapeNormalization::getMs()[i].invTransform(lc.dl);
				Point2f u = getShapePoint(shape, lc.la) ;
				u.x *= image.cols;
				u.y *= image.rows;
				u += m_dl;
				if (u.x < 0 || u.y < 0)
				{
					cout << shape;
					MyError("invalid data");
				}
				matP.at<float>(i,a) = image.at<float>(u);
			}
		}
	}






	static void GenPixelPixelCovariance(PixelPixelCovariance& _covP, const ShapeIndexedPixels & _matP)
	{
		int P = _matP.cols;
		int N = _matP.rows;
		Mat mean;
		calcCovarMatrix(_matP, _covP, mean, CV_COVAR_NORMAL | CV_COVAR_ROWS,CV_32FC1);
	}


	//void GenPixelPixelDiff(
	//	PixelPixelDiff& _X,  <--output N * P^2, 32FC1
	//	const ShapeIndexedPixels & _matP
	//	)
	static void GenPixelPixelDiff(PixelPixelDiff& _X, const ShapeIndexedPixels & _matP)
	{
		int P = _matP.cols;
		int N = _matP.rows;
		PixelPixelCovariance &Xmatrix = _X;
		const ShapeIndexedPixels &mat_q=_matP;
		vector<float> v1;
		Xmatrix = Mat::zeros(N, pow(P, 2), CV_32FC1);
#pragma omp parallel for
			for (int yy1 = 0; yy1 < mat_q.cols; yy1++){
				for (int yy2 = 0; yy2 <=yy1; yy2++){
					Mat diff = mat_q.col(yy1) - mat_q.col(yy2);
					diff.copyTo(Xmatrix.col(yy1*P + yy2));
					diff *= -1;
					diff.copyTo(Xmatrix.col(yy2*P + yy1));
				}
			}
	}



	//void GenThreshold(
	//	Mat &  thres, <--output: 1 * F, 32FC1
	//	const int _F
	//	)


	static void GenThreshold(Mat & thres, const int _F, const float a=-0.3, const float b=0.3)
	{
		thres = Mat(1, _F, CV_32FC1);
		srand((unsigned)time(NULL));
		for (int i = 0; i < _F; i++)
		{
			thres.at<float>(0, i) = (float)rand() / (float)(RAND_MAX)*2-1.0f;
		}
	}






	//void PartitionSamples(
	//	vector<vector<int>> & partition, <--output: indices of targets falling into each bin
	//	const Mat difference, 
	//	const Mat threshold, 
	//	const int _F
	//	)
	static void PartitionSamples(vector<vector<int>> & partition, const Mat difference, const Mat threshold, const int _F)
	{
		partition.clear();
		partition.resize(pow(2, _F));
		if (difference.cols>sizeof(int)* 8)
			MyError("exceed the range of int.");
		//cout << threshold << endl;
		for (int i = 0; i < difference.rows; i++)
		{
			int temp_bin_num = 0;
			//cout << difference.row(i) << endl;
			//cout << threshold << endl;
			for (int j = 0; j < difference.cols; j++)
			{
				if (difference.at<float>(i, j) >= threshold.at<float>(0, j))
				{
					temp_bin_num |= 1 << j;
				}
			}
			//cout << temp_bin_num << endl;
			partition[temp_bin_num].push_back(i);
		}
	}







	//static Mat ComputeBins(  <--return yb: predicted targets. 2^F * 2Nfp, 32FC1
	//	const vector<vector<int>> & partition, 
	//	const Mat Y, 
	//	const int _F, 
	//	const float beta = 1000.0
	//	)
	static Mat ComputeBins(const vector<vector<int>> & partition, const Mat Y, const int _F, const float beta = 1000.0){
		long fPow2 = pow(2, _F);
		Mat final_Yb_v = Mat::zeros(fPow2, Y.cols, CV_32FC1);
		//Mat zero_temp = Mat::zeros(1, Y.cols, CV_32FC1);
#pragma omp parallel for
		for (int i = 0; i < fPow2; i++){
			//	final_Yb_v.row(i) = zero_temp.row(0);
			if (partition[i].size() != 0)
			{
				for (vector<int>::const_iterator iter = partition[i].begin(); iter != partition[i].end(); ++iter){
					final_Yb_v.row(i) += Y.row(*iter);
				}
				Mat Ybrow = (1 / (1 + beta / partition[i].size())) * (final_Yb_v.row(i) / partition[i].size());
				Ybrow.row(0).copyTo(final_Yb_v.row(i)) ;
			}
		}
		return final_Yb_v;
	}







	//static void GenInternalRegressor(
	//	InternalRegressor&const regressor,   <--output Internal Regressor
	//	vector<RegressionTargets>  &vecY,	<--[0,K], size=K+1
	//	const PixelPixelCovariance &covP, 
	//	const int F, 
	//	const int ik <--ik=[1,K]
	//	)
	static void GenInternalRegressor(InternalRegressor& regressor, vector<RegressionTargets>  &vecY, const ShapeIndexedPixels& matP, const PixelPixelCovariance &covP, const PixelPixelDiff &X, const int F, const int P, const int ik)
	{
		vector<float> theta;
		CorrelationBasedFeatureSelection featureSelection;
		CorrelationBasedFeatureSelection::CBFSR cbfsR(matP.rows, F, CV_32FC1);
		vector<vector<int>> partitions;
		
		featureSelection.buildFeatureSelection(vecY[ik - 1], matP, covP, X, F, P, cbfsR);
		Mat pmf_pnf = cbfsR.bestFerns;

		Mat threshold;
	
		GenThreshold(threshold, F);

		PartitionSamples(partitions, cbfsR.bestFerns, threshold, F);
		//
		
		regressor.mn = cbfsR.mns;
		regressor.theta = threshold;
		regressor.yb = ComputeBins(partitions, vecY[ik-1], F);
		//

		Mat rk = ApplyInternalRegressor(regressor, pmf_pnf);
	
		if (rk.cols == vecY[ik - 1].cols && rk.rows == vecY[ik - 1].rows)
		{
			vecY[ik] = vecY[ik - 1] - rk;
		}
		else
		{
			MyError("Invalid Mat size!");
		}
	}





	static Shape ApplyInternalRegressor(const InternalRegressor& reg, const Mat & pmf_pnf)
	{
		if (reg.mn.size() != pmf_pnf.cols || pmf_pnf.cols != reg.theta.cols)
			MyError("Invalid Size!");
		long fpow2 = pow(2, reg.mn.size());
		if (fpow2 != reg.yb.rows)
			MyError("Invalid Size!");
		if (pmf_pnf.cols>sizeof(int)* 8)
		{
			MyError("exceeded the range of int.");
		}
		const Mat &yb = reg.yb;
		const Mat &threshold = reg.theta;
		Shape rY = Mat::zeros(pmf_pnf.rows, yb.cols, CV_32FC1);

#pragma omp parallel for
		for (int i = 0; i < pmf_pnf.rows; i++)
		{
			int temp_bin_num = 0;
			for (int j = 0; j < pmf_pnf.cols; j++)
			{
				if (pmf_pnf.at<float>(i, j) >= threshold.at<float>(0, j))
				{
					temp_bin_num |= 1 << j;
				}
			}
			if (temp_bin_num >= 0 && temp_bin_num < reg.yb.rows)
				rY.row(i) = yb.row(temp_bin_num);
			else
				MyError(temp_bin_num + " exceeded Yb's range of " + fpow2);
		}
		return rY;
	}

	static Shape ApplyStageRegressor(const StageRegressor& reg, const Image& i, const Shape& s)
	{
		ShapeIndexedPixels matP;
		vector<ImageAndShape> vec_is;
		vec_is.push_back(ImageAndShape(i, s));
	
		ExtractShapeIndexedPixels(matP, vec_is, reg.lc);
		Shape rS = Mat::zeros(s.rows,s.cols,CV_32FC1);
		const int K = reg.r.size();
		for (int k = 0; k < K;k++)
		{
			const InternalRegressor& r = reg.r[k];
			const int F = r.mn.size();

			Mat pmf_pnf = Mat::zeros(1, F, CV_32FC1);
			for (int f = 0; f < F; f++)
			{
				pmf_pnf.at<float>(0, f) = matP.at<float>(0, r.mn[f].first) - matP.at<float>(0, r.mn[f].second);
			}
			rS += ApplyInternalRegressor(r, pmf_pnf);

		}

		return rS;
	}







	static void GenLearnStageRegressor(StageRegressor&  SR, const Mat _Y, const Vec_ImageAndShape& _imageShapes, const TrainParams& _trainParams)
	{
		//cout << _imageShapes[0].second;
		//training images and corresponding estimated shapes 
		//{Ii,Si},[0,N)
		const Vec_ImageAndShape& vec_ImageShape = _imageShapes;

		//Regression Target
		//N* 2Nfp
		//32FC1
		const RegressionTargets &Y = _Y;

		//Train Parameters{Nfp, P, k, F, K}
		const TrainParams& trainParams = _trainParams;
		//Number of Feature Points
		const int Nfp = trainParams.Nfp;
		//Number of loc coordinates
		const int P = trainParams.P;
		//Number if Internal Regressor
		const int K = trainParams.K;
		//Loc Coordinate range [-k,k]
		const int k = trainParams.k;
		//Number of Ferns
		const int F = trainParams.F;


		//reference to Stage Regressor
		Vec_LocalCoordinate &vec_LocCoords=SR.lc;

		//reference to Stage Regressor, size= K
		vector<InternalRegressor> &vec_InternalR=SR.r;


		//N * P CV_32FC1
		ShapeIndexedPixels matP;

		//P * P CV_32FC1
		PixelPixelCovariance covP;
		//N* P^2 CV_32FC1
		PixelPixelDiff X;
		//(N * 2Nfp Matix)[0,K] size=K+1
		vector<RegressionTargets>  vecY;
		vecY.clear();
		vecY.resize(K + 1);
		vecY[0] = Y;


		//
		MyDebug("GenLocalCoordinates");
		GenLocalCoordinates(vec_LocCoords,Nfp, P, k);
		
		//
		MyDebug("ExtractShapeIndexedPixels");
		ExtractShapeIndexedPixels(matP, vec_ImageShape, vec_LocCoords);
		
		//
		MyDebug("GenPixelPixelDiff");
		//GenPixelPixelDiff(X, matP);
		GenPixelPixelCovariance(covP,matP);
	
		//
		vec_InternalR.clear();
		vec_InternalR.resize(K);
		MyDebug("GenInternalRegressor");
		//
		cout << "computing Internal Regressors...";
		for (int ik = 0; ik < K; ik++)
		{
			//
			cout << "\rComputing Internal Regressors " << ik + 1 << "/" << K<< " Step: 0";
			GenInternalRegressor(vec_InternalR[ik], vecY, matP,covP,X, F, P,ik + 1);
			
		}

		//
	}
};



#endif