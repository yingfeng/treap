#include <iostream>
#include "Treap.h"
#include <time.h>
// Test program
int main( )
{
    const int ITEM_NOT_FOUND = -9999;
    Treap *t = new Treap();
    int NUMS = 20;
    const int GAP  =   2;
    int i;
    Treap *t2 = new Treap();


    cout << "Checking... (no more output means success)" << endl;

    t->insert( 10,1 );
    t->insert( 4,6 );
    t->insert( 3,4 );
    t->insert( 20,7 );
    t->insert( 15,4 );
    t->insert( 5,1 );


    t2->insert( 4,3 );
    t2->insert( 20,2 );
    t2->insert( 15,1 );
    t2->insert( 1,10 );
    t2->insert( 6,5 );
    t2->insert( 5,7 );

    
    
    vector<pair<uint,uint> > p;
    t->preOrder(p,t->getRoot());
    for (int i = 0 ; i < p.size();i++) {
        cout << "d = " << p[i].first << " f = " << p[i].second << endl;
    }
    cout << "-------------------------" << endl;
    vector<pair<uint,uint> > p2;
    t2->preOrder(p2,t2->getRoot());
    for (int i = 0 ; i < p2.size();i++) {
        cout << "d = " << p2[i].first << " f = " << p2[i].second << endl;
    }

    t->intersect(t2);
    return 0;
}
