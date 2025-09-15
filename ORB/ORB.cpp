#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/features2d.hpp>
#include<opencv2/viz.hpp>


/*

What this code really does
Captures frames from your webcam in real-time.
Detects ORB keypoints and descriptors in each frame.
Matches keypoints between the previous and current frame.
Filters out poor matches.
Draws the good matches to visualize movement of features between frames.
Essentially, this is a simple visual tracking setup using ORB.
*/

using namespace std;
using namespace cv;

int main()
{

	VideoCapture cap(0);
	
	if(!cap.isOpened())
	{
		cerr<<"Camera is not getting opened\n";
	}
	
	Ptr<ORB> orb = ORB::create(7000);
	BFMatcher matcher(NORM_HAMMING);
	Mat Prev_frame_gray, Prev_desc;
	vector<KeyPoint>Prev_kp;

	bool F_frame = true;
	
	
	while(true)
	{
		Mat Curr_frame, gray,desc;
		vector<KeyPoint> Curr_kp;
		cap>>Curr_frame;
	
		if(Curr_frame.empty())
		{
			cerr<<"The frame is empty\n";
			break;
		}
		cvtColor(Curr_frame, gray, COLOR_BGR2GRAY);
		
		//imshow("Check", gray);
		
		orb->detectAndCompute(gray,noArray(),Curr_kp,desc);

		if(!F_frame)
		{
			vector<DMatch> matches;
			matcher.match(Prev_desc,desc,matches);
			
	
			double max_dist = 0, min_dist = 100;
			for(int i = 0;i<Prev_desc.rows; i++)
			{
				double distance = matches[i].distance;
				if(distance < min_dist)
				min_dist = distance;
				if(distance > max_dist)
				max_dist = distance;
 
			}
			vector<DMatch> good_matches;
			for(auto &m: matches)
			{
				if(m.distance <=max(2* min_dist , 30.0))
				{
					good_matches.push_back(m);
				}		
			}
		Mat outImg;
			drawMatches(Prev_frame_gray, Prev_kp, gray,Curr_kp,good_matches, outImg, Scalar::all(-1),Scalar::all(-1),vector<char>(),DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		imshow("ORB matches", outImg);
		}
	

		Prev_kp = Curr_kp;
		Prev_frame_gray = gray.clone();
		Prev_desc = desc.clone();
		F_frame = false;

		char ch = waitKey(2);
		if(ch=='q' || ch == 27)
			break;
		
	}
	cap.release();
	destroyAllWindows();

return 0;
}


