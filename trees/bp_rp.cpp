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

#include "bp_rp.h"

bp_rp::bp_rp(){
	S1_size = 0;
	S2_size = 0;
	lvl3_size = 0;
	b_lvl3 = 0;
	b = 0;
	S1 = NULL;
	S2 = NULL;
	P1 = NULL;
	P2 = NULL;
	Table_max_closers = NULL;
	Table_max_opens = NULL;
	Table_exceso = NULL;
	enclose_lvl3 = NULL;
	lvl3 = NULL;
}

bp_rp::bp_rp(uint *bitarray, int ba_size, int block_size){
	/*set bitarray for our implementation*/
	uint *S_aux = new uint[(ba_size+W-1)/W];
	for(int i=0; i< (ba_size+W-1)/W ; i++)
		S_aux[i]=0;
	for(int i=0; i< ba_size;i++){
		if(!bitget1(bitarray,i))
			bitset(S_aux,i);
	}

	uint *P_aux, *S2_aux, *P2_aux;
	S1_size = ba_size;
	b = block_size; 
	// cout << "Block Size: " << b << endl;
	// cout << "Size lvl1: " << S1_size << endl;
	S1 = new static_bitsequence_brw32(S_aux, ba_size, 20);
	/*create P (find the pionners and their answers)*/
	create_P(&P_aux, S_aux, ba_size);
	P1 = new static_bitsequence_rrr02(P_aux, ba_size);
	/*create S2*/
	S2_size = (int)P1->rank1(ba_size-1);
	// cout << "Size lvl2: " << S2_size << endl;
	create_S2(&S2_aux);
	S2 = new static_bitsequence_brw32(S2_aux, S2_size, 20);
	/*create P2*/
	create_P(&P2_aux, S2_aux, S2_size);
	P2 = new static_bitsequence_brw32(P2_aux, S2_size, 20);
	//P2 = new static_bitsequence_rrr02(P2_aux, S2_size);
	/*create last level*/
	lvl3_size = (int)P2->rank1(S2_size-1);
	// cout << "Size lvl3: " << lvl3_size << endl;
	calculate_lvl3();	
	/*create Tables*/
	Table_max_closers = new uint[(bits(8)*(1<<8)+W-1)/W];
	Table_max_opens = new uint[(bits(8)*(1<<8)+W-1)/W];
	Table_exceso = new uint[(bits(16)*(1<<8)+W-1)/W];
	create_tables();
}

void bp_rp::create_P(uint **p, uint *s, int s_size){
	uint *P_aux;
	uint *pairs = (uint *)malloc(sizeof(uint)*s_size); 
	int p_open, p_close=0;
	int last_open, last_close;
	uint pos = 0;
	uint ex;
	uint P_n = (s_size+W-1)/W;
	P_aux =  (uint *)malloc(sizeof(uint)*P_n); 
	/*ini P*/
	for(uint i=0; i<P_n; i++)
		P_aux[i]=0;
	/*the first bracket and the last are pioners*/
	bitset(P_aux,0);
	bitset(P_aux, s_size-1);
	pairs[s_size-1]=0;
	last_open = 0;
	last_close = s_size-1;
	/*for each position v where the bit is 1, search close(v) if is a pioner mark in P*/
	for(int i=2; i <= s_size/2; i++){	
		pos++;
		/*reach the a 1 in s*/
		while(!bitget(s,pos))
			pos++;
		p_open = pos;   /*select the i-th 1 in s*/
		ex = 1;
		/*get close of the i-th 1*/
		for(int j= p_open+1; j < s_size ;j++){
			if(bitget(s,j))
				ex++;
			else{
				ex--;
				if(ex == 0){
					p_close = j;
					pairs[j] = p_open;					
					break;
				}	
			}
		}
		if(p_close/b > p_open/b){  
			if( p_close/b != last_close/b ){
				bitset(P_aux,p_open);
				bitset(P_aux,p_close);
			}
			last_open = p_open;
			last_close = p_close;
		}
	}
	/*now search in the other direction*/
	last_open = s_size;
	last_close = 0;
	p_close = s_size-1;
	/*search for the pioner that close*/
	for(int i=1; i<= s_size/2; i++){
		/*find the next close bracket*/
		for(int j=p_close-1; j > 0; j--){
			if(!bitget(s,j)){
				p_close = j;
				break;
			}
		}
		/*get the open of p_close*/
		p_open = pairs[p_close];
		if( p_close/b != p_open/b ){
			if( p_open/b != last_open/b ){
				bitset(P_aux,p_open);
				bitset(P_aux,p_close);
			}
			last_open = p_open;
			last_close = p_close;
		}
	}
	*p = P_aux;
	delete []pairs;
}

