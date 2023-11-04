
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

    laneDetector.CheckCamPort();
    
    int preCnt = 0;
    bool startMovingFlag;

    while (true)
    {
	laneDetector.RunCam();
        laneDetector.DisplayCombinedImage();
	
        boxP = laneDetector.FindBox();
	
	laneImg = laneDetector.DrawLane(boxP);
	
	if (boxP.size() > 0) 
	{
	   	int flag = laneDetector.DetectObstacle(boxP);
		
		if (flag == 1){			
			putText(laneImg, "Can't parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
			cout << "can't parking" << endl;
		}
		else if (flag == 2){
            		putText(laneImg, "Can parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
			startMovingFlag = laneDetector.StartMoving();
			if (startMovingFlag == true) { 
			        preCnt += 1;
				if (preCnt >= 3){
					cout << "can parking" << boxP << endl;
					cout << "moving" << endl;
					preCnt = 0;
				}
			}
		}
	}
        imshow("laneImg", laneImg);

        char key = static_cast<char>(waitKey(30));
        if (key == 'q' || key == 27)
            break;
    }

    return 0;
}
