#include "Treap.h"
//#include <iostream.h>

/**
* Implements an unbalanced binary search tree.
* Note that all "matching" is based on the compares method.
* @author Mark Allen Weiss
*/
/**
 * Construct the treap.
 */
using namespace std;

Treap::Treap( const uint & notFound ) : ITEM_NOT_FOUND( notFound ) {
    nullNode = new TreapNode;
    nullNode->left = nullNode->right = NULL;
    nullNode->priority = INT_MIN;
    root = NULL;
}

Treap::Treap(){
    root = NULL;
    this->size = 0;
}


/**
 * Copy constructor.
 */

Treap::Treap( const Treap & rhs ) : ITEM_NOT_FOUND( rhs.ITEM_NOT_FOUND ) {
    nullNode = new TreapNode;
    nullNode->left = nullNode->right = NULL;
    nullNode->priority = INT_MAX;
    root = NULL;
    *this = rhs;
}

/**
 * Destructor for the tree.
 */

Treap::~Treap( ) {
    makeEmpty( );
}

/**
 * Insert x into the tree; duplicates are ignored.
 */

void Treap::insert( uint x, uint priority ) {
    this->root = insert( x, priority, this->root );
}

/**
 * Remove x from the tree. Nothing is done if x is not found.
 */

void Treap::remove( const uint & x ) {
    remove( x,  root );
}

/**
 * Find the smallest item in the tree.
 * Return smallest item or ITEM_NOT_FOUND if empty.
 */

const uint & Treap::findMin( ) const {
    if( isEmpty( ) )
        return ITEM_NOT_FOUND;

    TreapNode *ptr = root;
    while( ptr->left != NULL )
        ptr = ptr->left;

    return ptr->element;
}

/**
 * Find the largest item in the tree.
 * Return the largest item of ITEM_NOT_FOUND if empty.
 */

const uint & Treap::findMax( ) const {
    if( isEmpty( ) )
        return ITEM_NOT_FOUND;

    TreapNode *ptr = root;
    while( ptr->right != NULL )
        ptr = ptr->right;

    return ptr->element;
}

/**
 * Find item x in the tree.
 * Return the matching item or ITEM_NOT_FOUND if not found.
 */

const uint & Treap::find( const uint & x ) const {
    TreapNode *current = root;

    for( ; ; )
    {
        if( x < current->element )
            current = current->left;
        else if( current->element < x )
            current = current->right;
        else if( current != NULL )
            return current->element;
        else
            return ITEM_NOT_FOUND;
    }
}

/**
 * Make the tree logically empty.
 */

void Treap::makeEmpty( ) {
    makeEmpty( root );
}

/**
 * Test if the tree is logically empty.
 * Return true if empty, false otherwise.
 */

bool Treap::isEmpty( ) const {
    return root == NULL;
}

/**
 * Print the tree contents in sorted order.
 */

void Treap::printTree( ) const {
    if( isEmpty( ) )
        cout << "Empty tree" << endl;
    else
        printTree( root );
}


/**
 * Deep copy.
 */

const Treap & Treap::operator=( const Treap & rhs ) {
    if( this != &rhs ) {
        makeEmpty( );
        root = clone( rhs.root );
    }

    return *this;
}

/**
 * Internal method to insert into a subtree.
 * x is the item to insert.
 * t is the node that roots the tree.
 * Set the new root.
 */

TreapNode * Treap::insert( uint x, uint priority, TreapNode * t ) {
    if( t == NULL ) {
        // cout << "Entered here!" << endl;
        this->size++;
        return new TreapNode( x, NULL, NULL, priority ); 
    }
    if( x < t->element ) {
        t->left = insert( x, priority, t->left );
        if( t->left->priority >= t->priority ) {
            return rotateWithLeftChild( t );
        }
        return t;
    }
    
    if( t->element < x ) {
        t->right = insert( x, priority, t->right );
        if( t->right->priority > t->priority ) {
            return rotateWithRightChild( t );
        }
        return t;
    }

    return t;
       // else duplicate; do nothing
}
static bool next = false;
void intersect_aux(TreapNode *n1,TreapNode *n2) {

    if (n1 == NULL) return;
    if (n2 == NULL) return;
    
    
    //cout << "entered with:" << n1->element << " | " << n2->element << endl;
    if (n1->element == n2->element) { 
        cout << "found equal = " << n1->element << endl;
        intersect_aux(n1->left,n2->left);
        intersect_aux(n1->right,n2->left);
        return;
    } else {
        next = false;
        if (n1->element > n2->element) {
            intersect_aux(n1,n2->right);
            intersect_aux(n1->left,n2);
        }
        if (n1->element < n2->element) {
            intersect_aux(n1->right,n2);
            intersect_aux(n1,n2->left);
        }
    }
}

