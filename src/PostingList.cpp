#ifndef _POSTINGLIST_H_
#define _POSTINGLIST_H_
#include <vector>
#include "../encoding/coding_factory.h"
#include "factorization.h"
#include <WaveletTreeNoptrs.h>

using namespace std;
using namespace cds_static;

class PostingList {
public:
	uint *doc_id;
	uint *freq;
	uint *doc_compressed;
	uint n;
	size_t total_size;
	size_t total_sizef1;
	size_t total_size_wt;
	uint block_size;
	uint coder;
	factorization *freqs;


	PostingList(vector<uint> &v,vector< pair<uint,uint> > &t,uint coder) {
		// cout << v.size() << endl;
		block_size = 128;
		this->n = v.size();
		this->total_size = 0;
		this->total_sizef1 = 0;

		coding_factory  cf;
		coding *c = cf.get_coder(coder);
		this->coder = coder;
		
		
		uint f1 = 0;
		for (uint i = 0 ; i < n;i++) {
			if (t[i].second < 3) {
				f1++;
			}
		}
		freq = new uint[n-f1];
		
		uint count = 0;
		uint count2 = 0;
		uint *doc_id = new uint[n-f1];
		uint *freq1docs;
		if (f1 != 0) {
			freq1docs = new uint[f1];
		}
		for (uint i = 0 ; i < n;i++) {
			if (t[i].second < 3) {
				freq1docs[count] = t[i].first;
				count++;
			} else {
				freq[count2] = t[i].second;
				doc_id[count2] = v[i];
				count2++;
			}
		}
		this->freqs = new factorization(freq,count2);

	 //    Array *A = new Array(freq,count2);
	 //    BitSequenceBuilder * bsb = new BitSequenceBuilderRG(20);
  //       MapperNone * map = new MapperNone();
		// WaveletTreeNoptrs * wt = new WaveletTreeNoptrs(*A,bsb,map);
		// this->total_size_wt = wt->getSize();

		uint *doc_compressed = new uint[100+3*count2];
		
		uint offset = 0;
		uint size = 0;
		
		if (count2 != 0) { 
			for (uint i = 1 ; i < count2;i++) {
				doc_id[i] = doc_id[i] - doc_id[i-1];			
			}
			for (uint i = 0 ; i < 100 + 3*count2;i++){
				doc_compressed[i] = 0;
			}
			// cout << "f1 = " << f1 << endl;
			// cout << "count1 = " << count << endl;
			// cout << "count2= " << count2 << endl;
			for (uint i = 0 ; i < count2;i+=block_size) {
				if (n-f1 < block_size) {
					size += c->Compression(doc_id,doc_compressed,count2);
					this->total_size += size;
				} else if (i+block_size > n-f1) {
					offset = c->Compression(doc_id,doc_compressed,count2-i);
					size += offset;
					doc_id += count2-i;
					doc_compressed += offset;
					this->total_size += offset;
				} else {
					offset = c->Compression(doc_id,doc_compressed,block_size);
					size += offset;
					doc_id += block_size;
					doc_compressed += offset;
					this->total_size += offset;
				}
				if (this->total_size > 100 + 3*count2 ) {
					cout << "FUCK!" << endl;
					cout << "i = " << i << endl;
					cout << "total size = " << total_size << endl;
					cout << "offset = " << offset << endl;
				}
			}
		}
		size = 0;
		offset =0;
		if (f1 != 0) { 
			uint *doc_compressedf1 = new uint[100+3*f1];
			for (uint i = 1 ; i < f1;i++) {
				freq1docs[i] = freq1docs[i] - freq1docs[i-1];			
			}
			for (uint i = 0 ; i < 100+3*f1;i++) {
				doc_compressedf1[i] = 0;	
			}
			for (uint i = 0 ; i < f1;i+=block_size) {
				if (f1 < block_size) {
					size += c->Compression(freq1docs,doc_compressedf1,f1);
					this->total_sizef1 += size;
				} else if (i+block_size > f1) {
					offset = c->Compression(freq1docs,doc_compressedf1,f1-i);
					size += offset;
					freq1docs += f1-i;
					doc_compressedf1 += offset;
					this->total_sizef1 += offset;
				} else {
					offset = c->Compression(freq1docs,doc_compressedf1,block_size);
					size += offset;
					freq1docs += block_size;
					doc_compressedf1 += offset;
					this->total_sizef1 += offset;
				}
				if (this->total_sizef1 > 100 + 3*f1 ) {
					cout << "FUCK!" << endl;
					cout << "i = " << i << endl;
					cout << "total size f1 = " << total_sizef1 << endl;
					cout << "offset = " << offset << endl;
				}
			}
		}	
	}
	size_t getDocSize() {
//		cout << this->total_size << endl;
		return this->total_size + this->total_sizef1;
	}
	size_t getFreqSize() {
		return this->freqs->getSize();
	}
	// size_t getWtSize() {
	// 	return this->total_size_wt;
	// }
	uint getN() {
		return this->n;
	}
	uint getValue(uint pos) {
		uint offset = 0;
		unsigned int *c = new unsigned int[n];
		coding_factory  cf;
		coding *code = cf.get_coder(this->coder);
		for ( uint i = 0; i < n; i+= block_size){
			if (n < block_size) { 
				offset = code->Decompression(doc_compressed, c, n);
			} else {
				offset = code->Decompression(doc_compressed, c, block_size);
				doc_compressed += offset;
				c += block_size;
			}
			if ( i > pos ) {
				break;
			} 
		}
		uint result = 0;
		for (uint i = 0 ; i < pos ;i++) {
			result += c[i];
		}
		return result;
	}
};

#endif