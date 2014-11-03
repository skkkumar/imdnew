#include <cv.h>
#include <iostream>
#include <math.h>
class Utilities {
      public :
	double findSig ( int j, double fu,double fv, std::vector<cv::Point2f> cornersL1Local,std::vector<cv::Point2f> cornersL2Local);
    
        double findMu ( int j,std::vector<cv::Point2f> cornersL1Local,std::vector<cv::Point2f> cornersL2Local);
};