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

#include "bp_rmm.h"

bp_rmm::bp_rmm(){

}

bp_rmm::bp_rmm(int n, pb *B, int opt){
	pb *B_aux = new pb[(n+D-1)/D];
	for(int i=0; i<n;i++)
		setbit(B_aux,i,CP);
	for(int i=0; i<n;i++){
		if(!bitget1(B,i))
			setbit(B_aux,i,OP);
	}
	bp_construct(&b1, n, B_aux,opt);
	// delete []B_aux;
}

bp_rmm::~bp_rmm(){
	/*destructor???*/
	// delete b1;
	// typedef struct {
	//   i64 n;
	//   pb *B;
	// free(b1.B);
	//   darray *da;
	
	//   sb_type *sm, *sM;
	//   sb_type *sd;
	//   mb_type *mm, *mM;
	//   mb_type *md;

	//   mb_type *rmq_tbl;

	//   i64 m_ofs;

	//   darray *da_leaf;
	//   darray *da_inorder;
	//   darray *da_postorder;
	//   darray *da_dfuds_leaf;
	//   i64 idx_size;
	//   int opt;
	// } bp;

}

uint bp_rmm::excess(int v){
	return 2*rank_open(&b1,v) - (v+1);
}

uint bp_rmm::close(int v){
	return find_close(&b1, v);
}

uint bp_rmm::open(int v){
	return find_open(&b1, v);
}

uint bp_rmm::enclose(int v){
	return find_enclose(&b1,v);
}

uint bp_rmm::size(){
	uint mem = b1.idx_size;
	//cout << "mem2 = " << mem << endl;
	return mem;
}

int bp_rmm::save(FILE *fp){
	return 0;
}

bp_rmm * bp_rmm::load(FILE *fp){
	return NULL;
}

