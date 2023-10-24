
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "LaneDetector.h"

using namespace cv;

// input: 960x960 avm
// output: 주차장 4개 point, 주차 가능 flag
int main()
{
    LaneDetector laneDetector;
    Mat frameImg, laneImg;
    vector<vector<Point>> boxP;

    //VideoCapture cap("D:\\Project\\LaneDetector\\LaneDetector\\test.mp4");
    VideoCapture cap(0);

    if (!cap.isOpened())
    {
        cout << "Error!" << endl;
        return -1;
    }

    while (true)
    {
        cap >> frameImg;

        laneImg = frameImg.clone();
        boxP = laneDetector.FindBox(frameImg);
        if (boxP.size() > 0) 
            laneImg = laneDetector.DrawLane(frameImg, boxP);
        
        bool flag = laneDetector.DetectObstacle(boxP);
        if (flag == true)
            putText(laneImg, "Can't parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
        else if (flag == false)
            putText(laneImg, "Can parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);

        imshow("laneImg", laneImg);

        char key = static_cast<char>(waitKey(30));
        if (key == 'q' || key == 27)
            break;
    }

    if (false)
    {
        frameImg = imread("D:\\Project\\LaneDetector\\LaneDetector\\template.png");

        laneImg = frameImg.clone();
        boxP = laneDetector.FindBox(frameImg);
        if (boxP.size() > 0) {
            laneImg = laneDetector.DrawLane(frameImg, boxP);
        }
        imshow("laneImg", laneImg);
    }

    return 0;
}
