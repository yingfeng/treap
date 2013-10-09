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

#ifndef _UNPACK_H_
#define _UNPACK_H_

/*************************************************************/
/* macros for fast unpacking of integers of fixed bit length */
/*************************************************************/


#define MASK(j) bit_mask[j]
// static unsigned int bit_mask[33] = {
//     0x00000000,
//     0x00000001, 0x00000003, 0x00000007, 0x0000000f, 0x0000001f, 0x0000003f,
//     0x0000007f, 0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
//     0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff, 0x0001ffff, 0x0003ffff,
//     0x0007ffff, 0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
//     0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff, 0x1fffffff, 0x3fffffff,
//     0x7fffffff, 0xffffffff
// };



#define TESTBIT(buffer, bitp) ((((buffer)[(bitp)>>5])>>((bitp)&31))&1)

void unpack0(unsigned int *p, unsigned int *w, int BS);
void unpack1(unsigned int *p, unsigned int *w, int BS);
void unpack2(unsigned int *p, unsigned int *w, int BS);
void unpack3(unsigned int *p, unsigned int *w, int BS);
void unpack4(unsigned int *p, unsigned int *w, int BS);
void unpack5(unsigned int *p, unsigned int *w, int BS);
void unpack6(unsigned int *p, unsigned int *w, int BS);
void unpack7(unsigned int *p, unsigned int *w, int BS);
void unpack8(unsigned int *p, unsigned int *w, int BS);
void unpack9(unsigned int *p, unsigned int *w, int BS);
void unpack10(unsigned int *p, unsigned int *w, int BS);
void unpack11(unsigned int *p, unsigned int *w, int BS);
void unpack12(unsigned int *p, unsigned int *w, int BS);
void unpack13(unsigned int *p, unsigned int *w, int BS);
void unpack16(unsigned int *p, unsigned int *w, int BS);
void unpack20(unsigned int *p, unsigned int *w, int BS);
void unpack32(unsigned int *p, unsigned int *w, int BS);

typedef void (*pf)(unsigned int *p, unsigned int *w, int BS);


#endif
