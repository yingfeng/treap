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

#include "vbyte_coding.h"

vbyte_coding::vbyte_coding()
{
	type = 2;
}



void vbyte_coding::set_size(int s)
{
	return;
}

int vbyte_coding::get_type()
{
	return type;
}

void vbyte_coding::vbencoding(unsigned int _n)
{
	unsigned _num;
	unsigned char _barray[5];
	unsigned _i, _started = 0;
	_num = _n;
	for (_i = 0; _i < 5; _i++)
	{
		_barray[_i]=((_num&0x07F)<<1);
	    _num=_num>>7 & 0x0FFFFFFF;
	}
	for (_i = 4; _i > 0; _i--)
	{
		if ((_barray[_i] != 0) || (_started == 1))
		{
			_started = 1;
			*tmp = _barray[_i]|0x1;
			tmp++;
		}
	}

	*tmp = _barray[0] | 0x0;
	tmp++;
}

unsigned vbyte_coding::vbdecoding(unsigned char* _v)
{
	unsigned _n = ((*tmp>>1));
	if ((*tmp&0x1) != 0)
	{
		tmp++;
		_n = (_n << 7) | ((*tmp >> 1));
		if ((*tmp&0x1)!= 0)
		{
			tmp++;
			_n=(_n<<7)|(((*tmp)>>1));
		    if ((*tmp&0x1) != 0)
		    {
		    	tmp++;
		    	_n = (_n<<7) | ((*tmp>>1));
		    }
		 }
	}
	tmp++;
	return _n;
}

int vbyte_coding::Compression(unsigned* input, unsigned* output, int size)
{
	int i;
	tmp = (unsigned char*)output;
	for ( i = 0; i < size; i++)
	{
		vbencoding(input[i]);
	}
	//cout << ((unsigned int*)tmp) - ((unsigned*)output)<<endl;
	return (((unsigned int*)tmp) - ((unsigned*)output))+1;
}

int vbyte_coding::Decompression(unsigned* input, unsigned* output, int size)
{
	int i;
	tmp = (unsigned char*)input;
	for ( i = 0; i < size; i++)
	{
		output[i] = vbdecoding(tmp);
		//cout << output[i] << endl;
	}

	return (((unsigned int*)tmp) - ((unsigned int*)input))+1;
}


