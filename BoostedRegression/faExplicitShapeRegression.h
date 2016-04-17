#ifndef _EXPLICIT_SHAPE_REGRESSION_H_
#define _EXPLICIT_SHAPE_REGRESSION_H_
#include "fa.h"
class fa::ExplicitShapeRegression
{
	public:
		struct TrainParams;

		struct TrainParams
		{
			int Naug;
			int T;
		}TrainP;

		void ESRTraining(const vector<ImageAndShape>&imageShapes,const TrainParams& tp, const vector<Shape>& initSet)
		{
			vector<ImageShapeAndShape> imageShapeShapes;
			initialization(imageShapeShapes, imageShapes, tp.Naug, initSet);


			vector<const vector<Shape>> S;
			S.resize(tp.T + 1);

			vector<Shape> S0;
			S0.resize(imageShapeShapes.size());
			for (int i = 0; i<imageShapeShapes.size(); i++)
			{
				S0[i] = imageShapeShapes[i].second;
			}

			S[0] = S0;

			for (int t = 0; t < tp.T;t++)
			{
				const vector<Shape>& St_1 = S[t];
				vector<Shape> St;

			}
		}

		void initialization(vector<ImageShapeAndShape>&imageShapeShapes,const vector<ImageAndShape> &img_array, const int _D, const vector<Shape> & InitSet)
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