#include "basics.h"
typedef uint handle;
#define PRIME1 ((uint)4294967279u)
#define PRIME2 ((uint)4294967197u)
/*
uint bits(uint val) {
    uint ret = 0;
    while(val!=0) {
        ret++;
        val >>= 1;
    }
	return ret;
}*/
class Hashtable
{
public:
	uint size;    // # of table entries
	uint nbits;    // bits per entry
	uint *table;    // data

	Hashtable(uint n,uint vbits,float favtor);
	// frees the structure
	~Hashtable();

	 //inserts an entry 
	void insertHash (uint key, uint elem);
  // looks for a key, returns first value (zero => no values)
  // writes in pos a handle to get next values
	uint searchHash (uint key, handle *h);
  // gets following values using handle *h, which is rewritten
  // returns next value (zero => no more values)
	uint nextHash (handle *h);
	void saveHash(FILE *f);
	Hashtable loadHash(FILE *f);

	uint sizeofHash(){
		unsigned long long aux;
		aux = (((unsigned long long)(this->size+1))*this->nbits+W-1)/W;
		cout << "size of hash = " << sizeof(this) + aux*sizeof(uint) << endl;
		return sizeof(this) + aux*sizeof(uint);
	}

};