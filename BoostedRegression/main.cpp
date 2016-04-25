#include "common.h"
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include "fa.h"
#include "faInitializer.h"
#include "faShapeNormalization.h"
#include "faExplicitShapeRegression.h"
#include "faFileIO.h"
//#include "LearnStageRegressor.h"

using namespace cv;
using namespace std;
using namespace fa;
const string listFileName = "faceList.txt";

static void Training()
{
	vector<ImageAndShape> imageShapes;
	vector<Shape> initSet;
	Shape meanShape;


	Initializer::loadImageAndShape(imageShapes, initSet, meanShape, listFileName);
	ShapeNormalization::setMeanShape(meanShape);

	vector<StageRegressor> stageRegressors;
	ExplicitShapeRegression::TrainParams tp;
	//Initial Shapes per Image;
	tp.Naug = 20;
	//Number of Stage Regressors;
	tp.T = 2;
	ExplicitShapeRegression::ESRTraining(stageRegressors, imageShapes, tp, initSet);
	FileIO::saveTofile(stageRegressors, initSet, meanShape, "x.data");
}

static void Testing()
{
	vector<Image> imgs;
	vector<string> fnames;
	fnames.push_back("jiannan.jpg");
	Initializer::loadImages(imgs, fnames, "");
	imshow("xx", imgs[0]);
	waitKey(10);

	vector<ImageAndShape> imageShapes;
	vector<Shape> initSet;
	Shape meanShape;

	vector<StageRegressor> stageRegressors;

	FileIO::readFromfile(stageRegressors, initSet, meanShape, "x.data");

	ShapeNormalization::setMeanShape(meanShape);

	ExplicitShapeRegression::ESRTesting(imgs[0], stageRegressors, 20, initSet);
}
int main()
{
	Training();

	
}