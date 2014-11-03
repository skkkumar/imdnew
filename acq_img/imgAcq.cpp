
#include "imgAcq.h"

using namespace yarp::sig;
using namespace yarp::os;


    void imgAcq::returnVelocity(int i)
    {
      double position;
      for (int n = 0; n < total_no_of_velocityArray; n ++){
      enc->getEncoder(n, &position);
      if (i == 1){
      bottlevector1[n] = position;}
	else if (i == 2){
	  bottlevector2[n] = position;
	}
      }
      return;
    }

    void imgAcq::imageAcq()
    {
        if ( imageIn1.data == NULL )
        {
            imageIn1 = ( IplImage * ) imagePort.read()->getIplImage();
	    imagePort.getEnvelope(stamp);
	    returnVelocity(1);
            imageIn2 = ( IplImage * ) imagePort.read()->getIplImage();
// 	    imagePort.getEnvelope(stamp2);
            returnVelocity(2);
        }
        else
        {
            imageIn1 = imageIn2;
	    imagePort.getEnvelope(stamp);
	    for (int n = 0; n < total_no_of_velocityArray; n ++){
            bottlevector1[n] = bottlevector2[n];}
            imageIn2 = ( IplImage * ) imagePort.read()->getIplImage();
// 	    imagePort.getEnvelope(stamp2);
	    returnVelocity(2);
        }
        findDiffVelocity();
    }

        void imgAcq::findDiffVelocity()
    {
      for (int n = 0; n < total_no_of_velocityArray; n ++){
        if ( bottlevector1[n] != 0 and bottlevector2[n] != 0 )
        {
	  velocityArray[n] = bottlevector2[n] - bottlevector1[n];	  
        }
        else{
	  velocityArray[n] = 0;
	}
// 	std::cout << velocityArray[0]<<" "<< velocityArray[1]<<" "<< velocityArray[2]<<" "<< velocityArray[3]<<" "<< velocityArray[4]<<" "<< velocityArray[5] <<std::endl<<std::flush;
      }
    }
    
void imgAcq::loop()
{
    imageAcq();
    ImageOf<PixelRgb> &outImg1 = outPort1.prepare();
    IplImage tmp = imageIn1;
    outImg1.resize ( tmp.width, tmp.height );
    cvCopyImage ( &tmp, ( IplImage * ) outImg1.getIplImage() );
    outPort1.setEnvelope(stamp);
    ImageOf<PixelRgb> &outImg2 = outPort2.prepare();
    tmp = imageIn2;
    outImg2.resize ( tmp.width, tmp.height );
    cvCopyImage ( &tmp, ( IplImage * ) outImg2.getIplImage() );
    outPort2.setEnvelope(stamp);
    Bottle &velocityVector = velocityPort.prepare();
    velocityVector.clear();
//     std::cout << velocityArray[0]<<" "<< velocityArray[1]<<" "<< velocityArray[2]<<" "<< velocityArray[3]<<" "<< velocityArray[4]<<" "<< velocityArray[5] <<std::endl<<std::flush;
    velocityVector.addInt(total_no_of_velocityArray);
    for (int n = 1; n <= total_no_of_velocityArray; n ++){
//       std::cout<<velocityArray[n]<<" "<<std::flush;
    velocityVector.addDouble(velocityArray[n]);}
//     std::cout<< std::endl<<std::flush;
    velocityPort.setEnvelope(stamp);
    outPort1.write();
    outPort2.write();
    velocityPort.write();
}

bool imgAcq::open(String robotName, int no_of_velocityArray)
{
  cout<<robotName<<no_of_velocityArray<<endl<<flush;
    bool ret=true;
    ret=imagePort.open("/acq/image:i");  // give the port a name
    ret = ret && outPort1.open("/acq/image1:o");
    ret = ret && outPort2.open("/acq/image2:o");
    ret = ret && velocityPort.open("/acq/velocity:o");
    Property obGaze;
    obGaze.put("device", "remote_controlboard");
    obGaze.put ( "local","/mover/motor/client" );
    obGaze.put ( "remote","/"+robotName+"/head" );
    
    if ( !clientobsGaze.open(obGaze))
    {
      std::cout<<"Problem with opening"<<std::endl<<std::flush;
        return false;
    }
    clientobsGaze.view(pos);
    clientobsGaze.view(vel);
    clientobsGaze.view(enc);
    if (pos==NULL || vel==NULL || enc==NULL) 
    {
        printf("Cannot get interface to robot head\n");
        clientobsGaze.close();
        return 1;
    }
    total_no_of_velocityArray = no_of_velocityArray;
    bottlevector1 = new double[total_no_of_velocityArray];
    bottlevector2 = new double[total_no_of_velocityArray];
    velocityArray = new double[total_no_of_velocityArray];
    return ret;
}

bool imgAcq::close()
{
//     Bottle &target=velocityPort.prepare();
//     target.clear();
//     target.addDouble(0);
//     target.addDouble(0);
//     
//     target.addInt(0);    
//     
//     velocityPort.setStrict();
//     velocityPort.write();
//     

//     outPort.write();

    imagePort.close();
    outPort1.close();
    outPort2.close();
    velocityPort.close();
    clientobsGaze.close();
    return true;
}

bool imgAcq::interrupt()
{
    imagePort.interrupt();
    return true;
}

