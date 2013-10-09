#ifndef _INVERTEDINDEX_H_
#define _INVERTEDINDEX_H_
#include <algorithm>

#include "BitString.h"
#include "Array.h"
#include "factorization.h"
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include "utils.cpp"
#include "Treap.h"
#include "../encoding/pfor_coding.h"
#include "../encoding/coding_factory.h"
#include <map>
#include <cmath>
#include "runlength.cpp"
#include "PostingList.cpp"





using namespace cds_utils;
using namespace cds_static;
using namespace std;

bool reversesort (int i,int j) { return (i>j); }

class InvertedIndex {
	public:
		map<string,vector<pair<uint,uint> > > inverted_list;
		map<string,Treap*> inverted_treap;
		map<string,tree_ff*> inverted_ff;
		map<string,vector<pair<uint,uint> > >  lower_list;
		map<string,PostingList*> lower_index;
		
		factorization *doc_ids;
		factorization *freqs;
	    coding* docs_pfor;

		vector<uint> doc_ids_plain;
		vector<uint> freqs_plain;

		ifstream invlist;
		ifstream invfreq;
		ifstream words;
		int num_docs;
		int num_postings;
		uint num_terms;	
		uint max_doc_id;
		double factor;
		InvertedIndex(string inv_list,string inv_list_freq, string w, int num_docs,uint max_doc_id,double factor) {
			if (max_doc_id == 0) {
				this->max_doc_id = 1<<30;
				max_doc_id = 1<<30;
			}
			this->factor = factor;
			this->invlist.open(inv_list.c_str());
			this->invfreq.open(inv_list_freq.c_str());
			this->words.open(w.c_str());
			this->num_docs = num_docs;
			this->num_postings = 0;
			this->max_doc_id = max_doc_id;
			string line1;
			string line2;
			vector<uint *> dictionary;
			vector<string> terms;
			string sfilter = " \n\0";
			cout << "Reading Words..." << endl;

			while(getline(words,line1))
			{
				vector<string> postings_words;
				Tokenize(line1,postings_words,sfilter);
				for (uint i = 0 ; i < postings_words.size();i++) {
					terms.push_back(postings_words[i]);
				}
			}
			
			cout << "done!" << endl;
			cout << "Amount of Terms:" << terms.size() <<  endl;
			this->num_terms = terms.size();
			cout << "Reading invlist " << endl;
			uint aux = 0;
			line1 = "";
			while(getline(invlist,line1)) {

			//	uint *frequencies = new uint[num_docs];
			//	for (int i = 0 ; i < num_docs;i++)	{
			//		frequencies[i] = 0;
			//	}	
				getline(invfreq,line2);
				vector<string> postings,postings2;
				Tokenize(line1,postings,sfilter);
				Tokenize(line2,postings2,sfilter);
				for (uint i = 1 ;i < postings.size();i++) {
						if (atoi(postings2[i].c_str()) < 1) {
							continue;						
						}
			//			frequencies[atoi(postings[i].c_str())] = atoi(postings2[i].c_str());
					if (atoi(postings2[i].c_str()) != 0 ) {
						pair<uint,uint> p;
						// if (atoi(postings2[i].c_str()) > 10)
						//  	p = make_pair(atoi(postings[i].c_str()),10);
						// else
						if (atoi(postings[i].c_str()) <= this->max_doc_id) {
							p = make_pair(atoi(postings[i].c_str()),atoi(postings2[i].c_str()));
							inverted_list[terms[aux]].push_back(p);
							num_postings++;
						}
					}
				}
				aux++;
				if (aux == terms.size()) break;
			//	dictionary.push_back(frequencies);
			}
			// cout << "done!" << endl;
			cout << "Total number of postings: " << num_postings << endl;
			cout << "Total number of terms: " << inverted_list.size() << endl;
			// cout << "Naive Space: " << num_postings*64 << endl;

		}

