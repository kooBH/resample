/**********************************************************************

  testresample.c

  Real-time library interface by Dominic Mazzoni

  Based on resample-1.7:
    http://www-ccrma.stanford.edu/~jos/resample/

  Dual-licensed as LGPL and BSD; see README.md and LICENSE* files.

**********************************************************************/

#include "libresample.h"

#include "WAV.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN(A, B) (A) < (B)? (A) : (B)

void runtest(int srclen, double freq, double factor,
             int srcblocksize, int dstblocksize)
{
   int expectedlen = (int)(srclen * factor);
   int dstlen = expectedlen + 1000;
   float *src = (float *)malloc((srclen+100) * sizeof(float));
   float *dst = (float *)malloc((dstlen+100) * sizeof(float));
   void *handle;
   double sum, sumsq, err, rmserr;
   int i, out, o, srcused, errcount, rangecount;
   int statlen, srcpos, lendiff;
   int fwidth;

   printf("-- srclen: %d sin freq: %.1f factor: %.3f srcblk: %d dstblk: %d\n",
          srclen, freq, factor, srcblocksize, dstblocksize);

   for(i=0; i<srclen; i++)
      src[i] = sin(i/freq);
   for(i=srclen; i<srclen+100; i++)
      src[i] = -99.0;

   for(i=0; i<dstlen+100; i++)
      dst[i] = -99.0;

   handle = resample_open(1, factor, factor);
   fwidth = resample_get_filter_width(handle);
   out = 0;
   srcpos = 0;
   for(;;) {
      int srcBlock = MIN(srclen-srcpos, srcblocksize);
      int lastFlag = (srcBlock == srclen-srcpos);

      o = resample_process(handle, factor,
                           &src[srcpos], srcBlock,
                           lastFlag, &srcused,
                           &dst[out], MIN(dstlen-out, dstblocksize));
      srcpos += srcused;
      if (o >= 0)
         out += o;
      if (o < 0 || (o == 0 && srcpos == srclen))
         break;
   }
   resample_close(handle);

   if (o < 0) {
      printf("Error: resample_process returned an error: %d\n", o);
   }

   if (out <= 0) {
      printf("Error: resample_process returned %d samples\n", out);
      free(src);
      free(dst);
      return;
   }

   lendiff = abs(out - expectedlen);
   if (lendiff > (int)(2*factor + 1.0)) {
      printf("   Expected ~%d, got %d samples out\n",
             expectedlen, out);
   }
   
   sum = 0.0;
   sumsq = 0.0;
   errcount = 0.0;

   /* Don't compute statistics on all output values; the last few
      are guaranteed to be off because it's based on far less
      interpolation. */
   statlen = out - fwidth;

   for(i=0; i<statlen; i++) {
      double diff = sin((i/freq)/factor) - dst[i];
      if (fabs(diff) > 0.05) {
         if (errcount == 0)
            printf("   First error at i=%d: expected %.3f, got %.3f\n",
                   i, sin((i/freq)/factor), dst[i]);
         errcount++;
      }
      sum += fabs(diff);
      sumsq += diff * diff;
   }

   rangecount = 0;
   for(i=0; i<statlen; i++) {
      if (dst[i] < -1.01 || dst[i] > 1.01) {
         if (rangecount == 0)
            printf("   Error at i=%d: value is %.3f\n", i, dst[i]);
         rangecount++;
      }
   }
   if (rangecount > 1)
      printf("   At least %d samples were out of range\n", rangecount);

   if (errcount > 0) {
      i = out - 1;
      printf("   i=%d:  expected %.3f, got %.3f\n",
             i, sin((i/freq)/factor), dst[i]);
      printf("   At least %d samples had significant error.\n", errcount);
   }
   err = sum / statlen;
   rmserr = sqrt(sumsq / statlen);
   printf("   Out: %d samples  Avg err: %f RMS err: %f\n", out, err, rmserr);
   free(src);
   free(dst);
}

