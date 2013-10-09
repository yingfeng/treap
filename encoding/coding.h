// Copyright (c) 2008, WEST, Polytechnic Institute of NYU.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of WEST, Polytechnic Institute of NYU. nor the names 
// of its contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Torsten Suel, Jiangong Zhang, Jinru He
// 
// If you have any questions or problems about our codes, please contact:
// jhe@cis.poly.edu
// 
//
// the coding base class. all the encoding classes inherit from this.
// it includes some basic bitwise operation. 


#ifndef CODING_H_
#define CODING_H_

#include "unpack.h"

class coding
{
public:
	coding()
	{
		MASK[0] = 0x00000000;
		MASK[1] = 0x00000001;
		MASK[2] = 0x00000003;
		MASK[3] = 0x00000007;
		MASK[4] = 0x0000000f;
		MASK[5] = 0x0000001f;
		MASK[6] = 0x0000003f;
		MASK[7] = 0x0000007f;
		MASK[8] = 0x000000ff;
		MASK[9] = 0x000001ff;
		MASK[10] = 0x000003ff;
		MASK[11] = 0x000007ff;
		MASK[12] = 0x00000fff;
		MASK[13] = 0x00001fff;
		MASK[14] = 0x00003fff;
		MASK[15] = 0x00007fff;
		MASK[16] = 0x0000ffff;
		MASK[17] = 0x0001ffff;
		MASK[18] = 0x0003ffff;
		MASK[19] = 0x0007ffff;
		MASK[20] = 0x000fffff;
		MASK[21] = 0x001fffff;
		MASK[22] = 0x003fffff;
		MASK[23] = 0x007fffff;
		MASK[24] = 0x00ffffff;
		MASK[25] = 0x01ffffff;
		MASK[26] = 0x03ffffff;
		MASK[27] = 0x07ffffff;
		MASK[28] = 0x0fffffff;
		MASK[29] = 0x1fffffff;
		MASK[30] = 0x3fffffff;
		MASK[31] = 0x7fffffff;
		MASK[32] = 0xffffffff;

		unpack[0]= unpack0;
		unpack[1] = unpack1;
		unpack[2] =unpack2;
		unpack[3] = unpack3;
		unpack[4] = unpack4;
		unpack[5] = unpack5;
		unpack[6] = unpack6;
		unpack[7] = unpack7;
		unpack[8] = unpack8;
		unpack[9] = unpack9;
		unpack[10] = unpack10;
		unpack[11] = unpack11;
		unpack[12] = unpack12;
		unpack[13] = unpack13;
		unpack[14] = unpack16;
		unpack[15] = unpack20;
		unpack[16] = unpack32;


	}

	~coding()
	{
	}

	virtual int Compression(unsigned* input, unsigned* output, int size) = 0;
	virtual int Decompression(unsigned* input, unsigned* output, int size) = 0;

	//get the coding type
	virtual int get_type() = 0;

	/*******************************************
	*set the block size to the coding method. 
	* note: for some batch packing methods, such as
	* tubo-rice and pfordelta. in the implementation
	* it could only be set to 64, 128, or 256
	*******************************************/
	virtual void set_size(int size) = 0;
	

protected:

	/**********************************************************************
	* pack n elments into b-bits wide array.
	* v: input array, the number to be packed.
	* w: the output compressed array.
	* b: the bits-width per elment in the compressed array
	* n: the number of element to be packed
	* note: the function does not modified the output array pointer. you need
	* to calculate yourself outside of the function
	**********************************************************************/
	void pack(unsigned int *v, unsigned int b, unsigned int n, unsigned int *w)
	{
	  int i, bp, wp, s;

	  for (bp = 0, i = 0; i < n; i++, bp += b)
	  {
	    wp = bp>>5;
	    s = 32 - b - (bp & 31);
	    if (s >= 0)
	      w[wp] |= (v[i]<<s);
	    else
	    {
	      s = -s;
	      w[wp] |= (v[i]>>s);
	      w[wp+1] = (v[i]<<(32-s));
	    }
	  }
	}

	/**********************************************************************
	* read certain bits from input buffer
	* buf: the input buffer
	* bp:  the pointer indicate the current postion in input buffer
	* b: the number of bits to be read
	
	* return one decompressed number in the inputstream
	**********************************************************************/
	unsigned readBits(unsigned int *buf, unsigned int *bp,unsigned int b)
	{
		  unsigned int bPtr;
		  unsigned int w;
		  unsigned int v;

		  bPtr = (*bp)&31;
		  w = (32 - bPtr > b)? b : (32 - bPtr);
		  v = ((buf[(*bp)>>5]>>bPtr) & MASK[w]);
		  (*bp) += w;

		  if (b == w)  return(v);

		  v = v | ((buf[(*bp)>>5] & MASK[b-w])<<w);
		  (*bp) += (b-w);
		  return(v);
	}

	/***********************************************************************
	* pack the number into certain bits and write to the output stream
	* buf: output buffer
	* bp: the pointer indicates the current position in output buffer
	* val: the value of the number to be packed
	* bits: to pack as bits-width.
	***********************************************************************/
	
	void writeBits(unsigned int *buf, unsigned int *bp,unsigned int val, unsigned int bits)
	{
		unsigned int bPtr;
		unsigned int w;

		bPtr = (*bp)&31;
		if (bPtr == 0)
			buf[(*bp)>>5] = 0;

		w = (32 - bPtr > bits)? bits : (32 - bPtr);
		buf[(*bp)>>5] |= ((val&MASK[w])<<bPtr);
		(*bp) += w;

		if (bits-w > 0)
		{
			buf[(*bp)>>5] = (val>>w)&MASK[bits-w];             // MASK is needed
			(*bp) += (bits-w);
		}
	}

	/*************************************************/
	/* write a single bit in character-oriented form */
	/*************************************************/
	void setBit(unsigned char *buf, unsigned int *bp, unsigned int val)
	{                                     
	  unsigned int bPtr;
//	  unsigned int w;

	  bPtr = (*bp)&7;
	  if (bPtr == 0)  buf[(*bp)>>3] = 0;
	  if (val == 1)  buf[(*bp)>>3] |= (1<<bPtr);
	  (*bp)++;
	}

	
	unsigned MASK[33]; //the MASK is used for cache some intermdia value for pfordelta, and rice coding.
	pf unpack[17]; 
};

#endif /*CODING_H_*/
