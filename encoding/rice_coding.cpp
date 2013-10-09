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

#include "rice_coding.h"
#include <iostream>

using namespace std;

#define TESTBIT(buffer, bitp) ((((buffer)[(bitp)>>5])>>((bitp)&31))&1)
rice_coding::rice_coding()
{
	coding_type = 0;

}

int rice_coding::get_type()
{
	return coding_type;
}

void rice_coding::set_size(int size)
{
	this->block_size = size;
}

int rice_coding::Compression(unsigned* input, unsigned* output, int size)
{
	int i,s;
	unsigned int bb;
	unsigned int bp;
	int m = 0;
	int l = 1000000;

	for (i = 0, bb = 0; i < size; i++)
	{
		bb += input[i];
		while(bb > l)
		{
			bb -= l;
			m++;
		}
	}

	bb = (bb / size) + (m*l/ size);

	for (b = 0; bb > 0; bb = (bb>>1)) b++;

	if (b > 0) b--;

	//cout << b << endl;
	unsigned* aux1 = output;
	*aux1 = (unsigned)b;
	aux1++;

	for (i = 0, bp = 0; i < size; i++)
	{
		rice_encode( aux1, &bp, input[i], b);
	}
	if ( (bp & 0x1f) != 0 )
		s = (1 + (bp >> 5));
	else
		s = (bp >> 5);
	s += 1;
	return s;
}

void rice_coding::rice_encode(unsigned* buf, unsigned* bp, unsigned val, unsigned bits)
{
	unsigned int w;
	//if ( bits != 0)
	writeBits(buf, bp, val & MASK[bits], bits);
	for (w = (val>>bits); w > 0; w--)
	{
		writeBits(buf, bp, 1, 1);
	}
	writeBits(buf, bp, 0, 1);
}

unsigned rice_coding::rice_decode(unsigned* buf, unsigned* bp, unsigned bits)
{
	unsigned int v;
	int w;
	//if ( bits != 0)
	v = readBits(buf, bp, bits);
	while(TESTBIT(buf,*bp))
	{
		v += (1<<bits);
		(*bp)++;
	}

	(*bp)++;
	return(v);
}

/*unsigned rice_coding::readBits(unsigned int *buf, unsigned int *bp,unsigned int bits)
{
	  unsigned int bPtr;
	  unsigned int w;
	  unsigned int v;

	  bPtr = (*bp)&31;
	  w = (32 - bPtr > bits)? bits : (32 - bPtr);
	  v = ((buf[(*bp)>>5]>>bPtr) & MASK[w]);
	  (*bp) += w;

	  if (bits == w)  return(v);

	  v = v | ((buf[(*bp)>>5] & MASK[bits-w])<<w);
	  (*bp) += (bits-w);
	  return(v);
}

void rice_coding::writeBits(unsigned int *buf, unsigned int *bp,unsigned int val, unsigned int bits)
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
}*/

int rice_coding::Decompression(unsigned* input, unsigned* output, int size)
{
	int i;
	unsigned int bp = 0;
	unsigned* tmp = input;
	this->b = *tmp;
	tmp++;
	for (i = 0, bp = 0; i < size; i++)
		output[i] = rice_decode(tmp, &bp, this->b);

	int s;
	if ( bp & 0x1f )
		s = (1 + (bp >> 5));
	else
		s = (bp >> 5);
	return s+1;
}

/*rice_coding::~rice_coding()
{

}*/
