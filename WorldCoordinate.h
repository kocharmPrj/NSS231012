#pragma once
<<<<<<< Updated upstream
#include <iostream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#ifdef _DEBUG
#pragma comment(lib, "opencv_world480d.lib")
#else
#pragma comment(lib, "opencv_world480.lib")
#endif
using namespace std;
using namespace cv;
=======
#include "common.h"
>>>>>>> Stashed changes

class WorldCoordinate
{
public:
	WorldCoordinate();
<<<<<<< Updated upstream
	~WorldCoordinate();
public:
	vector<Point2f> DetectVertex(Mat frame, vector<Point3f> objectPoints, vector<Point2f> imagePoints);
	vector<Point2f> CalculateDistance(Mat drawing, vector<Point3f> objectPoints, vector<Point2f> imagePoints);
=======
	WorldCoordinate(double fx, double fy, double cx, double cy, double k1, double k2, double p1, double p2);
	~WorldCoordinate();
public:
	vector<Point2f> DetectVertex(Mat frame, vector<Point3f> objectPoints, vector<Point2f> imagePoints);
	string CalculateDistance(Mat drawing, vector<Point3f> objectPoints, vector<Point2f> imagePoints);
>>>>>>> Stashed changes
private:
	double fx = 612.577383;
	double fy = 572.970525;
	double cx = 313.033322;
	double cy = 220.421716;
	double k1 = -0.056895;
	double k2 = 0.259353;
	double p1 = 0.004600;
	double p2 = 0.002048;
};