int main(int argc, char **argv){

  WAV in_48k(1,48000);
  WAV mid_16k(1,16000);
  WAV out_48k(1,48000);

  in_48k.OpenFile("in_48k.wav");
  mid_16k.NewFile("mid_16k.wav");
  out_48k.NewFile("out_48k.wav");

  short* buf_io = new short[384];
  short* buf_mid = new short[128];

  float* buf_io_f = new float[384];
  float* buf_mid_f = new float[128];

   void *handle;
   int i, out, o, srcused, errcount, rangecount;
   int statlen, srcpos, lendiff;
   int fwidth;

   int srclen = 384;
   double factor = 0.3333;
   int expectedlen = (int)(srclen * factor);


   handle = resample_open(1, factor, factor);
   fwidth = resample_get_filter_width(handle);

   while (!in_48k.IsEOF()) {
    in_48k.ReadUnit(buf_io, 384);

    for (int i = 0; i < 384; i++) {
       buf_io_f[i] = static_cast<float>(buf_io[i])/32768.0;
    }

   o = resample_process(handle, factor,
                           buf_io_f, 384,
                           0, &srcused,
                           buf_mid_f, 128);
   for (int i = 0; i < 128; i++)
      buf_mid[i] = static_cast<short>(buf_mid_f[i]*32768.0);
   
   mid_16k.Append(buf_mid, 128);
  }
   mid_16k.Finish();
   in_48k.Finish();
  resample_close(handle);

  ///
   srclen = 128;
   factor = 3.0;
   expectedlen = (int)(srclen * factor);


   mid_16k.OpenFile("mid_16k.wav");
   handle = resample_open(1, factor, factor);
   fwidth = resample_get_filter_width(handle);

   while (!mid_16k.IsEOF()) {
    mid_16k.ReadUnit(buf_mid, srclen);

    for (int i = 0; i < srclen; i++) {
       buf_mid_f[i] = static_cast<float>(buf_mid[i])/32768.0;
    }

   o = resample_process(handle, factor,
                           buf_mid_f, 128,
                           0, &srcused,
                           buf_io_f, 384);
   for (int i = 0; i < 384; i++)
      buf_io[i] = static_cast<short>(buf_io_f[i]*32768.0);
   
   out_48k.Append(buf_io, 384);
  }
   mid_16k.Finish();
   out_48k.Finish();



  resample_close(handle);
  



  /*
   int i, srclen, dstlen, ifreq;
   double factor;

   printf("\n*** Vary source block size*** \n\n");
   srclen = 10000;
   ifreq = 100;
   for(i=0; i<20; i++) {
      factor = ((rand() % 16) + 1) / 4.0;
      dstlen = (int)(srclen * factor + 10);
      runtest(srclen, (double)ifreq, factor, 64, dstlen);
      runtest(srclen, (double)ifreq, factor, 32, dstlen);
      runtest(srclen, (double)ifreq, factor, 8, dstlen);
      runtest(srclen, (double)ifreq, factor, 2, dstlen);
      runtest(srclen, (double)ifreq, factor, srclen, dstlen);
   }

   printf("\n*** Vary dest block size ***\n\n");
   srclen = 10000;
   ifreq = 100;
   for(i=0; i<20; i++) {
      factor = ((rand() % 16) + 1) / 4.0;
      runtest(srclen, (double)ifreq, factor, srclen, 32);
      dstlen = (int)(srclen * factor + 10);
      runtest(srclen, (double)ifreq, factor, srclen, dstlen);
   }

   printf("\n*** Resample factor 1.0, testing different srclen ***\n\n");
   ifreq = 40;
   for(i=0; i<100; i++) {
      srclen = (rand() % 30000) + 10;
      dstlen = (int)(srclen + 10);
      runtest(srclen, (double)ifreq, 1.0, srclen, dstlen);
   }

   printf("\n*** Resample factor 1.0, testing different sin freq ***\n\n");
   srclen = 10000;
   for(i=0; i<100; i++) {
      ifreq = ((int)rand() % 10000) + 1;
      dstlen = (int)(srclen * 10);
      runtest(srclen, (double)ifreq, 1.0, srclen, dstlen);
   }

   printf("\n*** Resample with different factors ***\n\n");
   srclen = 10000;
   ifreq = 100;
   for(i=0; i<100; i++) {
      factor = ((rand() % 64) + 1) / 4.0;
      dstlen = (int)(srclen * factor + 10);
      runtest(srclen, (double)ifreq, factor, srclen, dstlen);
   }
*/
   return 0;
}
