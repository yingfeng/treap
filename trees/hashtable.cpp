#include "hashtable.h"
#include "basics.h"

Hashtable::Hashtable(uint n, uint vbits, float factor)
{
	int i,N;
	if (n == 0) return;
	N = (int)(n*factor); 
	if (N <= (int)n) 
		N =(int)(n+1);
	this->size = (1 << bits(N-1)) - 1;
	this->nbits = vbits;
	this->table = (uint *) malloc ((((this->size+1)*vbits+W-1)/W)*sizeof(uint));
#ifdef INDEXREPORT
	printf ("     Also created hash table of %i nbits\n",
			(((this->size+1)*vbits+W-1)/W)*W);
#endif
	for (i=(((this->size+1)*vbits+W-1)/W)-1;i>=0;i--) this->table[i] = 0;
}

void Hashtable::insertHash (uint key, uint value){ 
	uint pos = (key*PRIME1) & this->size;
	if (bitget_go(this->table, pos*this->nbits, this->nbits) != 0){ 
		do 
			pos = (pos + PRIME2) & this->size;
		while (bitget_go(this->table,pos*this->nbits,this->nbits) != 0);
	}
	bitput(this->table,pos*this->nbits,this->nbits,value);
}

uint Hashtable::searchHash (uint key, handle *h){ 
	*h = (key*PRIME1) & this->size;
	return bitget_go(this->table,*h*this->nbits,this->nbits);
}

uint Hashtable::nextHash (handle *h){ 
	*h = (*h +PRIME2) & this->size;
	return bitget_go(this->table,*h*this->nbits,this->nbits);
}

