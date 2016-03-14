#include "common.h"
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include "CFace.h"
#include "CCalculateRS.h"
#include "iostream"

int main(int argc, char** argv)
{
	CFace face1,face2,face3;
	face1.loadPoints("./landmarks/pose_0.land");
	face1.normalize_to_center();
	face1.drawFace("f1");
	face2.loadPoints("./landmarks/pose_1.land");
	face2.normalize_to_center();
	face2.drawFace("f2");
	face3.lmarkers = face1.lmarkers;
	face3.transform(0, 1);
	std::cout<<"xxxxxxxxx"<<face1.calcDiff(face3)<<std::endl;
	face3.drawFace("face3");
	CcalculateRS::calc(face1, face2);
	waitKey();
	system("Pause");
	return 0;
}