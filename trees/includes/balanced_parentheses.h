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

#ifndef BALANCED_PARENTHESES_H
#define	BALANCED_PARENTHESES_H

#include "basics.h"
#include "static_bitsequence.h"

#define RP 2
#define HB 3
#define RMM 4

class balanced_parentheses{
	public:
		virtual ~balanced_parentheses() {};
		virtual uint excess(int v) = 0;
		virtual uint close(int v) = 0;
		virtual uint open(int v) = 0;
		virtual uint enclose(int v) = 0;
		virtual uint size() = 0;
		virtual int save(FILE *fp) = 0;
		static balanced_parentheses * load(FILE *fp);
};

#include "bp_rp.h"
#include "bp_hb.h"
#include "bp_rmm.h"

#endif

