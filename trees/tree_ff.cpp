
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

#include "tree_ff.h"
		
tree_ff::tree_ff(uint *bitarray, uint ba_size, int opt){
	length = ba_size;
	topology = new bp_rmm(ba_size, bitarray, opt);
	label = NULL;
}

tree_ff::tree_ff(uint *bitarray, uint ba_size, int opt, uchar *labels){
	length = ba_size;
	label = labels;
	topology = new bp_rmm(ba_size, bitarray, opt);
}

tree_ff::~tree_ff(){
	delete topology;
	if (label) 
		delete [] label;
}

uint tree_ff::Root_Node(){
	return 0;
}

uint tree_ff::Parent(uint x){
	return  parent(&(((bp_rmm *)topology)->b1),x);
}

uint tree_ff::Child(uint x, int i){
	return child(&(((bp_rmm *)topology)->b1), x, i);
}

/*delta es negativo*/
uint tree_ff::Level_Ancestor(uint x, int delta){
	return level_ancestor(&(((bp_rmm *)topology)->b1), x, delta);
}

uint tree_ff::Depth(uint x){
	return depth(&(((bp_rmm *)topology)->b1), x);
}

uint tree_ff::Preorder_Rank(uint x){
	return preorder_rank(&(((bp_rmm *)topology)->b1), x);
}

uint tree_ff::Preorden_Select(uint x){
	return preorder_select(&(((bp_rmm *)topology)->b1), x);
}


uint tree_ff::Postorder_Rank(uint x){
	return  postorder_rank(&(((bp_rmm *)topology)->b1), x);
}

uint tree_ff::Inspect(uint p){
	return inspect(&(((bp_rmm *)topology)->b1), p);
}


bool tree_ff::Isleaf(uint x){
	return isleaf(&(((bp_rmm *)topology)->b1), x);
}

uint tree_ff::Subtree_Size(uint x){
	return subtree_size(&(((bp_rmm *)topology)->b1), x);
}

uint tree_ff::First_Child(uint x){
	return first_child(&(((bp_rmm *)topology)->b1), x);
}

uint tree_ff::Next_Sibling(uint x){
	return next_sibling(&(((bp_rmm *)topology)->b1), x);
}


uint tree_ff::Prev_Sibling(uint x){
	return prev_sibling(&(((bp_rmm *)topology)->b1), x);
}

uint tree_ff::Deepest_Node(uint x){
	return deepest_node(&(((bp_rmm *)topology)->b1), x);
}


uint tree_ff::Subtree_Height(uint x){
	return subtree_height(&(((bp_rmm *)topology)->b1), x);
}

bool tree_ff::Is_Ancestor(uint x, uint y){
	return is_ancestor(&(((bp_rmm *)topology)->b1), x, y);
}


uint tree_ff::Distance(uint x, uint y){
	return  distance(&(((bp_rmm *)topology)->b1), x,y);
}


uint tree_ff::Lca(uint x, uint y){
	return lca(&(((bp_rmm *)topology)->b1), x, y);
}


uint tree_ff::Level_Lefthmost(uint d){
	return level_lefthmost(&(((bp_rmm *)topology)->b1), d);
}

uint tree_ff::Level_Rigthmost(uint d){
	return level_rigthmost(&(((bp_rmm *)topology)->b1), d);
}


uint tree_ff::Degree(uint x){
	return degree(&(((bp_rmm *)topology)->b1), x);
}

uint tree_ff::Child_rank(uint x){
	return child_rank(&(((bp_rmm *)topology)->b1), x);;
}

uchar tree_ff::Get_Label(uint x){
	return label[Preorder_Rank(x)-1];
}

uint tree_ff::Labeled_Child(uint x, uchar s){
	return Labeled_Child_seq(x,s);
}

uint tree_ff::Labeled_Child_seq(uint x, uchar s){
	uint next = x+1;

	while(inspect( &(((bp_rmm *)topology)->b1) , next)== OP ){
		if(Get_Label(next)==s){
			return next;
		}
		next = topology->close(next)+1;
	}
	return (uint)-1;
}

uint tree_ff::Labeled_Child_bin(uint x, uchar s){
	int d, ls, le, m;
	uint y;
	uchar l;
	d = Degree(x);
	ls = 1;
	le = d;
	while ((ls-le+1) > 5) {
		// binary search over the children
		m = (le+ls)/2;
		y = Child(x, m);																					
		l = Get_Label(y);
		if (l==s) 
			return y;
		if (l > s) 
			le = m-1;
		else       
			ls = m+1;
	}
	y = Child(x, ls);
	while ((le-ls+1)>=1) {
		if (Get_Label(y) == s) 
			return y;
		y = Next_Sibling(y);
		ls++;
	}																				
	return (uint)-1; // no child to go down from node x by symbol s	
}

uint tree_ff::node_rank(uint x){
	return  Preorder_Rank(x);
}
   
uint tree_ff::node_select(uint x){
	return  Preorden_Select(x);
}

uint tree_ff::size(){
	// cout << "size tree_ff  = " <<  sizeof(tree_ff)+topology->size() << endl;
	// cout << "size topology =" << topology->size() << endl;
	return sizeof(tree_ff)+topology->size();
}


