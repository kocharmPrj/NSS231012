
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
        imshow("filterImg", filterImg);

        edgeImg = laneDetector.MakeEdge(filterImg);
        imshow("edgeImg", edgeImg);

        laneImg = frameImg.clone();
        boxP = laneDetector.FindBox(edgeImg);
        if (boxP.size() > 0) {
            laneImg = laneDetector.DrawLane(frameImg, boxP);
        }
        imshow("laneImg", laneImg);

        char key = static_cast<char>(waitKey(30));
        if (key == 'q' || key == 27)
            break;
    }

    if (false)
    {
        frameImg = imread("D:\\Project\\LaneDetector\\LaneDetector\\template.png");

        filterImg = laneDetector.FilterColors(frameImg);

        edgeImg = laneDetector.MakeEdge(filterImg);
        
        laneImg = frameImg.clone();
        boxP = laneDetector.FindBox(edgeImg);
        if (boxP.size() > 0) {
            laneImg = laneDetector.DrawLane(frameImg, boxP);
        }
        imshow("laneImg", laneImg);

    }

    return 0;
}
