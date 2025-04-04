#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void insert_fix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n);
    void rotateRight(AVLNode<Key, Value> *g);
    void rotateLeft(AVLNode<Key, Value> *g);
    AVLNode<Key, Value>* findAVLNode(const Key& key) const;
    AVLNode<Key, Value>* getRoot() const;
    void removeFix(AVLNode<Key, Value>* n, int diff);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */

template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::findAVLNode(const Key& key) const
{

    AVLNode<Key, Value>* root = getRoot();
    if (root == nullptr) {
        return nullptr;
    }

    while (root != nullptr) {
        if (key == root->getKey()) {
            return root;
        } else {
            if (key < root->getKey()) {
                root = root->getLeft();
            } else {
                root = root->getRight();
            }
        }
    }
    return nullptr;




    
}
template<typename Key, typename Value>
AVLNode<Key, Value>* AVLTree<Key, Value>::getRoot() const {
    return static_cast<AVLNode<Key, Value>*>(this->root_);
}


template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{

    // call interanl find to find they key and overwrite curr value
    AVLNode<Key, Value> * key = findAVLNode(new_item.first);

    if (key) {
        // update the value if key already exists
        key->setValue(new_item.second);
        return;
    }


    AVLNode<Key, Value>* node = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    if (getRoot() == nullptr) {
        this->root_ = node;
        node->setBalance(0);
        return;
    }


    AVLNode<Key, Value> *parent = getRoot();

    // regular insert
    while (true) {
        if (new_item.first < parent->getKey()) {
            if (parent->getLeft() == nullptr) {
                node->setParent(parent);
                parent->setLeft(node);
                // set balance
                node->setBalance(0);
                // check balance of parent
                if (parent->getBalance() == 1) {
                    parent->setBalance(0);
                    return;
                }
                // else update b(p)
                parent->setBalance(-1);
                // cal insert fix(p,n) (when b(p) was 0)
                insert_fix(parent, node);
                return;

            } else {
                parent = parent->getLeft();
            }
            

        } else {
            if (parent->getRight() == nullptr) {
                node->setParent(parent);
                parent->setRight(node);
                // set balance
                node->setBalance(0);
                // check balance of parent
                if (parent->getBalance() == -1 ) {
                    parent->setBalance(0);
                    return;
                }

                // else update b(p)
                parent->setBalance(1);
                insert_fix(parent, node);
                return;

                
            } else {
                parent = parent->getRight();
            }
            
        }

    }

}

