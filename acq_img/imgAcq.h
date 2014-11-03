
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>
#include <cv.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/all.h>
#include <iostream>
// #define total_no_of_velocityArray 6
using namespace cv;
using namespace yarp::dev;
using namespace std;
class imgAcq
{
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > imagePort;  // make a port for reading images
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > outPort1;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > outPort2;
    yarp::os::BufferedPort<yarp::os::Bottle> velocityPort;
    cv::Mat imageIn1;
    cv::Mat imageIn2;
    double* bottlevector1;
    double* bottlevector2;
    double* velocityArray;
    PolyDriver      clientobsGaze;
    IPositionControl *pos;
    IVelocityControl *vel;
    IEncoders *enc;
    yarp::os::Stamp stamp;
    int total_no_of_velocityArray;
public:

    imgAcq()
    {
        // constructor
    }
    void returnVelocity(int i);
    bool open(String robotName, int no_of_velocityArray);
    void imageAcq();
    bool close();
    void findDiffVelocity();
    void loop(); 

    bool interrupt();

};

   
   



   