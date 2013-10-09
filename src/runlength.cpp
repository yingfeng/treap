#include <iostream>
#include "Array.h"
#include <map>

using namespace std;
using namespace cds_utils;

class Runlength {
public:
	pair<Array *,Array *> content;
	uint length;
	uint array_size;
	map<uint,uint> f;

	Runlength(uint *a,uint size){
		this->length = size;

		for (uint i = 0 ; i < size;i++){
			if (this->f.find(a[i]) == f.end()) {
				this->f.insert(make_pair(a[i],1));
			}
			else {
				this->f[a[i]]++;
			}
//			cout << "setting " << a[i] << " to " << f[a[i]] << endl;
		}
		map<uint,uint>::reverse_iterator it;
		uint r_size = f.size();
		uint *r_array = new uint[r_size];
		uint *f_array = new uint[r_size];
		uint count = 0;
		for (it = f.rbegin();it != f.rend();it++) {
			uint key = it->first;
			uint freq = it->second;
			// cout << "r_array[" << count << "] = " << key << endl;
			// cout << "f_array[" << count << "] = " << freq << endl;
			r_array[count] = key;
			f_array[count] = freq;
			count++;
		}
		Array *r = new Array(r_array,count);
		Array *rf = new Array(f_array,count);

		this->content = make_pair(r,rf);
		this->array_size = count;

		delete[] r_array;
		delete[] f_array;
		f.clear();
	}
	size_t getSize() {
		return this->content.first->getSize() + this->content.second->getSize();
	}
	uint decode(uint pos) {
		uint sum = 0;
		for (uint i = 0 ; i < this->array_size ; i++) {
//			cout << "sum is " << sum << endl;
			sum += this->content.second->getField(i);
			if (sum >= pos) {
				return this->content.first->getField(i);
			}
		}
		return 0;
	}
	~Runlength()	{
		delete this->content.first;
		delete this->content.second;
	}
};
// void testRunLength() {
// 	uint t[13] = {3,3,2,2,2,2,2,2,1,1,1,1,1};
// 	Runlength *r = new Runlength(t,13);
// 	cout << "size is = " << r->getSize() << endl;
// 	cout << "naive size = " << 13*32/8 << endl;
// 	cout << "pos x = 3 is " << r->decode(3) << endl;
// }