	~InvertedIndex() {
		delete doc_ids;
		delete freqs;
		inverted_list.clear();
		inverted_treap.clear();
		inverted_ff.clear();
		lower_list.clear();
		lower_index.clear();
	}
	vector<size_t> estimateDocSorted() {
		// cout << "Starting doc_sorted estimation" << endl;
		size_t total_size = 0;
		coding_factory cf;
		vector<size_t> result;
		map<string,vector<pair<uint,uint> > >::iterator it;
		for (uint cod = 0 ; cod < 1 ; cod++) {
			coding* coder = cf.get_coder(cod);
			cout << "term_size = " << this->num_terms << endl;
			// cout << "inverted terms = " << this->inverted_list.size() << endl;
			for (it = this->inverted_list.begin();it != this->inverted_list.end();++it){
				
				string key = it->first;
		//		cout << "key = " << key << endl;
				vector<pair<uint,uint> > content = it->second;
				if (content.size() < 1) continue;

				uint *docs = new uint[content.size()];
			  	uint *docs2 = docs;
	
				vector<uint> doc_id;
				for (uint i = 0 ; i < content.size();i++ ) {
					pair<uint,uint> element = content[i];
					doc_id.push_back(element.first);
				}
				sort(doc_id.begin(),doc_id.end());
				uint *compressed = new uint[100+3*doc_id.size()];
				uint *compressed2 = compressed;
				docs[0] = doc_id[0];
				for (uint i = 1 ; i < doc_id.size();i++) {
					docs[i] = doc_id[i] - doc_id[i-1];
				}
				assert(doc_id.size() > 0);
				for (uint i = 0 ; i < 100 + 3*doc_id.size();i++){
					compressed[i] = 0;
				}
				uint block_size = 128;
				uint offset = 0;
				uint size = 0;
				for (uint i = 0 ; i < doc_id.size();i+=block_size) {
			//		cout << "block_size = " << i << " doc size = " << doc_id.size() << endl;
					if (doc_id.size() < block_size) {
						size += coder->Compression(docs,compressed,doc_id.size());
						total_size += size;
					} else if (i+block_size > doc_id.size()) {
//						cout << "block = " << doc_id.size()-i << endl;
						offset = coder->Compression(docs,compressed,doc_id.size()-i);
						size += offset;
						docs += doc_id.size()-i;
						compressed += offset;
						total_size += offset;
					} else {
						offset = coder->Compression(docs,compressed,block_size);
						size += offset;
						docs += block_size;
						compressed += offset;
						total_size += offset;
					}
				}
//				cout << "size = " << size << endl; 
				assert(size <= 100+3*doc_id.size());
				delete[] compressed2;
				delete[] docs2;
			
			}
			result.push_back(total_size);
			total_size = 0;
		}
		return result;
	}

	size_t estimateFreqSorted() {
		// cout << "Executing freq sorted estimation...." << endl;
		typedef map<string,vector<pair<uint,uint> > > myMap;
		myMap::iterator it;
		size_t total_size = 0;
		for (it = this->inverted_list.begin();it != this->inverted_list.end();++it){
			string key = it->first;
			vector<pair<uint,uint> > content = it->second;
			uint *freqs = new uint[content.size()];
			vector<uint> freq;
			for (uint i = 0 ; i < content.size();i++ ) {
				pair<uint,uint> element = content[i];
				freq.push_back(element.first);
			}
			sort(freq.begin(),freq.end(),reversesort);
			freqs[0] = freq[0];
			for (uint i = 1 ; i < freq.size();i++) {
				//cout << "i = " << i << "->" << freq[i] << endl;
				freqs[i] = freq[i-1] - freq[i];
	
				//cout << "delta i = " <<  i << "->" <<  freqs[i] << endl;
			}
			Runlength *rl = new Runlength(freqs,freq.size());
			total_size += rl->getSize();
			delete rl;
			delete []freqs;
		}
		return total_size;
	}

