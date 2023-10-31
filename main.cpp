
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
    vector<Point> boxP;
    vector<vector<Point>> boxPList;
    int cnt = 0;

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
	{
	   	int flag = laneDetector.DetectObstacle(boxP);
		
		if (flag == 1){			
			laneImg = laneDetector.DrawLane(frameImg, boxP);
			putText(laneImg, "Can't parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
			cout << flag << ":" <<  boxP << endl;
		}
		else if (flag == 2){
			laneImg = laneDetector.DrawLane(frameImg, boxP);
            		putText(laneImg, "Can parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
			cout << flag << ":" << boxP << endl;
		}
	}
        imshow("laneImg", laneImg);

        char key = static_cast<char>(waitKey(30));
        if (key == 'q' || key == 27)
            break;
    }

    return 0;
}
