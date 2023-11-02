
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
			cout << flag << ":" <<  boxP << endl;
		}
		else if (flag == 2){
            		putText(laneImg, "Can parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
			cout << flag << ":" << boxP << endl;
			if (laneDetector.StartMoving())
				cout << "moving" << endl;
		}
	}
        imshow("laneImg", laneImg);

        char key = static_cast<char>(waitKey(30));
        if (key == 'q' || key == 27)
            break;
    }

    return 0;
}
