#include "resampler.h"

#include "WAV.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

  constexpr int sr_io = 44100;
  constexpr int sr = 16000;

  WAV in_48k(1,sr_io);
  WAV mid_16k(1,sr);
  WAV out_48k(1,sr_io);

  int n_hop = 128;
  //int n_hop_io = 384;
  int n_hop_io = 352;


  short* buf_io = new short[n_hop_io];
  short* buf_mid = new short[n_hop];

  // Downsampling
  in_48k.OpenFile("in.wav");
  mid_16k.NewFile("mid_16k.wav");

   resampler r48to16(sr_io, sr, n_hop_io, n_hop);

   while (!in_48k.IsEOF()) {
    in_48k.ReadUnit(buf_io,n_hop_io);

    if (!r48to16.resample(buf_io, buf_mid))
      continue;

   mid_16k.Append(buf_mid, n_hop);
  }
   mid_16k.Finish();
   in_48k.Finish();

   // Upsampling
   mid_16k.OpenFile("mid_16k.wav");
   out_48k.NewFile("out.wav");
   resampler r16to48(sr, sr_io, n_hop,n_hop_io);

   while (!mid_16k.IsEOF()) {
     mid_16k.ReadUnit(buf_mid, n_hop);

     r16to48.resample(buf_mid, buf_io);
     
     out_48k.Append(buf_io, n_hop_io);
  }
   mid_16k.Finish();
   out_48k.Finish();


   return 0;
}
