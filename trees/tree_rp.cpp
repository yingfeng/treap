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


#include "tree_rp.h"
		
tree_rp::tree_rp(uint *bitarray, uint ba_size, int block_size){
	length = ba_size;
	topology = new bp_rp(bitarray, ba_size, block_size);
}


tree_rp::tree_rp(uint *bitarray, uint ba_size, int block_size, uchar *labels){
	label = labels;
	length = ba_size;
	topology = new bp_rp(bitarray, ba_size, block_size);
}


tree_rp::~tree_rp(){
	delete topology;
}


uint tree_rp::Root_Node(){
	return 0;
}

uint tree_rp::Parent(uint x){
	return topology->enclose(x);
}

uint tree_rp::Child(uint x, int i){
	uint next = x+1;
	for(int j=1; j<i; j++){
		if(!Inspect(next))
			return (uint)-1;
		next = topology->close(next)+1;
	}
	if(!Inspect(next))
		return (uint)-1;
	return next;
}

/*delta must be a negative number*/
uint tree_rp::Level_Ancestor(uint x, int delta){
	int d = delta;
	uint la = x;
	while(d<0){
		if(la==0) /*i'm in the root*/
			return (uint)-1;
		la = topology->enclose(la);
		d++;
	}
	return la;
}

uint tree_rp::Depth(uint x){
	return topology->excess(x);
}

uint tree_rp::Preorder_Rank(uint x){
	return ((bp_rp *)topology)->S1->rank1(x);
}

uint tree_rp::Preorden_Select(uint x){
	return ((bp_rp *)topology)->S1->select1(x);
}

uint tree_rp::Postorder_Rank(uint x){
	return (uint)-1;
}

uint tree_rp::Inspect(uint p){
	return ((bp_rp *)topology)->S1->access(p);
}

bool tree_rp::Isleaf(uint x){
	if(!Inspect(x+1))
		return true;
	return false;
}

uint tree_rp::Subtree_Size(uint x){
	uint ini, fin;
	ini = x;
	fin = topology->close(x);
	return (fin-ini+1)/2;
}

uint tree_rp::First_Child(uint x){
	if(Inspect(x+1))
		return x+1;
	return (uint)-1;
}

uint tree_rp::Next_Sibling(uint x){
	uint y = topology->close(x);
	if(Inspect(y+1))
		return y+1;
	return (uint)-1;
}

uint tree_rp::Prev_Sibling(uint x){
	if(x==0)
		return (uint)-1;
	if(!Inspect(x-1))
		return topology->open(x-1);
	return (uint)-1;
}

uint tree_rp::Deepest_Node(uint x){
	return (uint)-1;
}

uint tree_rp::Subtree_Height(uint x){
	return (uint)-1;
}

bool tree_rp::Is_Ancestor(uint x, uint y){
	if(x<=y && topology->close(x)>y)
		return true;
	return false;
}

uint tree_rp::Distance(uint x, uint y){
	return (uint)-1;
}

uint tree_rp::Lca(uint x, uint y){
	uint z;
	uint ex_x = topology->excess(x);
	uint ex_y = topology->excess(y);
	if (ex_x > ex_y) { z = x; x = y; y = z; }
	while (!Is_Ancestor(x,y)) 
		x = Parent(x);
	return x;
}

uint tree_rp::Level_Lefthmost(uint d){
	return (uint)-1;
}

uint tree_rp::Level_Rigthmost(uint d){
	return (uint)-1;
}

uint tree_rp::Degree(uint x){
	uint fin = topology->close(x);   
	uint next = x+1;
	uint cont = 0;
	while(next!=fin){
		cont++;
		next = topology->close(next)+1;                  
	}        
	return cont;
}

uint tree_rp::Child_rank(uint x){
	uint cont=0;
	uint prev;
	uint ini;
	if(x==0)
		return (uint)-1;
	ini = topology->enclose(x);
	prev = x-1;
	cont++;
	while(prev != ini){
		prev = topology->open(prev)-1;
		cont++;
	}
	return cont;
}

uchar tree_rp::Get_Label(uint x){
	return label[((bp_rp *)topology)->S1->rank1(x)-1];
}


uint tree_rp::Labeled_Child(uint x, uchar s){
	  uint next = x+1;
		while(((bp_rp *)topology)->S1->access(next)){
			if(Get_Label(next)==s){
				return next;      
			}
			next = topology->close(next)+1;
		}
		return (uint)-1;
}

uint tree_rp::node_rank(uint x){
	  return Preorder_Rank(x);
}

uint tree_rp::node_select(uint x){
	  return Preorden_Select(x);
}


uint tree_rp::size(){
	return sizeof(tree_rp)+topology->size();
}


