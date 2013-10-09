/* WaveletTreeRMQ.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * WaveletTreeRMQ definition
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
        this->n=a.getLength();
        this->length = n;
        this->am=am;
        am->use();
        uint * symbols = new uint[n];
        for(size_t i=0;i<n;i++)
            symbols[i] = am->map(a[i]);
        max_v=am->map(a.getMax());
        height=bits(max_v);
        uint *occurrences=new uint[max_v+1];
        for(uint i=0;i<=max_v;i++) occurrences[i]=0;
        for(uint i=0;i<n;i++)
            occurrences[symbols[i]]++;
        uint to_add=0;
        for(uint i=0;i<max_v;i++)
            if(occurrences[i]==0) to_add++;
        uint * new_symb = new uint[n+to_add];
        for(uint i=0;i<n;i++)
            new_symb[i] = symbols[i];

        delete [] symbols;

        to_add = 0;
        for(uint i=0;i<max_v;i++)
        if(occurrences[i]==0) {
            occurrences[i]++;
            new_symb[n+to_add]=i;
            to_add++;
        }
        uint new_n = n+to_add;
        for(uint i=1;i<=max_v;i++)
            occurrences[i] += occurrences[i-1];
        uint *oc = new uint[(new_n+1)/W+1];
        for(uint i=0;i<(new_n+1)/W+1;i++)
            oc[i] = 0;
        for(uint i=0;i<=max_v;i++)
            bitset(oc,occurrences[i]-1);
        bitset(oc,new_n);
        occ = bmb->build(oc,new_n+1);
        delete [] occurrences;
        this->n = new_n;
        uint ** _bm=new uint*[height];
        for(uint i=0;i<height;i++) {
            _bm[i] = new uint[new_n/W+1];
            for(uint j=0;j<new_n/W+1;j++)
                _bm[i][j]=0;
        }
        this->rmq = new RMQ*[height];
        
        build_level(_bm,new_symb,0,new_n,0);
        bitstring = new BitSequence*[height];
        for(uint i=0;i<height;i++) {
            bitstring[i] = bmb->build(_bm[i],new_n);
            delete [] _bm[i];
        }
        sortWeights(weight);

        delete [] _bm;

        // delete [] new_symb; // already deleted in build_level()!
        delete [] oc;
        bmb->unuse();
    }

   
    WaveletTreeRMQ::WaveletTreeRMQ(uint * symbols, size_t n, BitSequenceBuilder * bmb, Mapper * am, uint *weight, bool deleteSymbols) : Sequence(n) {
        bmb->use();
        this->n=n;
        this->am=am;
        am->use();
        for(uint i=0;i<n;i++)
            symbols[i] = am->map(symbols[i]);
        max_v=max_value(symbols,n);
        height=bits(max_v);
        uint *occurrences=new uint[max_v+1];
        for(uint i=0;i<=max_v;i++) occurrences[i]=0;
        for(uint i=0;i<n;i++)
            occurrences[symbols[i]]++;
        uint to_add=0;
        for(uint i=0;i<max_v;i++)
            if(occurrences[i]==0) to_add++;
        uint * new_symb = new uint[n+to_add];
        for(uint i=0;i<n;i++)
            new_symb[i] = symbols[i];

        if (deleteSymbols) {
            delete [] symbols;
            symbols = 0;
        }

        to_add = 0;
        for(uint i=0;i<max_v;i++)
        if(occurrences[i]==0) {
            occurrences[i]++;
            new_symb[n+to_add]=i;
            to_add++;
        }
        uint new_n = n+to_add;
        for(uint i=1;i<=max_v;i++)
            occurrences[i] += occurrences[i-1];
        uint *oc = new uint[(new_n+1)/W+1];
        for(uint i=0;i<(new_n+1)/W+1;i++)
            oc[i] = 0;
        for(uint i=0;i<=max_v;i++)
            bitset(oc,occurrences[i]-1);
        bitset(oc,new_n);
        occ = bmb->build(oc,new_n+1);
        delete [] occurrences;
        this->n = new_n;
        uint ** _bm=new uint*[height];
        for(uint i=0;i<height;i++) {
            _bm[i] = new uint[new_n/W+1];
            for(uint j=0;j<new_n/W+1;j++)
                _bm[i][j]=0;
        }


        this->rmq = new RMQ*[height];
        // this->rmq[0] = new RMQ((int*)weight,this->n);


        build_level(_bm,new_symb,0,new_n,0);
        bitstring = new BitSequence*[height];
        for(uint i=0;i<height;i++) {
            bitstring[i] = bmb->build(_bm[i],new_n);
            delete [] _bm[i];
        }
        delete [] _bm;

        if (!deleteSymbols)
            for(uint i=0;i<n;i++)
                symbols[i] = am->unmap(symbols[i]);

        // delete [] new_symb; // already deleted in build_level()!
        delete [] oc;
        bmb->unuse();
    }


    // symbols is an array of elements of "width" bits
   
    WaveletTreeRMQ::WaveletTreeRMQ():Sequence(0) {
        bitstring = NULL;
        occ = NULL;
        am = NULL;
    }

    WaveletTreeRMQ::~WaveletTreeRMQ() {
        if(bitstring) {
            for(uint i=0;i<height;i++)
                if(bitstring[i])
                    delete bitstring[i];
            delete [] bitstring;
        }
        if(occ)
            delete occ;
        if(am)
            am->unuse();
    }

    void WaveletTreeRMQ::save(ofstream & fp) const
    {
        uint wr = WVTREE_NOPTRS_HDR;
        saveValue(fp,wr);
        saveValue<size_t>(fp,n);
        saveValue(fp,max_v);
        saveValue(fp,height);
        am->save(fp);
        for(uint i=0;i<height;i++)
            bitstring[i]->save(fp);
        occ->save(fp);
    }

    WaveletTreeRMQ * WaveletTreeRMQ::load(ifstream & fp) {
        uint rd = loadValue<uint>(fp);
        if(rd!=WVTREE_NOPTRS_HDR) return NULL;
        WaveletTreeRMQ * ret = new WaveletTreeRMQ();
        ret->n = loadValue<size_t>(fp);
	ret->length = ret->n;
        ret->max_v = loadValue<uint>(fp);
        ret->height = loadValue<uint>(fp);
        ret->am = Mapper::load(fp);
        if(ret->am==NULL) {
            delete ret;
            return NULL;
        }
        ret->am->use();
        ret->bitstring = new BitSequence*[ret->height];
        for(uint i=0;i<ret->height;i++) 
            ret->bitstring[i] = NULL;
        for(uint i=0;i<ret->height;i++) {
            ret->bitstring[i] = BitSequence::load(fp);
            if(ret->bitstring[i]==NULL) {
                cout << "damn" << i << " " << ret->height << endl;
                delete ret;
                return NULL;
            }
        }
        ret->occ = BitSequence::load(fp);
        if(ret->occ==NULL) {
            delete ret;
            return NULL;
        }
        return ret;
    }

    inline uint get_start(uint symbol, uint mask) {
        return symbol&mask;
    }

    inline uint get_end(uint symbol, uint mask) {
        return get_start(symbol,mask)+!mask+1;
    }

    bool WaveletTreeRMQ::is_set(uint val, uint ind) const
    {
        assert(ind<height);
        return (val & (1<<(height-ind-1)))!=0;
    }

    uint WaveletTreeRMQ::set(uint val, uint ind) const
    {
        assert(ind<=height);
        return val | (1<<(height-ind-1));
    }

    vector<uint>  WaveletTreeRMQ::range_report_aux(size_t x_start,size_t x_end,size_t value)
    {
        vector<uint> result;
        //      result.reserve(1500);
        range_report(0,x_start,x_end,0,0,n-1,result,value);
        return result;
    }



    void topk_aux(uint x_start,uint x_end,vector<uint> &q,int k,RMQ *&r)
    {
         uint max = 0;
         //cout << "x_start = " << x_start << " | x_end =" << x_end << endl;
         //cout << " n = " << r->n << endl;
         if (x_start >= 0 && x_end <= r->n && x_end > x_start)
         {
             if (k < 1)
                 return;
             
             max = r->query(x_start,x_end);
           //  cout << "max = " << max << endl;
             if (max >= 0 && max <= x_end)
             {
                q.push_back(max);
         //       cout << "adding to vector" << max << endl;
                topk_aux(x_start,max-1,q,--k,r);
                topk_aux(max+1,x_end,q,--k,r);
            }
        }
        else
        {
    /*        for (int i = 0 ; i < q.size();i++)
            {
                cout << "vector[" << i << "] ->" << q[i] << endl;
            }
            cout << "vector size = " << q.size() << endl;*/
            return;
        }
    }


    vector<uint> topk(uint x_start,uint x_end,int k,RMQ *&r)
    {
        vector<uint> q;
        topk_aux(x_start,x_end,q,k,r);
        return q;
    }
   
    vector<uint>  WaveletTreeRMQ::range_report_2d_aux(size_t x_start,size_t x_end,size_t y_start,size_t y_end,RMQ *&r)    {
        //RMQ *t = new RMQ();
        vector<uint> result;
        //      result.reserve(1500);
        range_report_2d(0,x_start,x_end,y_start,y_end,0,0,n-1,result,0,r);
        return result;
    }

    void WaveletTreeRMQ::range_report_2d(uint lev, size_t x_start, size_t x_end,size_t y_start, size_t y_end,uint sym, size_t start, size_t end,vector<uint> &result,size_t freq,RMQ *&r)
    {
        size_t x_start_left,x_start_right,x_end_left,x_end_right;
        size_t before;
        size_t end_left;
        size_t end_right;
        size_t start_left;
        size_t start_right;
        // cout << "x_start = " << x_start << endl;
        // cout << "x_end = " << x_end << endl;
        // cout << "start = " << start << endl;
        // cout << "end = " << end << endl; 
        //  cout << "x_start= " <<  x_start << " x_end= " << x_end << endl;
        if (x_start > x_end || x_end > n-1)     
            return;

        if (am->unmap(sym) > y_end)
            return;
        if (lev < height)
        {
            BitSequence* bs = bitstring[lev];
            // cout << "[" << y_start << ", " << y_end << "] " << endl;
            // cout << "sym (map) = " << am->unmap(sym) << endl;
            // cout << "sym = " << sym << endl;
            //cout << "LEVEl " << lev << endl;
            //cout << "start -> " << start << " end -> " << end << endl;
            if (start==0) before=0;
            else before = bs->rank1(start-1);

                size_t rank_left = bs->rank1(start+x_start-1);
                size_t rank_right = bs->rank1(start+x_end);
               
                freq = (end - start + 1) - (rank_right - rank_left);
                x_start_left = x_start - (rank_left - before);
                x_end_left = x_end - (rank_right - before);
                end_left = end - (bs->rank1(end) - before);
                start_left = start;
                //cout << "izquierda | x_start_left = " << x_start_left << " x_end_left = " << x_end_left << endl;
                range_report_2d(lev+1,x_start_left,x_end_left,y_start,y_end,sym,start_left,end_left,result,freq,r);


                sym=set(sym,lev);
                if (am->unmap(sym) > y_end)
                    return;                       
                freq = rank_right - rank_left;
                start_right = end - (bs->rank1(end)-before) + 1;
                end_right = end;
                x_start_right = rank_left - before ;
                x_end_right = rank_right - before-1;   
                //cout << "derecha | x_start_left = " << x_start_right << " x_end_left = " << x_end_right << endl;
                range_report_2d(lev+1,x_start_right,x_end_right,y_start,y_end,sym,start_right,end_right,result,freq,r);

        }
        else
        {   
                if (x_start > x_end)
                return;
                BitSequence* bs = bitstring[lev-1];
                if (am->unmap(sym) <= y_end && am->unmap(sym) >= y_start)
                {
                    uint q_start;
                    uint q_end;
                    if (x_start == x_end){
                        q_start = start+x_start;
                        q_end = q_start+1;
                    }else {
                        q_start = start+x_start;
                        q_end = start+x_end;
                    }

                    int k = 10;
                    vector<uint> v_results = topk(q_start,q_end,bits(k)+1,r);
                    for (int i = 0 ; i < v_results.size();i++)
                    {
                        result.push_back(v_results[i]);    
                    }
                    
                    //v_queue.push_back(topk(q_start,q_end,bits(k),r));
                    // for (int i = 0; i< v_queue.size();i++)
                    // {
                    //     while(!v_queue[i].empty())
                    //     {
                    //         result.push(v_queue[i].top());
                    //         if (v_queue.size() != 0)
                    //             v_queue[i].pop();
                    //     }
                    // }

                //     cout << "Adding -> " << am->unmap(sym) << " -> freq = " << freq << endl;
                //     cout << "------------------------------------------" << endl << endl;
                }
                else
                    return;
               // }
        }   

    }

    uint getBit(uint d,uint pos) {
    if(d & (1<<pos))
        return 1;
    else 
        return 0;
    }
    uint lastBit(uint d) {
        uint count_bits=0;
        int aux = d;

        while(getBit(aux, count_bits++)==0) {
            if (count_bits > 32)
            return 1;
        }
        return count_bits; 
    }
   
    uint createMask(uint i,uint j) {
        uint aux = ~((~0u << j) & (~0u >> i));
        return aux;
    }

    vector<uint>  WaveletTreeRMQ::rr2d_aux(size_t x_start,size_t x_end,size_t y_start,size_t y_end,size_t y_start_aux, size_t y_end_aux)    {
        //RMQ *t = new RMQ();
        vector<uint> result;
        //      result.reserve(1500);
        rr2d(0,x_start,x_end,y_start,y_end,y_start,y_end,0,0,n-1,result);
        return result;
    }
    
    void WaveletTreeRMQ::rr2d(uint lev, size_t x_start, size_t x_end,size_t y_start, size_t y_end,size_t y_start_aux, size_t y_end_aux,uint sym, size_t start, size_t end,vector<uint> &result)
    {   
        // BitSequence *bs;
        // while (lev < height) {
            
        //     uint mask_left = createMask(0,height-lev-1);
        //     uint mask_right = createMask(0,height-lev);
        //     uint max_left = 0;
        //     if (sym == 0)
        //     max_left = mask_left;
        //     else
        //     max_left = sym | mask_left;

        //     uint max_right = set(sym,lev) | mask_right;
        //     uint min_left = max_left/2;
        //     uint min_right = max_right/2+1;
        //     cout << "level = " << lev << endl;
        //     cout << "max_symbol_left = " << max_left << endl;
        //     cout << "min_symbol_left = " << min_left << endl;
        //     cout << endl;
        //     cout << "max_symbol_right = " << max_right << endl;
        //     cout << "min_symbol_right = " << min_right << endl;
        //     cout << "symbol = " << sym << endl;
        //     bs = bitstring[lev];
        //     if (min_left <= y_end && max_left >= y_end){
        //         // if (y_end == max_left) {
        //         //     lev++;
        //         //     sym = set(sym,lev);
        //         //     cout << "symbol = " << sym << endl;
        //         //  //   break;    
        //         // }
        //         lev++;
        //     }
        //     if (min_right <= y_end && max_right >= y_end) {
        //         sym = set(sym,lev);
        //         lev++;
        //     }
            
        // }
        // uint count_bits = lastBit(sym);
        // uint  max_symbol |= ~(1<<(height-count_bits));
        // uint min_symbol = sym << height-count_bits;
        // if (sym > max_symbol) 
        //     return;
        // // bit = sym & (1<<lev)
        // // if (bit == 0) {
        // //     goleft()
        // // } else {
        // //     goright()
        // // }
        // BitSequence *bs = bitstring[lev]
        // for (int y = y_start ; y <= y_end;y++ )  {
        //     this->rmq[lev]->query(bs->rank1(x_start)-1,bs->rank1(x_end+1)-1)    
        // }
        
        size_t x_start_left,x_start_right,x_end_left,x_end_right;
        size_t before;
        size_t end_left;
        size_t end_right;
        size_t start_left;
        size_t start_right;

        uint mask = createMask(0,height-lev);
        uint mask2 = createMask(0,height-(lev+1));

        size_t min_left = sym;
        size_t max_left = sym | mask2;
        size_t min_right = set(sym,lev);
        size_t max_right = set(sym,lev) | mask;

        size_t y_start_left,y_end_left,y_start_right,y_end_right = 0;

        cout << "min_left =" << min_left << endl;
        cout << "max_left =" << max_left << endl;
        cout << endl;
        cout << "min_right =" << min_right << endl;
        cout << "max_right =" << max_right << endl;

        if (x_start > x_end || x_end > n-1)     
               return;

        if (y_start <= y_start_aux && y_end >= y_end_aux && lev != 0 ){
            cout << "FOUND IT!!!" << endl;
            cout << "FOUND IT!!!->level = " << lev <<  endl;
            cout << "y_start_aux =  " << y_start_aux <<  endl;
            cout << "y_end_aux =  " << y_end_aux <<  endl;
            cout << "x_start = " << x_start << endl;
            cout << "x_end = " << x_end << endl;
            uint m;
            if (lev == height-1) {
                m = this->rmq[lev]->query(x_start,x_end);
                cout << "m is = " << m << endl;
                return;
            } else {
                uint ret=sym;
                size_t n_start=x_start;
                size_t n_end=x_end;
                m = this->rmq[lev]->query(x_start,x_end);
                cout << "x_start = " << x_start << endl;
                cout << "x_end = " << x_end << endl;
                cout << "entering with m = " << m << endl;
                while(lev < height) {
                    if(bitstring[lev]->access(m)) {
                        ret=set(ret,lev);
                        m=bitstring[lev]->rank1(m-1)-bitstring[lev]->rank1(n_start-1);
                        n_start=(bitstring[lev]->rank1(n_end)-bitstring[lev]->rank1(n_start-1));
                        n_start=n_end-n_start+1;
                        m+=n_start;
                    } else {
                        m=m-n_start-(bitstring[lev]->rank1(m)-bitstring[lev]->rank1(n_start-1));
                        n_end=end-n_start-(bitstring[lev]->rank1(n_end)-bitstring[lev]->rank1(n_start-1));
                        n_end+=start;
                        m+=start;
                    }
                    lev++;
                }
                cout << "symbol is " << ret << endl;
              //  return;
            }
        }

        cout << "y_start =" << y_start << endl;
        cout << "y_end =" << y_end << endl;
        cout << endl;
        cout << "y_start_aux =" << y_start_aux << endl;
        cout << "y_end_aux =" << y_end_aux << endl;
        cout << endl;
       
       

        if (lev < height)
        {
            BitSequence* bs = bitstring[lev];
            if (start==0) before=0;
            else before = bs->rank1(start-1);

                size_t rank_left = bs->rank1(start+x_start-1);
                size_t rank_right = bs->rank1(start+x_end);
                int liveright = 1;
                int liveleft = 1;

                if (rank_left == rank_right) {
                    liveright = 0;
                }
                if ( (rank_right - rank_left) == (x_end - x_start)){
                    liveleft = 0;
                }


                x_start_left = x_start - (rank_left - before);
                x_end_left = x_end - (rank_right - before);
                end_left = end - (bs->rank1(end) - before);
                start_left = start;
                //cout << "izquierda | x_start_left = " << x_start_left << " x_end_left = " << x_end_left << endl;              

                if (y_start_aux >= min_left && y_start_aux <= max_left) {
                    y_start_left = y_start_aux;
                } else {
                    cout << "retornando 1" << endl;
                    return;
                }

                if (y_end_aux >= min_left && y_end_aux <= max_left) {
                     y_end_left = y_end_aux;
                } else if (y_start_aux >= min_left && y_start_aux <= max_left) {
                     y_start_left = y_start_aux;
                     y_end_left = max_left;
                }
                else {
                    cout << "retornando 2" << endl;
                    return;  
                } 
                cout << "y_start_left = " << y_end_right << endl;
                cout << "y_end_left = " << y_end_right << endl;
                cout << endl;
                if (liveleft)
                    rr2d(lev+1,x_start_left,x_end_left,y_start,y_end,y_start_left,y_end_left,sym,start_left,end_left,result);              
                
                if (y_start_aux >= min_right && y_start_aux <= max_right) {
                  y_start_right = y_start_aux;  
                } else if(y_end_aux >= min_right && y_end_aux <= max_right) {
                  y_start_right = min_right;  
                } else {
                    cout << "retornando en 3 " << endl;
                  return;  
                } 

                if (y_end_aux >= min_right && y_end_aux <= max_right) {
                  y_end_right = y_end_aux;  
                } else {
                    cout << "retornando en 4" << endl;
                  return;  
                } 
                 cout << "y_start_right = " << y_end_right << endl;
                 cout << "y_end_right = " << y_end_right << endl;
                sym=set(sym,lev);
                start_right = end - (bs->rank1(end)-before) + 1;
                end_right = end;
                x_start_right = rank_left - before ;
                x_end_right = rank_right - before-1;   
                //cout << "derecha | x_start_left = " << x_start_right << " x_end_left = " << x_end_right << endl;
                if (liveright)
                    rr2d(lev+1,x_start_right,x_end_right,y_start,y_end,y_start_right,y_end_right,sym,start_right,end_right,result);

        }
        else
        {   
                if (x_start > x_end)
                return;
                BitSequence* bs = bitstring[lev-1];
                // if (am->unmap(sym) <= y_end && am->unmap(sym) >= y_start)
                // {
                //     cout << sym << endl;
                //     uint q_start;
                //     uint q_end;
                //     if (x_start == x_end){
                //         q_start = start+x_start;
                //         q_end = q_start+1;
                //     }else {
                //         q_start = start+x_start;
                //         q_end = start+x_end;
                //     }

                //     int k = 10;
                //     vector<uint> v_results = topk(q_start,q_end,bits(k)+1,r);
                //     for (int i = 0 ; i < v_results.size();i++)
                //     {
                //         result.push_back(v_results[i]);    
                //     }                   
                // }
                // else
                //     return;
        }   

    }


    void WaveletTreeRMQ::range_report(uint lev, size_t x_start, size_t x_end,uint sym, size_t start, size_t end,vector <uint> &result,size_t value)
    {
        size_t x_start_left,x_start_right,x_end_left,x_end_right;
        size_t before;
        size_t end_left;
        size_t end_right;
        size_t start_left;
        size_t start_right;
        // cout << "x_start = " << x_start << endl;
        // cout << "x_end = " << x_end << endl;
        // cout << "start = " << start << endl;
        // cout << "end = " << end << endl; 
        //  cout << "x_start= " <<  x_start << " x_end= " << x_end << endl;
        if (x_start > x_end || x_end > n-1)     
            return;

        if (lev < height)
        {
            BitSequence* bs = bitstring[lev];
            //cout << "LEVEl " << lev << endl;
            //cout << "start -> " << start << " end -> " << end << endl;
            if (start==0) before=0;
            else before = bs->rank1(start-1);

                size_t rank_left = bs->rank1(start+x_start-1);
                size_t rank_right = bs->rank1(start+x_end);
                x_start_left = x_start - (rank_left - before);
                x_end_left = x_end - (rank_right - before);
                end_left = end - (bs->rank1(end) - before);
                start_left = start;
                //cout << "izquierda | x_start_left = " << x_start_left << " x_end_left = " << x_end_left << endl;
                range_report(lev+1,x_start_left,x_end_left,sym,start_left,end_left,result,value);

                sym=set(sym,lev);                       
                start_right = end - (bs->rank1(end)-before) + 1;
                end_right = end;
                x_start_right = rank_left - before ;
                x_end_right = rank_right - before-1;   
                //cout << "derecha | x_start_left = " << x_start_right << " x_end_left = " << x_end_right << endl;
                range_report(lev+1,x_start_right,x_end_right,sym,start_right,end_right,result,value);

        }
        else
        {   
                if (x_start > x_end)
                return;
                //if (am->unmap(sym) > value)
                //    return;

                // BitSequence* bs = bitstring[lev-1];
                // result.push_back(am->unmap(sym));
                // cout << "x_start = " << x_start << endl;
                // cout << "x_end = " << x_end << endl;
                // cout << "start = " << start << endl;
                // cout << "end = " << end << endl; 
                //cout << "length = " << bs->getLength() << endl;
               // cout << "rank1=" << bs->rank0(x_end) << endl;
               //                    cout << "position = " << this->select(am->unmap(sym),bs->rank0(x_end)) + (x_end - x_start) + i << endl;
                    cout << "Adding -> " << am->unmap(sym) << " , "  << endl;
               // }
        }   

    }

    uint WaveletTreeRMQ::access(size_t pos) const
    {
        uint level=0;
        uint ret=0;
        size_t start=0;
        size_t end=n-1;
        while(level<height) {
            assert(pos>=start && pos<=end);
            if(bitstring[level]->access(pos)) {
                ret=set(ret,level);
                pos=bitstring[level]->rank1(pos-1)-bitstring[level]->rank1(start-1);
                start=(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
                start=end-start+1;
                pos+=start;
            }
            else {
                pos=pos-start-(bitstring[level]->rank1(pos)-bitstring[level]->rank1(start-1));
                end=end-start-(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
                end+=start;
                pos+=start;
            }
            level++;
        }
        return am->unmap(ret);
    }

    size_t WaveletTreeRMQ::rank(uint symbol, size_t pos) const
    {
        symbol = am->map(symbol);
        uint level=0;
        size_t start=0;
        size_t end=n-1;
        size_t count=0;
        while(level<height) {
            if(is_set(symbol,level)) {
                pos=bitstring[level]->rank1(pos)-bitstring[level]->rank1(start-1)-1;
                count=pos+1;
                start=(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
                start=end-start+1;
                pos+=start;
            }
            else {
                pos=pos-start+bitstring[level]->rank1(start-1)-bitstring[level]->rank1(pos);
                count=pos+1;
                end=end-start-(bitstring[level]->rank1(end)-bitstring[level]->rank1(start-1));
                end+=start;
                pos+=start;
            }
            level++;
            if(count==0) return 0;
        }
        return count;
    }

    size_t WaveletTreeRMQ::select(uint symbol, size_t j) const
    {
        symbol = am->map(symbol);
        uint mask = (1<<height)-2;
        uint sum=2;
        uint level = height-1;
        size_t pos=j;
        while(true) {
            size_t start = get_start(symbol,mask);
            size_t end = min((size_t)max_v+1,start+sum);
            start = (start==0)?0:(occ->select1(start)+1);
            end = occ->select1(end+1)-1;
            if(is_set(symbol,level)) {
                uint ones_start = bitstring[level]->rank1(start-1);
                pos = bitstring[level]->select1(ones_start+pos)-start+1;
            }
            else {
                uint ones_start = bitstring[level]->rank1(start-1);
                pos = bitstring[level]->select0(start-ones_start+pos)-start+1;
            }
            mask <<=1;
            sum <<=1;
            if(level==0) break;
            level--;
        }
        return pos-1;
    }

    size_t WaveletTreeRMQ::getSizeRMQ()
    {
        size_t bytesRMQ = 0;    
        for(uint i=0;i<height;i++) {
            bytesRMQ += this->rmq[i]->getSize();
        }
        return bytesRMQ;
    }
    size_t WaveletTreeRMQ::getSize() const
    {
        size_t ptrs = sizeof(WaveletTreeRMQ)+height*sizeof(Sequence*);
        size_t bytesBitstrings = 0;
        size_t bytesRMQ = 0;
        
        for(uint i=0;i<height;i++) {
            bytesBitstrings += bitstring[i]->getSize();
            bytesRMQ += this->rmq[i]->getSize();
        }
        return bytesBitstrings+occ->getSize()+ptrs+bytesRMQ;
    }

    void WaveletTreeRMQ::sortWeights(uint *weight) {
        uint i = 0;
        queue<pair<uint, uint> > ranges;
        ranges.push(make_pair(0u, n));

        while (i < height) {
            pair<uint, uint> r = ranges.front();
            ranges.pop();

            uint ini = r.first;
            uint fin = r.second;
            uint w_left = 0;
            uint w_right = 0;

            for (int k = ini ; k < fin ; k++) {
                if (bitstring[i]->access(k) == 0) {
                    w_left++;
                } else {
                    w_right++;
                }
            }

            uint *weight_aux_left = new uint[w_left];
            uint *weight_aux_right = new uint[w_right];
            uint pos_left = 0, pos_right = 0;
            for (int k = ini; k < fin; k++) {
                if (bitstring[i]->access(k)) {
                    weight_aux_right[pos_right++] = weight[k];
                } else {
                    weight_aux_left[pos_left++] = weight[k];
                }
            }

            assert(pos_left == w_left);
            assert(pos_right == w_right);
            assert(pos_left + pos_right == fin - ini);

            for (int k = 0; k < pos_left; k++) 
                weight[k + ini] = weight_aux_left[k];
            for (int k = 0; k < pos_right; k++)
                weight[k + ini + pos_left] = weight_aux_right[k];

            ranges.push(make_pair(ini, ini+pos_left));
            ranges.push(make_pair(ini+pos_left, fin));

            if (fin == n) {
                rmq[i] = new RMQ((int*)weight, n);
                i ++;
            }
        }
    }

    void WaveletTreeRMQ::build_level(uint **bm, uint *symbols, uint level, uint length, uint offset) {
        if(level==height) {
            delete [] symbols;
            return;
        }
        // cout << "constructing rmq on level " << level << endl;
        // cout << "content is :" << endl; 
        // for (int i = 0 ; i < this->n;i++ ) {
        //     cout << " " << weight[i] << " ";
        // }
        // cout << endl;
        uint cleft=0;
        for(size_t i=0;i<length;i++)
            if(!is_set(symbols[i],level))
                cleft++;
        uint cright=length-cleft;
        uint *left=new uint[cleft], *right=new uint[cright];
        
        cleft=cright=0;
        // uint w_use_left = 0;
        // uint w_use_right = 0;
        for(size_t i=0;i<length;i++)
        if(!is_set(symbols[i],level)) {
            left[cleft++]=symbols[i];
            bitclean(bm[level],offset+i);
        }
        else {
            right[cright++]=symbols[i];
            bitset(bm[level],offset+i);
        }
        delete [] symbols;
        symbols = 0;

        build_level(bm,left,level+1,cleft,offset);
        left = 0;                // Gets deleted in recursion.
        build_level(bm,right,level+1,cright,offset+cleft);
        right = 0;               // Gets deleted in recursion.
        //delete [] left;
        //delete [] right;
    }

    // symbols is an array of elements of "width" bits.
    void WaveletTreeRMQ::build_level(uint **bm, uint *symbols, unsigned width, uint level, uint length, uint offset) {
        if(level==height) {
            delete [] symbols;
            return;
        }
        // cout << "constructing rmq on level " << level << endl;
        // this->rmq[level] = new RMQ((int*)weight,length);

        uint cleft=0;
        for(size_t i=0;i<length;i++)
            if(!is_set(get_field(symbols, width, i),level))
                cleft++;
        uint cright=length-cleft;
        uint *left=new uint[(cleft*width)/W + 1],
            *right=new uint[(cright*width)/W + 1];

                
        // uint *wleft = new uint[(cleft*width)+1],
        //     *wright = new uint[(cright*width)+1];
        // uint w_useright = 0;
        // uint w_useleft =0;

        cleft=cright=0;
        for(size_t i=0;i<length;i++)
        if(!is_set(get_field(symbols,width,i),level)) {
            set_field(left,width,cleft++,get_field(symbols, width,i));
            bitclean(bm[level],offset+i);
            // wleft[w_useleft] = weight[offset+i];
            // w_useleft++;
        }
        else {
            set_field(right,width,cright++,get_field(symbols,width,i));
            bitset(bm[level],offset+i);
            // wright[w_useright] = weight[offset+i];
            // w_useright++;

        }

        delete [] symbols;
        symbols = 0;

        build_level(bm,left,width,level+1,cleft,offset);
        left = 0;                // Gets deleted in recursion.
        build_level(bm,right,width,level+1,cright,offset+cleft);
        right = 0;               // Gets deleted in recursion.
        //delete [] left;
        //delete [] right;
    }

    uint WaveletTreeRMQ::max_value(uint *symbols, size_t n) {
        uint max_v = 0;
        for(size_t i=0;i<n;i++)
            max_v = max(symbols[i],max_v);
        return max_v;
    }

    uint WaveletTreeRMQ::max_value(uint *symbols, unsigned width, size_t n) {
        uint max_v = 0;
        for(size_t i=0;i<n;i++)
            max_v = max(get_field(symbols, width, i),max_v);
        return max_v;
    }

    uint WaveletTreeRMQ::bits(uint val) {
        uint ret = 0;
        while(val!=0) {
            ret++;
            val >>= 1;
        }
        return ret ;
    }

    size_t WaveletTreeRMQ::count(uint symbol) const
    {
        return occ->select1(am->map(symbol))-occ->select1(am->map(symbol)-1)+1;
    }

};
