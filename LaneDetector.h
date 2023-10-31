#pragma once
//#ifndef _LANE_DETECTOR_
//#define _LANE_DETECTOR_
//
//#define IMSHOW_ROI
//#ifdef _DEBUG
//#define IMSHOW_FRAME
////#define IMSHOW_TOP
//#define IMSHOW_FILTER
////#define IMSHOW_EDGE
////#define IMSHOW_HISTO
////#define DRAW_POINT_TOP
////#define HSV_TRACK_BAR
//#define IMSHOW_ROI
//#define IMSHOW_LINE
//#endif // _DEBUG

#include "OpencvInit.h"

using namespace std;
using namespace cv;

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


//#endif // _LANE_DETECTOR_
