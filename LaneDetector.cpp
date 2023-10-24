
#include "LaneDetector.h"
#include <algorithm>
LaneDetector::LaneDetector()
	: linesP({})
	, xyLines({})
	, imgCols(0)
	, imgRows(0)
	, xlineDetect(false)
	, ylineDetect(false)
{
}

LaneDetector::~LaneDetector()
{
}

// 1. 차선 색선택 fitering
Mat LaneDetector::FilterColors(Mat frameImg)
{
	//bilateralfiltering을 거친 후 흰색/노란색 색상의 범위를 정해 해당되는 차선을 필터링
	Mat output = Mat::zeros(frameImg.size(), CV_8UC1);
	imgCols = frameImg.cols;
	imgRows = frameImg.rows;
	UMat hsvImg;
	UMat whiteMask, whiteImg;
	UMat yellowMask, yellowImg;
	frameImg.copyTo(output);

	//차선 색깔 범위 
	Scalar lowerWhite = Scalar(200, 200, 200); //흰색 차선 (RGB)
	Scalar upperWhite = Scalar(255, 255, 255);
	Scalar lowerYellow = Scalar(10, 100, 140); //노란색 차선 (HSV)
	Scalar upperYellow = Scalar(40, 255, 255);

	// bilateralfilter(opencv내장함수)
	bilateralFilter(frameImg, output, 10, 50, 50);

	//흰색 필터링
	inRange(output, lowerWhite, upperWhite, whiteMask);
	bitwise_and(output, output, whiteImg, whiteMask);

	cvtColor(output, hsvImg, COLOR_BGR2HSV);

	//노란색 필터링
	inRange(hsvImg, lowerYellow, upperYellow, yellowMask);
	bitwise_and(output, output, yellowImg, yellowMask);

	//두 영상을 합친다.
	addWeighted(whiteImg, 1.0, yellowImg, 1.0, 0.0, output);

	return output;
}

// 2. edge 추출
Mat LaneDetector::MakeEdge(Mat frameImg) {
	Mat output;
	cv::Canny(frameImg, output, 50, 150);

	return output;
}

// 3. 직선 감지 및 ROI 설정
// line 추출 및 평행 수직 line seperate
void LaneDetector::GetXYLine(Mat img) {
	cv::HoughLinesP(img, linesP, 1, (CV_PI / 180), 100, 10, 200);

	Point start, end;
	vector<Vec4i> xlines, ylines;

	for (int i = 0; i < linesP.size(); i++) {
		Vec4i line = linesP[i];
		start = Point(line[0], line[1]);
		end = Point(line[2], line[3]);
		if (abs(static_cast<double>(end.y) - static_cast<double>(start.y)) < 30) {
			xlines.push_back(linesP[i]);
			xlineDetect = true;
		}
		else if (abs(static_cast<double>(end.x) - static_cast<double>(start.x)) < 30) {
			ylines.push_back(linesP[i]);
			ylineDetect = true;
		}
	}
	xyLines = {};
	xyLines.push_back(xlines);
	xyLines.push_back(ylines);	
}
bool LaneDetector::cmpX(Point a, Point b) {
	return a.x < b.x;
}
bool LaneDetector::cmpY(Point a, Point b) {
	return a.y < b.y;
}
// ROI box point 추출
vector<vector<Point>> LaneDetector::FindBox(Mat edgeImg) {
	GetXYLine(edgeImg);

	vector<vector<Point>> xline, yline;
	vector<vector<Point>> output;
	Vec4i fitXline, fitYline;
	vector<double> xm, ym;
	vector<Point> xb, yb;

	if (xlineDetect) {
		for(int i = 0; i< xyLines[0].size(); i++) {
			vector<Point> temp;
			temp.push_back(Point(xyLines[0][i][0], xyLines[0][i][1]));
			xline.push_back(temp);
		}
		if (xline.size() > 0) {
			for (int i = 0; i < xline.size(); i++) {
				fitLine(xline[i], fitXline, DIST_L2, 0, 0.01, 0.01);
				xm.push_back(fitXline[1] / fitXline[0]);
				xb.push_back(Point(fitXline[2], fitXline[3]));
			}
		}
	}
	if (ylineDetect) {
		for (int i = 0; i < xyLines[1].size(); i++) {
			vector<Point> temp;
			temp.push_back(Point(xyLines[1][i][0], xyLines[1][i][1]));
			yline.push_back(temp);
		}
		if (yline.size() > 0) {
			for (int i = 0; i < yline.size(); i++) {
				fitLine(yline[i], fitYline, DIST_L2, 0, 0.01, 0.01);
				ym.push_back(fitYline[1] / fitYline[0]);
				yb.push_back(Point(fitYline[2], fitYline[3]));
			}
		}
	}

	if (xb.size() > 1 && yb.size() > 0) {
		sort(xb.begin(), xb.end(), cmpY);
		sort(yb.begin(), yb.end(), cmpX);
		for (int i = 1; i < xb.size() - 2; i+=2) {
			double area = (xb[i + 1].y - xb[i].y) * yb[0].x;
			if (30000 < area) {
				vector<Point> temp;
				temp.push_back(Point(yb[0].x, xb[i].y));
				temp.push_back(Point(0, xb[i].y));
				temp.push_back(Point(0, xb[i + 1].y));
				temp.push_back(Point(yb[0].x, xb[i + 1].y));
				output.push_back(temp);
			}
		}
	}
	return output;
}
// ROI lane 색칠
Mat LaneDetector::DrawLane(Mat frameImg, vector<vector<Point>> boxPoints) {
	Mat output = frameImg.clone();

	for (int i = 0; i < boxPoints.size(); i++) {
		fillConvexPoly(output, boxPoints[i], Scalar(0, 230, 30), LINE_AA, 0);
		addWeighted(output, 0.3, frameImg, 0.7, 0, output);
		line(output, boxPoints[i][0], boxPoints[i][1], Scalar(0, 0, 255), 2, LINE_AA);
		line(output, boxPoints[i][2], boxPoints[i][3], Scalar(0, 0, 255), 2, LINE_AA);
		line(output, boxPoints[i][3], boxPoints[i][0], Scalar(0, 0, 255), 2, LINE_AA);
	}

	return output;
}
