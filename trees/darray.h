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

#ifndef DARRAY_H
#define DARRAY_H

#define OPT_NO_RANK (1<<30)


#define logR 16
#define R1 (1<<logR)
#define logRR 10
//#define logRR 8
#define RR (1<<logRR)
#define logRRR 7
#define RRR (1<<logRRR)

typedef struct {
  i64 n,m;
  i64 size;
  pb *buf;
#ifdef INDEX64
  i64 *lp;
  i64 *sl;
#else
  dword *lp;
  dword *sl;
#endif
  word *ss;
  dword *p;  // if (n < 2^{32} * L(=2^{10})), dword is enough.

  //dword *rl;
  i64 *rl;
  word *rm;
  byte *rs;

  i64 idx_size;
} darray;

int setbit(pb *B, i64 i,i64 x);
int setbits(pb *B, i64 i, i64 d, i64 x);
int getbit(pb *B, i64 i);
dword getbits(pb *B, i64 i, i64 d);
unsigned int popcount(pb x);

int darray_construct(darray *da, i64 n, pb *buf,int opt);
i64 darray_select(darray *da, i64 i,int f);
i64 darray_rank(darray *da, i64 i);
i64 darray_pat_construct(darray *da, i64 n, pb *buf, i64 k, pb pat, int opt);
i64 darray_pat_select(darray *da, i64 i, pb (*getpat)(pb *));
i64 darray_pat_rank(darray *da, i64 i, pb (*getpat)(pb *));

i64 darray_select_bsearch(darray *da, i64 i, pb (*getpat)(pb *));
#endif
