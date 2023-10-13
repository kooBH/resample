#include "resampler.h"

#include "WAV.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

  WAV in_48k(1,48000);
  WAV mid_16k(1,16000);
  WAV out_48k(1,48000);


  short* buf_io = new short[384];
  short* buf_mid = new short[128];

  // Downsampling
  in_48k.OpenFile("in_48k.wav");
  mid_16k.NewFile("mid_16k.wav");
   resampler r48to16(48000, 16000, 384);

   while (!in_48k.IsEOF()) {
    in_48k.ReadUnit(buf_io, 384);

    r48to16.resample(buf_io, buf_mid);

   mid_16k.Append(buf_mid, 128);
  }
   mid_16k.Finish();
   in_48k.Finish();

   // Upsampling
   mid_16k.OpenFile("mid_16k.wav");
   out_48k.NewFile("out_48k.wav");
   resampler r16to48(16000, 48000, 128);

   while (!mid_16k.IsEOF()) {
     mid_16k.ReadUnit(buf_mid, 128);

     r16to48.resample(buf_mid, buf_io);
     
     out_48k.Append(buf_io, 384);
  }
   mid_16k.Finish();
   out_48k.Finish();


   return 0;
}
