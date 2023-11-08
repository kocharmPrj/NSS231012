#include "socketForPc.hpp"
#include "LaneDetector.h"
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

std::string g_string;
std::string g_endString;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

using namespace cv;

int main(){
    pthread_t t_id[1];
    pthread_create(t_id, NULL, TX_thread, NULL);
    pthread_detach(t_id[0]);

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
                        putText(laneImg, "Can't parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 255), 2);
                        cout << "can't parking" << endl;
                }
                else if (flag == 2){
                        putText(laneImg, "Can parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 255, 255), 2);
                        startMovingFlag = laneDetector.StartMoving();
			// here?
                        if (startMovingFlag == true) {
                                preCnt += 1;
                                if (preCnt >= 3){
					// or here?
                                        cout << "can parking" << boxP << endl;
                                        cout << "moving" << endl;
                                        preCnt = 0;
					// 3 Lines Under to Ctrl bot
					pthread_mutex_lock(&g_mutex);
					g_string = "6 11"; // SIGNAL AS STOP
					pthread_mutex_unlock(&g_mutex);
					// Until here
                                }
                        }
                }
        }
        imshow("laneImg", laneImg);

        char key = static_cast<char>(waitKey(30));
        if (key == 'q' || key == 27){
	    pthread_mutex_lock(&g_mutex);
	    g_string = "q";
	    pthread_mutex_unlock(&g_mutex);
            break;
	}
    }
    
    pthread_testcancel(); // or use a different function

    return 0;
}
