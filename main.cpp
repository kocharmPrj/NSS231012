
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "LaneDetector.h"

using namespace cv;

int main()
{
    LaneDetector laneDetector;
    Mat frameImg, filterImg, edgeImg, laneImg;
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

        filterImg = laneDetector.FilterColors(frameImg);
        imshow("img_filter", filterImg);

        edgeImg = laneDetector.MakeEdge(filterImg);
        imshow("img_edges", edgeImg);

        laneImg = frameImg.clone();
        boxP = laneDetector.FindBox(edgeImg);
        if (boxP.size() > 0) {
            laneImg = laneDetector.DrawLane(frameImg, boxP);
        }
        imshow("img_lane", laneImg);

        char key = static_cast<char>(waitKey(30));
        if (key == 'q' || key == 27)
            break;
    }

    if (false)
    {
        frameImg = imread("D:\\Project\\LaneDetector\\LaneDetector\\template.png");


        // 1. filtering
        filterImg = laneDetector.FilterColors(frameImg);
        //imshow("img_filter", img_filter);


        // 2. canny edge 추출
        edgeImg = laneDetector.MakeEdge(filterImg);
        

        // 3. hought transform 직선 감지

        vector<vector<Point>> boxP;
        laneImg = frameImg.clone();
        boxP = laneDetector.FindBox(edgeImg);
        if (boxP.size() > 0) {
            laneImg = laneDetector.DrawLane(frameImg, boxP);
        }
        imshow("img_lane", laneImg);

    }

    return 0;
}
