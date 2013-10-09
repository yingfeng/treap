/* Copyright (C) 2010, Rodrigo Cánovas, all rights reserved.  
 *
 * This library is free software; you can redistribute it and/or
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

#include <SuffixTree.h>

using namespace std;
using namespace cds_utils;
using namespace cds_static;

SuffixTree * saveLoad(SuffixTree * bs) {
	ofstream ofs("cst.tmp");
	bs->save(ofs);
	ofs.close();
	ifstream ifs("cst.tmp");
	SuffixTree * ret = SuffixTree::load(ifs);
	ifs.close();
	return ret;
}

bool testSuffixTree(SuffixTree *s1){
	/*add any test you want*/
	return true;
}


int main(int argc, char *argv[]){
	char *text=NULL;
	size_t length=0;
	
	if(argc!=2) {
		cout << "Checks if the SuffixTree of the file <arch> is save/load correctly" << endl << endl;
		cout << "usage: " << argv[0] << " <arch>" << endl;
		return 0;
	}
	
	if(loadText(argv[1], &text, &length))
		return 1;
	
	/*create index*/

	SuffixTree *cst;
	
	SuffixTreeY csty(text, length, DAC, CN_NPR, 32); 
	cst	= saveLoad(&csty); 
	if(!testSuffixTree(cst)) {
		cerr << "ERROR TESTING SuffixTreeY" << endl;
		return -1;
	}
	size_t vr,vl,next_vl,next_vr;
	cst->Root(&vl,&vr);
	cout << "[" << vl << ", " << vr << "]" << endl;
	cst->Child(vl, vr, 'c', &next_vl, &next_vr);
	cout << "[" << next_vl << ", " << next_vr << "]" << endl;



	uchar p[] = "tctcactaagata";
	cout << p << endl;
	size_t pl, pr, cl, cr;
	cst->Root(&pl, &pr);
	size_t sa_val;
	size_t i = 0;
	while (p[i]){
		cout << "looking for " << p[i] << " ";
		cst->Child(pl, pr, p[i], &cl, &cr);
		if (cl == (size_t)-1 || cr == (size_t)-1)
			break;
		pl = cl;
		pr = cr;
		i++;
		cout << "[" << pl << ", "<< pr << "]" << endl;
		
		sa_val =  cst->Locate(pr,pr);
		for(uint j=0; j<i; j++)
			cout << text[sa_val+j];
		cout << endl;
		if(pr!=(length-1)){
			sa_val =  cst->Locate(pr+1,pr+1);
			for(uint j=0; j<i; j++)         
				cout << text[sa_val+j];     
			cout << endl;
		}
	}


	sa_val =  cst->Locate(pl,pl);

	for(i=0; i<20; i++)
		cout << text[sa_val+i];
	cout << endl;

	
	sa_val =  cst->Locate(pr,pr);
	for(i=0; i<30; i++)
		cout << text[sa_val+i];
	cout << endl;

	sa_val =  cst->Locate(pr+1,pr+1);
	for(i=0; i<30; i++)
		cout << text[sa_val+i];
	cout << endl;


	delete (SuffixTreeY *)cst;
	cout << "SuffixTree_Y OK\n" << endl;
	
	delete [] text;
	return 0;
}

