#include<iostream>
#include<opencv2/opencv.hpp>
#include<opencv2/features2d.hpp>
#include<opencv2/viz.hpp>

using namespace std;
using namespace cv;


/*

What this code really does
Captures frames from your webcam in real-time.
Detects ORB keypoints and descriptors in each frame.
Matches keypoints between the previous and current frame.
Filters out poor matches.
Draws the good matches to visualize movement of features between frames.
Essentially, this is a simple visual tracking setup using ORB.

*/

int main()
{
 	
VideoCapture cam(1);
//VideoCapture cam(0);--> to connect the mobile camera
 
if(!cam.isOpened())
{
	cerr<<"The camera is not getting opened\n";
    return -1;
}  
else 
{
	cout<<"Camera got connected"<<endl;
}

Ptr<ORB> orb = ORB::create(7000);

BFMatcher matcher(NORM_HAMMING);


Mat prev_gray, prev_desc, out_image;
vector<KeyPoint>prev_kp;
while(true)
{

	Mat curr_frame, curr_gray, curr_desc;
	vector<KeyPoint>curr_kp;
	cam>>curr_frame;
	
	if(!curr_frame.empty())
	{
		cvtColor(curr_frame, curr_gray,COLOR_BGR2GRAY);
		//imshow("Camera", curr_frame);

		orb->detectAndCompute(curr_gray,noArray(),curr_kp,curr_desc);
		
		if(!prev_desc.empty())
		{
			vector<DMatch> matches, good_matches; // each DMatch object represents one pair of matched descriptors (one from previous frame, one from current frame).
			matcher.match(prev_desc,curr_desc,matches);
			double max_dist = INT_MIN, min_dist = INT_MAX;
			for(size_t i = 0 ; i < matches.size(); i++)
			{
				double distance = matches[i].distance; //Hamming distance, this is literally "how many bits are different"
				if(max_dist < distance)
				{
					max_dist = distance;	
				}
				if(min_dist > distance)
				{
					min_dist = distance;
				}
			}
			for(auto &m : matches)
			{
				if(m.distance <= max(2* min_dist, 30.0))
				{
					good_matches.push_back(m);
				}
			}
			drawMatches(prev_gray, prev_kp, curr_gray,curr_kp,good_matches, out_image, Scalar::all(-1),Scalar::all(-1),vector<char>(),DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
		imshow("ORB matches", out_image);		
		}
		prev_kp = curr_kp;
		prev_gray = curr_gray;
		prev_desc = curr_desc;

		//prev_gray = curr_gray.clone();
		//prev_desc = curr_desc.clone();
		
		imshow("Camera",curr_gray);
	}


	int key = waitKey(1) & 0xFF;
	if(key==27 || key=='q' || key=='Q')
	{
		break;
	}
	else 
	{
	}
}
	cam.release();
	destroyAllWindows();
return 0;
}
