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
    // Mat cameraMatrix = (Mat_<double>(3,3) << 628.158, 0., 324.099, 0., 628.156, 260.908, 0, 0, 1);
    // Mat distCoeffs = (Mat_<double>(1,5) << 0.0995485, -0.206384, 0.00754589, 0.00336531, 0);

    Mat cameraMatrix = (Mat_<double>(3,3) << 1397.308072033392, 0, 451.7512695101312,0, 1599.989483675077, 434.3518851051213,0, 0, 1);
    Mat distCoeffs = (Mat_<double>(1,5) << -0.1478204897216296, 1.435862070485068, -0.02486353554177317, -0.325841494978132, 16.05022508212312);


    
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

        // Draw axis of marker
        aruco::drawAxis(frame, cameraMatrix, distCoeffs, rvecs[idx], tvecs[idx], markerLength / 2.0);

        // Get position of center of marker in camera frame
        Vec3d markerCenter = tvecs[idx] + Vec3d(0, 0, markerLength / 2.0);

        // Detect corners of cube in marker's local coordinate system
        float cubeLength = markerLength / 2.0;
        vector<Point3f> objectPoints;
        objectPoints.push_back(Point3f(-cubeLength, cubeLength, 0));
        objectPoints.push_back(Point3f(-cubeLength, cubeLength, cubeLength*2));
        objectPoints.push_back(Point3f(cubeLength, cubeLength, cubeLength*2));
        objectPoints.push_back(Point3f(cubeLength, cubeLength, 0));
        objectPoints.push_back(Point3f(cubeLength, -cubeLength, 0));
        objectPoints.push_back(Point3f(cubeLength, -cubeLength, cubeLength*2));
        objectPoints.push_back(Point3f(-cubeLength, -cubeLength, cubeLength*2));
        objectPoints.push_back(Point3f(-cubeLength, -cubeLength, 0));

        vector<Point2f> imagePoints;
        projectPoints(objectPoints, rvecs[idx], tvecs[idx], cameraMatrix, distCoeffs, imagePoints);

        // Draw red circles at corners of cube
        for (auto& p : imagePoints) {
            circle(frame, p, 5, Scalar(0, 0, 255), -1);
        }

        // Draw lines between the corners to form a cube
        // Define cube edges by connecting corners
        vector<pair<int, int>> edges = {{0, 1}, {1, 2}, {2, 3}, {3, 0},
                                        {4, 5}, {5, 6}, {6, 7}, {7, 4},
                                        {0, 7}, {1, 6}, {2, 5}, {3, 4}};

        // Draw edges of cube
        for (const auto& edge : edges) {
            line(frame, imagePoints[edge.first], imagePoints[edge.second], Scalar(0, 255, 0), 2);
        }



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


