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

#include "balanced_parentheses.h"

balanced_parentheses * balanced_parentheses::load(FILE * fp) {
	uint r;
	fread(&r,sizeof(uint),1,fp);
	cout << "Loading "<< r << endl;
	switch(r) {
		case RP: return bp_rp::load(fp);
		case HB: return bp_hb::load(fp);
		case RMM: return bp_rmm::load(fp);
	}				
	return NULL;
}




