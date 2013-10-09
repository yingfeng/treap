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


// Closed hash table that does not store the keys

// needs the maximum number of elements stored to be declared at creation time
// cannot remove elements
// does not store the key, rather, it reports all the collisioning values
// right value under collisions
// CANNOT STORE ZERO in the value

#ifndef HASHINCLUDED
#define HASHINCLUDED

#include "basics.h"

typedef struct shash{ 
	uint size;    // # of table entries
	uint bits;    // bits per entry
	uint *table;    // data
} *hash;

typedef uint handle;

// creates a table to store up to n values with guaranteed load factor.
// vbits = # of bits per entry, ENTRIES CANNOT HAVE VALUE ZERO
hash createHash (uint n, uint vbits, float factor);
  // frees the structure
void destroyHash (hash H);
  // inserts an entry 
void insertHash (hash H, uint key, uint elem);
  // looks for a key, returns first value (zero => no values)
  // writes in pos a handle to get next values
uint searchHash (hash H, uint key, handle *h);
  // gets following values using handle *h, which is rewritten
  // returns next value (zero => no more values)
uint nextHash (hash H, handle *h);

void saveHash(FILE *f, hash H);

hash loadHash(FILE *f);


uint sizeofHash(hash H);
// two large primes found with etc/hash.c

#define PRIME1 ((uint)4294967279u)
#define PRIME2 ((uint)4294967197u)

#endif
