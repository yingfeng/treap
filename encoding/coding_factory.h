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
// This is the coding factory, you could assign coding method yourself:
// eg: coding_factory cf;
//     coding* coder = cf.get_coder(RICE);
//	coder->compression(input, output, size);

#ifndef _CODING_FACTORY_H_
#define _CODING_FACTORY_H_
#include "coding.h"
#include "rice_coding.h"
#include "pfor_coding.h"
#include "vbyte_coding.h"
#include "s9_coding.h"
#include "s16_coding.h"
#include "rice_coding2.h"

#include <iostream>

using namespace std;


#define RICE 0
#define PFOR 1
#define VBYT 2
#define TRICE 3
#define S9 4
#define S16 5
#define ORACLE 6

class coding_factory
{
public:
	coding_factory();
	coding* get_coder(int type);
	~coding_factory();
};



#endif
