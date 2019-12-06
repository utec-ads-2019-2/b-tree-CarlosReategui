#ifndef NODE_H
#define NODE_H

#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

template <typename T>
class BTree;

template <typename T>
class Node {
    unsigned int t;
    vector<T> keys;
    vector<Node<T> *> children;
    bool isLeaf;
    unsigned int numberOfKeys;

    public: 
        Node(unsigned int t, bool isLeaf = true) : t(t), isLeaf(isLeaf)
        {
            keys.resize(2 * t - 1);
            children.resize(2 * t);
            numberOfKeys = 0;
        }


        void printInOrder()
        {
            int i;
            for (i = 0; i < numberOfKeys; ++i)
            {
                if (!isLeaf)
                    children[i]->printInOrder();
                cout << " " << keys[i];
            }
            if (!isLeaf)
                children[i]->printInOrder();
        }

        void splitChild(int index, Node<T> *node)
        {
            Node<T> *newNode = new Node(node->t, node->isLeaf);
            newNode->numberOfKeys = t - 1;

            for (int i = 0; i < t-1; i++)
                newNode->keys[i] = node->keys[i + t];

            if (!node->isLeaf)
                for (int i = 0; i < t; i++)
                    newNode->children[i] = node->children[i + t];

            node->numberOfKeys = t - 1;
            
            for (int i = numberOfKeys; i >= index + 1; --i)
                children[i + 1] = children[i];
            
            children[index + 1] = newNode;
            
            for (int i = numberOfKeys-1; i >= index; --i)
                keys[i + 1] = keys[i];

            keys[index] = node->keys[t - 1];

            numberOfKeys = numberOfKeys + 1;

            for (int i = t-1; i < node->keys.size(); ++i)
                node->keys[i] = 0;

            for (int i = t; i < node->children.size(); ++i)
                node->children[i] = nullptr;
        }

        void insertToNonFullNode(T key)
        {
            int index = numberOfKeys - 1;
            if (isLeaf)
            {
                while (index >= 0 and keys[index] > key)
                {
                    keys[index + 1] = keys[index];
                    --index;
                }
                keys[index + 1] = key;
                ++numberOfKeys;
            }
            else
            {
                while (index >= 0 and keys[index] > key)
                    --index;

                if (children[index + 1]->numberOfKeys == 2 * t - 1)
                {
                    splitChild(index + 1, children[index + 1]);
                    if (keys[index + 1] < key)
                        ++index;
                }
                children[index + 1]->insertToNonFullNode(key);
            }
        }

        Node<T>* search(T key)
        {
            int i = 0;
            while (i < numberOfKeys and key > keys[i])
                ++i;
            if (i < numberOfKeys and key == keys[i])
                return this;
            if (isLeaf)
                return nullptr;
            else
                return children[i]->search(key);
        }

        void remove(T key)
        {
            int index = getIndexOfKey(key);
            if (keys[index] == key and index < numberOfKeys)
            {
                if (isLeaf)
                    removeKeyFromLeaf(index);
                else
                    removeKeyFromInternalNode(index);
            }
            else
            {
                if (isLeaf)
                    return;

                bool indexEqualsNumberOfKeys = index == numberOfKeys;
                if (children[index]->numberOfKeys < t)
                    fillNonFullChild(index);
                if (indexEqualsNumberOfKeys and index > numberOfKeys)
                    children[index - 1]->remove(key);
                else
                    children[index]->remove(key);
            }
        }

        int getIndexOfKey(T key)
        {
            int index = 0;
            while (keys[index] < key and index < numberOfKeys)
                ++index;
            return index;
        }

        // trabajar con erase de vector
        void removeKeyFromLeaf(int index)
        {
            keys.erase(keys.begin() + index);
            keys.resize(2 * t - 1);
            --numberOfKeys;
        }

        void removeKeyFromInternalNode(int index)
        {
            int k = keys[index];
            if (children[index]->numberOfKeys > t - 1)
            {
                T previousKey = getPreviousKey(index);
                keys[index] = previousKey;
                children[index]->remove(previousKey);
            }
            else if (children[index + 1]->numberOfKeys > t - 1)
            {
                T nextKey = getNextKey(index);
                keys[index] = nextKey;
                children[index + 1]->remove(nextKey);
            }
            else
            {
                mergeChildren(index);
                children[index]->remove(k);
            }
        }

