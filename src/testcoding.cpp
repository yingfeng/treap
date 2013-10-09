#include "../encoding/coding_factory.h"
#include <iostream>
using namespace std;

int main() {
	int block_size = 128*128;
	int n = 128;

	unsigned int *a = new unsigned int[n];
	unsigned int *a2 = a;
	unsigned int *d = new unsigned int[n];

	int i;
	unsigned int offset;
	for (i = 0 ; i < n ; i++) {
		a[i] = i+1;
		d[i] = i+1;
	}
	bool flag = 0;
	for (int j = 0 ; j < 5 ;j++) {
		coding_factory cf;
		coding* coder = cf.get_coder(j);
		cout << "testing coder = " << j << endl;
		unsigned int *b = new unsigned int[n];
		unsigned int *b2 = b;
		
		for ( i = 0; i < n; i+= block_size ){
			if (n < block_size) { 
				offset = coder->Compression(a, b, n);
			} else {
				offset = coder->Compression(a, b, block_size);
				a += block_size;
				b += offset;
			}
		}

		b = b2;
		
		unsigned int *c = new unsigned int[n];
		unsigned int *c2 = c;
		for ( i = 0; i < n; i+= block_size){
			if (n < block_size) { 
				offset = coder->Decompression(b, c, n);
			} else {
				offset = coder->Decompression(b, c, block_size);
				b += offset;
				c += block_size;
			}
		}
		c = c2;

		for ( i = 0; i < n; i++) {
			cout << "d[" << i << "] =" << d[i] <<" , c[ " << i << "] =" << c[i] << endl;
			if ( d[i] != c[i])
				flag = 1;
		}

		if ( flag == 1)
			cout << "the result is not right!" << endl;
		else
			cout << "Yeah,!"<<endl;
		a = a2;
	}
}