	int getThreshold(string key,double factor) {
		vector<pair<uint,uint> > v = this->inverted_list[key];
		uint sum = 0;
		uint v_size = v.size();
		for (uint i = 0 ; i < v_size; i++ ) {
			pair<uint,uint> element = v[i];
			sum += element.second;
		}
		double avg = (double)sum*1.0/(double)v_size*1.0;
		double stdev = 0;
		for (uint i = 0;i<v_size;i++) {
			pair<uint,uint> element = v[i];
			stdev += (element.second - avg)*(element.second - avg);
		}
		//cout << avg << endl;
		stdev = stdev/v_size;
		stdev = sqrt(stdev);
		// if (v_size > 10) { 
		// 	cout << "sum =" << sum << endl;
		// 	cout << "stdev = " << stdev << endl;
		// 	cout << "avg = " << avg << endl;
		// 	cout << "factor = " << avg-factor*stdev << endl;
		// 	cout << " n = " << v_size << endl;
		// }
		int result = (avg-(factor*stdev));
		if (result < 1) {
			return 1;
		} else {
			return result;
		}
	}

	void  buildTreap() {
		// cout << "Building Treap..." << endl;
		typedef map<string,vector<pair<uint,uint> > > myMap;
		myMap::const_iterator it;
		double t_height = 0.0;
		double tr_height = 0.0;
		uint count = 0;
		uint count_treap = 0;
		uint threshold = 1;
		bool use_treap = false;
		for (it = this->inverted_list.begin();it != this->inverted_list.end();++it){
			string key = it->first;
			this->inverted_treap[key] = new Treap();
			vector<pair<uint,uint> > content = it->second;
			threshold = getThreshold(key,this->factor);
			vector<uint> lower_keys;
			for (uint i = 0 ; i < content.size();i++) {		
				pair<uint,uint> element = content[i];
				if (element.second <= threshold) {
					this->lower_list[key].push_back(make_pair(element.first,element.second));
					lower_keys.push_back(element.first);
				} else {
					use_treap = true;
					this->inverted_treap[key]->insert(element.first,element.second);
				}

			}
			if (use_treap) {
				count_treap++;
				use_treap = false;
			}
			count++;

			if (this->lower_list[key].size() != 0) {
				sort(lower_keys.begin(),lower_keys.end());
				this->lower_index[key] = new PostingList(lower_keys,this->lower_list[key],0);
			}
			TreapNode *root = this->inverted_treap[key]->getRoot();
			this->inverted_treap[key]->destify(root);
			t_height += this->inverted_treap[key]->maxDepthIterative(root);
			tr_height += log2(content.size())+1;
		}
		cout << "Treap Height = " << t_height/count_treap*1.00 << endl;
		cout << "Normal Height = " << tr_height/count*1.00 << endl;
		// cout << "Done!" << endl;
		this->buildFF();
	}