        void fillNonFullChild(int index)
        {
            if (index != 0 and children[index - 1]->numberOfKeys > t - 1)
                getKeyFromTheLeft(index);
            else if (index != numberOfKeys and children[index + 1]->numberOfKeys > t - 1)
                getKeyFromTheRight(index);
            else
            {
                if (index != numberOfKeys)
                    mergeChildren(index);
                else
                    mergeChildren(index - 1);
            }
        }

        void getKeyFromTheLeft(int index)
        {
            Node<T> *child = children[index];
            Node<T> *nodeToTheLeftOfChild = children[index - 1];

            for (int i = child->numberOfKeys - 1; i >= 0; --i)
                child->keys[i + 1] = child->keys[i];

            if (!child->isLeaf)
            {
                for (int i = child->numberOfKeys; i >= 0; --i)
                    child->children[i + 1] = child->children[i];
            }

            child->keys[0] = keys[index - 1];

            if (!child->isLeaf)
                child->children[0] = nodeToTheLeftOfChild->children[nodeToTheLeftOfChild->numberOfKeys];

            keys[index - 1] = nodeToTheLeftOfChild->keys[nodeToTheLeftOfChild->numberOfKeys - 1];

            ++child->numberOfKeys;
            --nodeToTheLeftOfChild->numberOfKeys;
        }

        void getKeyFromTheRight(int index)
        {
            Node<T> *child = children[index];
            Node<T> *nodeToTheRightOfChild = children[index + 1];

            child->keys[child->numberOfKeys] = keys[index];
            if (!child->isLeaf)
                child->children[child->numberOfKeys + 1] = nodeToTheRightOfChild->children[0];

            keys[index] = nodeToTheRightOfChild->keys[0];

            for (int i = 1; i < nodeToTheRightOfChild->numberOfKeys; ++i)
                nodeToTheRightOfChild->keys[i - 1] = nodeToTheRightOfChild->keys[i];

            if (!nodeToTheRightOfChild->isLeaf)
                for (int i = 1; i < nodeToTheRightOfChild->numberOfKeys + 1; ++i)
                    nodeToTheRightOfChild->children[i - 1] = nodeToTheRightOfChild->children[i];

//            for (int i = numberOfKeys; i < keys.size(); ++i)
//                keys[i] = 0;

            ++child->numberOfKeys;
            --nodeToTheRightOfChild->numberOfKeys;
        }

        T getPreviousKey(int index)
        {
            Node<T> *currentNode = children[index];
            while (!currentNode->isLeaf)
                currentNode = currentNode->children[currentNode->numberOfKeys];
            return currentNode->keys[currentNode->numberOfKeys - 1];
        }

        T getNextKey(int index)
        {
            Node<T> *currentNode = children[index];
            while (!currentNode->isLeaf)
                currentNode = currentNode->children[0];
            return currentNode->keys[0];
        }

        void mergeChildren(int index)
        {
            Node<T> *child1 = children[index];
            Node<T> *child2 = children[index + 1];

            child1->keys[t - 1] = keys[index];

            for (int i = 0; i < child2->numberOfKeys; ++i)
                child1->keys[i + t] = child2->keys[i];

            if (!child1->isLeaf)
                for (int i = 0; i < child2->numberOfKeys + 1; ++i)
                    child1->children[i + t] = child2->children[i];

            for (int i = index + 1; i < numberOfKeys; ++i)
                keys[i - 1] = keys[i];

            for (int i = index + 2; i < numberOfKeys + 1; ++i)
                children[i - 1] = children[i];

            child1->numberOfKeys += child2->numberOfKeys + 1;
            --numberOfKeys;

            delete child2;
        }

        void deleteAll()
        {
            for (auto node : children)
                if (node)
                    node->deleteAll();
            delete this;
        }

        ~Node()
        {
            children.clear();
            keys.clear();
            isLeaf = numberOfKeys = t = 0;
        }

    friend class BTree<T>;
};

#endif