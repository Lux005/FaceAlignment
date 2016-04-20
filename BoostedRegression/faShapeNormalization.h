//Face Alignment by Explicit Shape Regression C++ Implementation
//Lu, Xinzhong @ CPS.udayton.edu
//luxinzhong@msn.com
//Professor Dr. Shen,Ju
//Jan, 2016
#ifndef _SHAPE_NORMALIZATION_H_
#define _SHAPE_NORMALIZATION_H_
#include "common.h"
#include "fa.h"
#include <omp.h>

class fa::ShapeNormalization
{
private:
	static Shape meanShape;
	//
	static vector<ShapeNormalizer> vec_Ms;

	struct Face
	{
		Shape shape;
		ShapeNormalizer normalzier;
		float diff = 0;
		Point2f center;
		Face()
		{

		}
		Face(const Face& f)
		{
			f.shape.copyTo(shape);
		}

		Face(Shape& s) :shape(s)
		{
		}

		const int size() const
		{
			if (shape.cols <= 0 || shape.rows != 1)
				MyError("Invalid shape size.");
			return shape.cols / 2;
		}


		Point2f& setCenter(int id = -1)
		{
			if (id >= 0 && id < size())
			{
				center = getShapePoint(shape, id);
			}
			else
			{
				Point2f total(0, 0);
				for (int i = 0; i < size();i++)
					total += getShapePoint(shape, i);
				center = avg(total, size());
	
			}
			alignToCenter();
			return center;
		}

		void alignToCenter()
		{
			for (int i = 0; i < size(); i++)
			{
				Point2f t = getShapePoint(shape, i);
				t -= center;
				setShapePoint(shape, t, i);
			}
		}

		Point2f avg(Point2f& p1, int n){
			return Point2f(p1.x / n, p1.y / n);
		}

		
		float calcDiff(const Face& f)
		{

			if (f.shape.cols == 0 || this->shape.cols != f.shape.cols)
				MyError("Empty faceLandmark or sizes of two landmarks didn't match!");
			Mat diffM = f.shape - shape;
			Scalar _diff = cv::sum(diffM);
			diff = _diff[0];
			return diff;
		}


		void transform(const double _theta, const double& _scale = 1.0)
		{
			if (_scale<0.1 || _scale>10)
				MyError("Invalid param for scaling");
			normalzier = ShapeNormalizer(_theta, _scale);
			for (int i = 0; i < size(); i++)
			{
				Point2f p = getShapePoint(shape, i);
				p = normalzier.transform(p);
				setShapePoint(shape, p, i);
			}
		}


		void drawFace(string windowName, bool dotIndex = false, Scalar& color = Scalar(0, 0, 255))
		{
			const int s = 1000;
			Mat x(1000, 1000, CV_8UC3, Scalar(255, 255, 255));
			Point2f t, c;
			c.x = center.x * s;
			c.y = center.y * s;
			circle(x, c, 5, Scalar(255, 0, 255), -1);

			for (int i = 0; i < size(); i++)
			{
				Point2f p = getShapePoint(shape, i);
				circle(x, t, 2, color, -1);
				if (dotIndex)putText(x, std::to_string(i+1), t, 1, 1.0, Scalar(0, 0, 0), 1, 8, false);
			}
			imshow(windowName, x);
			cv::waitKey(100);

		}
		void drawFace(string windowName, Face& face2)
		{
			const int s = 200;
			Mat x(200, 200, CV_8UC3, Scalar(255, 255, 255));
			Point2f c;
			c.x = (center.x+0.5) * s;
			c.y = (center.y+0.5) * s;
			circle(x, c, 2, Scalar(255, 0, 255), -1);
			for (int i = 0; i <shape.cols/2; i++)
			{
				Point2f p = getShapePoint(shape, i)+Point2f(0.5,0.5);
				p = p * s;
				circle(x, p, 1, Scalar(0, 0, 255), -1);
			}
			for (int i = 0; i < face2.size(); i++)
			{
				Point2f p = getShapePoint(face2.shape, i) + Point2f(0.5, 0.5);
				p = p *s;
				circle(x, p, 1, Scalar(255, 0, 0), -1);
			}
			imshow(windowName, x);
			cv::waitKey(5);
		}
	};
public:

	static void setMeanShape(const Shape & mShape)
	{
		if (mShape.cols < 1 || mShape.rows != 1)
			MyError("Invalid Mean Shape");
		meanShape = mShape;
	}
	static Shape& getMeanShape()
	{
		if (meanShape.rows != 1 || meanShape.cols <= 0)
			MyError("Empty mean shape");
		return meanShape;
	}

	static ShapeNormalizer GenShapeNormalizer(const Shape& shape)
	{
		Face face, mFace, rFace;
		face.shape = shape;
		face.setCenter(-1);
		mFace.shape = getMeanShape();
		mFace.setCenter(-1);
		rFace = searchBestNormalizer(face, mFace);
		
		//rFace.drawFace("x", mFace);
		
		
		return rFace.normalzier;

	}
	static void setMs(const vector<Shape> &vec_Shape)
	{
		const int N = vec_Shape.size();
		if (N <= 0)
			MyError("Empty Shape.");
		vec_Ms.clear();
		vec_Ms.resize(N);
		cout << "calculating Normalizer...";
		for (int i = 0; i < N; i++)
		{
			vec_Ms[i] = GenShapeNormalizer(vec_Shape[i]);
			cout << "\rcalculating Normalizer..." << i + 1 << "/" << N << "  Best: " << vec_Ms[i].theta << " " << vec_Ms[i].scale ;
		}
		cout << endl;
	}
	static vector<ShapeNormalizer>& getMs()
	{
		if (vec_Ms.size() == 0)
			MyError("empty vector");
		return vec_Ms;
	}



	static Face searchBestNormalizer(const Face& modifyFace, const Face& targetFace, double Rotaion_Step = 1, int Rotation_Range = 2, double Scale_Step = 0.05, double Scale_Range = 0.1)
	{
		if (modifyFace.shape.rows == 0 || modifyFace.shape.rows != targetFace.shape.rows)
			MyError("Empty face shape or sizes of two shape didn't match!");
		//cout << "Rotation Step Size:" << Rotaion_Step << " Rotation Range: +-" << Rotation_Range << "  Scale Step Size: " << Scale_Step << "  Scale Range: +-" << Scale_Range * 100 << "%" << endl;
		const Face modify = modifyFace;
		const Face target = targetFace;
		Face bestFace = modify;
		bestFace.transform(0, 1);
		bestFace.calcDiff(target);
		int rec = 0;
		static const int rangeI = Rotation_Range * 10;
#pragma omp parallel for
		for (int ri = -rangeI; ri <= rangeI; ri += Rotaion_Step * 10)
		{
			double r = ri;
			r /= 10;
			for (double s = 1 - Scale_Range; s <= 1 + Scale_Range; s += Scale_Step)
			{
				//pgTimer;
				Face temp;
				#pragma omp critical
				{
					temp = Face(modify);
				}
				temp.transform(r, s);
				temp.calcDiff(target);
				//temp.drawFace("xx1", temp);
				bestFace = (bestFace.diff <= temp.diff) ? bestFace : temp;
				//pgTimer;
			}
			//#pragma omp critical
			//			{
			//				rec++;
			//				cout << "\rprogress: " << rec << "/" << Rotation_Range * 2 / Rotaion_Step + 1 << std::flush;
			//			}

		}
	
		return bestFace;
	}
};

#endif