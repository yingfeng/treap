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

#include "pfor_coding.h"



pfor_coding::pfor_coding():FRAC(0.1)
{
	coding_type = 1;
	cnum[0] = 0;
	cnum[1] = 1;
	cnum[2] = 2;
	cnum[3] = 3;
	cnum[4] = 4;
	cnum[5] = 5;
	cnum[6] = 6;
	cnum[7] = 7;
	cnum[8] = 8;
	cnum[9] = 9;
	cnum[10] = 10;
	cnum[11] = 11;
	cnum[12] = 12;
	cnum[13] = 13;
	cnum[14] = 16;
	cnum[15] = 20;
	cnum[16] = 32;

	//set default block_size as 128
	Bs = 128;
}

int pfor_coding::get_type()
{
	return coding_type;
}


/*pfor_coding::~pfor_coding()
{
}*/

void pfor_coding::set_size(int size)
{
	/*for pfordelta we use static block size*/
	if ( size < 64)
		Bs = 32;
	else if ( size < 128 )
		Bs = 64;
	else if (size < 256)
		Bs = 128;
	else
		Bs = 256;
}


int pfor_coding::Compression(unsigned* input, unsigned* output)
{
	int flag = -1;
	unsigned* w;
	for (int k = 0; flag < 0; k++)
	{
		w = output+1;
		flag = pfor_encode(&w, input, k);
	}

	*output = flag;
	return w - output;
}


int pfor_coding::Compression(unsigned* input, unsigned* output, int s)
{
	return Compression(input, output);
}


int pfor_coding::Decompression(unsigned* input, unsigned* output, int s)
{
	return Decompression(input, output);
}


int pfor_coding::Decompression(unsigned* input, unsigned* output)
{
	unsigned* tmp = input;
	int flag = (*tmp);
	b = cnum[((flag>>12) & 15)+2];
	unpack_count = ((flag>>12) & 15)+2;
	//cout << "upack cout:"<<unpack_count<<endl;
	t = (flag>>10) & 3;
	start = flag & 1023;

	tmp++;
	tmp = pfor_decode(output, tmp, flag);
	return tmp - input;

}

int pfor_coding::pfor_encode(unsigned int **w, unsigned int *p, int num)
{
	int i, j, l, n, bb, t, s;
	unsigned int m;
	unsigned int *ii;
	int b = cnum[num+2];
	int start;

	unsigned int out[Bs];
	unsigned int ex[Bs];

	if (b == 32)
	{
		for (i = 0; i < Bs; i++)  (*w)[i] = p[i];
		*w += Bs;
		return ((num<<12) + (2<<10) + Bs);
	}

	for (m = 0, i = 0; i < Bs; i++)
		if (p[i] > m)
			m = p[i];

	if (m < 256)
	{
		bb = 8;
		t = 0;
	}

	else if (m < 65536)
	{
		bb = 16;
		t = 1;
	}
	else
	{
		bb = 32;
		t = 2;
	}

	for (start = 0, n = 0, l = -1, i = 0; i < Bs; i++)
	{
		if ((p[i] >= (1 << b)) || ((l >= 0) && (i-l == (1<<b))))
		{
			if (l < 0)
				start = i;
			else
				out[l] = i - l - 1;

			ex[n++] = p[i];
			l = i;
		}
		else
			out[i] = p[i];
	}

	if (l >= 0)
		out[l] = (1<<b)-1;
	else
		start = Bs;

	if ((double)(n) <= FRAC * (double)(Bs))
	{
		s = ((b * Bs)>>5);
		for (i = 0; i < s; i++)
			(*w)[i] = 0;
		pack(out, b, Bs, *w);
		*w += s;

		s = ((bb * n)>>5) + ((((bb * n) & 31) > 0)? 1:0);
		for (i = 0; i < s; i++)
			(*w)[i] = 0;
		pack(ex, bb, n, *w);
		*w += s;
		return ((num<<12) + (t<<10) + start);
	}
	else
		return(-1);
}


unsigned* pfor_coding::pfor_decode(unsigned int *_p, unsigned int *_w, int flag)
{
	int i, s;
	unsigned int x;
	(unpack[unpack_count])(_p, _w, Bs);
	_w += ((b * Bs)>>5);

  	switch (t)
  	{
    		case(0):
      		for (s = start, i = 0; s < Bs; i++)
      		{
        		x = _p[s]+1;
        		_p[s] = (_w[i>>2]>>(24-((i&3)<<3)))&255;
        		s += x;
      		}
      		_w += (i>>2);

		if ((i&3) > 0)
    	  		_w++;
      		break;

    		case(1):
      		for (s = start, i = 0; s < Bs; i++)
      		{
        		x = _p[s]+1;
        		_p[s] = (_w[i>>1]>>(16-((i&1)<<4)))&65535;
        		s += x;
      		}
      		_w += (i>>1);
      		if ((i&1) > 0)  _w++;
      		break;

    		case(2):
      for (s = start, i=0; s < Bs; i++)
      {
        x = _p[s]+1;
        _p[s] = _w[i];
        s += x;
      }
      _w += i;
      break;
  }
  return(_w);
}



/*void pfor_coding::pack(unsigned int *v, unsigned int b, unsigned int n, unsigned int *w)
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
}*/

