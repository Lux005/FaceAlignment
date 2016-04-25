//Face Alignment by Explicit Shape Regression C++ Implementation
//Lu, Xinzhong @ CPS.udayton.edu
//luxinzhong@msn.com
//Professor Dr. Shen,Ju
//Jan, 2016
#ifndef _FA_INITIALIZER_H_
#define _FA_INITIALIZER_H_
#include "fa.h"
#include <fstream>
using namespace std;
using namespace cv;
class fa::Initializer
{
#define NUMBER_OF_POSE 20
#define ENLARGE_FACTOR 0.3
public:
	static void loadImageAndShape(vector<ImageAndShape>& imageShapes, vector<Shape>& initSet,Shape& meanShape, const string& fileName)
	{
		static const string POSE_FILE_PREFIX = "pose_";
		static const string POSE_FILE_SHAPE_POSTFIX = ".land";
		static const string POSE_FILE_IMAGE_POSTFIX = ".png";
		vector<string> dataList;
		loadList(dataList,fileName);
		imageShapes.clear();
		imageShapes.reserve(dataList.size()*NUMBER_OF_POSE);
		cout << "loading files....";
		for (int i = 0; i < dataList.size();i++)
		{
			const string &dataPath = dataList[i];
			vector<string> posePath(NUMBER_OF_POSE);
			for (int j = 0; j < NUMBER_OF_POSE; j++)
			{
				string temp = dataPath + POSE_FILE_PREFIX;
				temp += to_string(j);
				posePath[j] = temp;
			}

			vector<Shape> shapes;
			vector<Image> images;
			loadShapes(shapes, posePath, POSE_FILE_SHAPE_POSTFIX);
			loadImages(images, posePath, POSE_FILE_IMAGE_POSTFIX);
			Normalize(images,shapes);
			ShowImageAndShape(ImageAndShape(images[0],shapes[0]),"ImageAndShape");
			
			for (int i = 0; i < NUMBER_OF_POSE; i++)
			{
				imageShapes.push_back(ImageAndShape(images[i],shapes[i]));
			}
			cout << "\rloading files...." << i + 1 << "/" << dataList.size();
		}
		cv::destroyWindow("ImageAndShape");
		cout << endl;
		if (dataList.size()*NUMBER_OF_POSE != imageShapes.size())
			MyError("Incorrect Size of the images and shapes." << imageShapes.size());
		genInitSet(initSet, imageShapes,dataList.size(), NUMBER_OF_POSE);
		meanShape = genMeanShape(initSet);
		cout << "Images and shapes have been loaded: " << imageShapes.size() << endl;

	}

	static void loadList(vector<string>& fileList, const string& fileName)
	{
		ifstream fs;
		fs.open(fileName, ios::in);
		if (fs.fail()||fs.eof())
			MyError("Can't read file." << fileName);
		int count=0,line=2;
		fs >> count;
		if (count<1 || count>65000)
			MyError("Invalid number." << count);
		fileList.clear();
		while (!fs.fail()&&!fs.eof()&&fileList.size()<count)
		{
			int index=0;
			string filepath="";
			if (!(fs >> index >> filepath))
			{
				MyError("Invalid data at line:"<<line);
				break;
			}
			else
			{
				fileList.push_back(filepath);
			}
			line++;
		}
		fs.close();
		if (count != fileList.size())
			MyError("Incorrect number of items read." << fileList.size());
		MyDebug("Data list file read. #items:" << count);
	}
	static void loadShapes(vector<Shape>& shapes, const vector<string> &path,const string& postFix)
	{

		shapes.resize(path.size());
		for (int i = 0; i < path.size(); i++)
		{
			const string fileName = path[i] + postFix;
			Shape& shape = shapes[i];
			ifstream fs;
			fs.open(fileName, ios::in);
			if (fs.fail() || fs.eof())
				MyError("Can't read file."<< fileName);
			int count = 0, line = 2;
			fs >> count;
			if (count<1 || count>65000)
				MyError("Invalid number." << count);
			shape = Mat::zeros(1, count * 2, CV_32FC1);
			int scount = 0;
			while (!fs.fail() && !fs.eof())
			{
				float fx = 0, fy = 0;
				if (!(fs >> fx >> fy))
				{
					if (scount!=count)
						MyError("Invalid data at line:"<<line);
					break;
				}
				else
				{
					fy = 1.0 - fy;
					if (fx<0||fy<0)
						MyError("Invalid data at line:" << line);
					shape.at<float>(0, 2 * scount) = fx;
					shape.at<float>(0, 2 * scount + 1) = fy;
					scount++;
				}
				line++;
			}
			fs.close();
			if (count != scount)
				MyError("Incorrect number of landmarks read."<<scount);
		}
		//MyDebug("Shape file read.");
	}
	static void loadImages(vector<Image>& images, const vector<string> &path, const string& postFix)
	{
		images.clear();
		images.resize(path.size());
		for (int i = 0; i < path.size(); i++)
		{
			const string fileName = path[i] + postFix;
			Image& image = images[i];
			image = cv::imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
			if (image.cols>0 && image.rows > 0)
			{
				// Convert to Gray image
				if (image.type() != CV_8UC1)
					cvtColor(image, image, CV_RGB2GRAY);
				// Convert to Mat_<float> scale 0.0-1.0f
				image.convertTo(image, CV_32FC1);
				image /= 255.0;
			}
			else
				MyError("Unable to load image:"<<fileName);	
		}
		//MyDebug("Images loaded. #images:" << path.size());
	}


