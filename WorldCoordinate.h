#pragma once

#include "Common.h"

class WorldCoordinate
{
public:
	WorldCoordinate();
	~WorldCoordinate();
public:
	Mat FindCameraParameters(double fx, double fy, double cx, double cy);
	Mat FindDistortionParameters(double k1, double k2, double p1, double p2);
	vector<Point2f> DetectVertex(Mat frame, vector<Point3f> &objectPoints, vector<Point2f> &imagePoints, Mat camParam, Mat distCoeffs);
	vector<Point2f> FindCameraPosition(vector<Point3f> &objectPoints, vector<Point2f> &imagePoints, Mat camParam, Mat distCoeffs);
	void drawingWorldCoordinates(Mat drawing, vector<Point3f> objectPoints, vector<Point2f> imagePoints, Mat camParam, Mat distCoeffs);


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