void bp_rp::create_S2(uint **s){
	uint *S_aux;
	int pos;
	uint S_n = (S2_size+W-1)/W;
	S_aux = new uint[S_n];
	for(uint i=0; i < S_n; i++)
		S_aux[i]=0;
	for(uint i=1; i <= S2_size; i++){
		pos = P1->select1(i);
		if(S1->access(pos))
			bitset(S_aux,i-1);
	}
	*s=S_aux;
}


void bp_rp::calculate_lvl3(){
	uint *S3_aux;	
	int pos, pos_aux=0;
	ulong ex;
	uint S3_n = (lvl3_size+W-1)/W;
	S3_aux = new uint[S3_n];
	for(uint i=0; i < S3_n; i++)
		S3_aux[i]=0;
	for(uint i=1; i <= lvl3_size; i++){
		pos = P2->select1(i);
		if(S2->access(pos))
			bitset(S3_aux,i-1);
	}
	b_lvl3 = bits(lvl3_size-1);
	enclose_lvl3 = new uint[(lvl3_size*b_lvl3+W-1)/W];
	lvl3 = new uint[(lvl3_size*b_lvl3+W-1)/W];	
	set_field(lvl3, b_lvl3, 0, lvl3_size-1);
	set_field(lvl3, b_lvl3, lvl3_size-1, 0);

	for(uint i=2; i <= lvl3_size/2; i++){
		pos_aux++;
		while(!bitget(S3_aux,pos_aux))
			pos_aux++;
		pos = pos_aux;
		ex = 1;
		for(uint j = pos+1; j < lvl3_size; j++){
			if(bitget(S3_aux,j))
				ex++;
			else{
				ex--;
				if(ex == 0){
					set_field(lvl3, b_lvl3, pos, j);  
					set_field(lvl3, b_lvl3, j, pos);
					break;
				}
			}
		}
	}	
	/*search for enclose*/
	pos_aux = 0;
	for(uint i=2; i <= lvl3_size/2; i++){
		pos_aux++;
		while(!bitget(S3_aux,pos_aux))
			pos_aux++;
		pos = pos_aux;
		ex = lvl3_size;  //this value is just for be sure that ex will never be lower than 0
		for(uint j=pos+1; j<lvl3_size; j++){
			if(bitget(S3_aux,j))
				ex++;
			else{
				ex--;
				if(ex== lvl3_size-2){
					 set_field(enclose_lvl3, b_lvl3, pos, get_field(lvl3, b_lvl3, j));
					 break;
				}
			}
		}
	}
}

void bp_rp::create_tables(){
	int cont;
	int max_closer_cont;
	int max_open_cont;
	ulong len = bits(8);
	ulong len2 = bits(16);
	for(int i=0; i<(int)(1<<8); i++){
		cont=0;
		max_closer_cont=0;
		max_open_cont=0;
		for(int j=0; j<8;j++){
			if(bitget(&i,j)){
				cont++;
				if(cont > max_open_cont)
					max_open_cont = cont;
			}
			else{
				cont--;
				if((-1*cont) > max_closer_cont)
					max_closer_cont = -1*cont;
			}
		}/*cierra for j*/
		set_field(Table_max_closers, len, i, max_closer_cont);
		set_field(Table_max_opens, len, i, max_open_cont);
		set_field(Table_exceso, len2, i, cont+8);
	}/*cierra for i*/
}

bp_rp::~bp_rp(){
	delete S1;
	delete S2;
	delete P1;
	delete P2;
	delete [] Table_max_closers;
	delete [] Table_max_opens;
	delete [] Table_exceso;
	delete [] enclose_lvl3;
	delete [] lvl3;
}

uint bp_rp::excess(int v){
	return 2*S1->rank1(v) - (v+1);
}

uint bp_rp::close(int v){
	int q, p_prima, res;
	uint exceso = excess(v);
	int block_prima;
	int block =  v/b;  /*bloque al que pertenece v*/
	int near, v_rank;
	/*si esta en el mismo bloque*/
	near = GetCloser(v, block, S1, 0); 
	if(near!=0)
		return near;

	v_rank = P1->rank1(v);
	q = v_rank -1;
	/*calcular el close del ultimo pionero*/
	p_prima = P1->select1(close_prima(q)+1);
	block_prima =  p_prima/b;
	res = GetCloser(block_prima*b-1 ,block_prima, S1, excess(block_prima*b-1)-exceso);
	return res;
}