void Treap::intersect(Treap *t) {
    intersect_aux(t->getRoot(),this->getRoot());
}


/**
 * Internal method to remove from a subtree.
 * x is the item to remove.
 * t is the node that roots the tree.
 * Set the new root.
 */

void Treap::remove( const uint & x, TreapNode * & t ) {
    if( t != NULL ) {
        if( x < t->element )
            remove( x, t->left );
        else if( t->element < x )
            remove( x, t->right );
        else {
                // Match found
            if( t->left->priority > t->right->priority )
                rotateWithLeftChild( t );
            else
                rotateWithRightChild( t );

            if( t != NULL )      // Continue on down
                remove( x, t );
            else {
                delete t->left;
                t->left = NULL;  // At a leaf
            }
        }
    }
}

/**
 * Internal method to make subtree empty.
 */

void Treap::makeEmpty( TreapNode * & t ) {
    if( t != NULL ) {
        makeEmpty( t->left );
        makeEmpty( t->right );
        delete t;
    }
}

/**
 * Internal method to print a subtree in sorted order.
 * @param t the node that roots the tree.
 */

void Treap::printTree( TreapNode *t ) const {
    if( t != NULL ) {
        printTree( t->left );
        cout << t->element << endl;
        printTree( t->right );
    }
}


/**
 * Internal method to clone subtree.
 */

TreapNode * Treap::clone( TreapNode * t ) const {
    if( t == t->left )  // Cannot test against NULL!!!
        return NULL;
    else
        return new TreapNode( t->element, clone( t->left ),
                                       clone( t->right ), t->priority );
}

/**
 * Rotate binary tree node with left child.
 */

TreapNode* Treap::rotateWithLeftChild( TreapNode * & node ) const {
    TreapNode *result = node->left;
    TreapNode *x = result->right;
    result->right = node;
    node->left = x;  
    return result;
}

/**
 * Rotate binary tree node with right child.
 */

TreapNode* Treap::rotateWithRightChild( TreapNode * & node ) const {
    TreapNode *result = node->right;
    TreapNode *x = result->left;
    result->left = node;
    node->right = x;
    return result;
}


int Treap::getHeight(TreapNode *r )  {
    if (r == NULL || r->priority == 0) {
        return 0;
    }
    // cout << "element -> " << r->element << endl;
    return 1+(max(getHeight(r->left),getHeight(r->right)));   
}


uint Treap::maxDepthIterative(TreapNode *r) {
  if (!r) return 0;
  stack<TreapNode*> s;
  s.push(r);
  uint maxDepth = 0;
  TreapNode *prev = NULL;
  while (!s.empty()) {
    TreapNode *curr = s.top();
    if (!prev || prev->left == curr || prev->right == curr) {
      if (curr->left)
        s.push(curr->left);
      else if (curr->right)
        s.push(curr->right);
    } else if (curr->left == prev) {
      if (curr->right)
        s.push(curr->right);
    } else {
      s.pop();
    }
    prev = curr;
    if (s.size() > maxDepth)
      maxDepth = s.size();
  }
  return maxDepth;
}



TreapNode *  Treap::getRoot()  {
    return this->root;
}


void Treap::preOrder(vector<pair<uint,uint> > & r,TreapNode * n)  {
    if (n == NULL)
        return;
    r.push_back(make_pair(n->element,n->priority));
    preOrder(r,n->left);
    preOrder(r,n->right);
}

void Treap::preOrderDest(vector<pair<uint,uint> > & r,TreapNode * n) {
    if (n == NULL) 
        return;
    r.push_back(make_pair(n->element_dest,n->priority_dest));
    preOrderDest(r,n->left);
    preOrderDest(r,n->right);
}
void Treap::inOrder(vector<pair<uint,uint> > & r,TreapNode * n)  {
    if (n == NULL)
        return;
    inOrder(r,n->left);
    r.push_back(make_pair(n->element,n->priority));
    inOrder(r,n->right);
}

void Treap::postOrder(vector<pair<uint,uint> > & r,TreapNode * n)  {
    if (n == NULL)
        return;
    postOrder(r,n->left);
    postOrder(r,n->right);
    r.push_back(make_pair(n->element,n->priority));
}

void Treap::destify(TreapNode *n) {
    if (n == NULL) return;

    this->destify(n->left);
    if (n->left != NULL) {
            n->left->priority_dest = n->priority - n->left->priority;
            n->left->element_dest = n->element - n->left->element;
    }
    if (n->right != NULL) {
        n->right->priority_dest = n->priority -n->right->priority;
        n->right->element_dest = n->right->element - n->element;
    }
    this->destify(n->right);
}