	BitString * generateTreeBitmap(TreapNode *r,uint size) {
		if (!r) return 0;
		BitString *result = new BitString(size);
		vector<pair<int,TreapNode*> > s;
		uint bitmap_pos = 0;
		s.push_back(make_pair(1,r));
		while (!s.empty()) {
			pair<int,TreapNode*> curr = s.back();
			s.pop_back();
			if (curr.first == 1)  {
	        	// s.push_back(make_pair(0,curr.second));
				bitmap_pos++;
	    		if (curr.second->left != NULL) {
	        		s.push_back(make_pair(1,curr.second->left));
	        		// cout << "Adding left" << endl;
	        	} else {
		        	result->setBit(bitmap_pos);
		        	bitmap_pos++;
	        	}
	      		if (curr.second->right != NULL) {
	        		s.push_back(make_pair(1,curr.second->right));
	        		// cout << "Adding right" << endl;
	        	} else {
	        		result->setBit(bitmap_pos);
		        	bitmap_pos++;
	        	}
	        } else if (curr.first == 0) {
	        	result->setBit(bitmap_pos);
	        	bitmap_pos++;
	        	//s.pop_back();
	        }
		}
	//	cout << "bitmap_pos =" << bitmap_pos << endl;
		return result;
	}
	void buildFF() {
		// cout << "building ff" << endl;
		typedef map<string,vector<pair<uint,uint> > > myMap;
		myMap::const_iterator it;
		//int options = OPT_FAST_LCA | OPT_FAST_PREORDER_SELECT | OPT_FAST_LEAF_SELECT;
		int options = 0;
		size_t acum_size = 0;
		//size_t lower_size = 0;
		int count = 0;
		vector<uint> lower_vector;
		for (it = this->inverted_list.begin();it != this->inverted_list.end();++it){
			string key = it->first;
			
			TreapNode *root = this->inverted_treap[key]->getRoot();
			uint tree_size = this->inverted_treap[key]->size;
			vector<pair<uint,uint> > content = it->second;	
			vector<pair<uint,uint> > v;
			this->inverted_treap[key]->preOrderDest(v,root);
			if (count % 100000 == 0) 
				cout << (count*1.0/this->num_terms*1.0)*100.00 << endl;
 			for (uint i = 0 ; i < v.size();i++) {
				this->doc_ids_plain.push_back(v[i].first);
				this->freqs_plain.push_back(v[i].second);
			}

			count ++;
			if (root != NULL && tree_size > 0) {
				BitString *bs = generateTreeBitmap(root,2*tree_size);
				this->inverted_ff[key] = new tree_ff(bs->getData(),tree_size*2,options);
				acum_size += this->inverted_ff[key]->size();
			}
		}
		uint *freqs_array = new uint[freqs_plain.size()];
		uint *docs_array = new uint[doc_ids_plain.size()];
			
		for (uint i = 0 ; i < freqs_plain.size();i++) {
			freqs_array[i] = freqs_plain[i];
			docs_array[i] = doc_ids_plain[i];
		}

		typedef map<string,PostingList *>  map_lower;
		map_lower::const_iterator it_lower;
		size_t lower_size_docs = 0;
		size_t lower_size_freq = 0;
		size_t lower_size = 0;
//		size_t lower_size_wt = 0;
		for (it_lower = this->lower_index.begin();it_lower != this->lower_index.end();++it_lower){
			string key = it_lower->first;
			lower_size_docs += this->lower_index[key]->getDocSize();
			lower_size_freq += this->lower_index[key]->getFreqSize();
			lower_size += this->lower_index[key]->getN();
		//	lower_size_wt += this->lower_index[key]->getWtSize();
		}


   	    cout << "Constructing Doc Array " << endl;
		this->doc_ids = new factorization(docs_array,freqs_plain.size());
		cout << "Constructing Freq Array" << endl;
		this->freqs = new factorization(freqs_array,freqs_plain.size());
		//factorization *lower_test = new factorization(lower_array,lower_size);
		// Array *freq_test = new Array(freqs_array,freqs_plain.size());
		// Array *doc_test = new Array(docs_array,freqs_plain.size());
		cout << "------------------------------" << endl;
		cout << "FACTOR = " << this->factor << endl;
		cout << "DOC_ID = " << this->max_doc_id << endl;
		cout << "Postings in Lists:" << lower_size << endl;
		cout << "Postings in Treap:" << freqs_plain.size() << endl;
		cout << "Ratio of postings:" << (double)freqs_plain.size()*1.0/(double)lower_size*1.0 << endl;
		cout << "------------------------" << endl;
		cout << "docs (tree) size = " << this->doc_ids->getSize() << endl;
		cout << "freqs (tree) size = " << this->freqs->getSize() << endl;
		cout << "FF size = " << acum_size << endl;
		cout << "lower_size docs = " << lower_size_docs << endl;
		cout << "lower_size freq = " << lower_size_freq << endl;
		//cout << "lower_size freq(wt) = " << lower_size_wt << endl;
		size_t total = acum_size + this->doc_ids->getSize() + this->freqs->getSize() + lower_size_docs + lower_size_freq;
		cout << "total = " << total << endl;
		cout << "total (MB) = " << total/(1024.00*1024.00) << endl;
	}

	vector<pair<uint,uint> > Intersect(string &t1, string &t2) { 

	}
};
#endif

//97630735
// 0.79
// 0.62
