#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/objdetect.hpp>
#include<iostream>

using namespace std;
using namespace cv;

Mat img;
vector<vector<int>> newPoint;

//HSV values of choosen pen colors
vector < vector<int>> penColors{ {13,102,92,26,255,255},//yellow
								{99,114,82,124,255,199} };//blue
//color values
//I have realized that color values should be used backwards. I have some guesses but I am not specifially sure why.

vector<Scalar> colorValues{ {0,255,255},//yellow
							{255, 0, 0} };//blue


Point getContour(Mat imgMask) {

	
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	Point drawPoint(0, 0);
	for (int i = 0; i < contours.size(); i++) {
		
		int area = contourArea(contours[i]);
		string objectType;

		if (area > 1000) {

			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			boundRect[i] = boundingRect(conPoly[i]);

			drawPoint.x = boundRect[i].x + boundRect[i].width / 2;//to draw from the center, not from the edge
			drawPoint.y = boundRect[i].y;

		}

	}
	return drawPoint;
}

vector<vector<int>> findColor(Mat img) {
	
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);//to convert from BGR to HSV

	for (int i = 0; i < penColors.size(); i++) {

		//hue,saturation and value
		Scalar lower(penColors[i][0], penColors[i][1], penColors[i][2]);
		Scalar upper(penColors[i][3],penColors[i][4],penColors[i][5]);

		Mat mask;

		inRange(imgHSV, lower, upper, mask);//we will have multiple masks for each color

		Point drawPoint = getContour(mask);

		newPoint.push_back({ drawPoint.x,drawPoint.y,i });
		
		
	}
	return newPoint;
}

void drawOnScreen(vector<vector<int>> newPoint, vector<Scalar> colorValues) {

	for (int i = 0; i < newPoint.size(); i++) {
		
		circle(img, (Point(newPoint[i][0],newPoint[i][1])), 10, colorValues[newPoint[i][2]], FILLED);
	}

}

int main(){

	string path = "http://MY.DROID.CAM.IP:4747/mjpegfeed?640x480";//to use my mobile device as webcam and take live video from it

	VideoCapture cap(path);


	while (true) {
		cap.read(img);

		drawOnScreen(findColor(img),colorValues);

		imshow("Camera", img);
		waitKey(1);
	}

	


	return 0;
}
