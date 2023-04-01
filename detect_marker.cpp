#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;

int main(int argc, char **argv) {
    int waitTime = 10; // Define the delay in milliseconds between each frame display

    VideoCapture inputVideo; // Create a VideoCapture object to read from the camera
    inputVideo.open(0); // Open the default camera, which is usually the built-in webcam on the laptop

    if (!inputVideo.isOpened()) { // Check if the camera is opened successfully
        std::cerr << "ERROR: Could not open video device" << std::endl;
        return -1;
    }

    Ptr<aruco::Dictionary> dictionary = aruco::getPredefinedDictionary(aruco::DICT_ARUCO_ORIGINAL); // Create a dictionary object with predefined markers
    Ptr<aruco::DetectorParameters> detectorParams = aruco::DetectorParameters::create(); // Create a detector object with default parameters
    
    // Create a window for displaying the video feed
    namedWindow("out", WINDOW_NORMAL);
    while (inputVideo.grab()) { // Read frames from the camera
        Mat image, imageCopy; // Create two Mat objects to store the original and modified frames
        inputVideo.retrieve(image); // Retrieve the frame from the camera and store it in the 'image' Mat object
        image.copyTo(imageCopy); // Copy the 'image' Mat object to the 'imageCopy' Mat object

        std::vector<int> ids; // Create a vector to store the IDs of detected markers
        std::vector<std::vector<Point2f>> corners, rejected; // Create vectors to store the corners of detected markers and rejected candidates

        aruco::detectMarkers(image, dictionary, corners, ids, detectorParams); // Detect markers in the image
        std::cout << "Detected " << ids.size() << " markers" << std::endl;


        if (!ids.empty()) { // If any marker is detected
            aruco::drawDetectedMarkers(imageCopy, corners, ids); // Draw the detected markers on the 'imageCopy' Mat object
        }

        imshow("out", imageCopy); // Display the modified frame with detected markers in a window with the title "out"

        char key = (char)waitKey(waitTime); // Wait for the specified delay and get the pressed key
        if (key == 27) // If the pressed key is 'Esc'
            break; // Break the while loop and exit the program
    }

    return 0;
}
