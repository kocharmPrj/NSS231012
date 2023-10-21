#include "WorldCoordinate.h"

WorldCoordinate::WorldCoordinate()
{
	std::cout << "WorldCoordinate::Ctor" << std::endl;
}

WorldCoordinate::~WorldCoordinate()
{
	std::cout << "WorldCoordinate::Dtor" << std::endl;
}

vector<Point2f> WorldCoordinate::DetectVertex(Mat frame, vector<Point3f> objectPoints, vector<Point2f> imagePoints)
{
	Mat frame_gray;
	// Convert the image to grayscale
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

	// Detect edges in the image
	Mat edges;
	Canny(frame_gray, edges, 50, 500);

	// Find contours in the edge image
	vector<vector<Point>> contours;
	findContours(edges, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// Loop through the contours and find rectangles and world coordinates
	for (size_t i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);

		vector<Point> approx;
		approxPolyDP(contours[i], approx, arcLength(contours[i], true) * 0.15, true);

		if (approx.size() == 4)
		{
			if (area < 500) continue;
			// If the contour has 4 vertices, it's likely a rectangle
			for (size_t j = 0; j < 4; j++)
			{
				// Insert the 2D coordinates into the vector
				imagePoints.push_back(approx[j]);
			}
		}
	}
	return imagePoints;
}

vector<Point2f> WorldCoordinate::CalculateDistance(Mat drawing, vector<Point3f> objectPoints, vector<Point2f> imagePoints)
{
	// camera parameters
	double m[] = { fx, 0, cx, 0, fy, cy, 0, 0, 1 };	// intrinsic parameters
	Mat A(3, 3, CV_64FC1, m);	// camera matrix

	// distortion parameters
	double d[] = { k1, k2, p1, p2 };	// k1,k2: radial distortion, p1,p2: tangential distortion
	Mat distCoeffs(4, 1, CV_64FC1, d);

	// estimate camera pose
	Mat rvec, tvec;	// rotation & translation
	solvePnP(objectPoints, imagePoints, A, distCoeffs, rvec, tvec);

	// extract rotation matrix
	Mat R;
	Rodrigues(rvec, R);
	Mat R_inv = R.inv();

	// camera position (X,Y,Z)
	Mat Cam_pos = -R_inv * tvec;
	double* cworld = (double*)Cam_pos.data;

	// camera position
	double X, Y, Z;
	X = cworld[0];
	Y = cworld[1];
	Z = cworld[2];

	// Loop to draw markers and text on the image
	for (int i = 0; i < objectPoints.size(); i++)
	{
		//Converts pixel coordinates to normalized coordinates
		double u, v;
		u = (imagePoints[i].x - cx) / fx;
		v = (imagePoints[i].y - cy) / fy;
		Mat Pc = (Mat_<double>(3, 1) << u, v, 1); // 

		//Get world coordinate of P axis
		Mat Pw = R_inv * (Pc - tvec);	// world coordinate of P axis
		//Mat Cw = R_inv * (-tvec);		// world coordinate of Camera axis
		double* pworld = (double*)Pw.data;
		//double* cworld = (double*)Cam_pos.data;

		// Get the coordinate of P
		double k = (-cworld[2]) / (pworld[2] - cworld[2]);
		Mat P = Cam_pos + k * (Pw - Cam_pos);
		double* pp = (double*)P.data;

		// Convert 3D point to a string
		ostringstream text;
		text << "(" << fixed << setprecision(2) << pp[0] << ", " << fixed << setprecision(2) << pp[1] << ", " << fixed << setprecision(2) << pp[2] << ")";

		// Add 3D coordinates as text near each vertex
		cv::putText(drawing, text.str(), Point(imagePoints[i].x, imagePoints[i].y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 0, 255), 1);

		// Add Camera position as text
		ostringstream cam_pose_text;
		cam_pose_text << "cam position (" << X << ", " << Y << ", " << Z << " )";
		cv::putText(drawing, cam_pose_text.str(), Point(10, 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 255), 2);
	}

	double calculateX, calculateY;
	calculateX = 5 - X;
	calculateY = 5 - Y;

	vector<Point2f> calDist;
	calDist.push_back(Point(calculateX, calculateY));

	ostringstream distance;
	distance << "move X : " << calculateX << ", move Y : " << calculateY;
	cv::putText(drawing, distance.str(), Point(20, 460), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 2);

	return calDist;
}
