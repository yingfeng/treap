/**
 *    gen_patterns.cpp
 *    Copyright (C) 2011  Francisco Claude F.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fstream>
#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cassert>

using namespace std;

bool valid(vector<int> & offs, int pos, int len) {
  vector<int>::iterator it = lower_bound(offs.begin(),offs.end(),pos);
  it++;
  if(*it - len < pos) return false;
  return true;
}

int main(int argc, char **argv) {
  if(argc!=5) {
    cout << "usage: " << argv[0] << " <wordseq> <offsets> <n> <p>" << endl;
    return 0;
  }

  ifstream ws_fp(argv[1]);
  ifstream of_fp(argv[2]);
  int n = atoi(argv[3]);
  int p = atoi(argv[4]);

  assert(ws_fp.good() && of_fp.good());

  int ws_len;
  ws_fp >> ws_len;
  vector<int> ws;
  for(int i=0;i<ws_len;i++) {
    int v;
    ws_fp >> v;
    ws.push_back(v);
  }
  ws_fp.close();

  vector<int> offsets;
  int tmp;
  while((of_fp>>tmp)) offsets.push_back(tmp);

  for(int i=0;i<n;i++) {
    int pos = rand()%ws_len;
    while(!valid(offsets,pos,p)) pos=rand()%ws_len;
    cerr << p;
    for(int i=0;i<p;i++)
      cerr << " " << ws[pos+i];
    cerr << endl;
  }

  return 0;
}
