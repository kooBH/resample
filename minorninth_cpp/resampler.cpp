#pragma once

#include "resampler.h"

resampler::resampler(int sr_orig_, int sr_target_, int size_in_, int size_out){
    factor = (double)sr_target_ / (double)sr_orig_;
    handle = resample_open(1, factor, factor);
    fwidth = resample_get_filter_width(handle);

    bool res = sr_target_ % sr_orig_;

    srclen = size_in_;
    destlen = int(size_in_*factor)+res;

    printf("%d -> %d | factor : %lf | buf %d -> %d\n",sr_orig_,sr_target_,factor,srclen,destlen);

    double est = (128 - res) / factor;
    //printf("%lf %d\n",est, int(est));

    sz_in = size_in_;
    sz_out = size_out;

    buf_in = new float[sz_in];
    memset(buf_in, 0, sizeof(float)*sz_in);
    buf_out_rs = new float[destlen];
    memset(buf_out_rs, 0, sizeof(float)*destlen);
    buf_out_ret = new float[sz_out];
    memset(buf_out_ret, 0, sizeof(float)*sz_out);

    float_in = new float[srclen];
    float_out = new float[destlen];
}

resampler::~resampler(){
    delete[] float_in;
    delete[] float_out;
    delete[] buf_in;
    delete[] buf_out_rs;
    delete[] buf_out_ret;
    resample_close(handle);
}

int resampler::resample(short* float_inn, short* float_outut) {
  for(int i=0; i< srclen;i++)
    float_in[i] = (float)float_inn[i]/32768.0;

  int ret = this->resample(float_in, float_out);
  if (ret == 0) {
    printf("skip\n");
    return ret;
  }

  for(int i=0; i< sz_out;i++)
    float_outut[i] = (short)(float_out[i]*32768.0);
  return ret;
}

int resampler::resample(float* buf_in_, float* buf_ret) {
  int ret = 0;

  int n_out =  resample_process(handle, factor, buf_in_, srclen, 0, &srcused, buf_out_rs, destlen);

  // buffering
  for (int i = 0; i < n_out; i++) {
    buf_out_ret[idx_out++] = buf_out_rs[i];
    if (idx_out == sz_out) {
      memcpy(buf_ret, buf_out_ret, sizeof(float) * sz_out);
      idx_out = 0;
      ret = sz_out;
      printf("buffer rewind at %d\n",i);
    }
  }
  printf("resample : n_out : %d | idx_out : %d | srcused : %d\n",n_out, idx_out,srcused);

  return ret;
}
