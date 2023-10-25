
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "LaneDetector.h"

using namespace cv;


int main() {
    LaneDetector laneDetector;
    Mat frameImg, laneImg;
    vector<vector<Point>> boxP;

    laneDetector.CheckCamPort();

    while (true) {
        laneDetector.RunCam();
        laneDetector.DisplayCombinedImage();

        boxP = laneDetector.FindBox();
        laneImg = laneDetector.DrawLane(boxP);

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

    return 0;
}
