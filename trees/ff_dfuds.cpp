/* Copyright (C) 2010, Rodrigo Cánovas, all rights reserved.
 *
 *This library is free software; you can redistribute it and/or
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

#include "ff_dfuds.h"

#define bitget1(e,p) ((e)[(p)/32] & (1<<((p)%32)))

ff_dfuds::ff_dfuds(uint *par, uint n, symbol_T *_label, int opt)
 {
    int j, p;
    
    pb *B_aux = new pb[(2*n+D-1)/D];
    for(uint i=0; i<2*n;i++)
       setbit(B_aux,i,CP);
    for(uint i=0; i<2*n;i++){
       if(!bitget1(par,i))
          setbit(B_aux,i,OP);
    }
    bp_construct(&topology, 2*n, B_aux,opt);
    Nnodes = n;
    label = _label;

    boost = new treeNode[256];
    

    for (j=0; j<256; j++) boost[j] = NULLT;

    for (p=ROOT; inspect(&topology, p) != CP; p++);
    
    for (p--, j=p; p>=1; p--, j--) 
       boost[label[j]] = find_close(&topology,p)+1;
 }


ff_dfuds::~ff_dfuds()
 {
    delete boost;
    delete label;
 }


treeNode ff_dfuds::Root_Node()
 {
    return (treeNode)ROOT;
 }


treeNode ff_dfuds::Parent_Sequential(treeNode x)
 {
    uint p;
    pb *data = topology.B;
    
    if (x == Root_Node()) return NULLT; // parent of root
    
    p = find_open(&topology, x-1);    

    if ((p>=32) && (~(data[p/32])&((~0)<<(32-1-(p%32))))==0)
       p-=(p%32+1);
    
    while ((p>=32) && ~data[p/32]==0)
       p-=32;

    while (p!=0 && getbit(data,p)==OP) p--;
    
    return (treeNode)p+1;
 }



treeNode ff_dfuds::Parent_Fast(treeNode x)
 {
    uint p, r;
        
    if (x == Root_Node()) return NULLT; // parent of root
    
    p = find_open(&topology, x-1);
    
    r = rank_close(&topology, p);
    
    if (r==0) 
       return 1;
    else
       return (treeNode) select_close(&topology, r)+1;
 }


treeNode ff_dfuds::Child(treeNode x, int i)
 {
    uint d = Degree_Sequential(x);
    if ((uint)i > d) 
       return (treeNode)NULLT; // there is no such child
    else
       return (treeNode)find_close(&topology, x+d-i)+1; 
 }


treeNode ff_dfuds::Level_Ancestor(treeNode x, int delta)  // brute force
 {
    while (delta < 0) {
       x = Parent_Sequential(x);   // aqui cambiar la funcion Parent de acuerdo a lo que se quiera
       delta++;
    }
    return x;
 }


uint ff_dfuds::Depth(treeNode x) // brute force
 {
    treeNode r = Root_Node();
    uint d = 0;
    
    while (x!=r) {
       x = Parent_Sequential(x);
       d++;
    }
    return d;
 }


uint ff_dfuds::Preorder_Rank(treeNode x)
 {            
    return  rank_close(&topology,x-1);
 }

treeNode ff_dfuds::Preorder_Select(uint p)
 {
    if (p==0) return Root_Node();
    else
       return (treeNode) select_close(&topology, p)+1;
 }

uint ff_dfuds::Inspect(uint p)
 {
    return inspect(&topology,p);
 }


bool ff_dfuds::Isleaf(treeNode x)
 {
    if (Inspect(x) == CP && Inspect(x-1) == CP)
       return true;
    else 
       return false;
 }
 

uint ff_dfuds::Subtree_Size(treeNode x)
 {
    uint e, f;
    
    if (Inspect(x)==CP) return 1;
    e = find_enclose(&topology, x);
    f = find_close(&topology, e);
    return (f-x)/2 + 1; 
 }


treeNode ff_dfuds::First_Child(treeNode x)
 {
    if (Isleaf(x)) 
       return NULLT;
    else
       return Child(x, 1);
 }


treeNode ff_dfuds::Next_Sibling(treeNode x)
 {
    uint i;
    
    i = find_open(&topology, x-1)+1;
    
    if (Inspect(i)==OP) 
       return (treeNode)NULLT;  // there is no next sibling
    else
       return (treeNode)find_close(&topology, i)+1;
 }


treeNode ff_dfuds::Prev_Sibling(treeNode x)
 {
    uint i;
    
    i = find_open(&topology, x-1)-1;
    
    if (Inspect(i)==OP) 
       return (treeNode)NULLT;  // there is no next sibling
    else
       return (treeNode)find_close(&topology, i)+1;
 }
 
 
bool ff_dfuds::Is_Ancestor(treeNode x, treeNode y)
 {
    return (x <= y) && (y <= (treeNode)find_close(&topology, find_enclose(&topology, x))); 
 }
 

uint ff_dfuds::Distance(treeNode x, treeNode y)  // brute force
 {
    treeNode r = Root_Node();
    uint d = 0;
    
    while (y!=r && x!=y) {
       y = Parent_Fast(y);
       d++;
    }
    return d;
 }


uint ff_dfuds::Degree_Sequential(treeNode x)
 {
    pb iaux = x, *data = topology.B;
    
    if (Isleaf(x)) return 0;
    if ((~data[x/32]& ((~0)>>(x%32)))==0) x+=32-(x%32);
                   
    while (~data[x/32]==0)
       x+=32;
   
    while (getbit(data, x)==OP)
       x++;
    
    return x-iaux; 
 }


uint ff_dfuds::Degree_Fast(treeNode x)
 {
    if (Isleaf(x)) return 0;
    return (uint) select_close(&topology, rank_close(&topology, x)+1)-x;
 }


uint ff_dfuds::Child_Rank(treeNode x)
{
  uint i;
  
  if (x==Root_Node()) return 1;

  i = find_open(&topology, x);
  return (uint)select_close(&topology, rank_close(&topology, i-1)+1)-i;
}



// obtains the label of a node. Assumes that the
// labels are stored in reverse order.

symbol_T ff_dfuds::Get_Label(treeNode x)
 {
    uint p;
 
    if (x==Root_Node()) return label[0];
    
    // find the "(" corresponding to node i in the definition of its parent
     p = find_open(&topology, x-1);
    
    // the position of the letter corresponds to the number of "(" up to position p
    return label[rank_open(&topology, p)-1];
 }


treeNode ff_dfuds::Labeled_Child(treeNode x, symbol_T s)
 {
    treeNode j;
    symbol_T tc=~s; 
    uint r, d, last;
    if (x==Root_Node()) return boost[s];
    
    if (Isleaf(x)) return NULLT; // node is a leaf
    
    r     = rank_open(&topology, x-1);
    d     = Degree_Sequential(x);
    last  = r+d;
    j     = r;
    
    // First, binary search on the children of node x
    while (last-j > 5) {
       uint med = (last+j)/2;
       byte let=label[med];
       if (let == s) break;
       if (let > s) j = med;
       else last = med;
    }
  
    // Then, sequential scan if there are only a few children
    while (j < last) { // there is a child here
       tc = label[j];
       // is there a child by letter c?
       if (tc <= s) break;
       j++;
    }
    
    if (tc == s) return find_close(&topology, x+(j-r))+1;
    else return NULLT; // no child to go down by s

 }


uint ff_dfuds::size()
 {
    return sizeof(ff_dfuds) +          // size of the class itself
    topology.idx_size +   // parentheses
    sizeof(symbol_T)*Nnodes +       // labels
    sizeof(treeNode)*256;           // boost
 }


