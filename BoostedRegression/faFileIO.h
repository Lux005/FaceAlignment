#ifndef _FA_FILEIO_H_
#define _FA_FILEIO_H_
#include "common.h"
#include "fa.h"
#include <string>
#include "fstream"
using namespace std;
class fa::FileIO
{

private:
	static streampos fileSize;
	static ifstream fin;
	static ofstream fout;
	enum dataType
	{
		DEFAULT,
		STAGE_REGRESSOR,
		VECTOR,
		MAT,
		INTERNAL_REGRESSOR,
		LOCAL_COORDINATE,
		PAIR_INT_INT,
	};
	struct Block
	{
		dataType type;
		streampos  begin;
		size_t size;
	};




	template<class T>
	static void saveTofile(T& t, Block & b)
	{
		if (!fout.is_open())
			MyError("file isn't open");
		fout.seekp(0, ifstream::end);
		b.type = DEFAULT;
		b.begin = fout.tellp();
		fout.write((const char*)&t, sizeof(T));
		b.size = fout.tellp() - b.begin;
		if (b.size<1)
			MyError("Invalid Data End");
	}

	template<class T>
	static void readFromfile(T& t, const Block & b)
	{
		if (!fin.is_open())
			MyError("file isn't open");
		if (b.type != DEFAULT)
			MyError("Incorrect Data Type");
		fin.seekg(b.begin);
		fin.read((char*)&t, sizeof(T));
		if (fin.tellg() - b.begin != b.size)
			MyError("Invalid Data End");
	}



	static void saveTofile(StageRegressor& sr, Block & b)
	{
		if (!fout.is_open())
			MyError("file isn't open");
		fout.seekp(0, ifstream::end);
		b.type = STAGE_REGRESSOR;
		b.begin = fout.tellp();
		Block bs[2];
		fout.write((const char*)bs, 2 * sizeof(Block));
		saveTofile(sr.r, bs[0]);
		saveTofile(sr.lc, bs[1]);
		fout.seekp(b.begin);
		fout.write((const char*)bs, 2 * sizeof(Block));
		fout.seekp(0, ifstream::end);
		b.size = fout.tellp() - b.begin;
		if (b.size<1)
			MyError("Invalid Data End");
	}


	static void readFromfile(StageRegressor& sr, const Block & b)
	{
		if (!fin.is_open())
			MyError("file isn't open");
		if (b.type != STAGE_REGRESSOR)
			MyError("Incorrect Data Type");
		fin.seekg(b.begin);
		Block bs[2];
		fin.read((char*)bs,2* sizeof(Block));
		readFromfile(sr.r, bs[0]);
		readFromfile(sr.lc, bs[1]);
		if (fin.tellg() - b.begin != b.size)
			MyError("Invalid Data End");
	}





	static void saveTofile(InternalRegressor& ir, Block & b)
	{
			if (!fout.is_open())
				MyError("file isn't open");
			fout.seekp(0, ifstream::end);
			b.type = INTERNAL_REGRESSOR;
			b.begin = fout.tellp();
			Block bs[3];
			fout.write((const char*)bs, 3 * sizeof(Block));
			saveTofile(ir.mn, bs[0]);
			saveTofile(ir.theta, bs[1]);
			saveTofile(ir.yb, bs[2]);
			fout.seekp(b.begin);
			fout.write((const char*)bs, 3 * sizeof(Block));
			fout.seekp(0, ifstream::end);
			b.size = fout.tellp() - b.begin;
			if (b.size<1)
				MyError("Invalid Data End");
		}


	static void readFromfile(InternalRegressor& ir, const Block & b)
	{
		if (!fin.is_open())
			MyError("file isn't open");
		if (b.type != INTERNAL_REGRESSOR)
			MyError("Incorrect Data Type");
		fin.seekg(b.begin);
		Block bs[3];
		fin.read((char*)bs,3* sizeof(Block));
	
		readFromfile(ir.mn, bs[0]);
		readFromfile(ir.theta, bs[1]);
		readFromfile(ir.yb, bs[2]);
		if (fin.tellg() - b.begin != b.size)
			MyError("Invalid Data End");
	}


	static void saveTofile(LocalCoordinate& lc, Block& b)
	{
		if (!fout.is_open())
			MyError("file isn't open");
		fout.seekp(0, ifstream::end);
		b.type = LOCAL_COORDINATE;
		b.begin = fout.tellp();
		fout.write((const char*)&lc.la, sizeof(int));
		fout.write((const char*)&lc.dl.x, sizeof(float));
		fout.write((const char*)&lc.dl.y, sizeof(float));
		b.size = fout.tellp() - b.begin;
		if (b.size<1)
			MyError("Invalid Data End");
	}

	

	static void readFromfile(LocalCoordinate& lc, const Block& b)
	{
		if (!fin.is_open())
			MyError("file isn't open");
		if (b.type != LOCAL_COORDINATE)
			MyError("Incorrect Data Type");
		fin.seekg(b.begin);
		fin.read((char*)&lc.la, sizeof(int));
		fin.read((char*)&lc.dl.x, sizeof(float));
		fin.read((char*)&lc.dl.y, sizeof(float));
		if (fin.tellg() - b.begin != b.size)
			MyError("Invalid Data End");
	}


	
	static void  saveTofile(pair<int, int>& p, Block &b)
	{
		if (!fout.is_open())
			MyError("file isn't open");
		fout.seekp(0, ifstream::end);
		b.type = PAIR_INT_INT;
		b.begin = fout.tellp();
		fout.write((const char*)&p, sizeof(pair<int, int>));
		b.size = fout.tellp() - b.begin;
		if (b.size<1)
			MyError("Invalid Data End");
	}
	
	
	static void readFromfile(pair<int, int>& p, const Block& b)
	{
		if (!fin.is_open())
			MyError("file isn't open");
		if (b.type != PAIR_INT_INT)
			MyError("Incorrect Data Type");
		fin.seekg(b.begin);
		fin.read((char*)&p, sizeof(pair<int, int>));
		if (fin.tellg() - b.begin != b.size)
			MyError("Invalid Data End");
	}



	

