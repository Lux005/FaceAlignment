#ifndef _EXPLICIT_SHAPE_REGRESSION_H_
#define _EXPLICIT_SHAPE_REGRESSION_H_
#include "fa.h"
#include "faShapeNormalization.h"
#include "faLearnStageRegressor.h"
class fa::ExplicitShapeRegression
{
	public:
		struct TrainParams;

		struct TrainParams
		{
			int Naug;
			int T;
		};

		static void ESRTraining(vector<StageRegressor>&R,const vector<ImageAndShape>&imageShapes, const TrainParams& tp, const vector<Shape>& initSet)
		{
			const int N = imageShapes.size()*tp.Naug;
			const int Nfp = imageShapes[0].second.cols/2;
			if (N <= 0)
				MyError("Invalid value!");

			LearnStageRegressor::TrainParams lTp;
			lTp.Nfp = Nfp;
			lTp.F = 5;
			lTp.K = 500;
			lTp.k = 10;
			lTp.P = 400;


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
				StageRegressor &Rt = R[t];
				const vector<Shape>& St_1 = S[t];
				vector<Shape> &St=S[t+1];
				St.clear();
				St.resize(N);
			
		
				ShapeNormalziation::setMs(St_1);
				RegressionTargets Y = Mat::zeros(N, 2 * Nfp, CV_32FC1);
				for (int i = 0; i < N; i++)
				{
					Shape stemp = imageShapeShapes[i].first.second - imageShapeShapes[i].second;
					Y.row(i) = ShapeNormalziation::getMs()[i].transform(stemp);
				}

				LearnStageRegressor::GenLearnStageRegressor(Rt, Y, imageShapes, lTp);
				for (int i = 0; i < N; i++)
				{
					Shape stemp = LearnStageRegressor::ApplyStageRegressor(Rt, imageShapes[i].first, St_1[i]);
					St[i] = St_1[i] + ShapeNormalziation::getMs()[i].invTransform(stemp);
				}
			}
		}

		static void initialization(vector<ImageShapeAndShape>&imageShapeShapes,const vector<ImageAndShape> &img_array, const int _D, const vector<Shape> & InitSet)
		{
			int i = 1;
			vector<ImageShapeAndShape> &init_v = imageShapeShapes;
			imageShapeShapes.clear();

			ImageShapeAndShape ini;
			for (int C = 0; C < img_array.size(); C++)
			{
				for (int D = 0; D < 20; D++)
				{
					ini.first = img_array[C];
					ini.second = InitSet[0];
					init_v.push_back(ini);
					i++;
				}
			}
		
		}

};
#endif 