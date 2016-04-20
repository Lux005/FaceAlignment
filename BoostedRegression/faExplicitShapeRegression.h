#ifndef _EXPLICIT_SHAPE_REGRESSION_H_
#define _EXPLICIT_SHAPE_REGRESSION_H_
#include "fa.h"
#include "faShapeNormalization.h"
#include "faLearnStageRegressor.h"
#include <ctime>
class fa::ExplicitShapeRegression
{
	public:
		struct TrainParams;

		struct TrainParams
		{
			//Initial Shapes per Image;
			int Naug=20;
			//Number of Stage Regressors;
			int T=10;
		};

		static void ESRTraining(vector<StageRegressor>&R,vector<ImageAndShape>&imageShapes, const TrainParams& tp, const vector<Shape>& initSet)
		{
			const int N = imageShapes.size()*tp.Naug;
			const int Nfp = imageShapes[0].second.cols/2;
			if (N <= 0)
				MyError("Invalid value!");

			LearnStageRegressor::TrainParams lTp;
			lTp.Nfp = Nfp;
			lTp.F = 5;
			lTp.K = 50;//default 500
			lTp.k = 10;
			lTp.P = 40;//default 400


			vector<ImageShapeAndShape> imageShapeShapes;

			initialization(imageShapeShapes, imageShapes, tp.Naug, initSet);
	

			vector<vector<Shape>> S(tp.T + 1);
			vector<Shape> S0(N);
			for (int i = 0; i<N; i++)
			{
				S0[i] = imageShapeShapes[i].second;
				
			}
			S[0] = S0;

			R.clear();
			R.resize(tp.T);
			for (int t = 0; t < tp.T; t++)
			{
				MyDebug("Computing Stage Regressor:" << t + 1 << "/" << tp.T );
				StageRegressor &Rt = R[t];
				const vector<Shape>& St_1 = S[t];
				vector<Shape> &St=S[t+1];
				St.clear();
				St.resize(N);
				ShapeNormalization::setMs(St_1);
				RegressionTargets Y = Mat::zeros(N, 2 * Nfp, CV_32FC1);
				for (int i = 0; i < N; i++)
				{
					Shape stemp = imageShapeShapes[i].first.second - imageShapeShapes[i].second;
					//cout << ShapeNormalization::getMs()[i].transform(stemp)<<endl;
					 ShapeNormalization::getMs()[i].transform(stemp).copyTo(Y.row(i));
					//cout <<endl<<endl<<endl <<Y.row(i) << endl;
				}

				LearnStageRegressor::GenLearnStageRegressor(Rt, Y, imageShapes, lTp);
				for (int i = 0; i < N; i++)
				{
					Shape stemp = LearnStageRegressor::ApplyStageRegressor(Rt, imageShapes[i].first, St_1[i]);
					St[i] = St_1[i] + ShapeNormalization::getMs()[i].invTransform(stemp);
				}
				MyDebug("Stage Regressor:"<< t+1 <<"/"<<tp.T<<" Complelted!");
			}
		}

		static void initialization(vector<ImageShapeAndShape>&imageShapeShapes, vector<ImageAndShape> &img_array, const int _D, const vector<Shape> & InitSet)
		{
			if (img_array.size() < 1 || _D < 1 || InitSet.size() < 1)
				MyError("Invalid size.");
			vector<ImageAndShape> imageShapes;
			imageShapes.reserve(img_array.size()*_D);
			imageShapeShapes.clear();
			imageShapeShapes.reserve(img_array.size()*_D);

			srand((unsigned)time(NULL));
			for (int C = 0; C < img_array.size(); C++)
			{
				for (int D = 0; D < _D; D++)
				{
					int rd = (rand() % InitSet.size());
					ImageShapeAndShape ini;
					ini.first = img_array[C];
					ini.second = InitSet[rd];
					imageShapes.push_back(ini.first);
					imageShapeShapes.push_back(ini);
				}

			}
			img_array = imageShapes;
		}

};
#endif 