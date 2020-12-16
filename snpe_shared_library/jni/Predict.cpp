#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <sys/time.h>
#include <bits/stdc++.h>
#include "CheckRuntime.hpp"
#include "LoadContainer.hpp"
#include "SetBuilderOptions.hpp"
#include "LoadInputTensor.hpp"
#include "udlExample.hpp"
#include "CreateUserBuffer.hpp"
#include "PreprocessInput.hpp"
#include "SaveOutputTensor.hpp"
#include "Util.hpp"
#include "DlSystem/DlError.hpp"
#include "DlSystem/RuntimeList.hpp"
#include "DlSystem/UserBufferMap.hpp"
#include "DlSystem/UDLFunc.hpp"
#include "DlSystem/IUserBuffer.hpp"
#include "DlContainer/IDlContainer.hpp"
#include "SNPE/SNPE.hpp"
#include "DiagLog/IDiagLog.hpp"

/************************************************************************
* name : predictSnpe
* function: Predict output for classification model and segmentation model
************************************************************************/
std::vector<float> predictSnpe(std::unique_ptr<zdl::SNPE::SNPE> &snpe, std::vector<uint8_t> &inBuffer)
{
    std::vector<uint8_t> outBuffer;
    bool execStatus = false;
    zdl::DlSystem::UserBufferMap inputMap, outputMap;
    std::vector <std::unique_ptr<zdl::DlSystem::IUserBuffer>> snpeUserBackedInputBuffers, snpeUserBackedOutputBuffers;
    std::unordered_map <std::string, std::vector<uint8_t>> applicationOutputBuffers;
    std::unordered_map <std::string, std::vector<uint8_t>> applicationInputBuffers;
    struct timeval clt_start, clt_end;
    double time_taken;
    zdl::DlSystem::TensorShape tensorShape;
    const long unsigned int* inShape;
    int inSize;
    float * data;
    unsigned char floatBuffer[4];
    std::vector<float> outFloatBuffer;
    int counter = 0;

    tensorShape = snpe->getInputDimensions();
    inShape = tensorShape.getDimensions();
    inSize = inShape[1]*inShape[2]*inShape[3];
    if(inSize != inBuffer.size()/sizeof(float)) {
        std::cout<<"Input Size mismatch!"<<std::endl;
        std::cout<<"Expected: "<<inSize<<std::endl;
        std::cout<<"Got: "<<inBuffer.size()/sizeof(float)<<std::endl;
        return std::vector<float> ();
    }

    createOutputBufferMap(outputMap, applicationOutputBuffers, snpeUserBackedOutputBuffers, snpe, false);
    createInputBufferMap(inputMap, applicationInputBuffers, snpeUserBackedInputBuffers, snpe, false);
    
    if(!loadInputUserBufferFloatFromBufer(applicationInputBuffers, snpe, inBuffer))
    {
        return std::vector<float>();
    }

    gettimeofday(&clt_start, NULL);
    // Execute the input buffer map on the model with SNPE
    execStatus = snpe->execute(inputMap, outputMap);
    gettimeofday(&clt_end, NULL);
    time_taken = (clt_end.tv_sec - clt_start.tv_sec) * 1e6;
    time_taken = (time_taken + (clt_end.tv_usec - clt_start.tv_usec)) * 1e-6;
    std::cout<<"Model Performance"<<std::endl;
    if ((time_taken) != 0) {
        std::cout<<"Throuhput::"<<std::setprecision(10)<<time_taken<<std::endl;
        std::cout<<"FPS::"<<1.0/(time_taken)<<std::setprecision(5)<<std::endl;
    } else {
        std::cout<<"Throuhput::"<<std::setprecision(10)<<time_taken<<std::endl;
    }

    // Save the execution results only if successful
    if (execStatus == true)
    {
        if(!saveOutput(outputMap, applicationOutputBuffers, outBuffer))
        {
            return std::vector<float>();
        }
    }
    else
    {
        std::cerr << "Error while executing the network." << std::endl;
        return std::vector<float>();
    }


    for(int i=0; i<outBuffer.size(); i++) {
        floatBuffer[counter++] = outBuffer.at(i);
        if (counter % 4 == 0) {
            data = (float*) floatBuffer;
            outFloatBuffer.push_back(*data);
            counter = 0;
        }
    }

    return outFloatBuffer;
}
