// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>
#include <yarp/os/Network.h>


#include "collectnlearn.h"
#include <yarp/os/RFModule.h>

using namespace yarp::os;

class DetectorModule: public RFModule
{
   collectnlearn iA;
public:

    bool configure(ResourceFinder &rf)
    {
	int trainingdatasize_value=rf.check("trainingdatasize_value", Value(1000), "trainingdatasize_value (int)").asInt();
        return iA.open(trainingdatasize_value);
    }

    double getPeriod()
    {
        return 0.0;
    }
    
    bool updateModule()
    { 
        if (iA.loop()) {
	  return true;
	}
        else {
	  return false;
	}
    }

    bool interruptModule()
    {
        fprintf(stderr, "Interrupting\n");
        iA.interrupt();
        return true;
    }

    bool close()
    {
        fprintf(stderr, "Calling close\n");
        iA.close();
        return true;
    }

    //void respond();

};

int main(int argc, char *argv[]) 
{
    Network yarp;

    DetectorModule module;
    ResourceFinder rf;
    rf.setVerbose();
    rf.setDefaultConfigFile("collnlearnconfig.ini");
    rf.setDefaultContext("collnlearn/conf");
    rf.configure(argc,argv);
    if (!module.configure(rf))
    {
        fprintf(stderr, "Error configuring module returning\n");
        return -1;
    }


    module.runModule();

    printf("Module shutting down\n");

    return 0;
}