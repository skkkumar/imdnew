
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Image.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/dev/all.h>
#include "ml.h"
#include <cv.h>
#include <fstream>
#include <iostream>
using namespace yarp::dev;
using namespace std;
using namespace cv;
class collectnlearn
{
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > imagePort;  // make a port for reading images
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > origImgout1, origImgout2;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > inPort1;
    yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelRgb> > inPort2;
    yarp::os::BufferedPort<yarp::os::Bottle> velocityPort;
    yarp::os::BufferedPort<yarp::os::Bottle> collectnlearndata;
    cv::Mat imageIn1;
    cv::Mat imageIn2;
    double* velocityArray;
    
     ML2 *mlu, *mlv, *mlsiguu, *mlsigvv ,*mlsiguv;
    std::ofstream file;
    yarp::os::Stamp stamp1, stamp2, stamp3;
//     int stamp1, stamp2, stamp3;
    double mu,mv,siguu,siguv,sigvv,sigvu;
    int total_no_of_velocityArray, trainingdatasize_value;
    int datacollection_check;
public:
    void writeDataToFile ( );
    collectnlearn()
    {
        // constructor
    }
    void returnVelocity();
    bool dataAcq();
    bool open(int trainingdatasize_value);
    bool close();
    bool loop(); 

    bool interrupt();

};

   
   



   