template<class Key, class Value>
void AVLTree<Key, Value>::insert_fix(AVLNode<Key, Value>* p, AVLNode<Key, Value>* n) {
    if (p == nullptr || p->getParent() == nullptr) return;

    AVLNode<Key, Value>* g = p->getParent();
    // if p is g's left child
    if (p == g->getLeft()) {
        g->updateBalance(-1);
        int gBal = g->getBalance();

        if (gBal == 0) return;
        else if (gBal == -1) {
            insert_fix(g, p);
        }
        else if (gBal == -2) {
            if (n == p->getLeft()) {
                // Zig-zig (Left-Left)
                rotateRight(g);
                p->setBalance(0);
                g->setBalance(0);
            } else {
                // Zig-zag (Left-Right) 
                rotateLeft(p);
                rotateRight(g);

                if (n->getBalance() == -1) {
                    p->setBalance(0);
                    g->setBalance(1);
                    n->setBalance(0);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                } else { // +1
                    p->setBalance(-1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                
            }
            return;
        }
    } else {
        // p is right child of g
        g->updateBalance(+1);
        int gBal = g->getBalance();

        if (gBal == 0) return;
        else if (gBal == 1) {
            insert_fix(g, p);
        }
        else if (gBal == 2) {
            if (n == p->getRight()) {
                // Zig-zig (Right-Right)
                rotateLeft(g);
                p->setBalance(0);
                g->setBalance(0);
            } else {
                // Zig-zag (Right-Left)
                rotateRight(p);
                rotateLeft(g);

                if (n->getBalance() == 1) {
                    p->setBalance(0);
                    g->setBalance(-1);
                    n->setBalance(0);
                } else if (n->getBalance() == 0) {
                    p->setBalance(0);
                    g->setBalance(0);
                    n->setBalance(0);
                } else { // -1
                    p->setBalance(1);
                    g->setBalance(0);
                    n->setBalance(0);
                }
                
            }
            return;
        }
    }
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value> *g) {
   
    AVLNode<Key, Value>* p = g->getRight();
    AVLNode<Key, Value>* gp = g->getParent();

    
    if (p->getLeft()) {
        p->getLeft()->setParent(g);
        g->setRight(p->getLeft());
    } else {
        g->setRight(nullptr);

    }

    p->setLeft(g);
    g->setParent(p);

    p->setParent(gp);
    if (gp) {
        if (gp->getLeft() == g) {
            gp->setLeft(p);
        } else {
            gp->setRight(p);
        }
    } else {
        this->root_ = static_cast<Node<Key, Value>*>(p);
    }

  



}




template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value> *g) {
    AVLNode<Key, Value>* p = g->getLeft();
    AVLNode<Key, Value>* gp = g->getParent();

    if (p->getRight()) {
        
        p->getRight()->setParent(g);
        g->setLeft(p->getRight());
        
    } else {
        g->setLeft(nullptr);

    }
    // parent gp right child
    p->setRight(g);
    g->setParent(p);


    p->setParent(gp);
    // check if original subtree is right or left
    // if gp is nullptr then parent is root
    if (gp) {
        if (gp->getLeft() == g) {
            gp->setLeft(p);
        } else {
            gp->setRight(p);
        }
    } else {
        this->root_ = static_cast<Node<Key, Value>*>(p);
    }
} // end
/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{

AVLNode<Key, Value> * node = findAVLNode(key);

if (node == nullptr) {
    return;
}

if (node->getRight() && node->getLeft()) {
    AVLNode<Key, Value> *pred = static_cast<AVLNode<Key, Value>*> (this->predecessor(node));
    nodeSwap(pred, node);
}



// converges to 0 or 1 child scenario
AVLNode<Key, Value> *parent = node->getParent();



if (parent != nullptr) {
    int diff = 0;
    if (node == parent->getLeft()) {
        diff = 1;
    } else if (node == parent->getRight()) {
        diff = -1;
    }
    // parent->updateBalance(diff);
    // del key and update pointers - from bst.h
    // if its a leaf node
    if (node->getLeft() == nullptr && node->getRight() == nullptr) {
        // delete and unlink node
        // add root condition
        if (node == getRoot()) {
            this->root_ = nullptr;
        // node is left child
        } else if (node == parent->getLeft()) {
            parent->setLeft(nullptr);
            // diff = 1;
        // node is right child
        } else {
            parent->setRight(nullptr); 
            // diff = -1;
        }
        // delete node;
    } else if (node->getLeft() == nullptr && node->getRight()) {
        // promote right child and delete node;
        Node<Key, Value> * child = node->getRight();
        if (node == getRoot()) {
            this->root_ = child;        
        } else if (node == parent->getLeft()) {
            parent->setLeft(child);
            // diff = 1;
            
        } else {
            parent->setRight(child); 
            // diff = -1;
        }

        // make the child parent point to node's parent
        child->setParent(parent);
        // delete node;

    } else if (node->getRight() == nullptr && node->getLeft()) {
        Node<Key, Value> * child = node->getLeft();
        if (node == getRoot()) {
            this->root_ = child;        
        // if node is left child
        } else if (node == parent->getLeft()) {
            parent->setLeft(child);
        // if node is right child
        } else {
            parent->setRight(child); 
        }

        child->setParent(parent);
        // delete node;
        
    }

    


    delete node;
    removeFix(parent, diff);
}

}


template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* n, int diff) {
    if (n == nullptr) {
        return;
    }
    // Let p = parent(n) and if p is not NULL let ndiff (nextdiff) = +1 if n is a left child and -1 otherwise
    AVLNode<Key, Value> *p = n->getParent();
    int ndiff = 0;
    if (p) {
        if (n == p->getLeft()) {
            ndiff = 1;
        } else {
            ndiff = -1;
        }

    }
    if (diff == -1) {
        
        // Case 1: b(n) + diff == -2
        if (n->getBalance() + diff == -2) {
            AVLNode<Key, Value> *c = n->getLeft();

            if (c->getBalance() == -1) {
                rotateRight(n);
                n->setBalance(0);
                c->setBalance(0);
                removeFix(p, ndiff);
            } else if (c->getBalance() == 0) {
                rotateRight(n);
                n->setBalance(-1);
                c->setBalance(1);
                return;

            } else if (c->getBalance() == 1) {

            AVLNode<Key, Value> *g = c->getRight();
            rotateLeft(c);
            rotateRight(n);
            if (g->getBalance() == 1) {
                n->setBalance(0);
                c->setBalance(-1);
                g->setBalance(0);

            } else if (g->getBalance() == 0) {
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            } else if (g->getBalance() == -1) {
                n->setBalance(1);
                c->setBalance(0);
                g->setBalance(0);
            }
            removeFix(p, ndiff);


            }

        // case 2: == +2 mirror case
        } else if (n->getBalance() + diff == 2) {
            AVLNode<Key, Value>* c = n->getRight();
            if (c->getBalance() == 1) {
                rotateLeft(n);
                n->setBalance(0);
                c->setBalance(0);

            } else if (c->getBalance() == 0) {
                rotateLeft(n);
                n->setBalance(1);
                c->setBalance(-1);
                return;

            } else if (c->getBalance() == -1) {

                AVLNode<Key, Value> *g = c->getLeft();
                rotateRight(c);
                rotateLeft(n);
                if (g->getBalance() == -1) {
                    n->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);

                } else if (g->getBalance() == 0) {
                    n->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                } else if (g->getBalance() == 1) {
                    n->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                removeFix(p, ndiff);



            }

        }
         // case 2:
        else if (n->getBalance() + diff == -1) {
            // Case 2: b(n) + diff == -1: then b(n) = -1; // Done!
            n->setBalance(-1);
            return;
        } else if (n->getBalance() + diff == 0) {
            // Case 3: b(n) + diff == 0:
            n->setBalance(0);
            removeFix(p, ndiff);

        }
    } else if( diff == 1) {
         // Case 1: b(n) + diff == 2
    if (n->getBalance() + diff == 2) {
        AVLNode<Key, Value> *c = n->getRight();

        if (c->getBalance() == 1) {
            rotateLeft(n);
            n->setBalance(0);
            c->setBalance(0);
            removeFix(p, ndiff);
        } else if (c->getBalance() == 0) {
            rotateLeft(n);
            n->setBalance(1);
            c->setBalance(-1);
            return;
        } else if (c->getBalance() == -1) {
            AVLNode<Key, Value> *g = c->getLeft();
            rotateRight(c);
            rotateLeft(n);

            if (g->getBalance() == -1) {
                n->setBalance(0);
                c->setBalance(1);
                g->setBalance(0);
            } else if (g->getBalance() == 0) {
                n->setBalance(0);
                c->setBalance(0);
                g->setBalance(0);
            } else if (g->getBalance() == 1) {
                n->setBalance(-1);
                c->setBalance(0);
                g->setBalance(0);
            }
            removeFix(p, ndiff);
        }

    }

    // Case 2: b(n) + diff == 1
    else if (n->getBalance() + diff == 1) {
        n->setBalance(1);
        return;
    }

    // Case 3: b(n) + diff == 0
    else if (n->getBalance() + diff == 0) {
        n->setBalance(0);
        removeFix(p, ndiff);
    }



    }
}






template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