uint bp_rp::close_prima(int v){
	int q, p_prima, res;
	uint ex_s2;
	uint exceso = 2*S2->rank1(v)-(v+1);
	int block_prima;
	int block =  v/b;  /*bloque al que pertenece v*/
	/*si esta en el mismo bloque*/
	int near, v_rank;
	near = GetCloser(v,block, S2,0);
	if(near!=0)
		return near;
	
	v_rank = P2->rank1(v);
	q = v_rank -1;
	/*recuperar close del ultimo pionero*/
	p_prima = P2->select1((int)(get_field(lvl3, b_lvl3, q)+1));
	block_prima =  p_prima/b;
	uint v_prima = block_prima*b-1;
	ex_s2 =  2*S2->rank1(v_prima) - (v_prima+1);
	res=  GetCloser(v_prima ,block_prima, S2, ex_s2-exceso);
	return res;
}

int bp_rp::GetCloser(int v, int block, static_bitsequence *F, int cont_ini){
	int cont= cont_ini;
	int lim =  b*(block+1);
	int t=8;  /*tamaño tabla  1->no hay tablas*/
	int d;
	static_bitsequence_brw32 *F_aux = (static_bitsequence_brw32 *)F;
	for(int i=v+1; i<lim; i=i+t){
		/*si lo que queda por revizar es menor que el tamaño de la tabla-> secuencial*/
		if((lim-i)<t || t==1){
			for(int j=i; j<lim; j++){
				if(bitget(F_aux->data, j))
					cont++;
				else{
					cont--;
				if(cont==-1)
					return j;
				}
			}/*cierra for j*/
			return 0;
		} /*cierra if t==1*/
		else{
			d = (int)get_var_field(F_aux->data, i, i+t-1);	
			if(cont < (int)get_field(Table_max_closers,bits(t),d)){
				/*aqui esta la respuesta*/
				for(int j=i; j<(i+t); j++){
					if(bitget(F_aux->data,j))
						cont++;
					else{
						cont--;
						if(cont==-1)
							return j;
					}
				}/*cierra for j*/
				return 0;
			}
			else
				cont +=((int)get_field(Table_exceso,bits(2*t),d) -8); 
		}	
	}/*cierra for i*/
	return 0;
}


uint bp_rp::open(int v){
	int block =  v/b;
	int q, p_prima, res = 0;
	uint exceso = excess(v);
	int block_prima;
	int near, v_rank;
	static_bitsequence_brw32 *S_aux = (static_bitsequence_brw32 *)S1;
	/*if is in the same block*/
	near =  GetOpen(v, block, S1, -1); 
	if(near!=-1)
		return near;
	/*find the succ(v,P1)*/
	v_rank = P1->rank1(v);	
	if(P1->access(v))
		q = v_rank -1;
	else
		q = v_rank;
	p_prima = P1->select1(open_prima(q)+1);
	block_prima =  p_prima/b;
	int dif =  excess((block_prima+1)*b) - exceso;
	if(bitget(S_aux->data, (block_prima+1)*b))
		dif= dif-2;
	for(int i=(block_prima+1)*b-1; i >= p_prima; i--){
		if(bitget(S_aux->data,i)){
			dif--;
			if(dif== -1){
				res = i;
				break;
			}
		}
		else
			dif++;
	}
	return res;
}

int bp_rp::open_prima(int v){
	int block =  v/b;
	int q, p_prima, res = 0;
	uint exceso = 2*S2->rank1(v)-(v+1);
	int block_prima;
	int near, v_rank; 
	static_bitsequence_brw32 *S2_aux = (static_bitsequence_brw32 *)S2;
	/*si esta en el mismo bloque*/         
	near =  GetOpen(v, block, S2, -1);
	if(near!=-1)                
		return near;
	/*find the succ(v,P2)*/
	v_rank = P2->rank1(v); 
	if(P2->access(v))
		q = v_rank -1;
	else
		q = v_rank;

	p_prima = P2->select1((int)get_field(lvl3, b_lvl3, q)+1);
	block_prima =  p_prima/b;
	int dif = 2*S2->rank1((block_prima+1)*b) - ((block_prima+1)*b+1) - exceso;
	if(bitget(S2_aux->data,(block_prima+1)*b))
		dif= dif-2;
	for(int i=(block_prima+1)*b-1; i >= p_prima; i--){
		if(bitget(S2_aux->data, i)){
			dif--;
			if(dif== -1){
				res = i;
				break;
			}
		}
		else
			dif++;
	}
	return res;
}

