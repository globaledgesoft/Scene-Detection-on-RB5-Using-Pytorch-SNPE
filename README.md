
<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
* [Prerequisites](#prerequisites)
* [Getting Started](#getting-started)
  * [Steps to convert Pytorch Model to DLC](#Steps-to-convert-Pytorch-Model-to-DLC)
  * [Steps to build Shared Library (SNPE)](#Steps-to-build-Shared-Library-(SNPE))
  * [Steps to Build Main Application](#Steps-to-Build-Main-Application)
* [Usage](#usage)
* [Contributors](#contributors)

<!-- ABOUT THE PROJECT -->
## About The Project

This project is intended to build and deploy an scene detection application onto Qualcomm Robotics development Kit (RB5) that detects whether the bot is in an indoor or outdoor environment and takes necessary actions.
The source code is arranged in the following directories:
 * src : This directory contains code for the main application which act as a driver for capturing the video stream from camera and decide on the movement of bot according to the scene in which bot is present.
* snpe_shared_library : This directory contains the code for loading and inferring DLC model. 
* artifacts : This directory contains  pytorch, ONNX and DLC model of Scene detection.

## Prerequisites

1. A Linux workstation with Ubuntu 18.04.

2. Install Android Platform tools (ADB, Fastboot) 

3. Download and install the SDK [Manager](https://developer.qualcomm.com/qualcomm-robotics-rb5-kit/quick-start-guide/qualcomm_robotics_rb5_development_kit_bring_up/download-and-install-the-SDK-manager)

4. [Flash](https://developer.qualcomm.com/qualcomm-robotics-rb5-kit/quick-start-guide/qualcomm_robotics_rb5_development_kit_bring_up/flash-images) the RB5 firmware image on to the board

5. Setup the [Network](https://developer.qualcomm.com/qualcomm-robotics-rb5-kit/quick-start-guide/qualcomm_robotics_rb5_development_kit_bring_up/set-up-network) 


6. Setup the [SNPE SDK](https://developer.qualcomm.com/docs/snpe/setup.html) in the host system. You can download SNPE SDK [here](https://developer.qualcomm.com/downloads/qualcomm-neural-processing-sdk-ai-v143).

7. Setup pytorch and ONNX on the host system.

8. Setup opencv from source for cpp support on RB5.

9. Turtlebot burger is assembled, operational and is connected to RB3

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these steps.
1. Clone the  project repository from the gitlab to host system.
```sh
git clone <URL of REPO>
cd rb5-scene-detection
```
Create a directory named rb5-scene-detection in the home directory of RB5.
```sh
adb shell
mkdir /home/rb5-scene-detection
exit
```

3. Push the directories/files named bot_py, include, src and artifacts from project directory to newly created directory in RB5.
```sh
adb push artifacts /home/rb5-scene-detection
adb push bot_py /home/rb5-scene-detection
adb push src /home/rb5-scene-detection
adb push include /home/rb5-scene-detection
```

### Steps to convert Pytorch Model to DLC
In this project we have created a scene detection model using pytorch. As SNPE doesn’t  support pytorch models directly, we have to convert pytorch model to ONNX first. Later on we will use snpe-onnx-to-dlc script provided by SNPE SDK to convert onnx model to DLC. A python script to convert the pytorch model to onnx is present in the python_scripts directory. A pre-trained pytorch scene detection model is placed in the artifacts directory.  

1. Run the python script to convert pytorch model to ONNX
```sh
python pytorch_to_onnx.py
```
If successful, ONNX model (best_model.onnx) will be generated in artifacts.

2. Convert the onnx model to dlc by executing the following command:
```sh
snpe-onnx-to-dlc --input_network artifacts/best_model.onnx -o artifacts/best_model.dlc
```
3. Push the dlc model to RB5
```sh
adb push artifacts/best_model.dlc /home/rb5-scene-detection/artifacts
```

### Steps to build Shared Library (SNPE):
This shared library can be cross compiled by following  steps (If toolchain for cross compilation is not present in your system, skip step 2 and 3):
1. From project directory change the directory to that of shared library source
```sh
cd snpe_shared_library
```
2. Run the Makefile (Note: Make sure  App Tool chain SDK for cross compilation and setup for SNPE SDK in the host system is ready)
```sh
make -f Makefile.aarch64-linux-gcc4.9 
```
3. Once the build is successful, libSNPEScene.so will be created in obj/local/aarch64-linux-gcc4.9/

4. Push the shared library .so file to RB5
```sh
adb push obj/local/aarch64-linux-gcc4.9/libSNPESceneso /lib
adb push $SNPE_ROOT/lib/aarch64-linux-gcc4.9/libSNPE.so /lib
```

### Steps to Build Main Application:
1. Access RB5 through adb and change directory to /home/rb5-scene-detection
```sh
adb shell
cd /home/rb5-scene-detection
```
2. Execute the following command for compiling main application
```sh
g++ src/*.cpp -o rb5-scene-detection -I ./include/  -I $SNPE_ROOT/include/zdl/ -lSNPE -lSNPEScene `pkg-config --cflags --libs opencv`
```
3. If compilation is successful, an executable named rb5-scene-detection will be generated.

<!-- USAGE -->
## Usage
The executable takes 3 arguments. The Path to DLC model is compulsory and the other 2 arguments are optional. Arguments supported  are as follows:
1. -h <FILE> : Help
2. -d : Path to the DLC file
3. -r  <RUNTIME> : The runtime to be used [gpu, dsp, cpu] (cpu is default).
4. -c  <CAMERA ID> : Camera id of video stream (default is 0)

Follow the below steps for running the application:

1. Access the RB5 through adb
```sh
adb shell
cd /home/rb5-scene-detection
```
2. To run the application in CPU mode execute the following command in shell
```sh
./rb5-scene-detection -d <PATH-TO_DLC> -r cpu -c 0
```

<!-- ## Contributors -->
## Contributors
* [Rakesh Sankar](s.rakesh@globaledgesoft.com)
* [Steven P](ss.pandiri@globaledgesoft.com)
* [Ashish Tiwari](t.ashish@globaledgesoft.com)
* [Venkata Sai Kiran J](sv.jinka@globaledgesoft.com)
* [Arunraj A P](ap.arunraj@globaledgesoft.com)






