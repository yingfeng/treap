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

#ifndef BP_RMM_H
#define	BP_RMM_H

#include "balanced_parentheses.h"
#include "bp.h"

class bp_rmm: public balanced_parentheses{
	private:
		bp_rmm();
	public:
		bp b1;


		/*n:size of the bitmap
		 * pb == uint  -> B is the bitmap
		 * opt: options for the construction :  OPT_LEAF | 
		 * 																		OPT_DEGREE | 
		 * 																		OPT_INORDER | 
		 * 																		OPT_FAST_PREORDER_SELECT |
		 * 																		OPT_FAST_LEAF_SELECT |
		 * 																		OPT_FAST_INORDER_SELECT |
		 * 																		OPT_FAST_POSTORDER_SELECT
		 * */
		bp_rmm(int n, pb *B, int opt);
		virtual ~bp_rmm();
		virtual uint excess(int v);
		virtual uint close(int v);
		virtual uint open(int v);
		virtual uint enclose(int v);
		virtual uint size();
		virtual int save(FILE *fp);
		static bp_rmm * load(FILE *fp);
};

#endif

