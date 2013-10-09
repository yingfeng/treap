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

#include <sys/time.h>
#include <sys/resource.h>

#include "dfuds.h"

#define rank_OPEN rank_0
#define rank_CLOSE rank_1
#define select_OPEN select_0
#define select_CLOSE select_1

double getTime(void)
 {
    double usertime;
    struct rusage usage;

    getrusage (RUSAGE_SELF, &usage);

    usertime = (double) usage.ru_utime.tv_sec +
               (double) usage.ru_utime.tv_usec / 1000000.0;

    return (usertime);
 }

double t1;

dfuds::dfuds(uint *par, uint n, symbol_T *_label, uint block_size, bool _testing_times)
 {
    uint p, j;
    topology = new bp_hb(par, 2*n, block_size, true);
    Nnodes = n;
    label = _label;
    boost = new treeNode[256];
    
    testing_times = _testing_times;
    select_local_time=0.0; 
    nselects=0;

    for (j=0; j<256; j++) boost[j] = NULLT;

    for (p=ROOT; !bitget1(topology->data, p); p++);
    
    for (p--, j=p; p>=1; p--, j--) 
       boost[label[j]] = topology->close(p)+1;      
 }


dfuds::~dfuds()
 {
    delete topology;
    delete boost;
    delete label;
 }


treeNode dfuds::Root_Node()
 {
    return (treeNode)1;
 }


struct timeval tv1_1,tv2_2;

treeNode dfuds::Parent_Sequential(treeNode x)
 {
    uint p;
		uint *data = topology->data; 
    if (x == Root_Node()) return NULLT; // parent of root
    
    p = topology->open(x-1);    

    if ((p>=W) && (data[p/W]&(~((~0)<<(p%W))))==0) p-=(p%W+1);
		while ((p>=W) && data[p/W]==0)
			p-=32;
		while (p!=0 && !bitget1(data,p)) p--;
	  //p = ((static_bitsequence_brw32 *)topology->bdata)->prev(p);
    
    return (treeNode)p+1;
 }


treeNode dfuds::Parent_Fast(treeNode x)
 {
    uint p, r;
        
    if (x == Root_Node()) return NULLT; // parent of root
    
    p = topology->open(x-1);
    
    r = topology->bdata->rank1(p/*+1*/ /*OJO, el +1 posible fuente de error*/);
    
    return (treeNode)topology->bdata->select1(r)+1;
 }


treeNode dfuds::Child(treeNode x, int i)
 {
    uint d = Degree_Sequential(x);
    if ((uint)i > d) 
       return (treeNode)NULLT; // there is no such child
    else
       return (treeNode)topology->close(x+d-i)+1; 
 }


treeNode dfuds::Level_Ancestor(treeNode x, int delta)  // brute force
 {
    while (delta < 0) {
       x = Parent_Sequential(x);   // aqui cambiar la funcion Parent de acuerdo a lo que se quiera
       delta++;
    }
    return x;
 }


uint dfuds::Depth(treeNode x) // brute force
 {
    treeNode r = Root_Node();
    uint d = 0;
    
    while (x!=r) {
       x = Parent_Sequential(x);
       d++;
    }
    return d;
 }


uint dfuds::Preorder_Rank(treeNode x)
 {
    return topology->bdata->rank1(x-1);  // OJO, posible fuente de error
 }

treeNode dfuds::Preorder_Select(uint p)
 {
    if (p==0) return Root_Node();
    else
       return (treeNode)topology->bdata->select1(p)+1;
 }

uint dfuds::Inspect(uint p)
 {
    if (bitget1(topology->data,p))
       return 1;
    else
       return 0;
 }


bool dfuds::Isleaf(treeNode x)
 {
    if (Inspect(x) == CLOSING && Inspect(x-1) == CLOSING)
       return true;
    else 
       return false;
 }
 

uint dfuds::Subtree_Size(treeNode x)
 {
    uint e, f;
    
    if (Inspect(x)==CLOSING) return 1;
    e = topology->enclose(x);
    f = topology->close(e);
    return (f-x)/2 + 1; 
 }


treeNode dfuds::First_Child(treeNode x)
 {
    if (Isleaf(x)) 
       return NULLT;
    else
       return Child(x, 1);
 }


treeNode dfuds::Next_Sibling(treeNode x)
 {
    uint i;
    
    i = topology->open(x-1)+1;
    
    if (Inspect(i)==OPENING) 
       return (treeNode)NULLT;  // there is no next sibling
    else
       return (treeNode)topology->close(i)+1;
 }


treeNode dfuds::Prev_Sibling(treeNode x)
 {
    uint i;
    
    i = topology->open(x-1)-1;
    
    if (Inspect(i)==OPENING) 
       return (treeNode)NULLT;  // there is no next sibling
    else
       return (treeNode)topology->close(i)+1;
 }
 
 
bool dfuds::Is_Ancestor(treeNode x, treeNode y)
 {
    return (x <= y) && (y <= (treeNode)topology->close(topology->enclose(x))); 
 }
 

uint dfuds::Distance(treeNode x, treeNode y)  // brute force
 {
    treeNode r = Root_Node();
    uint d = 0;
    
    while (y!=r && x!=y) {
       y = Parent_Sequential(y);
       d++;
    }
    return d;
 }


uint dfuds::Degree_Sequential(treeNode x)
 {
    uint iaux = x, *data = topology->data;

    if (testing_times) t1 = getTime();
    
    if ((data[x/W]&((~0)<<(x%32)))==0) x+=32-(x%W);
    while (data[x/W]==0)
       x+=32;
   
    while (!bitget1(data, x))
       x++;
    
    if (testing_times) {
       select_local_time += (getTime()-t1);
       nselects++;
    }
    
    return x-iaux; 
 }


uint dfuds::Degree_Fast(treeNode x)
 {
    return (uint)topology->bdata->select1(topology->bdata->rank1(x) + 1)-x;
 }


uint dfuds::Child_Rank(treeNode x)
{
  uint i;
  
  if (x==Root_Node()) return 1;

  i = topology->open(x);
  return (uint)topology->bdata->select1(topology->bdata->rank1(i-1)+1) - i;
}



// obtains the label of a node. Assumes that the
// labels are stored in reverse order.

symbol_T dfuds::Get_Label(treeNode x)
 {
    uint p;
 
    if (x==Root_Node()) return label[0];
    
    // find the "(" corresponding to node i in the definition of its parent
     p = topology->open(x-1);
    
    // the position of the letter corresponds to the number of "(" up to position p
    return label[topology->bdata->rank0(p)-1]; 
 }


treeNode dfuds::Labeled_Child(treeNode x, symbol_T s)
 {
    treeNode j;
    symbol_T tc=~s; 
    uint r, d, last;
    if (x==Root_Node()) return boost[s];
    
    if (Isleaf(x)) return NULLT; // node is a leaf
    
    r     = topology->bdata->rank0(x-1);
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
    
    if (tc == s) return topology->close(x+(j-r))+1;
    else return NULLT; // no child to go down by s

 }


uint dfuds::size()
 {
    return sizeof(dfuds) +          // size of the class itself
    topology->size() +   // parentheses
    sizeof(symbol_T)*Nnodes +       // labels
    sizeof(treeNode)*256;           // boost
 }

void dfuds::printStatistics()
 {
    if (testing_times) 
       printf("Tiempo promedio select local: %0.10f secs/operation\n", select_local_time/nselects);
 }


