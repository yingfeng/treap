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
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

typedef long int i64;
typedef unsigned long int u64;

typedef dword pb;
#define logD 5
//typedef u64 pb;
//#define logD 6
#define D (1<<logD)


//typedef byte sb_type;
typedef word sb_type;

#ifdef INDEX64
  typedef i64 mb_type;
#else
  typedef dword mb_type;
#endif
