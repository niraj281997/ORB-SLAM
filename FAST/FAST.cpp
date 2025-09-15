#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/features2d.hpp>

using namespace cv;
using namespace std;
/*
 *   1. Captures frames from the webcam
 *   2. Converts them to grayscale
 *   3. Detects keypoints using the FAST algorithm
 *   4. Draws detected keypoints (green dots) on the frame
 *   5. Displays the result in a window called "FAST Features"
 */
int main()
{
	VideoCapture cap(0);
	if(!cap.isOpened())
	{
		cerr<<"The Camera is already not getting opened\n";
		return -1;
	}

	try
	{
		// FAST feature detector detection
		
		Ptr<FastFeatureDetector> detector = FastFeatureDetector::create();
		
		while(true)
		{
			Mat In_frame,gray, Out_frame;
			cap>>In_frame;
			
			while(In_frame.empty())
			{
				cerr<<"Blank frame"<<endl;
				break;	
			}
			
			cvtColor(In_frame, gray , COLOR_BGR2GRAY);
			
			vector<KeyPoint> keypoints;
			detector->detect(gray, keypoints);
			

			drawKeypoints(In_frame, keypoints, Out_frame, Scalar(0,255,0));
			
			imshow("Fast Feature", Out_frame);	
			
			char c = (char)waitKey(1);
			if(c=='q' || c==27)
			{
			break;
			}
		}
	}
	catch (const exception &e)
	{
		cerr<<"The error occurred in FAST deature operation"<<endl;
	}

	cap.release();
	destroyAllWindows();
	return 0;
}
