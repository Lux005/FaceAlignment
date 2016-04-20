#include "common.h"
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include "fa.h"
#include "faInitializer.h"
#include "faShapeNormalization.h"
#include "faExplicitShapeRegression.h"

//#include "LearnStageRegressor.h"

using namespace cv;
using namespace std;
using namespace fa;
const string listFileName = "faceList.txt";

int main()
{
	vector<ImageAndShape> imageShapes;
	vector<Shape> initSet;
	Shape meanShape;
	Initializer::loadImageAndShape(imageShapes, initSet, meanShape, listFileName);
	ShapeNormalization::setMeanShape(meanShape);
	vector<StageRegressor> stageRegressors;
	ExplicitShapeRegression::TrainParams tp;
	//Initial Shapes per Image;
	tp.Naug = 5;
	//Number of Stage Regressors;
	tp.T = 10;
	ExplicitShapeRegression::ESRTraining(stageRegressors, imageShapes, tp, initSet);

}