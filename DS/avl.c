/*
 * AVL Tree implementation
 *
 * Author: Arun Prakash Jana <engineerarun@gmail.com>
 * Copyright (C) 2015 by Arun Prakash Jana <engineerarun@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dslib.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "avl.h"

#define LEFT  0
#define RIGHT 1

/*
 * Struct to hold an AVL node and traversal
 * direction (left or right) to child node
 */
typedef struct {
	avl_p node;
	int direction;
} nodedata;

/*
 * Generate an AVL tree iteratively from an array of integers
 */
avl_p *generate_avl(int *arr, int len)
{
	int i = 1;
	avl_pp head = NULL;
	avl_p root = NULL;
	avl_p tmp = NULL;
	nodedata *p = NULL;
	stack_p stack = get_stack(); // Stack to rebalance each subtree bottom-up after insertion

	if (!arr || !len) {
		log(ERROR, "Invalid array.\n");
		return NULL;
	}

	root = (avl_p) calloc(1, sizeof(avl_t));
	if (!root) {
		log(ERROR, "calloc failed.\n");
		return NULL;
	}
	root->data = arr[0];
	head = calloc(1, sizeof(avl_p));
	*head = root;

	for (; i < len; i++) {
		while (root) {
			if (arr[i] < root->data) {
				if (!root->left) {
					/* Create an AVL node for new value */
					root->left = calloc(1, sizeof(avl_t));
					root->left->data = arr[i];
					root->height = height(root);

					/* Unwind stack and rebalance each node, if required */
					while ((p = pop(stack)) != NULL) {
						tmp = p->node;
						free(p);

						rebalance(stack, head, tmp, arr[i]);
					}

					root = *head; // Restart next element insertion from head
					break;
				}

				/* Push the parent node and traversal
				   direction in stack as we traverse down */
				nodedata *n = malloc(sizeof(nodedata));
				n->node = root;
				n->direction = LEFT;
				push(stack, n);

				/* Traverse further left */
				root = root->left;
			} else {
				if (!root->right) {
					root->right = calloc(1, sizeof(avl_t));
					root->right->data = arr[i];
					root->height = height(root);

					while ((p = pop(stack)) != NULL) {
						tmp = p->node;
						free(p);

						rebalance(stack, head, tmp, arr[i]);
					}

					root = *head;
					break;
				}

				nodedata *n = malloc(sizeof(nodedata));
				n->node = root;
				n->direction = RIGHT;
				push(stack, n);

				root = root->right;
			}
		}
	}

	destroy_stack(stack);

	return head;
}

/*
 * Rebalance subtree tmp based on balance factor & skew
 */
void rebalance(stack_p stack, avl_pp head, avl_p tmp, int data)
{
	nodedata *p = NULL;
	int direction;
	avl_p parent = NULL;

	if (BalanceFactor(tmp) == -2) { /* Right subtree longer */
		if ((p = pop(stack)) != NULL) {
			parent = p->node;
			direction = p->direction;
		}
		/* If p is NULL, this is the topmost node, update *head */

		if (data >= tmp->right->data) { /* Right-right skewed subtree */
			if (p)
				direction == RIGHT ?  (parent->right = RightRight(tmp))
					: (parent->left = RightRight(tmp));
			else
				*head = RightRight(tmp);
		} else { /* Right-left skewed subtree */
			if (p)
				direction == RIGHT ? (parent->right = RightLeft(tmp))
					: (parent->left = RightLeft(tmp));
			else
				*head = RightLeft(tmp);
		}
	} else if (BalanceFactor(tmp) == 2) { /* Left subtree longer */
		if ((p = pop(stack)) != NULL) {
			parent = p->node;
			direction = p->direction;
		}
		/* If p is NULL, this is the topmost node, update *head */

		if (data < tmp->left->data) { /* Left-left skewed subtree */
			if (p)
				direction == RIGHT ? (parent->right = LeftLeft(tmp))
					: (parent->left = LeftLeft(tmp));
			else
				*head = LeftLeft(tmp);
		} else { /* Left-right skewed subtree */
			if (p)
				direction == RIGHT ? (parent->right = LeftRight(tmp))
					: (parent->left = LeftRight(tmp));
			else
				*head = LeftRight(tmp);
		}
	}

	if (p)
		free(p);

	tmp->height = height(tmp);
}

/*
 * Calculate the height of a node in AVL tree
 */
int height(avl_p node)
{
	int lh, rh;

	if (!node)
		return 0;

	node->left == NULL ? lh = 0 : (lh = 1 + node->left->height);
	node->right == NULL ? rh = 0 : (rh = 1 + node->right->height);

	return (lh >= rh ? lh : rh);
}

/*
 * Rotate a node towards right
 */
avl_p RotateRight(avl_p node)
{
	avl_p left_node = node->left;
	node->left = left_node->right;
	left_node->right = node;

	node->height = height(node);
	left_node->height = height(left_node);

	return left_node;
}

/*
 * Rotate a node towards left
 */
avl_p RotateLeft(avl_p node)
{
	avl_p right_node = node->right;
	node->right = right_node->left;
	right_node->left = node;
	
	node->height = height(node);
	right_node->height = height(right_node);

	return right_node;
}

/*
 * Adjust a right right skewed subtree
 */
avl_p RightRight(avl_p node)
{
	return RotateLeft(node);
}

/*
 * Adjust a left left skewed subtree
 */
avl_p LeftLeft(avl_p node)
{
	return RotateRight(node);
}

/*
 * Adjust a left right skewed subtree
 */
avl_p LeftRight(avl_p node)
{
	node->left = RotateLeft(node->left);
	return RotateRight(node);
}

/*
 * Adjust a right left skewed subtree
 */
avl_p RightLeft(avl_p node)
{
	node->right = RotateRight(node->right);
	return RotateLeft(node);
}

/*
 * Calculate balance factor of subtree
 */
int BalanceFactor(avl_p node)
{
	int lh, rh;

	if (!node)
		return 0;

	node->left == NULL ? lh = 0 : (lh = 1 + node->left->height);
	node->right == NULL ? rh = 0 : (rh = 1 + node->right->height);

	return (lh - rh);
}

/*
 * Delete an AVL tree from its root
 */
int delete_avl(avl_p root)
{
	int count = 0;

	if (!root) {
		log(ERROR, "root invalid.\n");
		return -1;
	}

	if (root->left)
		count += delete_avl(root->left);

	if (root->right)
		count += delete_avl(root->right);

	free(root);

	return ++count;
}

/*
 * Print the values in an AVL tree in preorder
 */
int print_avl(avl_p root, avl_p parent)
{
	int count = 0;

	if (!root) {
		log(ERROR, "root invalid.\n");
		return -1;
	}

	++count;

	/* Print data value in the node */
	log(INFO, "data: %6d,  parent: %6d\n", root->data, parent->data);

	if (root->left) {
		log(INFO, "LEFT.\n");
		count += print_avl(root->left, root);
	}

	if (root->right) {
		log(INFO, "RIGHT.\n");
		count += print_avl(root->right, root);
	}

	return count;
}