#include <snpe_driver.hpp>
#include <stdlib.h>

/************************************************************************
* name : channelFirst
* function: function to convert image in channel first ordering
************************************************************************/
float *channelFirst(float *data) 
{
    int c = SNPE_IN_C;
    int w = SNPE_IN_W;
    int h = SNPE_IN_H;
    int fold_w_h = SNPE_IN_W*SNPE_IN_H;
    int fold_c_w = c*SNPE_IN_W;
    int fold_c_img_w = c*SNPE_IN_W;

    float * data_cp = (float *)malloc(SNPE_IN_H*SNPE_IN_W*SNPE_IN_C*sizeof(float));
    for(int k = 0; k < c; k++) {
        for(int j = 0; j < h; j++) {
            for(int i = 0; i < w; i++) {
                  
                    int dst_f = i + w*j + fold_w_h*k;
               
                    int dst_d = k + c*i + fold_c_w*j;
		    data_cp[dst_f] = data[dst_d];
	    }
	}
    }
    return data_cp;
}