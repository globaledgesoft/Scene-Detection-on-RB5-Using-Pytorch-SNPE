#include "opencv2/opencv.hpp"
#include <iostream>
#include <snpe_driver.hpp>
#include <LoadModel.hpp>
#include <Predict.hpp>
#include "SNPE/SNPE.hpp"
#include <math.h>
#include <unistd.h>
#include <sstream>

using namespace std;
using namespace cv;

/************************************************************************
* name : calculateProbabilty
* function: Apply softmax on the input vector 
************************************************************************/
void calculateProbabilty(std::vector <float> &snpeOutVec) {
  float m = *max_element(snpeOutVec.begin(), snpeOutVec.end());
  float sum = 0.0;
  int i;
  for (i = 0; i < snpeOutVec.size(); i++) {
    sum += expf(snpeOutVec[i]-m);
  }

  for (i = 0; i < snpeOutVec.size(); i++) {
      snpeOutVec[i] = expf(snpeOutVec[i] - m - log(sum));
  }  
}

int main(int argc, char *argv[]){

  Mat frame;
  Mat snpeInMat;
  Mat snpeOutMat; 
  int n = SNPE_IN_W * SNPE_IN_H * SNPE_IN_C * sizeof(float);
  float *data = NULL;
  uint8_t *dataChar = NULL;
  std::vector<uint8_t> snpeInVec(SNPE_IN_W*SNPE_IN_H*SNPE_IN_C*sizeof(float),0);
  std::vector<float> snpeOutVec(SNPE_OUT_COLS * SNPE_OUT_ROWS,0);
  std::unique_ptr<zdl::SNPE::SNPE> snpeModelHandler;
  static string modelPath;
  int sysType = CPU_T;
  int opt = 0;
  static string cameraId = "0";
  int camIdInt = 0;

  while ((opt = getopt(argc, argv, ":hd:r:c:")) != -1) {
    switch (opt) {
      case 'h':
        cout<< "ARGUMENTS:\n"
          << "-------------------\n"
          << "  -d  <FILE>   Path to the DLC file.\n"
          << "  -r  <RUNTIME> The runtime to be used [gpu, dsp, cpu] (cpu is default). \n"
          << "  -c  <CAMERA ID> Camera id of video stream (default is 0)"
          << "\n";
          exit(0);
      case 'd': 
        modelPath = optarg;
        break;
      case 'r':
        if (strcmp(optarg, "cpu") == 0)
          sysType = CPU_T;
        else if (strcmp(optarg, "gpu") == 0)
          sysType = GPU_T;
        else if (strcmp(optarg, "gpu") == 0)
          sysType = DSP_T;
        break;
      case 'c': 
        cameraId = optarg;
        break;
      default:
        std::cout << "Please run with the -h flag to see required arguments" << std::endl;
        exit(0);        
    }
  }

  if (modelPath.empty()) {
     std::cout << "Please run with the -h flag to see required arguments" << std::endl;
     exit(0);
  }

  //Load the DLC Model
  snpeModelHandler = loadModel(modelPath, sysType);
  if(snpeModelHandler == NULL) {
    exit(0);
  }
  std::cout<<"Model loaded"<<std::endl;

  VideoCapture cap(qtiqmmfsrc name=qmmf ! video/x-raw, format=NV12, width=640, height=480, framerate=30/1 ! videoconvert ! video/x-raw,format=BGR ! appsink drop=1", cv::CAP_GSTREAMER); 

  // Check if camera opened successfully
  if(!cap.isOpened())
  {
    cout << "Error opening video stream" << endl; 
    return -1; 
  } 

  // Default resolution of the frame is obtained.The default resolution is system dependent. 
  int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH); 
  int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
  while(cap.isOpened())
  {
    // Capture frame-by-frame 
    cap >> frame;
 
    // If the frame is empty, break immediately
    if (frame.empty())
    {
      std::cout<<"Could not frame"<<std::endl;
      break;
    }
    cvtColor(frame,frame,CV_BGR2RGB);
    resize(frame, snpeInMat, Size(SNPE_IN_W, SNPE_IN_H));
    snpeInMat.convertTo(snpeInMat,CV_32FC3, 1.0/255.0);
    data = (float *)snpeInMat.data;
    data = channelFirst(data);
    dataChar = (uint8_t*)data;
    snpeOutMat.create(SNPE_OUT_ROWS, SNPE_OUT_COLS, CV_32FC1);
    snpeInVec.assign(dataChar, dataChar + n);
    //Model Inferencing
    snpeOutVec = predictSnpe(snpeModelHandler, snpeInVec);
    if (snpeOutVec.empty()) {
      std::cout<<"Error: inference failed!"<<std::endl;
        return 0;  
    }
    //Applying softmax on input vector
    calculateProbabilty(snpeOutVec);
    for(int i=0; i<snpeOutVec.size();i++) {
      std::cout<<"probabilty : "<<i<<" :"<<snpeOutVec[i]<<std::endl;
    }
    std::cout<<"probabilty of being in indoor::"<<snpeOutVec[0]<<std::endl;
    std::cout<<"\n"<<std::endl;
    // Press  ESC on keyboard to  exit
    char c = (char)waitKey(1);
    if( c == 27 ) 
      break;
  }
  cap.release();
  return 0;
}

