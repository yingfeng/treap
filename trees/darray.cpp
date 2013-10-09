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


#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"
#include "darray.h"

#define PBS (sizeof(pb)*8)
#define logM 5
#define M (1<<logM)
#define logP 8
#define P (1<<logP)
#define logLL 16    // size of word
#define LL (1<<logLL)
#define logLLL 7
//#define logLLL 5
#define LLL (1<<logLLL)
//#define logL 10
//#define logL (logLL-3)
#define logL (logLL-1-5)
#define L (1<<logL)

#ifndef min
 #define min(x,y) ((x)<(y)?(x):(y))
#endif

#define mymalloc(p,n,f) {p = (__typeof(p))malloc((n)*sizeof(*p)); if ((p)==NULL) {printf("not enough memory\n"); exit(1);}}

int setbit(pb *B, i64 i,i64 x)
{
  i64 j,l;

  j = i / D;
  l = i % D;
  if (x==0) B[j] &= (~(1<<(D-1-l)));
  else if (x==1) B[j] |= (1<<(D-1-l));
  else {
    printf("error setbit x=%d\n",(int)x);
    exit(1);
  }
  return x;
}

int setbits(pb *B, i64 i, i64 d, i64 x)
{
  i64 j;

  for (j=0; j<d; j++) {
    setbit(B,i+j,(x>>(d-j-1))&1);
  }
  return x;
}

int getbit(pb *B, i64 i)
{
  i64 j,l;

  //j = i / D;
  //l = i % D;
  j = i >> logD;
  l = i & (D-1);
  return (B[j] >> (D-1-l)) & 1;
}

dword getbits(pb *B, i64 i, i64 d)
{
  dword x;
  i64 j;

  x = 0;
  for (j=0; j<d; j++) {
    x <<= 1;
    x += getbit(B,i+j);
  }
  return x;
}

i64 getpattern(pb *B, i64 i, i64 k, pb pat)
{
  i64 j;
  i64 x;
  x = 1;
  for (j=0; j<k; j++) {
    x &= getbit(B,i+j) ^ (~(pat>>(k-1-j)));
  }
  //printf("getpattern(%d) = %d\n",i,x);
  return x;
}


