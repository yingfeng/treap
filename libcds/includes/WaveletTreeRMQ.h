#ifndef _WVTREE_RMQ_H
#define _WVTREE_RMQ_H
#include <iostream>
#include <cassert>
#include <libcdsBasics.h>
#include <BitSequence.h>
#include <BitSequenceBuilder.h>
#include <Sequence.h>
#include <Mapper.h>
#include <queue>
#include <factorization.h>
//#include <WaveletTreeNoptrs.h>
#include <rmq.h>
using namespace rmq_space;
namespace cds_static
{

    class RangeInfo {
    public:
        uint x1,x2,y1,y2,w,pos,level,sym,levelpos,endx,startx;

        RangeInfo(uint x1,uint x2,uint y1,uint y2,uint w,uint pos,uint level,uint sym,uint levelpos,uint startx,uint endx){
            this->x1 = x1;
            this->x2 = x2;
            this->y1 = y1;
            this->y2 = y2;
            this->endx = endx;
            this->startx = startx;
            this->pos = pos;
            this->w = w;
            this->level = level;
            this->sym = sym;
            this->levelpos = levelpos;
            // cout << "***************" << endl;
            // cout << "creating: rangeinfo" << endl;
            // cout << "x1 = " << x1 << endl;
            // cout << "x2 = " << x2 << endl;
            // cout << "y1 = " << y1 << endl;
            // cout << "y2 = " << y2 << endl;
            // cout << "pos = " << pos << endl;
            // cout << "w = " << w << endl;
            // cout << "level = " << level << endl;
            // cout << "m = " << levelpos << endl;
            // cout << "sym = " << sym << endl;
            // cout << "startx = " << startx << endl;
            // cout << "endx = " << endx << endl;
            //  cout << "***************" << endl;
        }
    };


class CompareRangeInfo {
    public:
    bool operator()(RangeInfo*& t1, RangeInfo*& t2) 
    {
       if (t1->w > t2->w) return true;
       return false;
    }
};

class TopkResult {
public:
      uint w;
      uint pos;
      pair<uint,uint> r;
      TopkResult(uint w, uint pos) {
            this->w = w;
            this->pos = pos;
            r = make_pair(w,pos);
      }
};
class CompareTopkResult {
public:
    bool operator()(TopkResult*& t1, TopkResult*& t2) {
       if (t1->w > t2->w) return true;
       return false;
    }
};


    class WaveletTreeRMQ : public Sequence
    {
        public:

                  /** Builds a Wavelet Tree for the string stored in a.
                   * @param bmb builder for the bitmaps in each level.
                   * @param am alphabet mapper (we need all symbols to be used).
                   * */
                  WaveletTreeRMQ(const Array & a, BitSequenceBuilder * bmb, Mapper * am,uint *weight);
                  /** Builds a Wavelet Tree for the string
                   * pointed by symbols assuming its length
                   * equals n and uses bmb to build the bitsequence
                   * @param bmb builder for the bitmaps in each level.
                   * @param am alphabet mapper (we need all symbols to be used).

                  /** Destroys the Wavelet Tree */
                  virtual ~WaveletTreeRMQ();

                  virtual size_t rank(uint symbol, size_t pos) const;
                  virtual size_t select(uint symbol, size_t j) const;
                  virtual uint access(size_t pos) const;
                  virtual size_t getSize() const;

                  void range(uint ni1,uint ni2,uint i1, uint i2, uint j1, uint j2, uint leftb, uint rightb, uint symb, uint level,priority_queue<TopkResult*,vector<TopkResult*>,CompareTopkResult> &res,priority_queue<RangeInfo*,vector<RangeInfo*>,CompareRangeInfo> &ranges);
                  vector<pair<uint,uint> > range_call(uint i1, uint i2, uint j1, uint j2,uint k);
                  uint getMaxValue();
                  uint getHeight();
                  void topk(uint k,priority_queue<RangeInfo*,vector<RangeInfo*>,CompareRangeInfo> &ranges,priority_queue<TopkResult*,vector<TopkResult*>,CompareTopkResult> &res);
                  virtual uint quantile(size_t left,size_t right,uint q);

                  /* find the q-th smallest element in T[l..r] and return the freq */
                  pair<uint32_t,size_t> quantile_freq(size_t left,size_t right,uint q);

                  virtual size_t count(uint symbol) const;

                  virtual void save(ofstream & fp) const;
                  static WaveletTreeRMQ * load(ifstream & fp);

            protected:
                  WaveletTreeRMQ();

                  Mapper * am;
                  /** Only one bit-string for the Wavelet Tree. */
                  BitSequence **bitstring;
                  uint *OCC;
                  RMQ **rmq;
                  factorization *last_weight;

                  /** Length of the string. */
                  size_t n;
                 // uint *last_weight;

                  /** Height of the Wavelet Tree. */
                  uint height, max_v;

                  /** Obtains the maximum value from the string
                   * symbols of length n */
                  uint max_value(uint * symbols, size_t n);
                  uint max_value(uint * symbols, unsigned width, size_t n);

                  /** How many bits are needed to represent val */
                  uint bits(uint val);

                  /** Returns true if val has its ind-th bit set
                   * to one. */
                  bool is_set(uint val, uint ind) const;

                  /** Recursive function for building the Wavelet Tree. */
                  void build_level(uint **bm, uint *symbols, uint level, uint length, uint offset);
                  void build_level(uint **bm, uint *symbols, unsigned width, uint level, uint length, uint offset);
                  void sortWeights(uint *weight, uint *symbols);
    };
};
#endif