	//
	static void Normalize(vector<Image>& images, vector<Shape>&shapes)
	{
		if (images.size() < 1 || images.size() != shapes.size())
			MyError("Invalid vector size. Images:"<<images.size()<<" Shapes:"<<shapes.size());
		int pCount = shapes[0].cols/2;
		for (int j = 0; j < shapes.size();j++)
		{
			Shape& s = shapes[j];
			Image& img = images[j];
			if (s.cols != pCount * 2)
				MyError("Incorrect Shape Size."<<s.cols);
			float maxX = 0.0f;
			float minX = 99999.9f;
			float maxY = 0.0f;
			float minY = 99999.9f;
			for (int i = 0; i < pCount; i++)
			{
				float & x = s.at<float>(0, i * 2);
				float& y = s.at<float>(0, i * 2 + 1);
				maxX = max(maxX, x);
				minX = min(minX,x);
				maxY = max(maxY, y);
				minY = min(minY, y);
			}
			if (maxX<0 || maxY<0 || minX<0 || minY<0 || maxX>1 || maxY>1 || minX>1 || minY>1)
				MyError("Invalid Values!");
			float rangeX = maxX - minX;
			float rangeY = maxY - minY;
			if (rangeX<=0 || rangeY<=0 || rangeX>1 || rangeY>1)
				MyError("Invalid Values!");
		
			maxX = (maxX + rangeX*ENLARGE_FACTOR> 1) ? 1 : maxX + rangeX*ENLARGE_FACTOR;
			maxY = (maxY + rangeY* ENLARGE_FACTOR > 1) ? 1 : maxY + rangeY*ENLARGE_FACTOR;
			minX = (minX - rangeX*ENLARGE_FACTOR< 0) ? 0 : minX - rangeX*ENLARGE_FACTOR;
			minY = (minY - rangeY* ENLARGE_FACTOR < 0) ? 0 : minY - rangeY*ENLARGE_FACTOR;

			float scaleX = 1.0f/(maxX - minX);
			float sacleY = 1.0f/(maxY - minY);
			if (scaleX<=0 || sacleY<=0)
				MyError("Invalid Values!");
			for (int i = 0; i < pCount; i++)
			{
				float & x = s.at<float>(0, i * 2);
				float& y = s.at<float>(0, i * 2+1);
				x -= minX;
				y -= minY;
				x *= scaleX;
				y *= sacleY;
				if (x < 0 || y < 0 || x>1 || y>1)
					MyError("Invalid Values!");
			}
			Rect rect(minX * img.cols, minY* img.rows, (maxX - minX)* img.cols,(maxY-minY)*img.rows);
			img = img(rect);
			
		}



	}




	//
	static Shape genMeanShape(const vector<Shape>& shapes)
	{
		if (shapes.size() <1)
			MyError("Empty vector.");
		if (shapes[0].cols < 1)
			MyError("Empty Shape.")
		Shape meanShape =Mat::zeros(1, shapes[0].cols, CV_32FC1);
		for (Shape s : shapes)
		{
			if (s.cols != meanShape.cols || s.rows != 1)
				MyError("Invalid Shape.");
			meanShape += s;
		}
		meanShape /= shapes.size();
		return meanShape;
	}



	static void genInitSet(vector<Shape>&initSet, const vector<ImageAndShape>& imageShapes, const int nSet, const int nPose)
	{
		if (imageShapes.size() == 0 || imageShapes.size() != nSet*nPose)
			MyError("Empty Data");
		initSet.clear();
		initSet.resize(nPose);
		for (int i = 0; i < nPose; i++)
		{
			Shape& s = initSet[i];
				s = Mat::zeros(1, imageShapes[0].second.cols, CV_32FC1);
			for (int j = 0; j < nSet; j++)
			{
				s += imageShapes[j*nPose + i].second;
			}
			s = s / nSet;
		}
	}
};
#endif