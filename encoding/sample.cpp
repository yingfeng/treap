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

#include "coding_factory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MAXDOC 26000000

int get_data(unsigned int* doc,char* word, int *np)
{
	FILE* fdd;
	char file[1024];
	int nread = 0;
	int ndoc = 0;
	int gabarge = 0;
	int i,l,k;
	int freq;
	sprintf(file,"/data/k2/suel/compression/%s", word);
	fdd = fopen(file,"r");
	if(fdd==NULL)
	{
		perror(file);
		return (-1);
	}
	nread = fread(&gabarge, sizeof(int), 1, fdd);
	(*np) = gabarge;

	while(nread > 0)
	{
		nread = fread(&(doc[ndoc]),sizeof(int),1,fdd);
		if (nread <= 0)  break;
      		fread(&freq, sizeof(unsigned), 1, fdd);
		ndoc++;
	}
	fclose(fdd);
	return ndoc;
}



int prepare(int type)
{
	char fns[256];
	char word[64];
	int np = 0;
	int ndoc;
	unsigned* doc = new unsigned[MAXDOC];
	
	unsigned* docr = new unsigned[MAXDOC];
	
	unsigned* redoc = new unsigned[MAXDOC];

	//input pointer
	unsigned* doc_ptr;

	//output pointer
	unsigned* docr_ptr;

	unsigned 
	int id, occ;
	FILE* fdw = fopen("qwords.trec", "r");
	
	coding_factory cf;
	coding* coder = cf.get_coder(type);
       

	int block_size = 128;
	int offset = 0;
	int i;
	int flag = 0;

	while (!feof(fdw))
	{
		flag  = 0;
		memset(word, 0, 64);
		fscanf(fdw, "%s\t%u\t%u\n", word, &id, &occ);
		ndoc = get_data(doc, word, &np);

		doc_ptr = doc;
		docr_ptr = docr;

		if ( ndoc > 0)
		{
			for ( i = 0; i < ndoc; i+= block_size ){
				offset = coder->Compression(doc_ptr, docr_ptr, block_size);
				doc_ptr += block_size;
				docr_ptr += offset;
			}

			doc_ptr = redoc;
			docr_ptr = docr;

			for ( i = 0; i < ndoc; i+= block_size){
				offset = coder->Decompression(docr_ptr, doc_ptr, block_size);
				docr_ptr += offset;
				doc_ptr += block_size;
			}

			for ( i = 0; i < ndoc; i++)
			{
				if ( doc[i] != redoc[i])
					flag = 1;
			}

			if ( flag == 1)
				cout << "the result is not right!" << endl;
			else
				cout << "Yeah,!"<<endl;
		}
	}

	delete[] doc;
	delete[] docr;
	delete[] redoc;

}

int main()
{
	prepare(1);
}
