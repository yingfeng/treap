/* WaveletTreeNoptrs.cpp
 * Copyright (C) 2008, Francisco Claude.
 * Copyright (C) 2011, Matthias Petri.
 *
 *
 * WaveletTreeNoptrs definition
 *
 * This library is free software; you can redistribute it and/or
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
#include <queue>
#include <WaveletTreeRMQ.h>

namespace cds_static
{
    WaveletTreeRMQ::WaveletTreeRMQ(const Array & a, BitSequenceBuilder * bmb, Mapper * am,uint *weight) : Sequence(0) {
        bmb->use();
        this->n = a.getLength();
        this->length = n;
        this->am = am;
        am->use();
        uint * symbols = new uint[n];
        for(size_t i = 0; i < n; i++)
            symbols[i] = am->map(a[i]);
        max_v = am->map(a.getMax());
        height = bits(max_v);

        OCC = new uint[max_v + 2];
        for (uint i = 0; i <= max_v + 1; i++)
            OCC[i] = 0;
        for (uint i = 0; i < n; i++)
            OCC[symbols[i] + 1]++;

        uint to_add = 0;
        for (uint i = 1; i <= max_v + 1; i++)
            if (OCC[i] == 0) to_add++;

        uint * new_symb = new uint[n + to_add];
        for (uint i = 0; i < n; i++)
            new_symb[i] = symbols[i];
        
        to_add = 0;
        for (uint i = 1; i <= max_v + 1; i++)
        if (OCC[i] == 0) {
            OCC[i]++;
            new_symb[n + to_add] = i - 1;
            to_add++;
        }

        uint new_n = n + to_add;
        cout << "n = " << n << " new_n = " << new_n << endl;
        for(uint i = 1;i <= max_v + 1; i++)
            OCC[i] += OCC[i - 1];
        this->n = new_n;

        uint **_bm = new uint*[height];
        for(uint i = 0; i < height; i++) {
            _bm[i] = new uint[new_n / W + 1];
            for(uint j = 0; j < new_n / W + 1; j++)
                _bm[i][j] = 0;
        }
        this->rmq = new RMQ*[height + 1];
        build_level(_bm, new_symb, 0, new_n, 0);
        bitstring = new BitSequence*[height];
        for(uint i = 0; i < height; i++) {
            bitstring[i] = bmb->build(_bm[i], new_n);
            delete [] _bm[i];
        }

        uint maxw = weight[0];
        for (uint i = 0; i < length; i++)
            maxw = max(maxw, weight[i]);
        uint *newweights = new uint[n];
        for (int i = 0; i < length; i++)
            newweights[i] = weight[i];
        for (int i = length; i < n; i++)
            newweights[i] = maxw+1;
        sortWeights(newweights, symbols);

        delete [] _bm;
        delete [] symbols;

        bmb->unuse();
    }

    WaveletTreeRMQ::WaveletTreeRMQ():Sequence(0) {
        bitstring = NULL;
        OCC = NULL;
        am = NULL;
    }

    WaveletTreeRMQ::~WaveletTreeRMQ() {
        if (bitstring) {
            for (uint i = 0; i < height; i++)
                if (bitstring[i])
                    delete bitstring[i];
            delete [] bitstring;
        }
        delete [] OCC;
        if (am)
            am->unuse();
    }

    void WaveletTreeRMQ::save(ofstream & fp) const
    {
        uint wr = WVTREE_NOPTRS_HDR;
        saveValue(fp, wr);
        saveValue<size_t>(fp,n);
        saveValue(fp, max_v);
        saveValue(fp, height);
        am->save(fp);
        for (uint i = 0; i < height; i++)
            bitstring[i]->save(fp);
        saveValue<uint>(fp, OCC, max_v + 2);
    }

    WaveletTreeRMQ * WaveletTreeRMQ::load(ifstream & fp) {
        uint rd = loadValue<uint>(fp);
        if (rd != WVTREE_NOPTRS_HDR) return NULL;
        WaveletTreeRMQ * ret = new WaveletTreeRMQ();
        ret->n = loadValue<size_t>(fp);
        ret->length = ret->n;
        ret->max_v = loadValue<uint>(fp);
        ret->height = loadValue<uint>(fp);
        ret->am = Mapper::load(fp);
        if (ret->am == NULL) {
            delete ret;
            return NULL;
        }
        ret->am->use();
        ret->bitstring = new BitSequence*[ret->height];
        for(uint i = 0; i < ret->height; i++)
            ret->bitstring[i] = NULL;
        for(uint i = 0; i < ret->height; i++) {
            ret->bitstring[i] = BitSequence::load(fp);
            if (ret->bitstring[i] == NULL) {
                delete ret;
                return NULL;
            }
        }
        ret->OCC = loadValue<uint>(fp, ret->max_v + 2);
        return ret;
    }

    inline uint get_start(uint symbol, uint mask) {
        return symbol & mask;
    }

    inline uint get_end(uint symbol, uint mask) {
        return get_start(symbol, mask) + ~mask + 1;
    }

    bool WaveletTreeRMQ::is_set(uint val, uint ind) const
    {
        assert (ind < height);
        return (val & (1 << (height - ind - 1))) != 0;
    }

    uint WaveletTreeRMQ::access(size_t pos) const
    {
        uint ret = 0;

        size_t start = 0;
        for (uint level = 0; level < height; level++) {
            size_t optR, before = 0;
            if (start > 0)
                before = bitstring[level]->rank1(start - 1);

            if (bitstring[level]->access(pos, optR)) {
                ret |= (1 << (height - level - 1));
                pos = optR - 1 - before;
                start = OCC[ret];
                pos += start;
            } else {
                pos = optR - 1 + before;
            }
        }

        return am->unmap(ret);
    }


    size_t WaveletTreeRMQ::rank(uint symbol, size_t pos) const
    {
        symbol = am->map(symbol);;

        size_t start = 0;
        size_t count = 0;

        for(uint level = 0; level < height; level++) {

            uint masked = (symbol >> (height - level - 1)) << (height - level - 1);

            size_t before = 0;
            if (start > 0)
                before = bitstring[level]->rank1(start - 1);

            if (is_set(symbol, level)) {
                count = bitstring[level]->rank1(pos) - before;
                start = OCC[masked];
                pos = count + start - 1;
            } else {
                count = pos - start + before - bitstring[level]->rank1(pos) + 1;
                masked += (1 << (height - level - 1)); 
                pos = count + start - 1;
            }

            if (count == 0) return 0;
        }
        return count;
    }

    void WaveletTreeRMQ::range(uint ni1,uint ni2,uint i1, uint i2, uint j1, uint j2, uint leftb, uint rightb, uint symb, uint level,priority_queue<TopkResult*,vector<TopkResult*>,CompareTopkResult> &res,priority_queue<RangeInfo*,vector<RangeInfo*>,CompareRangeInfo> &ranges) {
        if (ni1 > ni2 || leftb > j2 || rightb < j1) return;
        if (leftb > j2 || rightb < j1) return;

        if (leftb >= j1 && rightb <= j2) {
            uint pos;
            uint start = ni1;
            if (level == height) {
            //    if (symb >= j1 && symb <= j2) {
                //    cout << "symb is " << symb << " j1 = " << j1 << " j2 = " << j2 << endl;
                    uint m = rmq[level]->query(start+i1,start+i2);
                    RangeInfo *ri = new RangeInfo(ni1+i1,ni1+i2,j1,j2,this->last_weight->access(m+1),m,level,symb,m,ni1,ni2);
                    ranges.push(ri);
              //  }
              //  return;
            } 
            if (start+i2 >= start+i1) {
                uint m = rmq[level]->query(start+i1,start+i2);
                pos = m;
                uint lev=level;
                uint ret=symb;
                size_t start_aux=ni1;
                size_t end = ni2;
                while(lev<height) {
                   // cout << "entered here!" << endl;
                    if(!(pos>=start_aux && pos<=end)) return;
                    if(bitstring[lev]->access(pos)) {
                        ret |= (1 << (height - lev - 1));
                        pos=bitstring[lev]->rank1(pos-1)-bitstring[lev]->rank1(start_aux-1);
                        start_aux=(bitstring[lev]->rank1(end)-bitstring[lev]->rank1(start_aux-1));
                        start_aux=end-start_aux+1;
                        pos+=start_aux;
                    }
                    else {
                        pos=pos-start_aux-(bitstring[lev]->rank1(pos)-bitstring[lev]->rank1(start_aux-1));
                        end=end-start_aux-(bitstring[lev]->rank1(end)-bitstring[lev]->rank1(start_aux-1));
                        end+=start_aux;
                        pos+=start_aux;
                    }
                    lev++;
                }
            //if (ret >= j1 && ret <= j2 ) {
              //  cout << "ret is " << ret << " j1 = " << j1 << " j2 = " << j2 << endl;
                 RangeInfo *ri = new RangeInfo(ni1+i1,ni1+i2,j1,j2,this->last_weight->access(pos+1),pos,level,symb,m,ni1,ni2);
                 ranges.push(ri);
            //}
           // return;
            } else return;
            return;
        }
    //}

        if (level == (int)height) return;

        // left side
        int newleftb = symb;
        int newrightb = (int)((uint)symb | (uint)((1u << (height - level - 1)) - 1));

        int start = ni1;
        int before = 0;
        if (start > 0)
            before = bitstring[level]->rank0(start - 1);
        int newi1 = ((i1 + start > 0) ? bitstring[level]->rank0(i1 + start - 1) : 0) - before;
        int newi2 = bitstring[level]->rank0(start + i2) - before - 1;
        int newni2 = ni1 + bitstring[level]->rank0(ni2) - before - 1;

        if (newi1 <= newi2) {
            range(ni1, newni2, newi1, newi2, j1, j2, newleftb, newrightb, symb, level + 1, res,ranges);
        }

        // right side
        newleftb = (int)((uint)symb | (1u << (height - level - 1)));
        newrightb = (int)((uint)symb | ((1u << (height - level)) - 1));

        start = ni1;
        if (start > 0)
            before = bitstring[level]->rank1(start - 1);
        int newni1 = newni2 + 1;
        newi1 = ((i1 + start > 0) ? bitstring[level]->rank1(start + i1 - 1) : 0) - before;
        newi2 = bitstring[level]->rank1(start + i2) - before - 1;

        if (newi1 <= newi2) {
            range(newni1, ni2, newi1, newi2, j1, j2, newleftb, newrightb, symb | (1 << (height - level - 1)), level + 1, res,ranges);
        }
    }

    void WaveletTreeRMQ::topk(uint k,priority_queue<RangeInfo*,vector<RangeInfo*>,CompareRangeInfo> &ranges,priority_queue<TopkResult*,vector<TopkResult*>,CompareTopkResult> &res) {
        uint count = 0;
        while (!ranges.empty() && count < k) {
            RangeInfo *r = ranges.top();
            ranges.pop();
            //cout << "Adding position " << r->pos << " weight = " << r->w << endl;
//            if (r->w != 0) {
                res.push(new TopkResult(r->w,r->pos));
                count ++;
//                continue;
  //          } else { continue; } 
            // if (r->levelpos-1 == r->x1) continue;
            // if (r->levelpos+1 == r->x2) continue;
           // if (r->level == height) continue;
            //if (r->levelpos == 0)  continue; 
            if (r->level == height) {
                if (r->x1 <= r->levelpos-1 && r->levelpos != 0) {
//	           if (r->sym >= r->y1 && r->sym <= r->y2) {
                      //  cout << "ret1 is " << r->sym << " j1 = " << r->y1 << " j2 = " << r->y2 << endl;

                        uint m = rmq[r->level]->query(r->x1,r->levelpos-1);
                        RangeInfo *ri = new RangeInfo(r->x1,r->levelpos-1,r->y1,r->y2,this->last_weight->access(m+1),m,r->level,r->sym,m,r->startx,r->endx);
                        ranges.push(ri);
                   // }
        //            continue;

                } 
                if (r->levelpos+1 <= r->x2 ) {
                 //   if (r->sym >= r->y1 && r->sym <= r->y2) {
                    //    cout << "ret2 is " << r->sym << " j1 = " << r->y1 << " j2 = " << r->y2 << endl;
                        uint m = rmq[r->level]->query(r->levelpos+1,r->x2);
                        RangeInfo *ri = new RangeInfo(r->levelpos+1,r->x2,r->y1,r->y2,this->last_weight->access(m+1),m,r->level,r->sym,m,r->startx,r->endx);
                        ranges.push(ri);
                  //}
          //          continue;
                }  
            } 
            if (r->x1 <= r->levelpos-1) {
                uint m = rmq[r->level]->query(r->x1,r->levelpos-1);
                uint pos = m;
                uint ret = r->sym;
                uint start = r->startx;
                uint end = r->endx;
                uint lev = r->level;
                while(lev<height) {
                   if (!(pos>=start && pos<=end)) continue;
                    if(bitstring[lev]->access(pos)) {
                        ret |= (1 << (height - lev - 1));
                        pos=bitstring[lev]->rank1(pos-1)-bitstring[lev]->rank1(start-1);
                        start=(bitstring[lev]->rank1(end)-bitstring[lev]->rank1(start-1));
                        start=end-start+1;
                        pos+=start;
                    }
                    else {
                        pos=pos-start-(bitstring[lev]->rank1(pos)-bitstring[lev]->rank1(start-1));
                        end=end-start-(bitstring[lev]->rank1(end)-bitstring[lev]->rank1(start-1));
                        end+=start;
                        pos+=start;
                    }
                    lev++;
                }
//                if (ret >= r->y1 && ret <= r->y2 ) {
                //    cout << "ret is " << ret << " j1 = " << r->y1 << " j2 = " << r->y2 << endl;

                    RangeInfo *ri = new RangeInfo(r->x1,r->levelpos-1,r->y1,r->y2,this->last_weight->access(pos+1),pos,r->level,r->sym,m,r->startx,r->endx);
                    ranges.push(ri);
//               }
            }
            if (r->levelpos+1 <= r->x2 ) {
                uint m = rmq[r->level]->query(r->levelpos+1,r->x2);
                uint pos = m;
                uint ret = r->sym;
                uint start = r->startx;
                uint end = r->endx;
                uint lev = r->level;
                while(lev<height) {
                    if(!(pos>=start && pos<=end)) continue;
                    if(bitstring[lev]->access(pos)) {
                        pos=bitstring[lev]->rank1(pos-1)-bitstring[lev]->rank1(start-1);
                        ret |= (1 << (height - lev - 1));
                        start=(bitstring[lev]->rank1(end)-bitstring[lev]->rank1(start-1));
                        start=end-start+1;
                        pos+=start;
                    }
                    else {
                        pos=pos-start-(bitstring[lev]->rank1(pos)-bitstring[lev]->rank1(start-1));
                        end=end-start-(bitstring[lev]->rank1(end)-bitstring[lev]->rank1(start-1));
                        end+=start;
                        pos+=start;
                    }
                    lev++;
                }
//                if (ret >= r->y1 && ret <= r->y2 ) {
                   // cout << "ret is " << ret << " j1 = " << r->y1 << " j2 = " << r->y2 << endl;
                    RangeInfo *ri = new RangeInfo(r->levelpos+1,r->x2,r->y1,r->y2,this->last_weight->access(pos+1),pos,r->level,r->sym,m,r->startx,r->endx);
                    ranges.push(ri);
//                }  
            }
        }
    }
    vector<pair<uint,uint> > WaveletTreeRMQ::range_call(uint i1, uint i2, uint j1, uint j2, uint k) {
        priority_queue<RangeInfo*,vector<RangeInfo*>,CompareRangeInfo> ranges;
        priority_queue<TopkResult*,vector<TopkResult*>,CompareTopkResult> res;       
        range(0,n-1,i1, i2, j1, j2, 0, max_v, 0, 0, res,ranges);
        topk(k,ranges,res);
        vector<pair<uint,uint> > result;
        while (!res.empty()) {
            result.push_back(res.top()->r);
            res.pop();
        }
        return result;
    }

    size_t WaveletTreeRMQ::select(uint symbol, size_t j) const
    {
        symbol = am->map(symbol);

        uint mask = (1 << height) - 2;
        uint sum = 2;

        size_t pos = j;

        for (int level = height - 1; level >= 0; level--) {

            size_t start = get_start(symbol, mask);
            start = OCC[start];

            uint ones_start = 0;
            if (start > 0)
                ones_start = bitstring[level]->rank1(start - 1);

            if (is_set(symbol,level)) {
                pos = bitstring[level]->select1(ones_start + pos) - start + 1;
            } else {
                pos = bitstring[level]->select0(start - ones_start + pos) - start + 1;
            }

            mask <<= 1;
            sum <<= 1;
        }

        return pos - 1;
    }

    size_t WaveletTreeRMQ::getSize() const
    {
        size_t ptrs = sizeof(WaveletTreeNoptrs) + height * sizeof(Sequence*);
        size_t bytesBitstrings = 0;
        size_t rmqs = 0;
        size_t dacs = 0;
        for(uint i = 0; i < height; i++)
            bytesBitstrings += bitstring[i]->getSize();
        for(uint i = 0; i < height; i++)
            rmqs += rmq[i]->getSize();
            dacs = this->last_weight->getSize();
        return bytesBitstrings + sizeof(uint) * (max_v + 2) + ptrs + rmqs + dacs;
    }

    void WaveletTreeRMQ::build_level(uint **bm, uint *symbols, uint level, uint length, uint offset) {
        // rmqs[level-1] = new RMQ((int*)weights, n);
        if (level == height) {
            delete [] symbols;
            return;
        }

        uint cleft = 0;
        for (size_t i = 0; i < length; i++)
            if (!is_set(symbols[i],level))
                cleft++;

        uint cright = length - cleft;

        // uint *leftw = new uint[cleft];
        // uint *rightw = new uint[cright];
        uint *left = new uint[cleft];
        uint *right = new uint[cright];
        cleft = cright = 0;
        for (size_t i = 0; i < length; i++) {
            if (!is_set(symbols[i], level)) {
                // leftw[cleft] = weights[i]
                left[cleft++] = symbols[i];
                bitclean(bm[level], offset + i);
            } else {
                // rightw[cright] = weights[i];
                right[cright++] = symbols[i];
                bitset(bm[level], offset + i);
            }
        }

        delete [] symbols;
        symbols = NULL;

        build_level(bm, left, level + 1, cleft, offset);
        left = NULL;             // Gets deleted in recursion.
        build_level(bm, right, level + 1, cright, offset + cleft);
        right = NULL;            // Gets deleted in recursion.
    }

    // symbols is an array of elements of "width" bits.
    void WaveletTreeRMQ::build_level(uint **bm, uint *symbols, unsigned width, uint level, uint length, uint offset) {
        if (level == height) {
            delete [] symbols;
            return;
        }

        uint cleft = 0;
        for (size_t i = 0; i < length; i++)
            if (!is_set(get_field(symbols, width, i), level))
                cleft++;

        uint cright = length - cleft;


        uint *left = new uint[(cleft * width) / W + 1];
        uint *right = new uint[(cright * width) / W + 1];
        cleft = cright = 0;
        for (size_t i = 0; i < length; i++) {
            if (!is_set(get_field(symbols, width, i), level)) {
                set_field(left, width, cleft++, get_field(symbols, width, i));
                bitclean(bm[level], offset + i);
            }
            else {
                set_field(right, width, cright++, get_field(symbols, width, i));
                bitset(bm[level], offset + i);
            }
        }

        delete [] symbols;
        symbols = NULL;

        build_level(bm, left, width, level + 1, cleft, offset);
        left = NULL;             // Gets deleted in recursion.
        build_level(bm, right, width, level + 1, cright, offset + cleft);
        right = NULL;            // Gets deleted in recursion.
    }

    uint WaveletTreeRMQ::max_value(uint *symbols, size_t n) {
        uint max_v = 0;
        for (size_t i = 0; i < n; i++)
            max_v = max(symbols[i], max_v);
        return max_v;
    }

    uint WaveletTreeRMQ::max_value(uint *symbols, unsigned width, size_t n) {
        uint max_v = 0;
        for (size_t i = 0; i < n; i++)
            max_v = max(get_field(symbols, width, i), max_v);
        return max_v;
    }

    uint WaveletTreeRMQ::bits(uint val) {
        uint ret = 0;
        while (val!=0) {
            ret++;
            val >>= 1;
        }
        return ret;
    }

    size_t WaveletTreeRMQ::count(uint symbol) const
    {
        uint mapped = am->map(symbol);
        return OCC[mapped + 1] - OCC[mapped] + 1;
    }

    uint WaveletTreeRMQ::quantile(size_t left,size_t right,uint q) {
        pair<uint,size_t> res = quantile_freq(left,right,q);
        return res.first;
    }

    pair<uint32_t,size_t> WaveletTreeRMQ::quantile_freq(size_t left,size_t right,uint q) {
        /* decrease q as the smallest element q=1 is
         * found by searching for 0 */
        q--;

        assert( right >= left );
        assert( (right-left+1) >= q );
        assert( right < length );

        uint sym = 0;
        uint freq = 0;
        uint level = 0;
        size_t start = 0, end = n-1;
        size_t before;
        BitSequence* bs;

        while(level<height) {
            bs = bitstring[level];

            /* calc start of level bound */
            if(start == 0) before = 0;
            else before = bs->rank1(start-1);

            /* number of 1s before T[l..r] */
            size_t rank_before_left = bs->rank1(start+left-1);
            /* number of 1s before T[r] */
            size_t rank_before_right = bs->rank1(start+right);
            /* number of 1s in T[l..r] */
            size_t num_ones = rank_before_right - rank_before_left;
            /* number of 0s in T[l..r] */
            size_t num_zeros = (right-left+1) - num_ones;

            /* if there are more than q 0s we go right. left otherwise */
            if(q >= num_zeros) { /* go right */
                freq = num_ones; /* calc freq */
                /* set bit to 1 in sym */
                sym = 1 << (height - level - 1); //set(sym,level);
                /* number of 1s before T[l..r] within the current node */
                left = rank_before_left - before;
                /* number of 1s in T[l..r] */
                right = rank_before_right - before - 1;
                q = q - num_zeros;
                /* calc starting pos of right childnode */
                start = end - (bs->rank1(end)-before) + 1;
            }                    /* go left q = q // sym == sym */
            else {
                freq = num_zeros;/* calc freq */
                /* number of zeros before T[l..r] within the current node */
                left = left - (rank_before_left - before);
                /* number of zeros in T[l..r] + left bound */
                right = right - (rank_before_right - before);
                /* calc end pos of left childnode */
                end = end - (bs->rank1(end) - before);
            }
            level++;
        }

        /* unmap symbol */
        return pair<uint,size_t>(am->unmap(sym),static_cast<uint>(freq));
    }

     uint WaveletTreeRMQ::getHeight() {
        return this->height;
     }
     uint WaveletTreeRMQ::getMaxValue() {
        return this->max_v;
     }

     void WaveletTreeRMQ::sortWeights(uint *weight, uint *symbols) {
        uint i = 0;
        queue<pair<size_t, size_t> > ranges;
        ranges.push(make_pair((size_t)0u, n));


        rmq[0] = new RMQ((int*)weight, n);
        while (i < height) {
            pair<size_t, size_t> r = ranges.front();
            ranges.pop();

            size_t ini = r.first;
            size_t fin = r.second;
            size_t w_left = 0;
            size_t w_right = 0;

          //  cout << " ini = " << ini << " fin = " << fin << " n = " << n << endl;

            for (size_t k = ini ; k < fin ; k++) {
                if (bitstring[i]->access(k)) {
                    assert(is_set(symbols[k], i));
                    w_right++;
                }
                else {
                    assert(!is_set(symbols[k], i));
                    w_left++;
                }
            }

            uint *weight_aux_left = new uint[w_left];
            uint *weight_aux_right = new uint[w_right];
            uint *symbols_aux_left = new uint[w_left];
            uint *symbols_aux_right = new uint[w_right];
            size_t pos_left = 0, pos_right = 0;
            for (size_t k = ini; k < fin; k++) {
                if (bitstring[i]->access(k)) {
                    weight_aux_right[pos_right] = weight[k];
                    symbols_aux_right[pos_right++] = symbols[k];
                }
                else {
                    symbols_aux_left[pos_left] = symbols[k];
                    weight_aux_left[pos_left++] = weight[k];
                }
            }

            assert(pos_left == w_left);
            assert(pos_right == w_right);
            assert(pos_left + pos_right == fin - ini);

            for (size_t k = 0; k < pos_left; k++) {
                weight[k + ini] = weight_aux_left[k];
                symbols[k+ini] = symbols_aux_left[k];
            }
            for (size_t k = 0; k < pos_right; k++) {
                weight[k + ini + pos_left] = weight_aux_right[k];
                symbols[k+ini+pos_left] = symbols_aux_right[k];
            }
            assert(pos_right + ini + pos_left == fin);

            if (pos_left > 0)
                ranges.push(make_pair(ini, ini+pos_left));
            if (ini + pos_left < fin)
                ranges.push(make_pair(ini+pos_left, fin));

            if (fin == n) {
           //     cout << "level = " << i << "of " << height << endl;
                i ++;
                rmq[i] = new RMQ((int*)weight, n);
            }
            
            delete[] weight_aux_right;
            delete[] weight_aux_left;
            delete [] symbols_aux_left;
            delete [] symbols_aux_right;
        }
        // for (size_t i = 0; i < n - 1; i++) {
        //     cout << symbols[i] << " " << symbols[i+1] << endl;
        //     assert(symbols[i] <= symbols[i+1]);
        // }
        this->last_weight = new factorization(weight,n);
    }

};
   
