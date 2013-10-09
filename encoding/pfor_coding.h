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
// This is the implementation of pfordelta, it's fast, but compression efficiency (bytes/docid) is .
// not as good as rice coding. you could hack it to make it more efficient :-)
// recall that pfordelta is a blockwise coding, you need to first set the block size to either 32, 64
// 128, 256... currently, according to the efficiency, you could only set to these numbers. the default
// value is 128

#ifndef PFOR_CODING_H_
#define PFOR_CODING_H_

#include "coding.h"

#include <iostream>

using namespace std;

#ifndef TEST
#define TEST 0
#endif

class pfor_coding : public coding
{
public:
	pfor_coding();

	virtual int Compression(unsigned*, unsigned*, int);
	int Compression(unsigned* in, unsigned* );
	int Decompression(unsigned* in, unsigned*);
	virtual int Decompression(unsigned*, unsigned*, int);
	//virtual ~pfor_coding();
	virtual int get_type();
	virtual void set_size(int size);
private:
	int pfor_encode(unsigned int **w, unsigned int *p, int num);
	unsigned* pfor_decode(unsigned int *_p, unsigned int *_w, int flag);
	//void pack(unsigned int *v, unsigned int b, unsigned int n, unsigned int *w);
	int Bs;
	int coding_type;

	//is the percentage of exceptions in the array. default value is 0.1
	float FRAC;
	int cnum[17];

	/***************************************
	 * meta data in pfordelta
	 **************************************/
	int b;
	int start;
	int t;
	int unpack_count;
};


#endif //PFOR_CODING_H_
