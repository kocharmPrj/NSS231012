#pragma once
#include "common.h"

class LaneDetector
{
public:
	LaneDetector();
	~LaneDetector();

	Mat FilterColors(Mat img);
	Mat MakeEdge(Mat img);
	static bool cmpX(Point a, Point b);
	static bool cmpY(Point a, Point b);
	void GetXYLine(Mat img);
	vector<Point> FindBox(Mat img);
	Mat DrawLane(Mat img, vector<Point> boxPoints);
	int DetectObstacle(vector<Point> boxPoints);

private:
	Mat filterImg, edgeImg, laneImg;
	vector<Vec4i> linesP;
	vector<vector<Vec4i>> xyLines;
	int imgCols, imgRows, noBoxCnt;
	vector<vector<Point>> boxPointList;
	bool xlineDetect, ylineDetect;
	int obstacleFlag;
};
