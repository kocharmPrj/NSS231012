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

	void CheckCamPort();
	void RunCam();
	void DisplayCombinedImage();

	Mat FilterColors(Mat img);
	Mat MakeEdge(Mat img);
	static bool cmpX(Point a, Point b);
	static bool cmpY(Point a, Point b);
	void GetXYLine(Mat img);
	vector<Point> FindBox();
	Mat DrawLane(vector<Point> boxPoints);
	int DetectObstacle(vector<Point> boxPoints);
	bool StartMoving();

private:
	int CamNum = 0;
	VideoCapture* cap = new VideoCapture[4];
	Mat* image = new Mat[4];
	Mat* temp = new Mat[4];
	string Name[4] = { "camera 0", "camera 1", "camera 2", "camera 3" };
	Mat combinedImage;
	
	Mat filterImg, edgeImg, laneImg;
	vector<Vec4i> linesP;
	vector<vector<Vec4i>> xyLines;
	int imgCols, imgRows, noBoxCnt;
	vector<vector<Point>> boxPointList;
	bool xlineDetect, ylineDetect;
	double xCenter, yCenter;
	int obstacleFlag;
};


//#endif // _LANE_DETECTOR_
