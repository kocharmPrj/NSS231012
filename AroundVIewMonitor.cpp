#pragma once

#include "common.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
using namespace cv;
using namespace std;

class AVM {
protected:
	int CamNum = 0;
	VideoCapture* cap = new VideoCapture[4];  //연결 카메라 대수
	Mat* image = new Mat[4];
	Mat* temp = new Mat[4];
	string Name[4] = { "camera 0", "camera 1", "camera 2", "camera 3" };
	Mat combinedImage;

public:
	void CheckCamPort() {
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

	void RunCam() {
		for (int i = 0; i < CamNum; i++) {
			if (cap[i].isOpened()) {
				cap[i].read(image[i]);
			}
		}
	}

	void DisplayCombinedImage() {

		combinedImage = Mat(Size(960,960), CV_8UC3);
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

	AVM() {
		CheckCamPort();
		cout << "=== 생성자 호출 : 연결된 카메라 대수 : " << CamNum << "===\n";
	}

	~AVM() {
		delete[] cap;
		delete[] image;
	}
};

int main(int argc, char* argv[])
{

	AVM AVM;
	while (true) {
		AVM.RunCam();
		AVM.DisplayCombinedImage();

		char key = (char)cv::waitKey(1);
		if (key == 'q' || key == 27)
		{
			break;
		}
	}
	return 0;
}