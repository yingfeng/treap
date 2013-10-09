/**
 *    intersection.h
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


#include <Sequence.h>

using namespace cds_utils;
using namespace cds_static;

uint getDoc(uint word, uint occ, Sequence *S, vector<uint> offsets) {
  uint pos = S->select(word,occ);
  if(pos>=S->getLength()) return (uint)-1;
  vector<uint>::iterator ptr = upper_bound(offsets.begin(), offsets.end(), pos);
  return (uint)(ptr-offsets.begin())-1;
}

uint nextDoc(uint word, Sequence *S, vector<uint> offsets, uint startDoc) {
  assert(startDoc+1!=0);
  uint cnt = S->rank(word,offsets[startDoc+1]-1)+1;
  if(cnt>=S->rank(word,S->getLength()-1)) return (uint)-1;
  return getDoc(word, cnt, S, offsets);
}

bool containsDoc(uint word, Sequence *S, vector<uint> offsets, uint doc) {
  assert(doc+1!=0);
  return S->rank(word,offsets[doc+1]-1) - S->rank(word,offsets[doc]) > 0;
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

vector<uint> intersect(vector<uint> list, Sequence * S, vector<uint> offsets) {
  vector<uint> result;
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
    
  return result;  
}
