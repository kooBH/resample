#pragma once

#include "libresample.h"

class resampler {
private : 
  float *buf_i;;
  float *buf_o;
  void* handle;

  int srclen;
  int destlen;
  int fwidth;
  int srcused;
  double factor;
  int expectedlen;

public : 
  resampler(int sr_orig, int sr_target, int size_buf);
~resampler();
void resample(short* buf_in, short*buf_out);
void resample(float * buf_in, float*buf_out);



};

resampler::resampler(int sr_orig_, int sr_target_, int size_buf_){
    factor = (double)sr_target_ / (double)sr_orig_;
    handle = resample_open(1, factor, factor);
    fwidth = resample_get_filter_width(handle);

    bool res = sr_target_ % sr_orig_;

    srclen = size_buf_;
    destlen = int(size_buf_*factor)+res;

    buf_i = new float[srclen];
    buf_o = new float[destlen];
}

resampler::~resampler(){
    delete[] buf_i;
    delete[] buf_o;
    resample_close(handle);
}

void resampler::resample(short* buf_in, short* buf_out) {

  for(int i=0; i< srclen;i++)
    buf_i[i] = (float)buf_in[i]/32768.0;

  resample_process(handle, factor, buf_i, srclen, 0, &srcused, buf_o, destlen);

  for(int i=0; i< destlen;i++)
    buf_out[i] = (short)(buf_o[i]*32768.0);
}

void resampler::resample(float* buf_in, float* buf_out) {

  resample_process(handle, factor, buf_in, srclen, 0, &srcused, buf_out, destlen);
}
