#include "fa.h"
#include "faShapeNormalization.h"
using namespace fa;
Shape ShapeNormalization::meanShape = Mat::zeros(1, 1, CV_32FC1);
vector<ShapeNormalizer> ShapeNormalization::vec_Ms;