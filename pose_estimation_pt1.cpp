#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{
    // Check command line arguments
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <dictionary_id> <marker_id> <marker_length>" << endl;
        return -1;
    }

    // Parse command line arguments
    int dictionaryId = stoi(argv[1]);
    int markerId = stoi(argv[2]);
    float markerLength = stof(argv[3]);

    // Initialize dictionary and parameters
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_ARUCO_ORIGINAL); // Create a dictionary object with predefined markers
    Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create(); // Create a detector object with default parameters

    Mat cameraMatrix = (Mat_<double>(3,3) << 628.158, 0., 324.099, 0., 628.156, 260.908, 0, 0, 1);
    Mat distCoeffs = (Mat_<double>(1,5) << 0.0995485, -0.206384, 0.00754589, 0.00336531, 0);

    // Create video capture object
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Failed to open camera." << endl;
        return -1;
    }

    // Create display window
    namedWindow("Pose Estimation", WINDOW_NORMAL);


    // Loop over frames
    while (true) {
        // Capture frame from camera
        Mat frame;
        cap >> frame;

        // Detect markers in frame
        vector<int> markerIds;
        vector<vector<Point2f>> markerCorners, rejectedCandidates;
        aruco::detectMarkers(frame, dictionary, markerCorners, markerIds);

        // Check if the detected marker ID matches the desired marker ID
        auto it = find(markerIds.begin(), markerIds.end(), markerId);
        if (it != markerIds.end()) {
            int idx = distance(markerIds.begin(), it);

            // Estimate pose of marker with specified ID
            vector<Vec3d> rvecs, tvecs;
            aruco::estimatePoseSingleMarkers(markerCorners, markerLength, cameraMatrix, distCoeffs, rvecs, tvecs);

            // Draw green frame surrounding marker
            aruco::drawDetectedMarkers(frame, vector<vector<Point2f>>{markerCorners[idx]}, vector<int>{markerIds[idx]});

            // Draw axes at middle of marker
            aruco::drawAxis(frame, cameraMatrix, distCoeffs, rvecs[idx], tvecs[idx], markerLength / 2.0);

            // Draw circle at middle of marker
            Point2f middle = (markerCorners[idx][0] + markerCorners[idx][2]) * 0.5f;
            cv::Point middle_2d = cv::Point(middle);
            circle(frame, middle_2d, 3, cv::Scalar(255, 0, 0), -1);

        // Get position of center of marker in camera frame
        Vec3d markerCenter = tvecs[idx] + Vec3d(0, 0, markerLength / 2.0);

        // Print x, y, and z coordinates
        string text_x = "x: " + to_string(markerCenter[0]);
        putText(frame, text_x, Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
        string text_y = "y: " + to_string(markerCenter[1]);
        putText(frame, text_y, Point(10, 70), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
        string text_z = "z: " + to_string(markerCenter[2]);
        putText(frame, text_z, Point(10, 100), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 255, 0), 2);
        }

    // Display frame
    imshow("Pose Estimation", frame);

    // Check for exit command
    if (waitKey(1) == 'q') {
        break;
    }
}
    // Release video capture object and close display window
    cap.release();
    destroyAllWindows();
    return 0;
}