int bp_rp::GetOpen(int v, int block, static_bitsequence *F, int cont_ini){
	int cont= cont_ini;
	int lim =  b*block;
	int t=1; /*tamaño tabla   1->no hay tablas*/
	int d;
	static_bitsequence_brw32 *F_aux = (static_bitsequence_brw32 *)F;
	for(int i=v-1; i>=lim; i=i-t){
		/*si lo que queda por revizar es menor que el tamaño de la tabla-> secuencial*/
		if((i-lim)< t || t==1){
			for(int j=i; j>=lim; j--){
				if(bitget(F_aux->data,j)){
					cont++;
					if(cont == 0)
						return j;
				}
				else
					cont--;
			}/*cierra for j*/
			return -1;
		} /*cierra if t==1*/
		else{
			d = (int)get_var_field(F_aux->data, i, i+t-1);	
			if(cont < (int)get_field(Table_max_closers,bits(t),d)){
				/*aqui esta la respuesta*/
				for(int j=i; j<(i+t); j++){
					if(bitget(F_aux->data,j))
						cont++;
					else{
						cont--;
						if(cont==-1)
							return j;
					}
				}/*cierra for j*/
				return -1;
			}
			else
				cont +=((int)get_field(Table_exceso,bits(2*t),d) -8); 
		}	
	}/*cierra for i*/
	return -1;
}

uint bp_rp::enclose(int v){
	static_bitsequence_brw32 *S_aux = (static_bitsequence_brw32 *)S1;
	if(!bitget(S_aux->data,v)){
		v = open(v);
	}
	if(v == 0)
		return (uint)-1;
	uint exceso = excess(v);
	int block = v/b;
	int near, p, q, p_prima, v_rank;
	/*search if enclose(v) is in the same block of v*/
	near = GetOpen(v, block, S1, -1);
	if(near!=-1)
		return near;
	/*forward search in the block*/
	int cont=0;
	for(uint i=v+1; i< ((v+b)/b)*b; i++){
		if(bitget(S_aux->data,i))
			cont++;
		else{
			cont--;
			if(cont==-2)
				return open(i);
		}
	}

	/*find the succ(p,P)*/
	v_rank = P1->rank1(v);
	if(P1->access(v)){
		p = v;
		q = v_rank -1;
	}
	else{
		p = (int)P1->select1(v_rank+1);
		q = v_rank;
	}
	if(!bitget(S_aux->data,p))
		p_prima = open(p);
	else
		p_prima = P1->select1(enclose_prima(q)+1);	
	uint p_val =b*((p_prima+b)/b) +1;
	uint exceso_aux = excess(p_val);
	int prev_bit = bitget(S_aux->data, p_val);
	for(int i= (int)(p_val-1); i >= p_prima; i--){
		if(bitget(S_aux->data,i)){
			if(prev_bit)
				exceso_aux--;
			else
				exceso_aux++;
			if( exceso_aux == (exceso-1))
				return i;
			prev_bit = 1;
		}
		else{
			if(prev_bit)
				exceso_aux--;
			else
				exceso_aux++;
			prev_bit = 0;
		}
	}
	return 0;
}

int bp_rp::enclose_prima(int v){
	static_bitsequence_brw32 *S_aux = (static_bitsequence_brw32 *)S2;
	if(!bitget(S_aux->data,v)){
		v = open_prima(v);
	}
	if(v == 0)
		return -1;
	uint exceso = 2*S2->rank1(v) - (v+1);  //  exceso(S2,v);
	int block = v/b;
	int near, p, q, p_prima, v_rank;
	/*search if enclose_prima(v) is in the same block of v*/
	near = GetOpen(v, block, S2, -1);
	if(near!=-1)
		return near;		
	int cont=0;
	/*buscar cercano )*/
	for(uint i=v+1; i<((v+b)/b)*b; i++){
		if(bitget(S_aux->data,i))
			cont++;
		else{
			cont--;
			if(cont==-2)
				return open_prima(i);
		}
	}
	/*find the succ(p,P2)*/
	v_rank = P2->rank1(v);
	if(P2->access(v) ){
		p = v;
		q = v_rank -1;
	}
	else{
		p = (int)P2->select1(v_rank+1);
		q = v_rank;
	}

	if(!bitget(S_aux->data,p))
		p_prima = open_prima(p);
	else
		p_prima = P2->select1( get_field(enclose_lvl3, b_lvl3, q) +1);

	uint val_b = ((p_prima+b)/b)*b+1;
	uint exceso_aux =  2*S2->rank1(val_b) - (val_b+1);
	int prev_bit = bitget(S_aux->data,val_b);
	for(int i= val_b-1; i >= p_prima; i--){
		if(bitget(S_aux->data,i)){
			if(prev_bit)
				exceso_aux--;
			else
				exceso_aux++;
			if( exceso_aux == (exceso-1))
				return i;
			prev_bit = 1;
		}
		else{
			if(prev_bit)
				exceso_aux--;
			else
				exceso_aux++;
			prev_bit = 0;
		}
	}
//	cout << "Error" << endl;
	return 0;
}


