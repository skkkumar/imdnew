// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include <stdio.h>
#include <yarp/os/Network.h>
#include <iostream>

#include "imgAcq.h"
#include <yarp/os/RFModule.h>

using namespace yarp::os;
using namespace std;
class DetectorModule: public RFModule
{
   imgAcq iA;
public:

    bool configure(ResourceFinder &rf)
    {
        String robotName=rf.check("robot", Value("icubSim"), "Robot name (string)").asString().c_str();
	int no_of_velocityArray=rf.check("no_of_velocityArray", Value(6), "no_of_velocityArray (int)").asInt();
        return iA.open(robotName, no_of_velocityArray);
    }

    double getPeriod()
    {
        return 0.0;
    }
    
    bool updateModule()
    { 
        iA.loop();
        return true; 
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
    rf.setDefaultConfigFile("config.ini");
    rf.setDefaultContext("acq_img/conf");
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