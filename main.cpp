#include <iostream>

#include "btree.h"

using namespace std;

int main()
{
    BTree<int> *bTree = new BTree<int>(2);
    for (int i = 1; i <= 5; ++i)
        bTree->insert(i);

    bTree->print();
//    for (int i = 40; i >= 1; --i)
//        bTree->remove(i);

    bTree->print();

    //delete bTree;




    bTree->remove(1);


    bTree->print();
//    cout << bTree.search(1) << endl;
//    cout << bTree.search(2) << endl;
//    cout << bTree.search(3) << endl;
//    cout << bTree.search(0) << endl;

    return EXIT_SUCCESS;
}