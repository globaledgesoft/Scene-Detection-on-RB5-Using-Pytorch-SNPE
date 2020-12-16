//==============================================================================
//
//  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
//  All Rights Reserved.
//  Confidential and Proprietary - Qualcomm Technologies, Inc.
//
//==============================================================================

#include <iostream>
#include <algorithm>
#include <sstream>
#include <unordered_map>

#include "SaveOutputTensor.hpp"
#include "Util.hpp"

#include "SNPE/SNPE.hpp"
#include "DlSystem/ITensor.hpp"
#include "DlSystem/StringList.hpp"
#include "DlSystem/TensorMap.hpp"
#include "DlSystem/TensorShape.hpp"

// Print the results to raw files
// ITensor
bool saveOutput (zdl::DlSystem::TensorMap outputTensorMap,
                 const std::string& outputDir,
                 int num,
                 size_t batchSize)
{
    // Get all output tensor names from the network
    zdl::DlSystem::StringList tensorNames = outputTensorMap.getTensorNames();

    // Iterate through the output Tensor map, and print each output layer name to a raw file
    for( auto& name : tensorNames)
    {
        // Split the batched output tensor and save the results
        for(size_t i=0; i<batchSize; i++) {
            std::ostringstream path;
            path << outputDir << "/"
                 << "Result_" << num + i << "/"
                 << name << ".raw";
            auto tensorPtr = outputTensorMap.getTensor(name);
            size_t batchChunk = tensorPtr->getSize() / batchSize;

            if(!SaveITensorBatched(path.str(), tensorPtr, i, batchChunk))
            {
                return false;
            }
        }
    }
    return true;
}

// Save the output to the buffer passed
bool saveOutput (zdl::DlSystem::UserBufferMap& outputMap,
                 std::unordered_map<std::string,std::vector<uint8_t>>& applicationOutputBuffers,
                 std::vector<uint8_t> &buffer)
{
   // Get all output buffer names from the network
   const zdl::DlSystem::StringList& outputBufferNames = outputMap.getUserBufferNames();

   // Iterate through output buffers
   for(auto& name : outputBufferNames)
   {
        buffer = applicationOutputBuffers.at(name);
   }
   return true;
}
