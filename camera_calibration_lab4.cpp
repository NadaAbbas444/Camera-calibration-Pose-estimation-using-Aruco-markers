#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/aruco.hpp"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    // Check if the number of arguments is correct
    if (argc != 8) {
    cout << argv[0] << " <dictionary> <detector parameters> <rows> <cols> <marker size> <marker spacing> <calibration parameters file>" << endl;
     return -1;
    }    
    // Read the command line arguments
    int nu_rows = atoi(argv[3]);   // Number of markers in X direction
    int nu_columns = atoi(argv[4]);   // Number of markers in Y direction
    float markerLength = atof(argv[5]);    // Length of markers in meters
    float markersdistancing = atof(argv[6]);   // Spacing between markers in meters
    string filename = argv[7];  // Output file name for calibration parameters

    std::cout<<nu_rows<<" "<<nu_columns<<" "<<markerLength<<" "<<markersdistancing<<" "<<filename<<endl;     // Print the command line arguments
    FileStorage fs(filename, FileStorage::WRITE);     // Create a file storage object to write calibration parameters to file
    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(cv::aruco:: DICT_ARUCO_ORIGINAL);     // Get the dictionary of ArUco markers to use

    // Create a grid board object
    Ptr<aruco::GridBoard> gridboard = aruco::GridBoard::create(nu_rows, nu_columns, markerLength, markersdistancing, dictionary);
    Ptr<aruco::Board> board = gridboard.staticCast<aruco::Board>();

    // Initialize variables for camera calibration
    Mat cameraMatrix, distCoeffs;
    vector<int> IDs; // list of IDs detected
    vector<vector<Point2f>> Corners; // list of corners detected
    Size imagesize;
    vector <int> counter;

    // Initialize the camera
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Unable to open camera." << endl;
        return -1;
    }
    namedWindow("take a pic", WINDOW_NORMAL);

    // Initialize variables for processing video feed
    Mat frame;
    int num = 1;

    // Main loop for processing video feed
    while (true)
    {
        // Capture a frame from the camera
        cap >> frame;
        // Detect ArUco markers in the frame
        vector<int> markerIds;
        vector<vector<Point2f>> markerCorners;
        aruco::detectMarkers(frame, dictionary, markerCorners, markerIds);

        // Check if the 'c' key has been pressed and if enough markers have been detected
        char key = waitKey(1);
        if (key == 'c'){
            if (markerIds.size()== nu_rows * nu_columns)
            {
                imagesize = frame.size();
                counter.push_back(nu_rows*nu_columns);

                std::cout<<"Picture "<<num<<endl;

                // Add detected marker IDs and corners to vectors
                for (int i = 0; i < markerIds.size(); i++){
                    IDs.push_back(markerIds[i]);
                }
                for (int i = 0; i < markerCorners.size(); i++){
                    Corners.push_back(markerCorners[i]);
                }
                num++;                
            }
            // Display the processed frame in the output window
            cv::imshow("take a pic", frame);

        }
    int keypress = waitKey(1);
    if (keypress == 27 || keypress == 'q') {
        break;
    }
    
    }
    std::cout<<" calibrating "<<endl;
    aruco::calibrateCameraAruco(Corners, IDs, counter, board, imagesize, cameraMatrix, distCoeffs, noArray(), noArray());
    std::cout<<"done calibrating "<<endl;
    std::cout<<"Camera matrix" << cameraMatrix<< endl;
    std::cout<<"Distortion Coeffecient" << distCoeffs<< endl;

    fs << "Camera Matrix" << cameraMatrix << "Distortion Coefficients" << distCoeffs;
    cap.release();
    cv::destroyAllWindows();
    fs.release();

    return 0;
}


