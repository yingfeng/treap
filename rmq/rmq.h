#ifndef _RMQ__H
#define _RMQ__H

#include <iostream>
#include <tree_ff.h>

namespace rmq_space
{
	class Tree
	{
		public:
			int pos;
			Tree *prevSibl,*lastChild;
	};

	class Path
	{
	public:
		Tree *node;
		Path *next;
	};
	// typedef struct sTree
	// { 
	//   int pos;
	//   struct sTree *prevSibl,*lastChild;
	//   //*Tree;
	// }*Tree;

	// typedef struct sPath
	// { 
	//   Tree node;
	//   struct sPath *next;
	//   //*Path;
	// }*Path;


	class RMQ
	{
	public:
		size_t n;
		unsigned int *bitmap;
		tree_ff *t;
		int  mapTree (unsigned int *bitmap, Tree *T, int i);
		unsigned int *buildrmq (int *A, size_t n);
		RMQ(int *A,size_t n);
		~RMQ();
		unsigned int query(size_t i,size_t j);
		size_t getSize();
		
	};
};
#endif
