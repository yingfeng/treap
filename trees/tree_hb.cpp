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

#include "tree_hb.h"
		
tree_hb::tree_hb(uint *bitarray, uint ba_size, int block_size){
	length = ba_size;
	topology =  new bp_hb(bitarray, ba_size, block_size, true);
}

tree_hb::tree_hb(uint *bitarray, uint ba_size, int block_size, uchar *labels){
	label = labels;
	length = ba_size;
	topology =  new bp_hb(bitarray, ba_size, block_size, true);
}

tree_hb::~tree_hb(){
	delete topology;
}

uint tree_hb::Root_Node(){
	return 0;
}

uint tree_hb::Parent(uint x){
	return topology->enclose(x);
}

uint tree_hb::Child(uint x, int i){
	int next = x+1;
	for(int j=1; j<i; j++){
		if(Inspect(next))   /* next == end*/
			return (uint)-1;
		next = topology->close(next)+1;
	}
	if(Inspect(next))   /* next == end*/
		return (uint)-1;
	return next;
}

uint tree_hb::Level_Ancestor(uint x, int delta){
	int d = delta;
	uint la = x;
	while(d<0){
		if(la==0)
			return (uint)-1;
		la = topology->enclose(la);
		d++;
	}
	return la;
}

uint tree_hb::Depth(uint x){
	return topology->excess(x);
}

uint tree_hb::Preorder_Rank(uint x){
	return ((bp_hb *)topology)->bdata->rank0(x);
}

uint tree_hb::Preorden_Select(uint x){
	return ((bp_hb *)topology)->bdata->select0(x);
}

uint tree_hb::Postorder_Rank(uint x){
	return (uint)-1;
}


uint tree_hb::Inspect(uint p){
	return ((bp_hb *)topology)->bdata->access(p);
}
		
bool tree_hb::Isleaf(uint x){
	if(Inspect(x+1))
		return true;
	return false;
}

uint tree_hb::Subtree_Size(uint x){
	uint fin = topology->close(x);
	return (fin-x+1)/2;
}

uint tree_hb::First_Child(uint x){
	if(Inspect(x+1))
		return (uint)-1;
	return x+1;
}

uint tree_hb::Next_Sibling(uint x){
	uint y = topology->close(x);
	if(Inspect(y+1))
		return (uint)-1;
	return y+1;
}


uint tree_hb::Prev_Sibling(uint x){
	if(x==0)
		return (uint)-1;
	if(Inspect(x-1))
		return topology->open(x-1);
	return (uint)-1;
}

uint tree_hb::Deepest_Node(uint x){
	return (uint)-1;
}
	
uint tree_hb::Subtree_Height(uint x){
	return (uint)-1;
}


bool tree_hb::Is_Ancestor(uint x, uint y){
	if(x==0)
		return true;
	if(x<=y && topology->close(x) > y)
		return true;
	return false;
}

uint tree_hb::Distance(uint x, uint y){
	return (uint)-1;
}

uint tree_hb::Lca(uint x, uint y){
	uint ex_x = topology->excess(x);
	uint ex_y = topology->excess(y);
	uint z;
	if (ex_x > ex_y) { 
		z = x; 
		x = y; 
		y = z; 
	}
	while(!Is_Ancestor(x,y)) 
		x = Parent(x);
	return x;
}

uint tree_hb::Level_Lefthmost(uint d){
	return (uint)-1;
}

uint tree_hb::Level_Rigthmost(uint d){
	return (uint)-1;
}

uint tree_hb::Degree(uint x){
	uint next = x+1;           
	uint cont = 0;
	while( !Inspect(next) ){                          
		cont++;
		next = topology->close(next)+1;                                    
	}                
	return cont;
}

uint tree_hb::Child_rank(uint x){
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

uchar tree_hb::Get_Label(uint x){
		return label[((bp_hb *)topology)->bdata->rank0(x)-1];
}

uint tree_hb::Labeled_Child(uint x, uchar s){
	uint next = x+1;
	while(!Inspect(next)){
		if(Get_Label(next)==s)
			return next;
		next = topology->close(next)+1;
	}
	return (uint)-1;
}

uint tree_hb::node_rank(uint x){
	return Preorder_Rank(x);
}
 
uint tree_hb::node_select(uint x){
	return Preorden_Select(x);
}

uint tree_hb::size(){
	return sizeof(tree_hb)+topology->size();
}


