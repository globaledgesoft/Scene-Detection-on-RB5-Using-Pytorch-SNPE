#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>
#include <algorithm>
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
* name : loadModel
* function: Load the dlc model
************************************************************************/
std::unique_ptr<zdl::SNPE::SNPE> loadModel(std::string &dlc, int systemType) 
{
   static zdl::DlSystem::Runtime_t runtime = zdl::DlSystem::Runtime_t::CPU;
   static zdl::DlSystem::RuntimeList runtimeList;
   bool usingInitCaching = false;

   if (systemType == 0) {
      runtime = zdl::DlSystem::Runtime_t::CPU;
   } else if(systemType == 1) {
      runtime = zdl::DlSystem::Runtime_t::GPU;
   } else if (systemType == 2) {
      runtime = zdl::DlSystem::Runtime_t::DSP;
   }
   std::cout<<"Loaded Model Located at: "<<dlc<<std::endl;
   std::ifstream dlcFile(dlc);
   if (!dlcFile) {
      std::cout << "Dlc file not valid. Please ensure that you have provided a valid dlc for processing." << std::endl;
      return NULL;
   }
   zdl::DlSystem::UDLFactoryFunc udlFunc = UdlExample::MyUDLFactory;
   zdl::DlSystem::UDLBundle udlBundle; udlBundle.cookie = (void*)0xdeadbeaf, udlBundle.func = udlFunc;
   runtime = checkRuntime(runtime);
   std::unique_ptr<zdl::DlContainer::IDlContainer> container = loadContainerFromFile(dlc);
   if (container == nullptr)
   {
      std::cerr << "Error while opening the container file." << std::endl;
      return NULL;
   }
   bool useUserSuppliedBuffers = true;
   std::unique_ptr<zdl::SNPE::SNPE> snpe;
   zdl::DlSystem::PlatformConfig platformConfig;
   snpe = setBuilderOptions(container, runtime, runtimeList, udlBundle, useUserSuppliedBuffers, platformConfig, usingInitCaching);
   if (snpe == nullptr) {
      std::cerr << "Error while building SNPE object." << std::endl;
      return NULL;
   }
   if (usingInitCaching) {
      if (container->save(dlc)) {
         std::cout << "Saved container into archive successfully" << std::endl;
      }
      else {
         std::cout << "Failed to save container into archive" << std::endl;
      }
   }

   return snpe;
}
