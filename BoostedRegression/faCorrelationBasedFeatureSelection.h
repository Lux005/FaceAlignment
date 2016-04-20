//Face Alignment by Explicit Shape Regression C++ Implementation
//Lu, Xinzhong @ CPS.udayton.edu
//luxinzhong@msn.com
//Professor Dr. Shen,Ju
//Jan, 2016
#ifndef _CORRELATIONBASEDFEATURESELECTION_H_
#define _CORRELATIONBASEDFEATURESELECTION_H_
#include <cmath>
#include "fa.h"

class fa::CorrelationBasedFeatureSelection{
//Declaration
public:
	int patch_size;
	int * X_sotrted_index=nullptr;
	struct CBFSR{
		Mat bestFerns;
		vector<pair<int, int>> mns;
		CBFSR(const int N,const int F, const int MatType)
		{
			mns.resize(F);
			bestFerns = Mat(N, F, MatType);
		}
	};
	CorrelationBasedFeatureSelection();
	~CorrelationBasedFeatureSelection();
	void buildFeatureSelection(const Mat&_Y, const Mat& _X, const int _F, CBFSR &result);
	Mat GenrandnGaussian(int _Nfp);
	void NCCMatch(Mat _Yprob, Mat _X);
};

//Definition

fa::CorrelationBasedFeatureSelection::CorrelationBasedFeatureSelection()
{

	patch_size = 10;
}
fa::CorrelationBasedFeatureSelection::~CorrelationBasedFeatureSelection()
{
	delete[] X_sotrted_index;
}
void fa::CorrelationBasedFeatureSelection::buildFeatureSelection(const Mat&_Y, const Mat& _X, const int _F, CBFSR &result)
{
	cout << "[FeatureSelection] Feature selection start" << endl;
	if (X_sotrted_index != nullptr)
		delete[] X_sotrted_index;
	X_sotrted_index = new int[_X.cols];
	Mat Yprob = Mat::zeros(_Y.rows, 1, CV_32FC1);
	//cout << _Y.cols / 2 << endl;
	Yprob = _Y * GenrandnGaussian(_Y.cols / 2);
	//cout << Yprob << endl;
	NCCMatch(Yprob, _X);
	for (int j = 0; j < _F; j++){
		//int index = X_sotrted_index[j];
		//cout << j + 1 << ") " << "1st: " << vv[index].first << ", 2nd: " << vv[index].second << ", color difference: " << color_diff[j] << endl;
		result.mns[j].first = X_sotrted_index[j] / (int)pow(patch_size, 2);
		result.mns[j].second = X_sotrted_index[j] % (int)pow(patch_size, 2);
		result.bestFerns.col(j) = _X.col(X_sotrted_index[j]);
		cout <<"m: " << result.mns[j].first << "   n:" << result.mns[j].second << endl;
		//cout << result[j].bestfern << endl;
	}

	cout << "[FeatureSelection] Feature selection complete" << endl;
}

fa::Mat fa::CorrelationBasedFeatureSelection::GenrandnGaussian(int _Nfp){
	Mat gaussian_mat(2 * _Nfp, 1, CV_32FC1);
	Mat mean = cv::Mat::zeros(1, 1, CV_32FC1);
	Mat sigma = cv::Mat::ones(1, 1, CV_32FC1);
	randn(gaussian_mat, mean, sigma);
	//	cout << gaussian_mat << endl;
	return gaussian_mat;
}

void fa::CorrelationBasedFeatureSelection::NCCMatch(Mat _Yprob, Mat _X){
	Mat result;
	float temp;

	matchTemplate(_X, _Yprob, result, CV_TM_CCORR_NORMED);
	//cout << result << endl;
	for (int j = 0; j < result.cols; j++)
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
	}
}

#endif