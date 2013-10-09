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

#ifndef TREE_FF_DFUDS
#define TREE_FF_DFUDS
#include "ff_dfuds.h"

#ifndef uchar
#define uchar unsigned char
#endif


class tree_ff_dfuds{
	private:
		uchar *label;
	  uint length;
	
	public:
		ff_dfuds *topology;
		tree_ff_dfuds(uint *bitarray, uint ba_size, uchar *labels, int opt);		
		~tree_ff_dfuds();
		uint Root_Node();
		uint Parent(uint x);
		uint Child(uint x, int i);
		uint Level_Ancestor(uint x, int delta);
		uint Depth(uint x);
		uint Preorder_Rank(uint x);
		uint Preorden_Select(uint x);
		uint Postorder_Rank(uint x);  /*not implemented*/
		uint Inspect(uint p);
		bool Isleaf(uint x);
		uint Subtree_Size(uint x);
		uint First_Child(uint x);
		uint Next_Sibling(uint x);
		uint Prev_Sibling(uint x);
		uint Deepest_Node(uint x);   /*not implemented*/
		uint Subtree_Height(uint x); /*not implemented*/
		bool Is_Ancestor(uint x, uint y);
		uint Distance(uint x, uint y);
		uint Lca(uint x, uint y);
		uint Level_Lefthmost(uint d);  /*not implemented*/
		uint Level_Rigthmost(uint d);  /*not implemented*/
		uint Degree(uint x);
		uint Child_rank(uint x);
		uchar Get_Label(uint x);
		uint Labeled_Child(uint x, uchar s);
		uint node_rank(uint x);
		uint node_select(uint x);
		uint size();
};

#endif



