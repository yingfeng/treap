/* Copyright (C) 2010, Rodrigo Cánovas, all rights reserved.
 *
 *This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

// Closed hash table
#include "hash.h"

  // creates a table to store up to n values with guaranteed load factor.
  // vbits = # of bits per entry, ENTRIES CANNOT HAVE ZERO VALUE

hash createHash (uint n, uint vbits, float factor){ 
	hash H = (hash)malloc (sizeof(struct shash));
	int i,N;
	if (n == 0) return NULL;
	N = (int)(n*factor); 
	if (N <= (int)n) 
		N =(int)(n+1);
	H->size = (1 << bits(N-1)) - 1;
	H->bits = vbits;
	H->table = (uint *) malloc ((((H->size+1)*vbits+W-1)/W)*sizeof(uint));
#ifdef INDEXREPORT
	printf ("     Also created hash table of %i bits\n",
			(((H->size+1)*vbits+W-1)/W)*W);
#endif
	for (i=(((H->size+1)*vbits+W-1)/W)-1;i>=0;i--) H->table[i] = 0;
	return H;
}

// frees the structure
void destroyHash (hash H){ 
	if (H == NULL) 
		return;
	free (H->table);
	free (H);
}

void saveHash(FILE *f, hash H){
	uint indicator;
	if(H){
		indicator = 1;
		fwrite(&indicator, sizeof(uint), 1, f);
		fwrite(&H->size, sizeof(uint), 1, f);
		fwrite(&H->bits, sizeof(uint), 1, f);
		fwrite(H->table, sizeof(uint), ((H->size+1)*H->bits+W-1)/W, f);
	}
	else {
		indicator = 0;
		fwrite(&indicator, sizeof(uint), 1, f);
	}
}   
   
hash loadHash(FILE *f){
	hash H;
	uint indicator;
	fread(&indicator, sizeof(uint), 1, f);
	if(indicator){
		H = (hash)malloc(sizeof(struct shash));
		fread(&H->size, sizeof(uint), 1, f);
		fread(&H->bits, sizeof(uint), 1, f);
		H->table = (uint *)malloc((((H->size+1)*H->bits+W-1)/W)*sizeof(uint));
		fread(H->table, sizeof(uint), ((H->size+1)*H->bits+W-1)/W, f);      
	}
	else
		H = NULL;
	return H;
}
   
// inserts an entry, not prepared for overflow
void insertHash (hash H, uint key, uint value){ 
	uint pos = (key*PRIME1) & H->size;
	if (bitget_go(H->table, pos*H->bits, H->bits) != 0){ 
		do 
			pos = (pos + PRIME2) & H->size;
		while (bitget_go(H->table,pos*H->bits,H->bits) != 0);
	}
	bitput(H->table,pos*H->bits,H->bits,value);
}

// looks for a key, returns first value (zero => no values)
// writes in pos a handle to get next values
uint searchHash (hash H, uint key, handle *h){ 
	*h = (key*PRIME1) & H->size;
	return bitget_go(H->table,*h*H->bits,H->bits);
}

// gets following values using handle *pos, which is rewritten
// returns next value (zero => no more values)
uint nextHash (hash H, handle *h){ 
	*h = (*h +PRIME2) & H->size;
	return bitget_go(H->table,*h*H->bits,H->bits);
}

uint sizeofHash(hash H){
	if(H){
		unsigned long long aux;
		aux = (((unsigned long long)(H->size+1))*H->bits+W-1)/W;
		return sizeof(struct shash) + aux*sizeof(uint);
	}
	else
		return 0;
}

