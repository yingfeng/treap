#ifndef TREAP_H_
#define TREAP_H_

#include <limits.h>
//#include "dsexceptions.h"
#include <iostream>       // For NULL
#include <vector>
#include <stack>
#include "../trees/tree_ff.h"

// Treap class
//
// CONSTRUCTION: with ITEM_NOT_FOUND object used to signal failed finds
//
// ******************PUBLIC OPERATIONS*********************
// void insert( x )       --> Insert x
// void remove( x )       --> Remove x
// uint find( x )   --> Return item that matches x
// uint findMin( )  --> Return smallest item
// uint findMax( )  --> Return largest item
// boolean isEmpty( )     --> Return true if empty; else false
// void makeEmpty( )      --> Remove all items
// void printTree( )      --> Print tree in sorted order


  // Node and forward declaration because g++ does
  // not understand nested classes.
using namespace std;
class Treap;

class TreapNode {
    
    

  public:
    TreapNode *left;
    TreapNode *right;
    
    uint element;
    uint priority;
    uint element_dest;
    uint priority_dest;

    TreapNode( ) {
        this->left = NULL;
        this->right = NULL;
        this->priority = 0;
    }
    TreapNode( uint theElement, TreapNode *lt, TreapNode *rt, uint pr ) {
        this->element = theElement;
        this->left = lt;
        this->right = rt;
        this->priority = pr;
    }
      
    friend class Treap;
};

class Treap {
  public:
    Treap( const uint & notFound );
    Treap( const Treap & rhs );
    Treap();
    ~Treap( );

    uint size;
    const uint & findMin( ) const;
    const uint & findMax( ) const;
    const uint & find( const uint & x ) const;
    bool isEmpty( ) const;
    void printTree( ) const;

    void makeEmpty( );
    void insert( uint x , uint priority);
    void remove( const uint & x );
    int getHeight(TreapNode *r);
    uint maxDepthIterative(TreapNode *r);
    TreapNode * getRoot() ;
    void preOrderDest(vector<pair<uint,uint> > & r,TreapNode * n);
    void preOrder(vector<pair<uint,uint> > & r,TreapNode * n);
    void inOrder(vector<pair<uint,uint> > & r,TreapNode * n);
    void postOrder(vector<pair<uint,uint> > & r,TreapNode * n);
    void destify(TreapNode *n);
    void intersect(Treap *t);


    const Treap & operator=( const Treap & rhs );

  private:
    TreapNode *root;
    uint ITEM_NOT_FOUND;
    TreapNode *nullNode;

      // Recursive routines
    TreapNode *  insert( uint x, uint priority, TreapNode * t );
    void remove( const uint & x, TreapNode * & t );
    void makeEmpty( TreapNode * & t );
    void printTree( TreapNode *t ) const;


      // Rotations
    TreapNode *  rotateWithLeftChild( TreapNode * & t ) const;
    TreapNode *  rotateWithRightChild( TreapNode * & t ) const;
    TreapNode * clone( TreapNode * t ) const;
};

#include "Treap.cpp"
#endif
