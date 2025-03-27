#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int height(Node *root) {
	if (root == nullptr) {
		return 0;
	}
	int lheight = height(root->left);
	int rheight = height(root->right);

    if (lheight > rheight) {
        return lheight + 1;
    } else {
        return rheight + 1;
    }

}

bool equalPaths(Node * root)
{
    // Add your code below
    if (root == nullptr) return true;
    if ((root->right && root->left == nullptr)) {
        return equalPaths(root->right);
    }
    if ((root->left && root->right == nullptr) ) {
        return equalPaths(root->left);
    }


	if (abs(height(root->right) - height(root->left)) >= 1) {
		return false;
    }

	return true;


}

