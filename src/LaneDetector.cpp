
#define _CRT_SECURE_NO_WARNINGS
#include "LaneDetector.h"
#include <algorithm>
LaneDetector::LaneDetector()
	: linesP({})
	, xyLines({})
	, imgCols(0)
	, imgRows(0)
	, noBoxCnt(0)
	, boxPointList({}) 
	, xlineDetect(false)
	, ylineDetect(false)
	, xCenter(0)
	, yCenter(0)
	, obstacleFlag(0)
{
}

LaneDetector::~LaneDetector()
{
}


// <AVM> 
// input: 4개의 cam
// output: 960x960 combined image

// 1. 카메라 연결 확인
void LaneDetector::CheckCamPort() {
	int startFlag = 0;
	for (int i = 0; i < 4; i++) {
		cap[i] = VideoCapture(i*2);
		if (cap[i].isOpened()) {
			cout << "Opened Cam Port is : " << i << '\n';
			CamNum++;
			startFlag = 1;
		}
	}
	if (startFlag == 0) {
		cout << "No camera connected" << '\n';
		return;
	}
}

// 2. 카메라 실행
void LaneDetector::RunCam() {
	for (int i = 0; i < CamNum; i++) {
		if (cap[i].isOpened()) {
			cap[i].read(image[i]);
		}
	}
}

// 3. 이미지 컴바인
void LaneDetector::DisplayCombinedImage() {

	combinedImage = Mat(Size(960, 960), CV_8UC3);
	int width = combinedImage.cols;
	int height = combinedImage.rows;
	int centerX = width / 2;
	int centerY = height / 2;
	std::vector<cv::Point> pts1, pts2, pts3, pts4;
	//1번 좌측화면
	pts1.push_back(Point(centerX, centerY));
	pts1.push_back(Point(160, 160));
	pts1.push_back(Point(0, 160));
	pts1.push_back(Point(0, 800));
	pts1.push_back(Point(160, 800));
	//2번 전방화면
	pts2.push_back(Point(centerX, centerY));
	pts2.push_back(Point(160, 160));
	pts2.push_back(Point(160, 0));
	pts2.push_back(Point(800, 0));
	pts2.push_back(Point(800, 160));
	//3번 우측화면
	pts3.push_back(Point(centerX, centerY));
	pts3.push_back(Point(800, 160));
	pts3.push_back(Point(width, 160));
	pts3.push_back(Point(width, 800));
	pts3.push_back(Point(800, 800));
	//4번 후방화면
	pts4.push_back(Point(centerX, centerY));
	pts4.push_back(Point(160, 800));
	pts4.push_back(Point(160, height));
	pts4.push_back(Point(800, height));
	pts4.push_back(Point(800, 800));

	// 4개의 삼각형에 해당하는 마스크 이미지를 생성
	Mat subImage1 = Mat::zeros(height, width, combinedImage.type());
	Mat subImage2 = Mat::zeros(height, width, combinedImage.type());
	Mat subImage3 = Mat::zeros(height, width, combinedImage.type());
	Mat subImage4 = Mat::zeros(height, width, combinedImage.type());

	vector<vector<Point>> polygons1 = { pts1 };
	vector<vector<Point>> polygons2 = { pts2 };
	vector<vector<Point>> polygons3 = { pts3 };
	vector<vector<Point>> polygons4 = { pts4 };

	fillPoly(subImage1, polygons1, Scalar(255, 255, 255));
	fillPoly(subImage2, polygons2, Scalar(255, 255, 255));
	fillPoly(subImage3, polygons3, Scalar(255, 255, 255));
	fillPoly(subImage4, polygons4, Scalar(255, 255, 255));

	//Rect rect(80, 0, 480, 480);

	/*image[0] = image[0](rect);
	image[1] = image[1](rect);
	image[2] = image[2](rect);
	image[3] = image[3](rect);*/


	/*resize(image[0], image[0], Size(width / 2, height / 2));
	resize(image[1], image[1], Size(width / 2, height / 2));
	resize(image[2], image[2], Size(width / 2, height / 2));
	resize(image[3], image[3], Size(width / 2, height / 2));*/

	//빈 공간 줄이기
	//화면 회전
	rotate(image[2], image[2], ROTATE_90_COUNTERCLOCKWISE);
	rotate(image[1], image[1], ROTATE_90_CLOCKWISE);
	rotate(image[3], image[3], ROTATE_180);

	Mat subH = subImage1 + subImage3;
	Mat subV = subImage2 + subImage4;
	

	Mat comH = Mat(Size(width, height), CV_8UC3);
	Mat comV = Mat(Size(width, height), CV_8UC3);

	hconcat(image[2], image[1], comH);  // 좌측과 우측 결합
	vconcat(image[0], image[3], comV);  // 상단과 하단 결합

	//사각형 mask 생성
	Rect horizental(0, 160, 960, 640);
	Rect vertical(160, 0, 640, 960);
	
	comH.copyTo(subH(horizental), subH(horizental));
	comV.copyTo(subV(vertical), subV(vertical));

	combinedImage = subH + subV;

	
	waitKey(1);
}


