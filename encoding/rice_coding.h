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

#ifndef RICE_CODING_H_
#define RICE_CODING_H_
#include "coding.h"

class rice_coding: public coding
{
public:
	rice_coding();
	virtual int Compression(unsigned*, unsigned*, int);
	virtual int Decompression(unsigned*, unsigned*, int);
	//virtual ~rice_coding();
	virtual int get_type();
	virtual void set_size(int size);
private:
	int b;
	int coding_type;
	//void writeBits(unsigned int *buf, unsigned int *bp,unsigned int val, unsigned int bits);
	//unsigned int readBits(unsigned int *buf, unsigned int *bp,unsigned int bits);
	//void setBit(unsigned char *buf, unsigned int *bp, unsigned int val);
	void rice_encode(unsigned int *buf, unsigned int *bp,unsigned int val, unsigned int bits);
	unsigned rice_decode(unsigned int *buf, unsigned int *bp,unsigned int bits);

	int block_size;

};

#endif /*RICE_CODING_H_*/
