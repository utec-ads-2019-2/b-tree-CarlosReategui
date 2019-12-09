#ifndef BTREE_H
#define BTREE_H

#include "node.h"

template <typename T>
class BTree
{
    private:
        Node<T> *root;
        unsigned int t;

    public:
        BTree(unsigned int t) : t(t), root(nullptr) {};

        bool search(T key)
        {
            return root != nullptr ? root->search(key) != nullptr :  false;
        }

        bool insert(T key)
        {
            if (search(key))
                return false;
            else
            {
                if (!root)
                {
                    root = new Node<T>(t);
                    root->keys[0] = key;
                    ++root->numberOfKeys;
                    return true;
                }
                else
                {
                    if (root->numberOfKeys == 2 * t - 1)
                    {
                        Node<T> *newNode = new Node<T>(t, false);
                        newNode->children[0] = root;
                        newNode->splitChild(0, root);

                        unsigned int indexOfChildren = 0;
                        if (newNode->keys[0] < key)
                            ++indexOfChildren;
                        newNode->children[indexOfChildren]->insertToNonFullNode(key);

                        root = newNode;
                        return true;
                    }
                    else
                    {
                        root->insertToNonFullNode(key);
                        return true;
                    }
                }
            }
        }

        bool remove(T key)
        {
            if (!root)
                return false;

            root->remove(key);

            if (root->numberOfKeys == 0)
            {
                Node<T> *tempRoot = root;
                root->isLeaf ? root = nullptr : root = root->children[0];
                delete tempRoot;
            }

            // Qué se retorna aquí?
        }

        void print()
        {
            if (root)
            {
                //root->printInOrder();
                root->recorrerNodes();
                cout << endl;
            }
        }

        ~BTree()
        {
            if (root)
                root->deleteAll();
            root = nullptr;
        }
};


#endif