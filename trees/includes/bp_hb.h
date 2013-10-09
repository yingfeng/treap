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

#ifndef BP_HB_H
#define	BP_HB_H

/*Gonzalo Navarro implementation for
 * balanced parentheses.
 *
 * note that a open bracket is represented with a 0
 * and a close bracket with a 1
 * */
#include "balanced_parentheses.h"
#include "hash.h"

class bp_hb: public balanced_parentheses{
	private:
		uint n;            							// # of parentheses
		uint sbits;        							// bits for near pointers
		uint b;            							// bits for block
		hash sftable;      // table of far forward pointers
		hash sbtable;      // table of far backward pointers
		hash bftable;      // table of near forward pointers
		hash bbtable;      // table of near backward pointers
		hash open_sbtable; // table of far backward pointers (for findopen)
		hash open_bbtable; // table of near backward pointers (for findopen)
		
		byte FwdPos[256][W/2];
		byte BwdPos[256][W/2];
		char Excess[256];

		bp_hb();
		uint calcsizes(uint posparent, uint posopen, uint *near, uint *far, uint *pnear, uint *pfar);
		uint filltables(uint posparent, uint posopen, bool bwd);
		void fcompchar (byte x, byte *pos, char *excess);
		void bcompchar (byte x, byte *pos);

	public:
		uint *data;                     // string
		static_bitsequence *bdata;      // bitmap of string

		/* creates a parentheses structure from a bitstring, which gets owned
		 length is the total number of parentheses, opening + closing
		 bwd says if you will want to perform findopen and enclose
		*/
		bp_hb(uint *string, uint length, uint block_size, bool bwd);
		virtual ~bp_hb();
		virtual uint excess(int v);
		virtual uint close(int v);
		virtual uint open(int v);
		virtual uint parent(int v);
		virtual uint enclose(int v);
		virtual uint size();
		virtual int save(FILE *fp);
		static bp_hb * load(FILE *fp);
};

#endif

 /* I have decided not to implement Munro et al.'s scheme, as it is too
	* complicated and the overhead is not so small in practice. I have opted 
	* for a simpler scheme. Each open (closing) parenthesis will be able to
	* find its matching closing (open) parenthesis. If the distance is shorter
	* than b, we will do it by hand, traversing the string. Otherwise, the
	* answer will be stored in a hash table. In fact, only subtrees larger than 
	* s will have the full distance stored, while those between b and s will
	* be in another table with just log s bits. The traversal by hand proceeds
	* in fact by chunks of k bits, whose answers are precomputed.
	* Space: there cannot be more than n/s subtrees larger than s, idem b.
	* So we have (n/s)log n bits for far pointers and (n/b)log s for near 
	* pointers. The space for the table is 2^k*k*log b. The optimum s=b log n,
	* in which case the space is n/b(1 + log b + log log n) + 2^k*k*log b.
	* Time: the time is O(b/k), and we want to keep it O(log log n), so
	* k = b/log log n.
	* (previous arguments hold if there are no unary nodes, but we hope that 
	* there are not too many -- in revtrie we compress unary paths except when
	* they have id)
	* Settings: using b = log n, we have 
	* space = n log log n / log n + 2^(log n / log log n) log n
	* time = log log n
	* In practice: we use k = 8 (bytes table), b = W (so work = 4 or 8)
	* and space ~= n/3 + 10 Kbytes (fixed table). 
	* Notice that we need a hash table that stores only the deltas and does not
	* store the keys! (they would take log n instead of log s!). Collisions are
	* resolved as follows: see all the deltas that could be and pick the smallest
	* one whose excess is the same of the argument. To make this low we use a
	* load factor of 2.0, so it is 2n/3 after all.
	* We need the same for the reverses, for the forward is only for ('s and
	* reverses for )'s, so the proportion stays the same.
	* We also need the stream to be a bitmap to know how many open parentheses
	* we have to the left. The operations are as follows:
	* findclose: use the mechanism described above
	* findparent: similar, in reverse, looking for the current excess - 1
	* this needs us to store the (near/far) parent of each node, which may
	* cost more than the next sibling.
	* excess: using the number of open parentheses
	* enclose: almost findparent
	*/                                                                           	        