	static void  saveTofile(Mat& m, Block &b)
	{
		if (!fout.is_open())
			MyError("file isn't open");
		fout.seekp(0, ifstream::end);
		b.type = MAT;
		b.begin = fout.tellp();
		fout.write((const char*)&m.rows, sizeof(int));
		fout.write((const char*)&m.cols, sizeof(int));
		fout.write((const char*)m.data, m.rows*m.cols*sizeof(float));
		b.size = fout.tellp() - b.begin;
		if (b.size<1)
			MyError("Invalid Data End");
	}

	

	static void readFromfile(Mat& m, const Block& b)
	{
		if (!fin.is_open())
			MyError("file isn't open");
		if (b.type != MAT)
			MyError("Incorrect Data Type");
		fin.seekg(b.begin);
		int c, r;
		fin.read((char*)&r, sizeof(int));
		fin.read((char*)&c, sizeof(int));
		m = Mat(r, c, CV_32FC1);
		fin.read((char*)m.data, sizeof(float)*r*c);
		if (fin.tellg() - b.begin != b.size)
			MyError("Invalid Data End");
	}



	template<class T>
	static void saveTofile(vector<T>& v, Block & b)
	{
		if (!fout.is_open())
			MyError("file isn't open");
		fout.seekp(0, ifstream::end);
		b.type = VECTOR;
		b.begin = fout.tellp();
		int size = v.size();
		if (size < 1)
			MyError("Invalid size");
		Block* bs = new Block[size];
		fout.write((const char*)&size, sizeof(int));
		fout.write((const char*)bs, size * sizeof(Block));
		for (int i = 0; i < size; i++)
		{
			saveTofile(v[i], bs[i]);
		}
		fout.seekp(b.begin);
		fout.write((const char*)&size, sizeof(int));
		fout.write((const char*)bs, size * sizeof(Block));
		fout.seekp(0, ifstream::end);
		b.size = fout.tellp() - b.begin;
		delete[] bs;
	}

	template<class T>
	static void readFromfile(vector<T>& v, const Block & b)
	{
		if (!fin.is_open())
			MyError("file isn't open");
		if (b.type != VECTOR)
			MyError("Incorrect Data Type");
		fin.seekg(b.begin);
		int size;
		fin.read((char*)&size, sizeof(int));
		if (size < 1)
			MyError("Invalid size");
		v.clear();
		v.reserve(size);
		Block* bs = new Block[size];
		fin.read((char*)bs, size*sizeof(Block));
		for (int i = 0; i < size; i++)
		{
			T t;
			readFromfile(t, bs[i]);
			v.push_back(t);
		}
		if (fin.tellg() - b.begin != b.size)
			MyError("Invalid Data End ");
		delete[] bs;
	}


	static void openR(string  fileName)
	{
		if (fin.is_open())
			fin.close();
		fin.open(fileName, ios::in | ios::binary);
		if (fin.fail())
			MyError("Can't open file to read.");
	}
	static void closeR()
	{
		if (fin.is_open())
			fin.close();
	}
	static void openW(string  fileName)
	{
		if (fout.is_open())
			fout.close();
		fout.open(fileName, ios::out | ios::binary);
		if (fout.fail())
			MyError("Can't open file to write.");
	}
	static void closeW()
	{
		if (fout.is_open())
			fout.close();
	}


	public:



	static void saveTofile(vector<StageRegressor>&R, vector<Shape>& initSet, Shape& meanShape, string  fileName)
	{
		if (R.size() <= 0 || initSet.size() <= 0 || meanShape.cols <= 0 || meanShape.rows <= 0)
			MyError("Invalid Data");
		openW(fileName);
		if (!fout.is_open())
			MyError("file isn't open");
		fout.seekp(0, ifstream::end);
		Block bs[3];
		fout.write((const char*)bs, 3 * sizeof(Block));
		saveTofile(R, bs[0]);
		saveTofile(initSet, bs[1]);
		saveTofile(meanShape, bs[2]);
		fout.seekp(0,ifstream::beg);
		fout.write((const char*)bs, 3 * sizeof(Block));
		closeW();
	}


	static void readFromfile(vector<StageRegressor>&R, vector<Shape>& initSet, Shape& meanShape, string  fileName)
	{
		openR(fileName);
		if (!fin.is_open())
			MyError("file isn't open");
		fin.seekg(0,ifstream::beg);
		Block bs[3];
		fin.read((char*)bs, 3 * sizeof(Block));
		readFromfile(R, bs[0]);
		readFromfile(initSet, bs[1]);
		readFromfile(meanShape, bs[2]);
		if (R.size() <= 0||initSet.size()<=0||meanShape.cols<=0||meanShape.rows<=0)
			MyError("Invalid Data");
		closeR();
	}


};
#endif