
#define _CRT_SECURE_NO_WARNINGS
#include "LaneDetector.h"
#include <algorithm>
LaneDetector::LaneDetector()
	: linesP({})
	, xyLines({})
	, imgCols(0)
	, imgRows(0)
	, xlineDetect(false)
	, ylineDetect(false)
	, obstacleFlag(false)
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
		cap[i] = VideoCapture(i);
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
	pts1.push_back(Point(240, 240));
	pts1.push_back(Point(0, 240));
	pts1.push_back(Point(0, 720));
	pts1.push_back(Point(240, 720));
	//2번 전방화면
	pts2.push_back(Point(centerX, centerY));
	pts2.push_back(Point(240, 240));
	pts2.push_back(Point(240, 0));
	pts2.push_back(Point(720, 0));
	pts2.push_back(Point(720, 240));
	//3번 우측화면
	pts3.push_back(Point(centerX, centerY));
	pts3.push_back(Point(720, 240));
	pts3.push_back(Point(width, 240));
	pts3.push_back(Point(width, 720));
	pts3.push_back(Point(720, 720));
	//4번 후방화면
	pts4.push_back(Point(centerX, centerY));
	pts4.push_back(Point(240, 720));
	pts4.push_back(Point(240, height));
	pts4.push_back(Point(720, height));
	pts4.push_back(Point(720, 720));

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

	resize(image[0], image[0], Size(width / 2, height / 2));
	resize(image[1], image[1], Size(width / 2, height / 2));
	resize(image[2], image[2], Size(width / 2, height / 2));
	resize(image[3], image[3], Size(width / 2, height / 2));

	//화면 회전
	Point2f center(image[1].cols / 2.0, image[1].rows / 2.0);
	Mat right = getRotationMatrix2D(center, -90, 1.0);
	Mat left = getRotationMatrix2D(center, 90, 1.0);
	warpAffine(image[0], image[0], left, image[1].size());
	warpAffine(image[2], image[2], right, image[1].size());

	Mat bottom = getRotationMatrix2D(center, 180, 1.0);
	warpAffine(image[3], image[3], bottom, image[1].size());

	Mat subH = subImage1 + subImage3;
	Mat subV = subImage2 + subImage4;

	Mat comH = Mat(Size(width, height), CV_8UC3);
	Mat comV = Mat(Size(width, height), CV_8UC3);

	hconcat(image[0], image[2], comH);  // 좌측과 우측 결합
	vconcat(image[1], image[3], comV);  // 상단과 하단 결합

	//사각형 mask 생성
	Rect horizental(0, 240, 960, 480);
	Rect vertical(240, 0, 480, 960);

	comH.copyTo(subH(horizental), subH(horizental));
	comV.copyTo(subV(vertical), subV(vertical));

	combinedImage = subH + subV;

	imshow("Combined Frame", combinedImage);

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
vector<vector<Point>> LaneDetector::FindBox() {
	filterImg = FilterColors(combinedImage);
	edgeImg = MakeEdge(filterImg);
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
Mat LaneDetector::DrawLane(vector<vector<Point>> boxPoints) {
	Mat output = combinedImage.clone();

	if (boxPoints.size() > 0) {
		for (int i = 0; i < boxPoints.size(); i++) {
			fillConvexPoly(output, boxPoints[i], Scalar(0, 230, 30), LINE_AA, 0);
			line(output, boxPoints[i][0], boxPoints[i][1], Scalar(0, 0, 255), 2, LINE_AA);
			line(output, boxPoints[i][2], boxPoints[i][3], Scalar(0, 0, 255), 2, LINE_AA);
			line(output, boxPoints[i][3], boxPoints[i][0], Scalar(0, 0, 255), 2, LINE_AA);
		}
		addWeighted(output, 0.3, combinedImage, 0.7, 0, output);
	}

	return output;
}

// 4. 장애물 감지
bool LaneDetector::DetectObstacle(vector<vector<Point>> boxPoints) {
	for (int i = 0; i < boxPoints.size(); i++) {
		double xLen = boxPoints[i][3].x - boxPoints[i][1].x;
		double yLen = boxPoints[i][3].y - boxPoints[i][1].y;
		double xCenter = (boxPoints[i][3].x + boxPoints[i][1].x) / 2;
		double yCenter = (boxPoints[i][3].y + boxPoints[i][1].y) / 2;
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

		imshow("obstacleImg", obstacleImg);

		for (int row = 0; row < obstacleImg.rows; row++) {
			for (int col = 0; col < obstacleImg.cols; col++) {
				pixelSum += obstacleImg.data[row * obstacleImg.cols + col];
			}
		}

		char mystr_1[30];
		sprintf(mystr_1, "pixelSum : %d", pixelSum);
		putText(obstacleImg, mystr_1, Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
		imshow("obstacleImg", obstacleImg);

		if (pixelSum > 10) obstacleFlag = true;
		else obstacleFlag = false;
	}

	return obstacleFlag;
}