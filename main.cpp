#include "LaneDetector.h"
#include "WorldCoordinate.h"

const String window_capture_name = "Video Capture";
const String window_detection_name = "Object Detection";

int main()
{
	LaneDetector* _LaneDetector = new LaneDetector;
	WorldCoordinate* _WorldCoordinate = new WorldCoordinate;

	Mat frameImg, laneImg;
	vector<Point> boxP;
	vector<vector<Point>> boxPList;
	int cnt = 0;

	VideoCapture cap(0);//USB-1��
	if (!cap.isOpened())
	{
		cerr << "Camera open failed!" << endl;
		return -1;
	}

	while (true)
	{
		cap >> frameImg; //������ �޾ƿͼ� ��Ʈ������ ����
		if (frameImg.empty())
		{
			break;
		}

		laneImg = frameImg.clone();
		boxP = _LaneDetector->FindBox(frameImg);

		if (boxP.size() > 0)
		{
			int flag = _LaneDetector->DetectObstacle(boxP);

			if (flag == 1) {
				laneImg = _LaneDetector->DrawLane(frameImg, boxP);
				putText(laneImg, "Can't parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
				cout << flag << ":" << boxP << endl;
			}
			else if (flag == 2) {
				laneImg = _LaneDetector->DrawLane(frameImg, boxP);
				putText(laneImg, "Can parking", Point(80, 80), FONT_HERSHEY_COMPLEX, 1, (255, 255, 255), 2);
				cout << flag << ":" << boxP << endl;
			}
		}

		// matching pairs
		vector<Point3f> objectPoints;	// 3d world coordinates
		vector<Point2f> imagePoints;	// 2d image coordinates

		// Insert the 3D coordinates into the vector
		objectPoints.push_back(Point3f(0.0, 20, 0.0));
		objectPoints.push_back(Point3f(0.0, 0.0, 0.0));
		objectPoints.push_back(Point3f(20, 0.0, 0.0));
		objectPoints.push_back(Point3f(20, 20, 0.0));

		// Insert the 2D coordinates into the vector
		for (const auto& point : boxP) {
			// Convert each Point to Point2f and store in the new vector
			imagePoints.emplace_back(static_cast<float>(point.x), static_cast<float>(point.y));
		}



		// ���⿡�� �̹��� �ȼ���ǥ �� ����ִ��� Ȯ��
		if (imagePoints.empty())
		{
			imshow(window_detection_name, laneImg);
		}
		else
		{
			string distanceString;
			distanceString = _WorldCoordinate->CalculateDistance(laneImg, objectPoints, imagePoints);
			imshow(window_detection_name, laneImg);
		}

		imshow(window_capture_name, frameImg);
		imshow(window_detection_name, laneImg);

		char key = (char)waitKey(30); //30ms���� ������ �޾ƿ� : waitKey
		if (key == 'q' || key == 27) //q �Ǵ� Escape Ű ������ Ż��
		{
			break;
		}
		
	}
	
	delete _WorldCoordinate;
	delete _LaneDetector;

	return 1;
}
