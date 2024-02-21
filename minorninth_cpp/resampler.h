#pragma once

#include "libresample.h"
#include <stdio.h>
#include <string.h>

class resampler {
private : 

  //--- variables for resampler ---//
  int srclen;
  int destlen;
  int fwidth;
  int srcused;
  double factor;
  int expectedlen;

  void* handle;

  //--- variables for buffer ---//
  int sz_in;
  int sz_out;

  int idx_in = 0;
  int idx_out = 0;

  // sample buffer
  float* buf_in;
  float* buf_out_rs; // resampler function  buffer
  float* buf_out_ret; // resample wrapper buffer

  // float buffer
  float *float_in;
  float *float_out;

public : 
  resampler(int sr_orig, int sr_target, int size_in, int size_out);
~resampler();
int resample(short* buf_in, short*buf_out);
int resample(float * buf_in, float*buf_out);



};
