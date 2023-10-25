#pragma once

#include "OpencvInit.h"

using namespace std;
using namespace cv;

class LaneDetector {
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
	vector<vector<Point>> FindBox();
	Mat DrawLane(vector<vector<Point>> boxPoints);
	bool DetectObstacle(vector<vector<Point>> boxPoints);

private:
	int CamNum = 0;
	VideoCapture* cap = new VideoCapture[4];  //연결 카메라 대수
	Mat* image = new Mat[4];
	Mat* temp = new Mat[4];
	string Name[4] = { "camera 0", "camera 1", "camera 2", "camera 3" };
	Mat combinedImage;

	Mat filterImg, edgeImg, laneImg;
	vector<Vec4i> linesP;
	vector<vector<Vec4i>> xyLines;
	int imgCols, imgRows;
	bool xlineDetect, ylineDetect, obstacleFlag;
};


//#endif // _LANE_DETECTOR_
