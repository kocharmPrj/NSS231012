#include "Common.h"
#include "WorldCoordinate.h"

const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";

int main()
{
	VideoCapture cap(0);//USB-1개
	if (!cap.isOpened())
	{
		cerr << "Camera open failed!" << endl;
		return -1;
	}

	Mat frame, frame_gray;
	WorldCoordinate _WorldCoordinate;

	while (true)
	{
		cap >> frame; //프레임 받아와서 매트릭스에 저장
		if (frame.empty())
		{
			break;
		}

		// matching pairs
		vector<Point3f> objectPoints;	// 3d world coordinates
		vector<Point2f> imagePoints;	// 2d image coordinates

		// Insert the 3D coordinates into the vector
		objectPoints.push_back(Point3f(0.0, 10, 0.0));
		objectPoints.push_back(Point3f(0.0, 0.0, 0.0));
		objectPoints.push_back(Point3f(10, 0.0, 0.0));
		objectPoints.push_back(Point3f(10, 10, 0.0));

		// Detect edges in the pixel coordinates
		imagePoints = _WorldCoordinate.DetectVertex(frame, objectPoints, imagePoints);
		vector<Point2f> cameraPosition;
		cameraPosition = _WorldCoordinate.FindCameraPosition(frame, objectPoints, imagePoints);



		imshow(window_capture_name, frame);
		
		char key = (char)waitKey(10); //30ms마다 프레임 받아옴 : waitKey
		if (key == 'q' || key == 27) //q 또는 Escape 키 누르면 탈출
		{
			break;
		}
		
	}
	
	return 1;
}
