
#include "kpe.h"

using namespace yarp::sig;
using namespace yarp::os;



    std::vector<cv::Point2f> kpe::harrisCorner2 ( cv::Mat src )
    {
        cv::Mat src2;
        cv::cvtColor ( src, src2, CV_BGR2GRAY );
        std::vector<cv::Point2f> cornersA;
        goodFeaturesToTrack( src2,cornersA,maxCorners,qualityLevel,minDistance,cv::Mat(),blockSize,true,k);
        return cornersA;
    }
    
    void kpe::returnVelocity()
    {
      Bottle *message=velocityPort.read();
      velocityPort.getEnvelope(stamp3);
      total_no_of_velocityArray = message->get(0).asDouble();
      velocityArray = new double[total_no_of_velocityArray];
      for (int n = 1; n <= total_no_of_velocityArray; n ++){
      velocityArray[n] = message->get(n).asDouble();}
      message->clear();
//       std::cout << velocityArray[0]<<" "<< velocityArray[1]<<" "<< velocityArray[2]<<" "<< velocityArray[3]<<" "<< velocityArray[4]<<" "<< velocityArray[5] <<std::endl<<std::flush;
      return;
    }

    bool kpe::dataAcq()
    {
      ImageOf<PixelRgb> *pImgrgbIn1=inPort1.read();
      ImageOf<PixelRgb> *pImgrgbIn2=inPort2.read();
      if (pImgrgbIn1 != NULL && pImgrgbIn2 != NULL ){
	
      imageIn1 = ( IplImage * ) pImgrgbIn1->getIplImage();
      inPort1.getEnvelope(stamp1);
      imageIn2 = ( IplImage * ) pImgrgbIn2->getIplImage();
      inPort2.getEnvelope(stamp2);
//       cout<<stamp1.getTime()<<endl<<flush;
//       cout<<stamp2.getTime()<<endl<<flush;
//       cout<<stamp3.getTime()<<endl<<flush;
      
      returnVelocity();
//       cout<<"2"<<round(abs(stamp2.getTime()-stamp3.getTime()))<<endl<<flush;
//       cout<<"3"<<round(abs(stamp1.getTime()-stamp2.getTime()))<<endl<<flush;
//         cout<<"1"<<round(abs(stamp3.getTime()-stamp1.getTime()))<<endl<<flush;
//       cout<<stamp2.getTime()-stamp3.getTime()<<endl<<flush;
      if ((round(abs((stamp1.getTime()-stamp2.getTime()))) == 0) && (round(abs(stamp2.getTime()-stamp3.getTime())) == 0) && (round(abs(stamp3.getTime()-stamp1.getTime())) == 0)){
// 	cout<<"Inside"<<endl<<flush;
	return true;
      }
      else{
	return false;
      }
      }
      else{
	return true;
      }
    }    
