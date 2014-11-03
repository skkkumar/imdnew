
#include "utility.h"
double Utilities::findSig ( int j, double fu,double fv, std::vector<cv::Point2f> cornersL1Local,std::vector<cv::Point2f> cornersL2Local)
    {        double u,sigu;
            if (cornersL2Local.size()){
	double i2x , i2y , i1x , i1y ;
	u = 0;
        for ( int i=0; i < cornersL2Local.size(); i++ )
        {
	   i2x = (double)cornersL2Local[i].x;
	   i2y = (double)cornersL2Local[i].y;
	   i1x = (double)cornersL1Local[i].x;
	   i1y = (double)cornersL1Local[i].y;
	    if ( j == 1 )
                u = u + pow ( ( i2x - i1x ) - fu, 2.0 );// * ( ( cornersL2[i].x - cornersL1[i].x ) - mu ) ;
            if ( j == 2 )
                u = u + pow ( ( i2y - i1y ) - fv, 2.0 );// * ( ( cornersL2[i].y - cornersL1[i].y ) - mv ) ;
            if ( j == 3 )
                u = u + ((( i2x - i1x ) - fu) * (( i2y - i1y ) - fv));
            if ( j == 4 )
                u = u + (( i2y - i1y ) - fv) * (( i2x - i1x ) - fu);
        }
        sigu = u/cornersL2Local.size();
	    }
	    else{
	      sigu = 0;
	    }
	return sigu;
    }
    
        double Utilities::findMu ( int j,std::vector<cv::Point2f> cornersL1Local,std::vector<cv::Point2f> cornersL2Local)
    {        double u,mu;
      if (cornersL2Local.size()){
	double i2x , i2y , i1x , i1y ;
	u = 0;
        for ( int i=0; i < cornersL2Local.size(); i++ )
        {
	   i2x = (double)cornersL2Local[i].x;
	   i2y = (double)cornersL2Local[i].y;
	   i1x = (double)cornersL1Local[i].x;
	   i1y = (double)cornersL1Local[i].y;
            if ( j == 1 )
                u += ( i2x - i1x);
            if ( j == 2 )
                u += ( i2y - i1y );
        }
        mu = u/cornersL2Local.size();
	}
	else {
	  mu = 0;
	}
// 	cout << u << " "<< mu << " " << cornersL2Local.size() << endl << flush;
        return mu;
    }