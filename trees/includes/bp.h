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

#ifndef BP_H
#define BP_H

#include "typedef.h"
#include "darray.h"
#define OP 1
#define CP 0

#define OPT_MIN 0
#define OPT_MAX 1
#define OPT_LEFT 0
#define OPT_RIGHT 2

#define OPT_LEAF (1<<0)
#define OPT_INORDER (1<<1)
#define OPT_DEGREE (1<<2)
#define OPT_FAST_PREORDER_SELECT (1<<3)
#define OPT_FAST_LEAF_SELECT (1<<4)
#define OPT_FAST_INORDER_SELECT (1<<5)
#define OPT_FAST_POSTORDER_SELECT (1<<6)
#define OPT_DFUDS_LEAF (1<<7)
#define OPT_FAST_DFUDS_LEAF_SELECT (1<<8)
#define OPT_FAST_LCA (1<<9)

#define logSB 9
//#define logSB 2
#define SB (1<<logSB)
#define logMB 14
//#define logMB 3
#define MB (1<<logMB)

#define ETW 8  // width of excess lookup table
#define W1 2    // branching factor

#ifndef min
 #define min(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef max
 #define max(x,y) ((x)>(y)?(x):(y))
#endif


typedef struct {
  i64 n;
  pb *B;
  darray *da;
  sb_type *sm, *sM;
  sb_type *sd;
  mb_type *mm, *mM;
  mb_type *md;

  mb_type *rmq_tbl;

  i64 m_ofs;

  darray *da_leaf;
  darray *da_inorder;
  darray *da_postorder;
  darray *da_dfuds_leaf;
  i64 idx_size;
  int opt;
} bp;

i64 bp_construct(bp *b,i64 n, pb *B, i64 opt);
void printbp(bp *b, i64 s, i64 t);


i64 rank_open(bp *b, i64 s);
i64 rank_close(bp *b, i64 s);
i64 select_open(bp *b, i64 s);
i64 select_close(bp *b, i64 s);


i64 root_node(bp *b);
i64 find_close(bp *b,i64 s);
i64 find_open(bp *b,i64 s);
i64 find_enclose(bp *b,i64 s);
i64 parent(bp *b,i64 s);
i64 level_ancestor(bp *b,i64 s,i64 d);
i64 depth(bp *b, i64 s);
i64 preorder_rank(bp *b,i64 s);
i64 postorder_rank(bp *b,i64 s);
i64 inspect(bp *b, i64 s);
i64 isleaf(bp *b, i64 s);
i64 rmq(bp *b, i64 s, i64 t, i64 opt);
i64 lca(bp *b, i64 s, i64 t);
i64 std_rmq(bp *b, i64 s, i64 t, i64 opt);
i64 fast_rmq(bp *b, i64 s, i64 t, int opt);
i64 subtree_size(bp *b, i64 s);
i64 first_child(bp *b, i64 s);
i64 next_sibling(bp *b, i64 s);
i64 prev_sibling(bp *b, i64 s);
i64 deepest_node(bp *b,i64 s);
i64 subtree_height(bp *b,i64 s);
i64 is_ancestor(bp *b, i64 s, i64 t);
i64 distance(bp *b, i64 s, i64 t);
i64 level_lefthmost(bp *b, i64 d);
i64 level_rigthmost(bp *b, i64 d);
i64 degree(bp *b,i64 s);

// not efficient
i64 naive_degree(bp *b, i64 s);
i64 naive_child(bp *b, i64 s, i64 d);
i64 naive_child_rank(bp *b, i64 t);
i64 naive_rmq(bp *b, i64 s, i64 t,i64 opt);
i64 postorder_select(bp *b,i64 s);
i64 postorder_select_bsearch(bp *b,i64 s);

// using preorder select index
i64 preorder_select(bp *b,i64 s);

// using leaf index
i64 leaf_rank(bp *b,i64 s);
i64 leaf_size(bp *b, i64 s);
i64 leftmost_leaf(bp *b, i64 s);
i64 rightmost_leaf(bp *b, i64 s);

// using leaf select index
i64 leaf_select(bp *b,i64 s);

// using inorder index
i64 inorder_rank(bp *b,i64 s);

// using inorder select index
i64 inorder_select(bp *b,i64 s);

// using degree index
i64 fast_degree(bp *b,i64 s, i64 t, i64 ith);
i64 child_rank(bp *b, i64 t);
i64 child(bp *b, i64 s, i64 d);


i64 blog(i64 x);
pb getpat_preorder(pb *b);
pb getpat_leaf(pb *b);
pb getpat_inorder(pb *b);
pb getpat_postorder(pb *b);


i64 fwd_excess(bp *b,i64 s, i64 rel);
i64 bwd_excess(bp *b,i64 s, i64 rel);

extern i64 *matchtbl,*parenttbl;
void make_naivetbl(pb *B,i64 n);

extern int popCount[1<<ETW];
extern int fwdtbl[(2*ETW+1)*(1<<ETW)];
extern int bwdtbl[(2*ETW+1)*(1<<ETW)];
extern int mintbl_li[1<<ETW], mintbl_lv[1<<ETW];
extern int mintbl_ri[1<<ETW], mintbl_rv[1<<ETW];
extern int maxtbl_li[1<<ETW], maxtbl_lv[1<<ETW];
extern int maxtbl_ri[1<<ETW], maxtbl_rv[1<<ETW];

extern int minmaxtbl_i[4][1<<ETW], minmaxtbl_v[4][1<<ETW];
extern int degtbl[1<<ETW];
extern int degtbl2[(2*ETW+1)*(1<<ETW)];
extern int childtbl[(ETW)*(1<<ETW)];
extern int depthtbl[(2*ETW+1)*(1<<ETW)];
extern int childtbl2[2*ETW+1][ETW][(1<<ETW)];
#endif
