
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>
#include <cv.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/all.h>
#include "utility.h"
#include <iostream>
using namespace cv;
using namespace yarp::dev;
using namespace std;
#define harriscorner2_maxCorners 200
#define harriscorner2_qualityLevel 0.01
#define harriscorner2_minDistance 1.0
#define harriscorner2_blockSize 5
#define harriscorner2_k 0.04
// #define total_no_of_velocityArray 6
class kpe
{
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > imagePort;  // make a port for reading images
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > origImgout1, origImgout2;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > inPort1;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > inPort2;
    yarp::os::BufferedPort<yarp::os::Bottle> velocityPort;
    yarp::os::BufferedPort<yarp::os::Bottle> kpedata;
    cv::Mat imageIn1;
    cv::Mat imageIn2;
    double* velocityArray;
    int maxCorners;
    double qualityLevel;
    double minDistance;
    int blockSize ;
    double k ;
    yarp::os::Stamp stamp1, stamp2, stamp3;
//     int stamp1, stamp2, stamp3;
    double mu,mv,siguu,siguv,sigvv,sigvu;
    Utilities t;
    int total_no_of_velocityArray;
public:

    kpe()
    {
        // constructor
    }
    void returnVelocity();
    bool dataAcq();
    std::vector<cv::Point2f> harrisCorner2 ( cv::Mat src );
    bool open();
    bool close();
    bool loop(); 

    bool interrupt();

};

   
   



   