bool kpe::loop()
{
    if (dataAcq()){
      	    Mat tempimageL1;
	    imageIn1.copyTo(tempimageL1);
	    ImageOf<PixelRgb> &outImg = imagePort.prepare();
            // *************** Obtain feature points ************************  Feature extracting
            std::vector<cv::Point2f> cornersL2dummy = harrisCorner2( imageIn2 );
            // ***************************************** Tracking ***************************
            std::vector<uchar> features_found;
            std::vector<float> feature_errors;
            std::vector<cv::Point2f> cornersL1dummy, cornersL1,cornersL2;
            cv::Mat imageL11;
            cv::Mat imageL22;
            cv::cvtColor ( imageIn2, imageL22, CV_BGR2GRAY );
            cv::cvtColor ( tempimageL1, imageL11, CV_BGR2GRAY );
            calcOpticalFlowPyrLK ( imageL22, imageL11, cornersL2dummy, cornersL1dummy, features_found, feature_errors ,Size ( 21, 21 ), 5, cvTermCriteria ( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3 ), 0 );
	    for ( int i=0; i < features_found.size(); i++ )
            {
	      if(features_found[i] && feature_errors[i] < 15){
	      cornersL1.push_back(cornersL1dummy[i]);
	      cornersL2.push_back(cornersL2dummy[i]);
	      }
            }
	    
	    
            for ( int i=0; i < cornersL2.size(); i++ )
            {
                circle ( tempimageL1, Point ( cornersL2[i].x, cornersL2[i].y ), 5,  Scalar ( 0 ), 2, 8, 0 );
            }
            // *********************** to display the feature points in the optical flow in the image **********************
            for ( int i=0; i < cornersL2.size(); i++ )
            {
                Point p0 ( ceil ( cornersL2[i].x ), ceil ( cornersL2[i].y ) );
                Point p1 ( ceil ( cornersL1[i].x ), ceil ( cornersL1[i].y ) );
                line ( tempimageL1 , p0, p1, CV_RGB ( 0,255,255 ), 2 );
            }
            IplImage tmp = tempimageL1;
            outImg.resize ( tmp.width, tmp.height );
            cvCopyImage ( &tmp, ( IplImage * ) outImg.getIplImage() );
	    imagePort.setEnvelope(stamp1);
	    
	    
	    mu = t.findMu ( 1,cornersL1,cornersL2 );
            mv = t.findMu ( 2,cornersL1,cornersL2 );
            siguu = t.findSig ( 1,mu,mv,cornersL1,cornersL2 );
            sigvv = t.findSig ( 2,mu,mv,cornersL1,cornersL2 );
            siguv = t.findSig ( 3,mu,mv,cornersL1,cornersL2 );
	    
	    ImageOf<PixelRgb> &outImg1 = origImgout1.prepare();
	    tmp = imageIn1;
	    outImg1.resize ( tmp.width, tmp.height );
	    cvCopyImage ( &tmp, ( IplImage * ) outImg1.getIplImage() );
	    origImgout1.setEnvelope(stamp1);
	    ImageOf<PixelRgb> &outImg2 = origImgout2.prepare();
	    tmp = imageIn2;
	    outImg2.resize ( tmp.width, tmp.height );
	    cvCopyImage ( &tmp, ( IplImage * ) outImg2.getIplImage() );
	    origImgout2.setEnvelope(stamp1);
	    Bottle kpedatavector = kpedata.prepare();
	    kpedatavector.clear();
	    for (int n = 0; n < total_no_of_velocityArray; n ++){
	    kpedatavector.addDouble(velocityArray[n]);}
	    kpedatavector.addDouble(mu);
	    kpedatavector.addDouble(mv);
	    kpedatavector.addDouble(siguu);
	    kpedatavector.addDouble(sigvv);
	    kpedatavector.addDouble(siguv);
	    kpedata.setEnvelope(stamp1);
	    imagePort.write();
	    origImgout1.write();
	    origImgout2.write();
	    kpedata.write();
	    return true;
    }
    else{
      return false;
    }
}

bool kpe::open()
{
    bool ret=true;
    ret=imagePort.open("/kpe/image:o");  // give the port a name
    ret = ret && inPort1.open("/kpe/image1:i");
    ret = ret && inPort2.open("/kpe/image2:i");
    ret = ret && velocityPort.open("/kpe/velocity:i");
    ret = ret && kpedata.open("/kpe/data:o");
    ret = ret && origImgout1.open("/kpe/image1:o");
    ret = ret && origImgout2.open("/kpe/image2:o");
     maxCorners = harriscorner2_maxCorners;
     qualityLevel = harriscorner2_qualityLevel;
     minDistance = harriscorner2_minDistance;
     blockSize = harriscorner2_blockSize;
     k = harriscorner2_k;
    
    
    return ret;
}

bool kpe::close()
{
    imagePort.close();
    inPort1.close();
    inPort2.close();
    velocityPort.close();
    kpedata.close();
    origImgout1.close();
    origImgout2.close();
    return true;
}

bool kpe::interrupt()
{
    inPort1.interrupt();
    inPort2.interrupt();
    velocityPort.interrupt();
    return true;
}