static const unsigned int popCount[] = {
0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

static int selecttbl[8*256];

unsigned int popcount(pb x)
{
  pb r;
#if 0
  r = x;
  r = r - ((r>>1) & 0x77777777) - ((r>>2) & 0x33333333) - ((r>>3) & 0x11111111);
  r = ((r + (r>>4)) & 0x0f0f0f0f) % 0xff;
#elif 1
  r = x;
  r = ((r & 0xaaaaaaaa)>>1) + (r & 0x55555555);
  r = ((r & 0xcccccccc)>>2) + (r & 0x33333333);
  //r = ((r & 0xf0f0f0f0)>>4) + (r & 0x0f0f0f0f);
  r = ((r>>4) + r) & 0x0f0f0f0f;
  //r = ((r & 0xff00ff00)>>8) + (r & 0x00ff00ff);
  r = (r>>8) + r;
  //r = ((r & 0xffff0000)>>16) + (r & 0x0000ffff);
  r = ((r>>16) + r) & 63;
#else
  r = popCount[x & 0xff];
  x >>= 8;
  r += popCount[x & 0xff];
  x >>= 8;
  r += popCount[x & 0xff];
  x >>= 8;
  r += popCount[x & 0xff];
#endif
  return r;
}

unsigned int popcount8(pb x)
{
  dword r;
#if 1
  r = x;
  r = ((r & 0xaa)>>1) + (r & 0x55);
  r = ((r & 0xcc)>>2) + (r & 0x33);
  r = ((r>>4) + r) & 0x0f;
#else
  r = popCount[x & 0xff];
#endif
  return r;
}

void make_selecttbl(void)
{
  i64 i,x,r;
  pb buf[1];

  for (x = 0; x < 256; x++) {
    setbits(buf,0,8,x);
    for (r=0; r<8; r++) selecttbl[(r<<8)+x] = -1;
    r = 0;
    for (i=0; i<8; i++) {
      if (getbit(buf,i)) {
	selecttbl[(r<<8)+x] = i;
	r++;
      }
    }
  }
}


int darray_construct(darray *da, i64 n, pb *buf, int opt)
{
  i64 i,j,k,m;
  i64 nl;
  i64 p,pp;
  i64 il,is,ml,ms;
  i64 r,m2;
  i64 p1,p2,p3,p4,s1,s2,s3,s4;

  da->idx_size = 0;

  make_selecttbl();


  if (L/LLL == 0) {
    printf("ERROR: L=%d LLL=%d\n",L,LLL);
    exit(1);
  }

  m = 0;
  for (i=0; i<n; i++) m += getbit(buf,i);
  da->n = n;
  da->m = m;
  //printf("n=%d m=%d\n",n,m);

  da->buf = buf;

  if (opt & (~OPT_NO_RANK)) {  // construct select table
#if 0
    mymalloc(s,m,0);
    m = 0;
    for (i=0; i<n; i++) {
      if (getbit(buf,i)) {
	m++;
	s[m-1] = i;
      }
    }
#endif    
    nl = (m-1) / L + 1;
    mymalloc(da->lp,nl+1,1);  da->idx_size += (nl+1)*sizeof(*da->lp);
    mymalloc(da->p,nl+1,1);  da->idx_size += (nl+1)*sizeof(*da->p);
#if 0
    printf("lp table: %d bytes (%1.2f bpc)\n",(nl+1)*sizeof(*da->lp), (double)(nl+1)*sizeof(*da->lp) * 8/n);
    printf("p table: %d bytes (%1.2f bpc)\n",(nl+1)*sizeof(*da->p), (double)(nl+1)*sizeof(*da->p) * 8/n);
#endif    

    for (r = 0; r < 2; r++) {
      s1 = s2 = s3 = s4 = 0;
      p1 = p2 = p3 = p4 = -1;
    
      ml = ms = 0;
      for (il = 0; il < nl; il++) {
	//pp = s[il*L];
	while (s1 <= il*L) {
	  if (getbit(buf,p1+1)) s1++;
	  p1++;
	}
	pp = p1;
	da->lp[il] = pp;
	i = min((il+1)*L-1,m-1);
	//p = s[i];
	while (s2 <= i) {
	  if (getbit(buf,p2+1)) s2++;
	  p2++;
	}
	p = p2;
	//printf("%d ",p-pp);
	if (p - pp >= LL) {
	  if (r == 1) {
	    for (is = 0; is < L; is++) {
	      if (il*L+is >= m) break;
	      //da->sl[ml*L+is] = s[il*L+is];
	      while (s3 <= il*L+is) {
		if (getbit(buf,p3+1)) s3++;
		p3++;
	      }
	      da->sl[ml*L+is] = p3;
	    }
	  }
	  da->p[il] = -(ml+1);
	  ml++;
	} else {
	  if (r == 1) {
	    for (is = 0; is < L/LLL; is++) {
	      if (il*L+is*LLL >= m) break;
	      while (s4 <= il*L+is*LLL) {
		if (getbit(buf,p4+1)) s4++;
		p4++;
	      }
	      //da->ss[ms*(L/LLL)+is] = s[il*L+is*LLL] - pp;
	      da->ss[ms*(L/LLL)+is] = (word)(p4 - pp);
	    }
	  }
	  da->p[il] = ms;
	  ms++;
	}
      }
      if (r == 0) {
	mymalloc(da->sl,ml*L+1,1);  da->idx_size += (ml*L+1)*sizeof(*da->sl);
	mymalloc(da->ss,ms*(L/LLL)+1,1);  da->idx_size += (ms*(L/LLL)+1)*sizeof(*da->ss);
#if 0
	printf("sl table: %d bytes (%1.2f bpc)\n",(ml*L+1)*sizeof(*da->sl), (double)(ml*L+1)*sizeof(*da->sl) * 8/n);
	printf("ss table: %d bytes (%1.2f bpc)\n",(ms*(L/LLL)+1)*sizeof(*da->ss), (double)(ms*(L/LLL)+1)*sizeof(*da->ss) * 8/n);
#endif
      }
    }
    //free(s);
  } else { // no select table
    da->lp = NULL;
    da->p = NULL;
    da->sl = NULL;
    da->ss = NULL;
  }

  // construct rank table

  if ((opt & OPT_NO_RANK) == 0) {
    mymalloc(da->rl,n/R1+2,1);  da->idx_size += (n/R1+2)*sizeof(*da->rl);
    mymalloc(da->rm,n/RR+2,1);  da->idx_size += (n/RR+2)*sizeof(*da->rm);
    mymalloc(da->rs,n/RRR+2,1);  da->idx_size += (n/RRR+2)*sizeof(*da->rs);
#if 0
    printf("rl table: %d bytes (%1.2f bpc)\n",(n/R1+2)*sizeof(*da->rl), (double)(n/R1+2)*sizeof(*da->rl) * 8/n);
    printf("rm table: %d bytes (%1.2f bpc)\n",(n/RR+2)*sizeof(*da->rm), (double)(n/RR+2)*sizeof(*da->rm) * 8/n);
    printf("rs table: %d bytes (%1.2f bpc)\n",(n/RRR+2)*sizeof(*da->rs), (double)(n/RRR+2)*sizeof(*da->rs) * 8/n);
#endif
    r = 0;
    for (k=0; k<=n+R1; k+=R1) {
      da->rl[k/R1] = r;
      m2 = 0;
      for (i=0; i<R1; i+=RR) {
	if (k+i <= n) da->rm[(k+i)/RR] = (word)m2;
	m = 0;
	for (j=0; j<RR; j++) {
	  if (k+i+j < n && getbit(buf,k+i+j)==1) m++;
	  if (j % RRR == RRR-1) {
	    if (k+i+j <= n) da->rs[(k+i+j)/RRR] = (byte)m;
	    m2 += m;
	    m = 0;
	  }
	}
	if (m != 0) {
	  printf("???\n");
	}
	//m2 += m;
      }
      r += m2;
    }
  }

  return 0;
}

i64 darray_rank0(darray *da, i64 i)
{
  i64 r,j;
  pb *p;

#if (RRR == D*2)
  r = da->rl[i>>logR] + da->rm[i>>logRR] + da->rs[i>>logRRR];
  p = da->buf + ((i>>logRRR)<<(logRRR-logD));
  j = i & (RRR-1);
  if (j < D) r += popcount(*p >> (D-1-j));
  else r += popcount(*p) + popcount(p[1] >> (D-1-(j-D)));
#else

  j = i & (RRR-1);
  if (j < RRR/2) {
    r = da->rl[i>>logR] + da->rm[i>>logRR] + da->rs[i>>logRRR];
    p = da->buf + ((i>>logRRR)<<(logRRR-logD));
    while (j >= D) {
      r += popcount(*p++);
      j -= D;
    }
    r += popcount(*p >> (D-1-j));
  } else {
    j = RRR-1 - (i & (RRR-1));
    i += j+1;
    r = da->rl[i>>logR] + da->rm[i>>logRR] + da->rs[i>>logRRR];
    p = da->buf + ((i>>logRRR)<<(logRRR-logD));
    while (j >= D) {
      r -= popcount(*--p);
      j -= D;
    }
    if (j > 0) r -= popcount(*--p << (D-j));
  }

#endif

  return r;
}

i64 darray_rank(darray *da, i64 i)
{
  i64 r,j;
  pb *p;

  r = da->rl[i>>logR] + da->rm[i>>logRR];
  j = (i>>logRRR) & (RR/RRR-1);
  while (j > 0) {
    r += da->rs[((i>>logRR)<<(logRR-logRRR))+j-1];
    j--;
  }

  p = da->buf + ((i>>logRRR)<<(logRRR-logD));
  j = i & (RRR-1);
  while (j >= D) {
    r += popcount(*p++);
    j -= D;
  }
  r += popcount(*p >> (D-1-j));

  return r;
}

i64 darray_select_bsearch(darray *da, i64 i, pb (*getpat)(pb *))
{
  i64 l,r,m,n;
  i64 t,x,rr;
  pb *p,w;

  // for debug
  //s = darray_select(da,i,1);
  //
  //printf("select(%d)=%d\n",i,s);



  if (i == 0) return -1;

  //printf("i=%d da->m=%d\n",i,da->m);
  if (i > da->m) {
    return -1;
  }

  i--;



  //n = da->m;
  n = da->n;

  t = i;

  l = 0;  r = (n-1)>>logR;
  // find the smallest index x s.t. rl[x] >= t
  while (l < r) {
    m = (l+r)/2;
    //printf("[%d,%d] m=%d rl[m+1]=%d t=%d\n",l,r,m,da->rl[m+1],t);
    if ((i64)da->rl[m+1] > t) { // m+1 is out of range
      r = m;  // new r = m >= l
    } else {
      l = m+1; // new l = m+1 <= r
    }
  }
  x = l;
  t -= da->rl[x];

  x <<= logR;

  l = x >> logRR;  r = (min(x+R1-1,n))>>logRR;
  while (l < r) {
    m = (l+r)/2;
    //printf("[%d,%d] m=%d rm[m+1]=%d t=%d\n",l,r,m,da->rm[m+1],t);
    if (da->rm[m+1] > t) { // m+1 is out of range
      r = m;
    } else {
      l = m+1; // new l = m+1 <= r
    }
  }
  x = l;
  t -= da->rm[x];

  x <<= logRR;

#if 0
  l = x >> logRRR;  r = (min(x+RR-1,n))>>logRRR;
  while (l < r) {
    m = (l+r)/2;
    //printf("m=%d rs[m+1]=%d t=%d\n",m,da->rs[m+1],t);
    if (da->rs[m+1] > t) { // m+1 is out of range
      r = m;
    } else {
      l = m+1; // new l = m+1 <= r
    }
  }
  x = l;
  t -= da->rs[x];
#else
  l = x >> logRRR;
  while (t > da->rs[l]) {
    //printf("l=%d rs[l]=%d t=%d\n",l,da->rs[l],t);
    t -= da->rs[l];
    l++;
  }
  x = l;
#endif

  x <<= logRRR;

  p = &da->buf[x >> logD];
  while (1) {
    //printf("x=%d m=%d t=%d\n",x,m,t);
    m = popcount(getpat(p));
    if (m > t) break;
    t -= m;
    x += D;
    p++;
  }

  w = getpat(p);
  while (1) {
    rr = popCount[w >> (D-8)];
    if (rr > t) break;
    t -= rr;
    x += 8;
    w <<= 8;
  }
  x += selecttbl[((t-0)<<8)+(w>>(D-8))];

#if 0
  if (x != s) {
    printf("error x=%d s=%d\n",x,s);
  }
#endif
  return x;
}

i64 darray_pat_rank(darray *da, i64 i, pb (*getpat)(pb *))
{
  i64 r,j;
  pb *p;

  r = da->rl[i>>logR] + da->rm[i>>logRR];
  j = (i>>logRRR) & (RR/RRR-1);
  while (j > 0) {
    r += da->rs[((i>>logRR)<<(logRR-logRRR))+j-1];
    j--;
  }

  p = da->buf + ((i>>logRRR)<<(logRRR-logD));
  j = i & (RRR-1);
  while (j >= D) {
    r += popcount(getpat(p));
    p++;
    j -= D;
  }
  r += popcount(getpat(p) >> (D-1-j));

  return r;
}


i64 darray_select(darray *da, i64 i,int f)
{
  i64 p,r;
  i64 il;
  i64 rr;
  pb x;
  pb *q;

  if (i == 0) return -1;

  if (i > da->m) {
    return -1;
    //printf("ERROR: m=%d i=%d\n",da->m,i);
    //exit(1);
  }

  i--;

  il = da->p[i>>logL];
  if (il < 0) {
    il = -il-1;
    p = da->sl[(il<<logL)+(i & (L-1))];
  } else {
    p = da->lp[i>>logL];
    p += da->ss[(il<<(logL-logLLL))+(i & (L-1))/LLL];
    r = i - (i & (LLL-1));

    q = &(da->buf[p>>logD]);

    if (f == 1) {
      rr = p & (D-1);
      r -= popcount(*q >> (D-1-rr));
      p = p - rr;
      
      while (1) {
	rr = popcount(*q);
	if (r + rr >= i) break;
	r += rr;
	p += D;
	q++;
      }
      
      x = *q;
      while (1) {
	//rr = popcount(x >> (D-8));
	rr = popCount[x >> (D-8)];
	//rr = popcount8(x >> (D-8));
	if (r + rr >= i) break;
	r += rr;
	p += 8;
	x <<= 8;
      }
      p += selecttbl[((i-r-1)<<8)+(x>>(D-8))];
    } else {
      rr = p & (D-1);
      r -= popcount((~(*q))  >> (D-1-rr));
      p = p - rr;
      
      while (1) {
	rr = popcount(~(*q));
	if (r + rr >= i) break;
	r += rr;
	p += D;
	q++;
      }
      
      x = ~(*q);

      while (1) {
	//rr = popcount(x >> (D-8));
	rr = popCount[x >> (D-8)];
	//rr = popcount8(x >> (D-8));
	if (r + rr >= i) break;
	r += rr;
	p += 8;
	x <<= 8;
      }
      p += selecttbl[((i-r-1)<<8)+(x>>(D-8))];
    }
  }
  return p;
}

i64 darray_pat_select(darray *da, i64 i, pb (*getpat)(pb *))
{
  i64 p,r;
  i64 il;
  i64 rr;
  pb x;
  pb *q;

  if (i == 0) return -1;

  if (i > da->m) {
    return -1;
    //printf("ERROR: m=%d i=%d\n",da->m,i);
    //exit(1);
  }

  i--;

  il = da->p[i>>logL];
  if (il < 0) {
    il = -il-1;
    p = da->sl[(il<<logL)+(i & (L-1))];
  } else {
    p = da->lp[i>>logL];
    p += da->ss[(il<<(logL-logLLL))+(i & (L-1))/LLL];
    r = i - (i & (LLL-1));

    q = &(da->buf[p>>logD]);

    rr = p & (D-1);
    r -= popcount(getpat(q) >> (D-1-rr));
    p = p - rr;
    
    while (1) {
      rr = popcount(getpat(q));
      if (r + rr >= i) break;
      r += rr;
      p += D;
      q++;
    }
    
    x = getpat(q);
    while (1) {
      //rr = popcount(x >> (D-8));
      rr = popCount[x >> (D-8)];
      //rr = popcount8(x >> (D-8));
      if (r + rr >= i) break;
      r += rr;
      p += 8;
      x <<= 8;
    }
    p += selecttbl[((i-r-1)<<8)+(x>>(D-8))];
  }
  return p;
}

i64 darray_pat_construct(darray *da, i64 n, pb *buf, i64 k, pb pat, int opt)
{
  i64 i;
  pb *b;
  mymalloc(b,(n+D-1)/D,0);

  for (i=0; i<n-k+1; i++) {
    setbit(b,i,getpattern(buf,i,k,pat));
  }
  for (i=n-k+1; i<n; i++) {
    setbit(b,i,0);
  }

  darray_construct(da,n,b,opt);
  da->buf = buf;

  free(b);
  
  return 0;
}
