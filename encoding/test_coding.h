/*
 * test_coding.h
 *
 *  Created on: 2008-7-21
 *  Author: jhe
 */

#ifndef TEST_CODING_H_
#define TEST_CODING_H_
#include "coding.h"
#include "inputstream.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DIR
#define DIR "pfor2"
#endif

#include "metadata.h"
#include "list_reader.h"
#include <time.h>

#include <vector>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>	
#include <regex.h>
#include <fcntl.h>
#include <strings.h>


using namespace std;
#define MAXDOC 26000000

unsigned* doc_buf = new unsigned[128];
unsigned* freq_buf = new unsigned[128];

int i = 0;

int cnum[13] = {2,3,4,5,6,7,8,9,10,12,16,20,32};


int offsets;
unsigned bs, pc,coder, bc;
metadata* md ;
float* pre_doc;

float diff_clock(clock_t* s, clock_t* e)
{
        return (1.0 * (*e - *s) / CLOCKS_PER_SEC);
}

unsigned* data;
metadata* meta;

int init(int type)
{
	data = new unsigned[1000*1024*1024];
	meta = new metadata[100*1024*1024];
	pre_doc = new float[26000000];

	char fn[256];
	sprintf(fn, "index/main%d", type);
	
	FILE* datas = fopen(fn, "r");
	fread(data,1000*1024*1024, sizeof(unsigned),  datas);
	fclose(datas);
	printf("finish!\n loading meta...");
	
	sprintf(fn, "index/meta%d", type);
	FILE* file2 = fopen(fn, "r");
		
	fread(meta,100*1024*1024, sizeof(metadata), file2);

	fclose(file2);

	FILE* file3 = fopen("index/pre", "r");
	fread(pre_doc, 26000000, sizeof(float), file3);
	printf("finish!\n");
}

int compare(const void* e1, const void* e2)
{
	log_entity* a = (log_entity*)e1;
	log_entity* b = (log_entity*)e2;
	
	if ( a->posting_count < b->posting_count)
		return -1;

	else if ( a->posting_count > b->posting_count)
		return 1;

	return 0;
}

float test_reader(long* offset, long* offset2, log_entity* entities, int* lengths, int num, const char* fn, int line, unsigned** doc, int* ndocs, int type, int* cpy_count)
{

	char fns[256], fns2[256];
	//char fns3[256];
	sprintf(fns, "/data/c3/jhe/test/re/%d", line);
	sprintf(fns2, "/data/c3/jhe/test/re/%d_1", line);

	
	ifstream in(fns);
	istream_iterator<int> begins(in), eos;
	vector<int> result(begins, eos);
	
	ifstream in2(fns2);
	istream_iterator<int> begin2(in2), eos2;
	vector<int> result2(begin2, eos2);


	vector<int> results;
	vector<int> results2;
	results.reserve(MAXDOC);
	results2.reserve(MAXDOC);




	clock_t b2, e2;
	b2 = clock();
	int did;
  	int i, d, f=0;
  	int isize = 0;
  	float s, max = -1.0;
  	int maxid;
  clock_t begin, end;
  list_reader* readers = new list_reader[num];
	int jj = 0;
	
   	ifstream i_streams; 
	ifstream i_streams2;
	unsigned* data_off[num];
	metadata* meta_off[num];
	e2 = clock();

	//printf("Time: %.12lf s\n", diff_clock(&b2, &e2));
	for ( i = 0; i < num; i++)
	{
		entities[i].data_length = offset[i];
		entities[i].meta_length = offset2[i];
	}
	
	qsort(entities, num ,sizeof(log_entity), compare);
	 begin = clock();
  	for (i = 0; i < num; i++){

		data_off[i] = data+(entities[i].data_length);
		meta_off[i] = meta+(entities[i].meta_length);

		readers[i].openList(data_off[i], meta_off[i], &entities[i]);
	  	
  	}


  for (did = 0; (did < 25205180 && did >= 0 ); did++)
  {
       did = readers[0].nextGEQ(did);

    	for (i = 1; (i < num) && ((d = readers[i].nextGEQ(did)) == did); i++);

    	if (d > did)
      		did = d-1;
    	else if ( d == did && d!=26000000) 
    	{
		results.push_back(did);
		for ( s = 0, i = 0, f =0 ; i < num; i++)
		{
			f +=readers[i].get_freq();
       		s += entities[i].pre * (float)(f) / ((float)(f) + pre_doc[did]);

		}
			results2.push_back(f);
    	
    	}
    }

   	end = clock();

	if ( result != results){
		cout << "result is not right" <<endl;
		cout<<"result size:"<<results.size() << " correct size:"<<result.size()<<endl;
		cout<<"result size:"<<results.size() << " correct size:"<<result.size()<<endl;
		int size = (results.size() > result.size() ? result.size():results.size());
		for (int l = 0; l < results.size(); l++){
			if ( result[l] != results[l]){
				cout <<l << " " << results[l] <<endl;
				//check_num(doc, ndocs, num, result[l]);
			}
		}
	}
	if (result2 != results2){
		cout << "freq result is not right!"<<endl;
		cout<<"result size:"<<results2.size() << " correct size:"<<result2.size()<<endl;
		for (int l = 0; l < results2.size(); l++){
			if ( result2[l] != results2[l]){
				//cout <<l << " " << results2[l]<< " " << result2[l] <<endl;
				//check_num(doc, ndocs, num, result[l]);
			}
		}
	}
	else
	{
		cout<<"yeah, I am success! size:"<<result.size() <<" size:"<< results.size()<<endl;
	}

	
	for (i = 0; i < num; i++) {
		readers[i].closelist();
	}
	printf("Time: %.12lf s\n", diff_clock(&begin, &end));
	return diff_clock(&begin, &end);
}




#endif /* TEST_CODING_H_ */
