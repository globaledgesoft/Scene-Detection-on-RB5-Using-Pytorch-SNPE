#ifndef SNPE_DRIVER
#define SNPE_DRIVER

#define SNPE_IN_W 224
#define SNPE_IN_H 224
#define SNPE_IN_C 3
#define SNPE_OUT_ROWS 1
#define SNPE_OUT_COLS 2

#define CPU_T 0
#define GPU_T 1
#define DSP_T 2

float *channelFirst(float *data);

#endif