uint bp_rp::size(){
	uint mem = sizeof(bp_rp);
	mem += S1->size();
	// cout << "S: " << S1->size() << " bytes" << endl;
	mem += P1->size();
	// cout << "P: " << P1->size() << " bytes" << endl;
	mem += S2->size();
	// cout << "S2: " << S2->size() << " bytes" << endl;
	mem += P2->size();
	// cout << "P2: " << P2->size() << " bytes" << endl;
	mem += sizeof(uint)*2*((bits(8)*(1<<8)+W-1)/W);
	mem += sizeof(uint)*((bits(16)*(1<<8)+W-1)/W);
	// cout << "Tables: " <<  sizeof(uint)*(2*((bits(8)*(1<<8)+W-1)/W)+(bits(16)*(1<<8)+W-1)/W) << " bytes" << endl;
	mem += sizeof(uint)*2*((lvl3_size*b_lvl3+W-1)/W);
	// cout << "lvl3: " << sizeof(uint)*2*((lvl3_size*b_lvl3+W-1)/W) << " bytes" << endl;
	return mem;
}

int bp_rp::save(FILE *fp){
	uint wr = RP;
	fwrite(&wr,sizeof(uint),1,fp);
	fwrite(&S1_size,sizeof(uint),1,fp);
	fwrite(&S2_size,sizeof(uint),1,fp);
	fwrite(&lvl3_size,sizeof(uint),1,fp);
	fwrite(&b_lvl3,sizeof(uint),1,fp);
	fwrite(&b,sizeof(uint),1,fp);			 
	if(S1->save(fp) != 0 || P1->save(fp) != 0){
		cout << "Error" << endl;
		return 1;
	}
	if(S2->save(fp) != 0 || P2->save(fp) != 0){               
		cout << "Error" << endl;                   
		return 1;                     
	}
	fwrite(enclose_lvl3,sizeof(uint),((lvl3_size*b_lvl3+W-1)/W),fp);
	fwrite(lvl3,sizeof(uint),((lvl3_size*b_lvl3+W-1)/W),fp);
	return 0;
}

bp_rp * bp_rp::load(FILE *fp){
	bp_rp *new_bp = new bp_rp();
	fread(&new_bp->S1_size,sizeof(uint),1,fp);
	fread(&new_bp->S2_size,sizeof(uint),1,fp);
	fread(&new_bp->lvl3_size,sizeof(uint),1,fp);
	fread(&new_bp->b_lvl3,sizeof(uint),1,fp);
	fread(&new_bp->b,sizeof(uint),1,fp);
	new_bp->S1 = static_bitsequence::load(fp);
	new_bp->P1 = static_bitsequence::load(fp);
	new_bp->S2 = static_bitsequence::load(fp);
	new_bp->P2 = static_bitsequence::load(fp);
	uint tam = (new_bp->lvl3_size*new_bp->b_lvl3+W-1)/W;
	new_bp->enclose_lvl3 = new uint[tam];
	fread(new_bp->enclose_lvl3,sizeof(uint),tam,fp);      
	new_bp->lvl3 = new uint[tam];   
	fread(new_bp->lvl3,sizeof(uint),tam,fp);
	/*create Tables*/
	new_bp->Table_max_closers = new uint[(bits(8)*(1<<8)+W-1)/W];
	new_bp->Table_max_opens = new uint[(bits(8)*(1<<8)+W-1)/W];
	new_bp->Table_exceso = new uint[(bits(16)*(1<<8)+W-1)/W];
	new_bp->create_tables();
	return new_bp;
}

