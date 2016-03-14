#include "common.h"
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include "CFace.h"
#include "CCalculateRS.h"
#include "iostream"

int main(int argc, char** argv)
{
	typedef CFace Face;
	typedef CFace::lmPoint lmPoint;
	Face face1,face2,face3;
	lmPoint center1, center2, center3;
	face1.loadPoints("./landmarks/pose_0.land");
	center1=face1.setCenter();

	face1.drawFace("f1",true);

	face2.loadPoints("./landmarks/pose_1.land");
	center2 = face2.setCenter();

	face2.drawFace("f2",true);
	
	face3=CcalculateRS::calc(face1, face2);
	face3.drawFace("result",face1);
	waitKey();
	system("Pause");
	return 0;
}