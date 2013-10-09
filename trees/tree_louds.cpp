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


#include "tree_louds.h"
#include <sys/time.h>
		
tree_louds::tree_louds(uint *bitarray, uint ba_size, bool med){
	 testing_times = med;
	 select_local_time=0.0;
	 nselects=0;
	length = ba_size;
	bitrank = new static_bitsequence_brw32(bitarray, length, 20);
}

tree_louds::tree_louds(uint *bitarray, uint ba_size, uchar *labels){
	label = labels;
	length = ba_size;
	bitrank = new static_bitsequence_brw32(bitarray, length, 20);
	topology = ((static_bitsequence_brw32 *)bitrank)->data;
}

tree_louds::~tree_louds(){
	delete bitrank;
}

uint tree_louds::Root_Node(){
	return 2;
}

struct timeval tv1_l,tv2_l;
uint tree_louds::Parent(uint x){
	if(x==2)
		return (uint)-1;
	uint se = bitrank->rank1(x -1);
	se =  bitrank->select0(se);

	if (testing_times) gettimeofday(&tv1_l,NULL);

	uint ret = ((static_bitsequence_brw32 *)bitrank)->prev(se)+1;

	if (testing_times) {
		gettimeofday(&tv2_l,NULL);
		select_local_time += (tv2_l.tv_sec-tv1_l.tv_sec)*1000000+(tv2_l.tv_usec-tv1_l.tv_usec);
		nselects++;
	}

	return  ret;
}

void tree_louds::printStatistics(){
	if (testing_times)
		printf("Tiempo promedio select local: %0.10f \n", select_local_time/nselects);
}

uint tree_louds::Child(uint x, int i){
	uint end = ((static_bitsequence_brw32 *)bitrank)->next(x) -1;
	if((x+i-1)>end)
		return (uint)-1;	
	uint ra = bitrank->rank0(x) + i -1;
	return bitrank->select1(ra)+1;
}

/*delta must be a negative number*/
uint tree_louds::Level_Ancestor(uint x, int delta){
	int d = delta;
	uint la = x;
	while(d < 0){
		if(la==2)
			return (uint)-1;
		la = Parent(la);
		d++;
	}
	return la;
}

uint tree_louds::Depth(uint x){
	uint y=x;
	uint d =0;
	while(y!=2){
		d++;
		y = Parent(y);
	}
	return d;
}

uint tree_louds::Preorder_Rank(uint x){
	return (uint)-1;
}

uint tree_louds::Preorden_Select(uint x){
	return (uint)-1;
}

uint tree_louds::Postorder_Rank(uint x){
	return (uint)-1;
}

uint tree_louds::Inspect(uint p){
	return bitrank->access(p);
}
 
bool tree_louds::Isleaf(uint x){
	if(Inspect(x))
		return true;
	return false;
}

uint tree_louds::Subtree_Size(uint x){
	/*
	//if x is a leaf
	if(bitrank->access(x))
		return 1;
	//if not
	uint res, deg, c_i, c_f;
	uint ra_i_1, ra_i_0, ra_f_1;
	uint end = ((static_bitsequence_brw32 *)bitrank)->next(x);
	deg = end +x;
	res = deg;
	//firts get the position of the 1st child
	ra_i_0 = bitrank->rank0(x);
	c_i = bitrank->select1(ra_i_0)+1;
	if(deg==1)
		return res + Subtree_Size(c_i);
	//get the position of the last child of x
	c_f = bitrank->select1(ra_i_0+deg-1)+1;
	
	while(c_i < c_f ){
		ra_i_1 = bitrank->rank1(c_i);
		ra_f_1 = bitrank->rank1(c_f);
		res += ra_f_1 -  ra_i_1;
		//get the first 0 since c_i
		if(bitrank->access(c_i))
			c_i = bitrank->select0(c_i-ra_i_1+2);
		//get the last 0 until c_f
		if(bitrank->access(c_f))
			c_f = bitrank->select0(c_f-ra_f_1+1);
	}
  */
	/*is not implemented yet*/
	return (uint)-1;
}

uint tree_louds::First_Child(uint x){
	if(bitrank->access(x))
		return (uint)-1;
	uint ra = bitrank->rank0(x);
	return bitrank->select1(ra)+1;
}

uint tree_louds::Next_Sibling(uint x){
	if(x==2)
		return (uint)-1;
	uint se = bitrank->rank1(x -1);
	se =  bitrank->select0(se);
	if(bitrank->access(se+1))
		return (uint)-1;
	se = bitrank->rank0(se+1);
	return bitrank->select1(se)+1;
}

uint tree_louds::Prev_Sibling(uint x){
	if(x==2)
		return (uint)-1;    
	uint se = bitrank->rank1(x -1);
	se =  bitrank->select0(se);
	if(bitrank->access(se-1))
		return (uint)-1;
	se = bitrank->rank0(se-1);
	return bitrank->select1(se)+1;
}
 
uint tree_louds::Deepest_Node(uint x){
	return (uint)-1;
}

uint tree_louds::Subtree_Height(uint x){
	return (uint)-1;
}

bool tree_louds::Is_Ancestor(uint x, uint y){
	if(x==2)
		return true;
	if(x > y)
		return false;
	while(x<y){
		y = Parent(y);
	}
	if(x==y)
		return true;
	return  false;
}

uint tree_louds::Distance(uint x, uint y){
	return (uint)-1;
}

uint tree_louds::Lca(uint x, uint y){
	if(x==2 || y==2)
		return 2;
	while (x != y){
		if(x<y)
			y = Parent(y);
		else
			x =  Parent(x);
	}
	return x;
}

uint tree_louds::Level_Lefthmost(uint d){
	return (uint)-1;
}
 
uint tree_louds::Level_Rigthmost(uint d){
	return (uint)-1;
}

uint tree_louds::Degree(uint x){
	return ((static_bitsequence_brw32 *)bitrank)->next(x) - x;
}
 
uint tree_louds::Child_rank(uint x){
	if(x==2)
		return (uint)-1;
	uint y = bitrank->rank1(x-1);
	y = bitrank->select0(y);
	return y  - ((static_bitsequence_brw32 *)bitrank)->prev(y);
}

uint tree_louds::Labeled_Child(uint x, uchar s){
	if(bitget(topology,x))
		return (uint)-1; // node is a leaf
	uint j;
	uchar tc;
	uint r, d, last;
	r = bitrank->rank0(x-1);
	d = ((static_bitsequence_brw32 *)bitrank)->next(x) - x;
	last  = d;
	j =0;
	
	while ( last-j > 5) { 
		uint med = (last+j)/2;
		uchar let=label[med+r];
		if (let == s) 
			return Child(x, med+1);
		if (let > s)
			last = med;
		else 
			j = med;
	}

	// Then, sequential scan if there are only a few children
	while (j < last) { // there is a child here
		tc = label[j+r];
		if (tc == s)
			return Child(x, j+1);
		// is there a child by letter c?
		if (tc > s) 
			break;
		j++;
	}
	return (uint)-1;
}

uint tree_louds::node_rank(uint x){
	return bitrank->rank1(x-1);
}
 
uint tree_louds::node_select(uint x){
	return bitrank->select1(x)+1;
}

uint tree_louds::size(){
	return sizeof(tree_louds)+ bitrank->size();
}


