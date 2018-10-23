/******************************************************************************
 *  Copyright (c) 2016, Xilinx, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1.  Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2.  Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *  3.  Neither the name of the copyright holder nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION). HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************/

 /******************************************************************************
 *
 *  Authors: Giulio Gambardella <giuliog@xilinx.com>
 *           Thomas B. Preusser <thomas.preusser@utexas.edu>
 *             Marie-Curie Fellow, Xilinx Ireland, Grant Agreement No. 751339
 *           Christoph Doehring <cdoehrin@xilinx.com>
 *
 *  @file slidingwindow.h
 *
 *  Library of templated HLS functions for BNN deployment. 
 *  This file lists a set of convenience funtions used to implement  
 *  Sliding window generator for convolutions
 *
 *****************************************************************************/

#ifndef SLIDINGWINDOW_H
#define SLIDINGWINDOW_H
 
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) > (y)) ? (y) : (x))

// sliding window unit that produces several vectors simultaneously for feeding
// a matrix multiple vectors unit
template<unsigned int ConvKernelDim, 
		 unsigned int IFMChannels,
		 unsigned int Input_precision,		// Number of bits for each pixel
		 unsigned int IFMDim, 
		 unsigned int OFMDim,
		 unsigned int SIMD,
		 unsigned int Stride = 1>  			
 
void ConvolutionInputGenerator(
		stream<ap_uint<SIMD*Input_precision> > & in,
		stream<ap_uint<SIMD*Input_precision> > & out,
		const unsigned int numReps = 1) {
	
  if(IFMChannels % SIMD != 0) {
    cout << "Error: IFM channels has to be a multiple of SIMD" << endl;
  }
  if(ConvKernelDim % Stride != 0) {
    cout << "Error: Kernel size has to be multiple of Stride" << endl;
  }
  const unsigned int multiplying_factor = IFMChannels/SIMD;
  const unsigned int number_blocks = ConvKernelDim/Stride + 1 ;
  ap_uint<SIMD*Input_precision> inputBuf[number_blocks][Stride * IFMDim * multiplying_factor];

#pragma HLS ARRAY_PARTITION variable=inputBuf complete dim=1
#pragma HLS RESOURCE variable inputBuf core=RAM_2P
  const unsigned int cycles_write_block = (OFMDim * ConvKernelDim * ConvKernelDim * multiplying_factor);
  const unsigned int cycles_read_block = Stride * IFMDim * multiplying_factor;
  const unsigned int max_cycles = MAX(cycles_write_block,cycles_read_block);
  const unsigned int baseIter = IFMDim * ConvKernelDim * multiplying_factor// Initial buffer
			                  + OFMDim * MAX(cycles_write_block,cycles_read_block);
  unsigned int counter_internal_block = 0;
  unsigned int current_block_write = 0;
  unsigned int next_block_write = 0;	
  unsigned int current_line = 0;
  unsigned int read_block = 0; 
  unsigned int inp = 0, ofm_y = 0, ofm_x = 0, k_y = 0, k_x = 0, count_simd =0;
#pragma HLS reset variable=inp
  for (unsigned int count_image = 0; count_image < numReps; count_image++) {
    for (unsigned int i = 0; i < baseIter; i++) {
#pragma HLS PIPELINE II=1
      if (inp < IFMDim * ConvKernelDim*multiplying_factor) {// Initial buffer of ConvKernelDim lines	
        ap_uint<SIMD*Input_precision> inElem;
        inElem = in.read();
        inputBuf[current_block_write][current_line] = inElem;
        current_line++;
        inp++;
        if (current_line == Stride * IFMDim * multiplying_factor ) {
          current_line = 0;
          current_block_write++;
          if (current_block_write == number_blocks) {
            current_block_write=0;
          }
          read_block++;
          counter_internal_block = 0;
        }
      } else {
        if (counter_internal_block < cycles_write_block-1) { // We are writing output, MMV IFMChan per cycle
          unsigned int current_block_read = (current_block_write + 1 + k_y / Stride);
          if (current_block_read >= number_blocks) {
            current_block_read-= number_blocks;
		  }
          unsigned int current_line_in_block = ((k_y%Stride) * IFMDim + ofm_x*Stride + k_x)*multiplying_factor + count_simd;
          ap_uint<SIMD*Input_precision> outElem = inputBuf[current_block_read][(current_line_in_block)];
          out.write(outElem);
          count_simd++;
          if (count_simd == multiplying_factor) {
            count_simd=0;					
            k_x++;
            if (k_x == ConvKernelDim) {
              k_x = 0;
              k_y++;
              if (k_y == ConvKernelDim) {
                k_y = 0;
                ofm_x ++;
                if (ofm_x == OFMDim) {
                  ofm_x = 0;
                  ofm_y++;
                  if (ofm_y == OFMDim) {
                    ofm_y = 0;
                    inp = 0;
                  }
                }
              }
            }
          }
        }
        if ((counter_internal_block < cycles_read_block-1) && (read_block<IFMDim/Stride)) { // In parallel we write in the buffer, in the current block write if we still need to
          ap_uint<SIMD*Input_precision> inElem;
          inElem = in.read();
          inputBuf[current_block_write][current_line] = inElem;
#pragma AP dependence variable=inputBuf intra false
#pragma AP dependence variable=inputBuf inter false
          current_line++;
          if (current_line == Stride * IFMDim * multiplying_factor) {// We read the whole block, we change the next block in which we want to we
            // We filled up a block, let's not read until
            current_line = 0;
            read_block++;
            current_block_write++;
            if (current_block_write == number_blocks) {
              current_block_write=0;
			}
#pragma AP dependence variable=current_block_write intra false	
          }
        }
        counter_internal_block++; // = (counter_internal_block +1) % max_cycles;
        if (counter_internal_block == (max_cycles-1)) {
          counter_internal_block = 0;
        }
      }
    } // End base_iter
	read_block = 0;
  } // End count_image
} // End generator

#endif
