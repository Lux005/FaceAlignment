//Face Alignment by Explicit Shape Regression C++ Implementation
//Lu, Xinzhong @ CPS.udayton.edu
//luxinzhong@msn.com
//Professor Dr. Shen,Ju
//Jan, 2016
#ifndef _CORRELATIONBASEDFEATURESELECTION_H_
#define _CORRELATIONBASEDFEATURESELECTION_H_
#include <cmath>
#include "omp.h"
#include "fa.h"

class fa::CorrelationBasedFeatureSelection{
//Declaration
public:
	struct CBFSR{
		Mat bestFerns;
		vector<pair<int, int>> mns;
		CBFSR(const int N,const int F, const int MatType)
		{
			mns.resize(F);
			bestFerns = Mat::zeros(N, F, MatType);
		}
	};
	CorrelationBasedFeatureSelection();
	~CorrelationBasedFeatureSelection();
	void buildFeatureSelection(const Mat&_Y, const Mat& _matP, const Mat& _covP, const Mat& _X, const int _F, const int _P, CBFSR &result);
	Mat GenrandnGaussian(int _Nfp);
	void NCCMatch(int & rIndex, const Mat _Yprob, const Mat _X);
	void CorrelationMatch(int & rIndex, const Mat& _Yprob, const Mat &_covP, const Mat& _matP);
	float Covariance(const Mat& m1, const Mat&m2);
};

//Definition

fa::CorrelationBasedFeatureSelection::CorrelationBasedFeatureSelection()
{
}
fa::CorrelationBasedFeatureSelection::~CorrelationBasedFeatureSelection()
{
}
void fa::CorrelationBasedFeatureSelection::buildFeatureSelection(const Mat&_Y, const Mat& _matP,const Mat& _covP, const Mat& _X, const int _F, const int _P, CBFSR &result)
{
	//cout << _Y.cols / 2 << endl;
//#pragma omp parallel for
	for (int j = 0; j < _F; j++){
		Mat Yprob = Mat::zeros(_Y.rows, 1, CV_32FC1);
		
		
		Yprob = _Y * GenrandnGaussian(_Y.cols / 2);
		int rIndex = -1;
		//cout << Yprob << endl;
		if ( _covP.cols ==_P&&_covP.rows==_P)
		{
			CorrelationMatch(rIndex, Yprob, _covP, _matP);
		}
		else if (_X.cols==_P*_P)
		{
			NCCMatch(rIndex, Yprob, _X);
		}
		else
			MyError("Invalid Size!");

		
		result.mns[j].first = rIndex / _P;
		result.mns[j].second = rIndex % _P;
		Mat dd = _matP.col(result.mns[j].first) - _matP.col(result.mns[j].second);
		dd.copyTo(result.bestFerns.col(j));
		//cout <<"m: " << result.mns[j].first << "   n:" << result.mns[j].second << endl;
		//cout << result[j].bestfern << endl;
	
	}
	//cout << result.bestFerns << endl;
}

fa::Mat fa::CorrelationBasedFeatureSelection::GenrandnGaussian(int _Nfp){
	Mat gaussian_mat(2 * _Nfp, 1, CV_32FC1);
	Mat mean = cv::Mat::zeros(1, 1, CV_32FC1);
	Mat sigma = cv::Mat::ones(1, 1, CV_32FC1);
	randn(gaussian_mat, mean, sigma);
	//	cout << gaussian_mat << endl;
	return gaussian_mat;
}
float fa::CorrelationBasedFeatureSelection::Covariance(const Mat& m1, const Mat&m2)
{
	if (m1.rows != m2.rows || m1.rows <= 0 || m1.cols != 1 || m2.cols != 1)
		MyError("Invalid Size");
	Mat m(m1.rows,2,CV_32FC1);
	Mat result,mean;

	m1.col(0).copyTo(m.col(0));
	m2.col(0).copyTo(m.col(1));


	calcCovarMatrix(m, result, mean, CV_COVAR_NORMAL | CV_COVAR_ROWS,CV_32FC1);

	return result.at<float>(0, 1);
}
void fa::CorrelationBasedFeatureSelection::CorrelationMatch(int & rIndex, const Mat& _Yprob, const Mat &_covP, const Mat& _matP)
{
	const int P = _covP.cols;
	Mat NCC_result(1, P*P, CV_32FC1);
	Mat covYp(1, P, CV_32FC1);
	Mat mean;
#pragma omp parallel for
	for (int i = 0; i < P; i++)
	{
		covYp.at<float>(0, i) = Covariance(_Yprob, _matP.col(i));
	}
	float varYp = Covariance(_Yprob, _Yprob);
	int mf = 0, nf = 0;
	float corrf=-99.9;

	for (int m = 0; m < P;m++)
		for (int n = 0; n < P; n++)
		{
			if (m != n)
			{
				float corr = covYp.at<float>(0, m) - covYp.at<float>(0, n);
				float xx = _covP.at<float>(m, n);
				corr /= sqrt(varYp*xx);
				if (corr>corrf)
				{
					mf = m;
					nf = n;
					corrf = corr;
				}
			}
			
		}
		rIndex = mf*P + nf;

}
void fa::CorrelationBasedFeatureSelection::NCCMatch(int & rIndex, const Mat _Yprob, const Mat _X)
{

	Mat NCC_result(1, _X.cols, CV_32FC1);
	float nmY ;
	enum
	{
		MATRIX,
		COLUMN,
	}Method;
	Method = COLUMN;
	if (Method == MATRIX)
	{

		nmY = norm(_Yprob, NORM_L2);
		NCC_result = (_X.t()*_Yprob).t();
		Mat powX;
		pow(_X, 2, powX);
		Mat sumX;
		reduce(powX, sumX, 0, CV_REDUCE_SUM);
		sqrt(sumX, sumX);
		sumX += nmY;
		NCC_result /= sumX;
	}
	if (Method == COLUMN)
	{
		nmY = norm(_Yprob, NORM_L2);
		NCC_result = (_X.t()*_Yprob).t();
#pragma omp parallel for
		for (int i = 0; i < _X.cols; i++){
			NCC_result.at<float>(0, i) /= (norm(_X.col(i), NORM_L1) *nmY);
		}
	}
	
	NCC_result = abs(NCC_result);
	Mat sortedIndex;
	cv::sortIdx(NCC_result, sortedIndex, SORT_ASCENDING + CV_SORT_EVERY_ROW);
	
	rIndex = sortedIndex.at<int>(0, 0);

	//cout << result << endl;
	/*for (int j = 0; j < result.cols; j++)
	{
		X_sotrted_index[j] = j;
	}
	for (int j = 0; j < result.cols; j++){
		for (int jj = j + 1; jj < result.cols; jj++)
		{
			if (result.at<float>(0, j) < result.at<float>(0, jj))
			{
				temp = result.at<float>(0, j);
				result.at<float>(0, j) = result.at<float>(0, jj);
				result.at<float>(0, jj) = temp;

				temp = X_sotrted_index[j];
				X_sotrted_index[j] = X_sotrted_index[jj];
				X_sotrted_index[jj] = temp;
			}
		}
	}*/
}

#endif