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


#include "tree_dfuds.h"
		
tree_dfuds::tree_dfuds(uint *bitarray, uint ba_size, uchar *labels, uint block_size, bool test){
	topology = new dfuds(bitarray, ba_size/2, labels, block_size, test);
	length = ba_size;
}

tree_dfuds::~tree_dfuds(){
	delete topology;
}

uint tree_dfuds::Root_Node(){
	return topology-> Root_Node();
}

uint tree_dfuds::Parent(uint x){
	return topology->Parent_Sequential(x);
}

uint tree_dfuds::Child(uint x, int i){
	return topology->Child(x,i);
}

/*delta must be a negative number*/
uint tree_dfuds::Level_Ancestor(uint x, int delta){
	return topology->Level_Ancestor(x, delta);
}

uint tree_dfuds::Depth(uint x){
	return topology->Depth(x);
}

uint tree_dfuds::Preorder_Rank(uint x){
	return  topology->Preorder_Rank(x);
}

uint tree_dfuds::Preorden_Select(uint x){
	return topology->Preorder_Select(x);
}

uint tree_dfuds::Postorder_Rank(uint x){
	return (uint)-1;
}
 
uint tree_dfuds::Inspect(uint p){
	return topology->Inspect(p);
}

bool tree_dfuds::Isleaf(uint x){
	return topology->Isleaf(x);
}

uint tree_dfuds::Subtree_Size(uint x){
	return topology->Subtree_Size(x);
}

uint tree_dfuds::First_Child(uint x){
	return topology->First_Child(x);
}

uint tree_dfuds::Next_Sibling(uint x){
	return topology->Next_Sibling(x);
}

uint tree_dfuds::Prev_Sibling(uint x){
	return topology->Prev_Sibling(x);
}

uint tree_dfuds::Deepest_Node(uint x){
	return (uint)-1;
}

uint tree_dfuds::Subtree_Height(uint x){
	return (uint)-1;
}

bool tree_dfuds::Is_Ancestor(uint x, uint y){
	return topology->Is_Ancestor(x, y);
}

uint tree_dfuds::Distance(uint x, uint y){
	return topology->Distance(x, y);
}

uint tree_dfuds::Lca(uint x, uint y){
	uint dx, dy;
	uint r = Root_Node();
	if (x == r || y == r) 
		return r;

	dx = Depth(x);
	dy = Depth(y);
	/*reach the same depth*/
	if (dx < dy) {
		while (dx < dy) {
			y = Parent(y);
			dy--;
		}
	}
	else {
		while (dy < dx) {
			x = Parent(x);
			dx--;
		}   
	}
	/*go up until they reach the same node*/
	while (dx){
		x = Parent(x);
		y = Parent(y);
		dx--;
		if (x == y) 
			break;
	}
	return x; 
}

uint tree_dfuds::Level_Lefthmost(uint d){
	return (uint)-1;
}
 
uint tree_dfuds::Level_Rigthmost(uint d){
	return (uint)-1;
}

uint tree_dfuds::Degree(uint x){
	return topology->Degree_Sequential(x);
}

uint tree_dfuds::Child_rank(uint x){
	return topology->Child_Rank(x);
}

uchar tree_dfuds::Get_Label(uint x){
	return topology->Get_Label(x);
}

uint tree_dfuds::Labeled_Child(uint x, uchar s){
	return topology->Labeled_Child(x,s);
}

uint tree_dfuds::node_rank(uint x){
	  return Preorder_Rank(x);
}

uint tree_dfuds::node_select(uint x){
	  return Preorden_Select(x);
}


uint tree_dfuds::size(){
	/*size of the structure less the size used for store the labels*/
	return topology->size() - sizeof(uchar)*length/2;
}


