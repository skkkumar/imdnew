
#include "collectnlearn.h"

using namespace yarp::sig;
using namespace yarp::os;



    
    void collectnlearn::returnVelocity()
    {
      Bottle *message=velocityPort.read();
      velocityPort.getEnvelope(stamp3);
      total_no_of_velocityArray = message->get(0).asDouble();
      velocityArray = new double[total_no_of_velocityArray];
      for (int n = 1; n <= total_no_of_velocityArray; n ++){
      velocityArray[n] = message->get(n).asDouble();}
      mu = message->get(total_no_of_velocityArray+1).asDouble();
      mv = message->get(total_no_of_velocityArray+2).asDouble();
      siguu = message->get(total_no_of_velocityArray+3).asDouble();
      sigvv = message->get(total_no_of_velocityArray+4).asDouble();
      siguv = message->get(total_no_of_velocityArray+5).asDouble();
      message->clear();
//       std::cout << velocityArray[0]<<" "<< velocityArray[1]<<" "<< velocityArray[2]<<" "<< velocityArray[3]<<" "<< velocityArray[4]<<" "<< velocityArray[5] <<std::endl<<std::flush;
      return;
    }

    bool collectnlearn::dataAcq()
    {
      ImageOf<PixelRgb> *pImgrgbIn1=inPort1.read();
      ImageOf<PixelRgb> *pImgrgbIn2=inPort2.read();
      if (pImgrgbIn1 != NULL && pImgrgbIn2 != NULL ){
	
      imageIn1 = ( IplImage * ) pImgrgbIn1->getIplImage();
      inPort1.getEnvelope(stamp1);
      imageIn2 = ( IplImage * ) pImgrgbIn2->getIplImage();
      inPort2.getEnvelope(stamp2);
      returnVelocity();
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
    
    void collectnlearn::writeDataToFile ( )
    {
	file.open("velocityArray.csv", std::fstream::app);
	for (int n = 0; n < total_no_of_velocityArray; n ++){
        file<<velocityArray[n]<<",";
	}
	file<<mu<<",";
	file<<mv<<",";
	file<<siguu<<",";
	file<<sigvv<<",";
	file<<siguv;
	file<<endl;
	file.close();
    }
    
bool collectnlearn::loop()
{
    if (dataAcq()){

	    if (datacollection_check < trainingdatasize_value){
	      writeDataToFile();
	      if (datacollection_check == 0){
		cout<<"Collecting data"<<endl<<flush;
	      }
	      datacollection_check ++;
	    }
	    else{
	      cout<<"Training Data"<<endl<<flush;
	      mlu->trainData();
	      mlv->trainData();
	      mlsiguu->trainData();
	      mlsigvv->trainData();
	      mlsiguv->trainData();
	    }
	    ImageOf<PixelRgb> &outImg1 = origImgout1.prepare();
	    IplImage tmp = imageIn1;
	    outImg1.resize ( tmp.width, tmp.height );
	    cvCopyImage ( &tmp, ( IplImage * ) outImg1.getIplImage() );
	    origImgout1.setEnvelope(stamp1);
	    ImageOf<PixelRgb> &outImg2 = origImgout2.prepare();
	    tmp = imageIn2;
	    outImg2.resize ( tmp.width, tmp.height );
	    cvCopyImage ( &tmp, ( IplImage * ) outImg2.getIplImage() );
	    origImgout2.setEnvelope(stamp1);
	    Bottle collectnlearndatavector = collectnlearndata.prepare();
	    collectnlearndatavector.clear();
	    for (int n = 0; n < total_no_of_velocityArray; n ++){
	    collectnlearndatavector.addDouble(velocityArray[n]);}
	    collectnlearndatavector.addDouble(mu);
	    collectnlearndatavector.addDouble(mv);
	    collectnlearndatavector.addDouble(siguu);
	    collectnlearndatavector.addDouble(sigvv);
	    collectnlearndatavector.addDouble(siguv);
	    collectnlearndata.setEnvelope(stamp1);
	    imagePort.write();
	    origImgout1.write();
	    origImgout2.write();
	    collectnlearndata.write();
	    return true;
    }
    else{
      return false;
    }
}

bool collectnlearn::open(int datasize_value)
{
    bool ret=true;
    ret=imagePort.open("/collectnlearn/image:o");  // give the port a name
    ret = ret && inPort1.open("/collectnlearn/image1:i");
    ret = ret && inPort2.open("/collectnlearn/image2:i");
    ret = ret && velocityPort.open("/collectnlearn/velocity:i");
    ret = ret && collectnlearndata.open("/collectnlearn/data:o");
    ret = ret && origImgout1.open("/collectnlearn/image1:o");
    ret = ret && origImgout2.open("/collectnlearn/image2:o");
    trainingdatasize_value = datasize_value;
    datacollection_check = 0;
    file.open("velocityArray.csv", std::fstream::trunc);
    file.close();
    	 mlu = new ML2(0,  "flowstatistics_train_mu");
	 mlv = new ML2(1,  "flowstatistics_train_mv");
	 mlsiguu = new ML2(2,  "flowstatistics_train_siguu");
	 mlsigvv = new ML2(3,  "flowstatistics_train_sigvv");
	 mlsiguv = new ML2(4,  "flowstatistics_train_siguv");
    return ret;
}

bool collectnlearn::close()
{
    imagePort.close();
    inPort1.close();
    inPort2.close();
    velocityPort.close();
    collectnlearndata.close();
    origImgout1.close();
    origImgout2.close();
    return true;
}

bool collectnlearn::interrupt()
{
    inPort1.interrupt();
    inPort2.interrupt();
    velocityPort.interrupt();
    return true;
}

