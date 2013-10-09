/**
 *    intersection.cpp
 *    Copyright (C) 2011  Francisco Claude F.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include<iostream>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<vector>
#include<map>
#include<list>
#include<queue>
#include<stack>
#include<cassert>
#include<cmath>
#include<set>

#include<Sequence.h>
#include<Array.h>
#include<BitSequence.h>
#include<timing.h>

using namespace std;
using namespace cds_static;
using namespace cds_utils;

uint getDoc(uint word, uint occ, Sequence *S, vector<uint> & offsets) {
  uint pos = S->select(word, occ);
  if(pos>=S->getLength()) return (uint)-1;
  //if(pos==(uint)-1) return (uint)-1;
  vector<uint>::iterator ptr = upper_bound(offsets.begin(), offsets.end(), pos);
  return (uint)(ptr-offsets.begin())-1;
}

uint nextDoc(uint word, Sequence *S, vector<uint> & offsets, uint startDoc) {
  //assert(startDoc+1!=0);
  startDoc++;
  uint cnt = startDoc==0? 1 : (S->rank(word,offsets[startDoc]-1)+1);
  if(cnt > S->rank(word,S->getLength()-1)) return (uint)-1;
  //if(cnt==0) return (uint)-1;
  //assert(getDoc(word, cnt, S, offsets)!=(uint)-1);
  return getDoc(word, cnt, S, offsets);
}

bool containsDoc(uint word, Sequence *S, vector<uint> & offsets, uint doc) {
  //assert(doc+1!=0);
  return S->rank(word,offsets[doc+1]-1) - (doc>0?S->rank(word,offsets[doc]-1):0) > 0;
}

class WordsLenCmp {
 public:
  Sequence * Seq;
  WordsLenCmp(Sequence *S) {
    Seq = S;
  }
  bool operator()(const uint & w1, const uint & w2) {
    return Seq->rank(w1,Seq->getLength()-1) < Seq->rank(w2,Seq->getLength()-1);
  }
};

void intersect(vector<uint> & list, Sequence * S, vector<uint> & offsets, vector<uint> & result) {
  //vector<uint> result;
  uint ls = list.size();
  WordsLenCmp cmp(S);
  sort(list.begin(),list.end(),cmp);
  uint elim = getDoc(list[0],1,S,offsets);

  while(true) {
    uint k=1;
    for(;k<ls;k++) {
      if(!containsDoc(list[k],S,offsets,elim))
	break;
    }
    if(k==ls) result.push_back(elim);
    elim = nextDoc(list[0],S,offsets,elim);
    //cout << "elim=" << elim << endl;
    if(elim==(uint)-1) break;
  }
    
  //return result;  
}

void intersect2(vector<uint> & list, Sequence * S, vector<uint> & offsets, vector<uint> & result) {
  //vector<uint> result;
  uint ls = list.size();
  WordsLenCmp cmp(S);
  sort(list.begin(),list.end(),cmp);
  uint elim = getDoc(list[0],1,S,offsets);

  while(true) {
    uint k=1;
    while(k<ls) {
      uint v = nextDoc(list[k],S,offsets,elim-1);
      if(v == (uint)-1) return;
      if(v>elim) {
	elim = nextDoc(list[0],S,offsets,v-1);
	if(elim == (uint)-1) return;
	k = 1;
      } else {
	k++;
      }
    }
    result.push_back(elim);
    elim = nextDoc(list[0],S,offsets,elim);
    if(elim==(uint)-1) break;
  }
    
  //return result;  
}

void intersect3(vector<uint> & list, Sequence * S, vector<uint> & offsets, vector<uint> & result) {
  //vector<uint> result;
  uint ls = list.size();
  WordsLenCmp cmp(S);
  sort(list.begin(),list.end(),cmp);
  uint elim = getDoc(list[0],1,S,offsets);

  while(true) {
    uint p=1, k=1;
    while(k<ls) {
      uint v = nextDoc(list[p],S,offsets,elim-1);
      if(v == (uint)-1) return;
      if(v>elim) {
	elim = v;
	k = 1;
      } else {
	k++;
      }
      p = (p+1)%ls;
    }
    result.push_back(elim);
    elim = nextDoc(list[0],S,offsets,elim);
    if(elim==(uint)-1) break;
  }
}

int main(int argc, char **argv) {
  if(argc!=5) {
    cout << "usage: " << argv[0] << " <text> <offsets> <queries> <seq>" << endl;
    cout << " seq=1: Wavelet Tree Noptrs RG5" << endl;
    cout << " seq=2: Wavelet Tree Noptrs RG10" << endl;
    cout << " seq=3: Wavelet Tree Noptrs RG20" << endl;
    cout << " seq=4: Wavelet Tree Noptrs RRR16" << endl;
    cout << " seq=5: Wavelet Tree Noptrs RRR32" << endl;
    cout << " seq=6: Wavelet Tree Noptrs RRR64" << endl;
    cout << " seq=7: GMR B=RG20 X=RG20 PS=16" << endl;
    return 0;
  }

  uint seq = transform(string(argv[4]));

  ifstream input_seq(argv[1]);
  Array * A = new Array(input_seq);
  input_seq.close();

  SequenceBuilder * sb,*sb1,*sb2,*sb3;
  Sequence * S;
  switch(seq) {
  case 1: S = new WaveletTreeNoptrs(*A, new BitSequenceBuilderRG(5),new MapperNone()); break;
  case 2: S = new WaveletTreeNoptrs(*A, new BitSequenceBuilderRG(10),new MapperNone()); break;
  case 3: S = new WaveletTreeNoptrs(*A, new BitSequenceBuilderRG(20),new MapperNone()); break;
  case 4: S = new WaveletTreeNoptrs(*A, new BitSequenceBuilderRRR(16),new MapperNone()); break;
  case 5: S = new WaveletTreeNoptrs(*A, new BitSequenceBuilderRRR(32),new MapperNone()); break;
  case 6: S = new WaveletTreeNoptrs(*A, new BitSequenceBuilderRRR(64),new MapperNone()); break;
  case 7: 
    sb = new SequenceBuilderGMRChunk(new BitSequenceBuilderRG(20),new PermutationBuilderMRRR(16, new BitSequenceBuilderRG(20)));
    S = new SequenceGMR(*A, 2*A->getMax(), new BitSequenceBuilderRG(20), sb);
    break;
  case 8:
    sb1 = new SequenceBuilderWaveletTree(new BitSequenceBuilderRG(20), new MapperNone());
    sb2 = new SequenceBuilderGMRChunk(new BitSequenceBuilderRG(20),new PermutationBuilderMRRR(16, new BitSequenceBuilderRG(20)));
    sb3 = new SequenceBuilderGMR(new BitSequenceBuilderRG(20),sb2);
    S = new SequenceAlphPart(*A, 10, sb1, sb3);
    break;
  }
  cout << "Sequence size: " << S->getSize() << endl;

  ifstream input_offs(argv[2]);
  uint of;
  vector<uint> offsets;
  while((input_offs>>of)) offsets.push_back(of);
  offsets.push_back(S->getLength());
  input_offs.close();

  cout << "Structure size: " << S->getSize()+offsets.size()*sizeof(uint) << endl;

  cout << "No of documents: " << offsets.size() << endl;
  cout << "No of words: " << A->getMax() << endl;

  ifstream input(argv[3]);
  vector<vector<uint> > queries;
  uint qs;
  while((input >> qs)) {
    vector<uint> query;
    for(uint i=0;i<qs;i++) {
      uint tmp;
      input >> tmp;
      query.push_back(tmp);
    }
    queries.push_back(query);
  }
  input.close();

#define ERROUT 0

  uint acc = 0;
  start_timing();
  vector<uint> aux;
  for(uint i=0;i<queries.size();i++) {
    aux.clear();
    intersect2(queries[i],S,offsets,aux);
    #if ERROUT
    for(uint j=0;j<aux.size();j++) {
      cerr << " " << aux[j];
      acc += aux[j];
    }
    cerr << endl;
    #else
    acc += aux.size();
    #endif
  }
  double t=get_timing();
  cout << "acc: " << acc << endl;
  cout << "Total time: " << t << " ms" << endl;
  cout << "Time per query: " << t/queries.size() << " ms" << endl;

  delete A;
  delete S;
}