// <lane detection>
// input: 960x960 combined image
// output: 주차장 4개 point, 주차 가능 여부 flag

// 1. 차선 색선택 fitering
Mat LaneDetector::FilterColors(Mat img)
{
	//bilateralfiltering을 거친 후 흰색/노란색 색상의 범위를 정해 해당되는 차선을 필터링
	Mat output = Mat::zeros(img.size(), CV_8UC1);
	imgCols = img.cols;
	imgRows = img.rows;
	UMat hsvImg;
	UMat whiteMask, whiteImg;
	UMat yellowMask, yellowImg;
	img.copyTo(output);

	//차선 색깔 범위 
	Scalar lowerWhite = Scalar(200, 200, 200); //흰색 차선 (RGB)
	Scalar upperWhite = Scalar(255, 255, 255);
	Scalar lowerYellow = Scalar(10, 100, 140); //노란색 차선 (HSV)
	Scalar upperYellow = Scalar(40, 255, 255);

	// bilateralfilter(opencv내장함수)
	bilateralFilter(img, output, 10, 50, 50);

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
Mat LaneDetector::MakeEdge(Mat img) {
	Mat output;
	cv::Canny(img, output, 50, 150);

	return output;
}

// 3. 직선 감지 및 ROI 설정
// line 추출 및 평행 수직 line seperate
void LaneDetector::GetXYLine(Mat img) {
	cv::HoughLinesP(img, linesP, 1, (CV_PI / 180), 100, 10, 200);

	Point start, end;
	vector<Vec4i> xlines, ylines;

	for (size_t i = 0; i < linesP.size(); i++) {
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
vector<Point> LaneDetector::FindBox() {
	filterImg = FilterColors(combinedImage);
	edgeImg = MakeEdge(filterImg);
	GetXYLine(edgeImg);

	vector<vector<Point>> xline, yline;
	vector<Point> output;
	Vec4i fitXline, fitYline;
	vector<double> xm, ym;
	vector<Point> xb, yb;

	if (xlineDetect) {
		for(size_t i = 0; i< xyLines[0].size(); i++) {
			vector<Point> temp;
			temp.push_back(Point(xyLines[0][i][0], xyLines[0][i][1]));
			xline.push_back(temp);
		}
		if (xline.size() > 0) {
			for (size_t i = 0; i < xline.size(); i++) {
				fitLine(xline[i], fitXline, DIST_L2, 0, 0.01, 0.01);
				xm.push_back(fitXline[1] / fitXline[0]);
				xb.push_back(Point(fitXline[2], fitXline[3]));
			}
		}
	}
	if (ylineDetect) {
		for (size_t i = 0; i < xyLines[1].size(); i++) {
			vector<Point> temp;
			temp.push_back(Point(xyLines[1][i][0], xyLines[1][i][1]));
			yline.push_back(temp);
		}
		if (yline.size() > 0) {
			for (size_t i = 0; i < yline.size(); i++) {
				fitLine(yline[i], fitYline, DIST_L2, 0, 0.01, 0.01);
				ym.push_back(fitYline[1] / fitYline[0]);
				yb.push_back(Point(fitYline[2], fitYline[3]));
			}
		}
	}

	if (xb.size() > 3 && yb.size() > 0) {
		sort(xb.begin(), xb.end(), cmpY);
		sort(yb.begin(), yb.end(), cmpX);
		double area = (xb[2].y - xb[1].y) * yb[0].x;
		if (30000 < area && area < 150000) {
			vector<Point> temp;
			temp.push_back(Point(yb[0].x, xb[1].y));
			temp.push_back(Point(0, xb[1].y));
			temp.push_back(Point(0, xb[2].y));
			temp.push_back(Point(yb[0].x, xb[2].y));
			boxPointList.push_back(temp);
			noBoxCnt = 0;
		} 
		else noBoxCnt += 1;
	}
	else noBoxCnt += 1;
	
	if (noBoxCnt == 10) {
		output = {};
		boxPointList = {};
		noBoxCnt = 0;
	}
	if (boxPointList.size() > 0)
		output = boxPointList.back();

	return output;
}
// ROI lane 색칠
Mat LaneDetector::DrawLane(vector<Point> boxPoints) {
	if(boxPoints.size() > 0){
		fillConvexPoly(combinedImage, boxPoints, Scalar(0, 230, 30), LINE_AA, 0);
		line(combinedImage, boxPoints[0], boxPoints[1], Scalar(0, 0, 255), 2, LINE_AA);
		line(combinedImage, boxPoints[2], boxPoints[3], Scalar(0, 0, 255), 2, LINE_AA);
		line(combinedImage, boxPoints[3], boxPoints[0], Scalar(0, 0, 255), 2, LINE_AA);
		addWeighted(combinedImage, 0.3, combinedImage, 0.7, 0, combinedImage);
	}
	return combinedImage;
}

// 4. obastacle detection 
int LaneDetector::DetectObstacle(vector<Point> boxPoints) {
	if (noBoxCnt > 0) 
		return 3;
	double xLen = boxPoints[3].x - boxPoints[1].x;
	double yLen = boxPoints[3].y - boxPoints[1].y;
	xCenter = (boxPoints[3].x + boxPoints[1].x) / 2;
	yCenter = (boxPoints[3].y + boxPoints[1].y) / 2;
	double xPadding = xLen * 0.3;
	double yPadding = yLen * 0.3;
	vector<Point> roi;
	Mat maskedImg, obstacleImg;
	uchar pixelSum = 0;
	
	roi.push_back(Point(xCenter + xPadding, yCenter - yPadding));
	roi.push_back(Point(xCenter - xPadding, yCenter - yPadding));
	roi.push_back(Point(xCenter - xPadding, yCenter + yPadding));
	roi.push_back(Point(xCenter + xPadding, yCenter + yPadding));

	maskedImg = Mat::zeros(edgeImg.rows, edgeImg.cols, CV_8UC1);
	fillConvexPoly(maskedImg, roi, Scalar(255, 255, 255), LINE_8);
	bitwise_and(edgeImg, maskedImg, obstacleImg);

	for (int row = 0; row < obstacleImg.rows; row++) {
		for (int col = 0; col < obstacleImg.cols; col++) {
			pixelSum += obstacleImg.data[row * obstacleImg.cols + col];
		}
	}

	if (pixelSum > 10) obstacleFlag = 1;
	else obstacleFlag = 2;

	return obstacleFlag;
}

// 5. start moving flag
bool LaneDetector::StartMoving() {
	if (yCenter > 420 && yCenter <550)
		return true;
	else
		return false;
}
