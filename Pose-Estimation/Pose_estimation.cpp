#include <iostream>
#include <algorithm> 
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>

using namespace cv;
using namespace std;


class ORBExtractor {
public:
    ORBExtractor(int numFeatures = 3000) {
        orb = ORB::create(numFeatures);
    }

    void compute(const Mat& image, vector<KeyPoint>& keypoints, Mat& descriptors) {
        orb->detectAndCompute(image, noArray(), keypoints, descriptors);
    }

private:
    Ptr<ORB> orb;
};

// -------- Feature Matcher --------
class ORBMatcher {
public:
    ORBMatcher() : matcher(NORM_HAMMING) {}

    bool findGoodMatches(const Mat& desc1, const Mat& desc2, vector<DMatch>& goodMatches) {
        if (desc1.empty() || desc2.empty() ||
            desc1.type() != desc2.type() || desc1.cols != desc2.cols) {
            cerr << "Descriptor mismatch or empty descriptors. Skipping frame.\n";
            return false;
        }

        vector<DMatch> matches;
        matcher.match(desc1, desc2, matches);

        double minDist = 1e9;
        for (const auto& m : matches) {
            minDist = std::min(minDist, static_cast<double>(m.distance));
        }
        for (const auto& m : matches) {
            if (m.distance <= max(2 * minDist, 30.0))
                goodMatches.push_back(m);
        }

        return !goodMatches.empty();
    }

private:
    BFMatcher matcher;
};


class PoseEstimator {
public:
    PoseEstimator(const Mat& intrinsics) : K(intrinsics) {}

    bool estimatePose(const vector<KeyPoint>& kp1, const vector<KeyPoint>& kp2,
                      const vector<DMatch>& matches, Mat& R, Mat& t) {
        if (matches.size() < 8) return false;

        vector<Point2f> pts1, pts2;
        for (const auto& m : matches) {
            pts1.push_back(kp1[m.queryIdx].pt);
            pts2.push_back(kp2[m.trainIdx].pt);
        }

        Mat E = findEssentialMat(pts1, pts2, K, RANSAC, 0.999, 1.0);
        if (E.empty()) return false;

        recoverPose(E, pts1, pts2, K, R, t);
        return true;
    }

private:
    Mat K;
};


int main() {
    VideoCapture camera(0);
    if (!camera.isOpened()) {
        cerr << "Failed to open camera.\n";
        return -1;
    }

    // Initialize modules
    ORBExtractor extractor;
    ORBMatcher matcher;
    Mat K = (Mat_<double>(3, 3) << 700, 0, 320,
                                   0, 700, 240,
                                   0,   0,   1);
    PoseEstimator poseEstimator(K);

    // State variables
    bool isFirstFrame = true;
    Mat prevGray, prevDesc;
    vector<KeyPoint> prevKeypoints;

    // Trajectory visualization
    Mat trajectory = Mat::zeros(600, 600, CV_8UC3);
    Mat globalPose = Mat::eye(4, 4, CV_64F);
    double scale = 50.0;

    while (true) {
        Mat frame, gray, desc;
        camera >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        vector<KeyPoint> keypoints;
        extractor.compute(gray, keypoints, desc);

        if (keypoints.empty() || desc.empty()) {
            cerr << "No features detected. Skipping frame.\n";
            continue;
        }

        if (!isFirstFrame) {
            vector<DMatch> goodMatches;
            if (matcher.findGoodMatches(prevDesc, desc, goodMatches)) {
                Mat R, t;
                if (poseEstimator.estimatePose(prevKeypoints, keypoints, goodMatches, R, t)) {
                    // Update global pose
                    Mat Rt;
                    hconcat(R, t, Rt);
                    Mat RtHomogeneous = Mat::eye(4, 4, CV_64F);
                    Rt.copyTo(RtHomogeneous(Range(0, 3), Range(0, 4)));
                    globalPose = globalPose * RtHomogeneous;

                    // Extract camera position
                    Point3d camPos(globalPose.at<double>(0, 3),
                                   globalPose.at<double>(1, 3),
                                   globalPose.at<double>(2, 3));

                    // Draw trajectory
                    int x = int(camPos.x * scale) + 300;
                    int y = int(camPos.z * scale) + 300;
                    circle(trajectory, Point(x, y), 2, Scalar(0, 255, 0), -1);
                    putText(trajectory, "Camera", Point(x, y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);
                    imshow("Camera Trajectory", trajectory);

                    // Show feature matches
                    Mat matchImg;
                    drawMatches(prevGray, prevKeypoints, gray, keypoints, goodMatches, matchImg,
                                Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
                    imshow("Feature Matches", matchImg);
                }
            }
        }

       
        prevGray = gray.clone();
        prevKeypoints = keypoints;
        prevDesc = desc.clone();
        isFirstFrame = false;

        if (waitKey(1) == 27) break; // ESC to exit
    }

    camera.release();
    destroyAllWindows();
    return 0;
}
