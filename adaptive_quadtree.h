/*
	Program:
		quadtree
	Description:
	        Declaration for a quadtree

	Copyright (C) 2008 José Antonio Villarreal

	Contacts:	quetzal1910@gmail.com

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*****************************************************************************
*                                                                            *
*  ------------------------------- quadtree.h -----------------------------  *
*                                                                            *
*****************************************************************************/

#ifndef QUADTREE_H
#define QUADTREE_H

#include <stdlib.h>
#include "dlist.h"

#define NUM_CHILDREN 4


/*****************************************************************************
*                                                                            *
*  Define a structure for quadtree nodes.                                    *
*                                                                            *
*****************************************************************************/


typedef struct QuadTreeNode_ {

void               *data;
struct QuadTreeNode_       **children;
DList *children_list;
struct QuadTreeNode_ *parent;
int visited;
double mass;
struct point_t cm;
Rectangle          *rect;
                   
} QuadTreeNode;

/*****************************************************************************
*                                                                            *
*  Define a structure for quadtrees.                                         *
*                                                                            *
*****************************************************************************/

typedef struct QuadTree_ {

int                size;

int                (*compare)(const void *key1, const void *key2);
void               (*destroy)(void *data);

QuadTreeNode         *root;

} QuadTree;

/*****************************************************************************
*                                                                            *
*  --------------------------- Public Interface ---------------------------  *
*                                                                            *
*****************************************************************************/

void quadtree_init(QuadTree *tree, void (*destroy)(void *data));

void quadtree_destroy(QuadTree *tree);

QuadTreeNode* quadtree_child(QuadTreeNode *node, int pos);

int quadtree_ins_node(QuadTree *tree, QuadTreeNode *node, const void *data, int pos);

void quadtree_rem_child(QuadTree *tree, QuadTreeNode *node, int pos);

void quadtree_mark(QuadTreeNode* node);

#define quadtree_size(tree) ((tree)->size)

#define quadtree_root(tree) ((tree)->root)

#define quadtree_is_eob(node) ((node) == NULL)

#define quadtree_is_leaf(node) ((node)->children == NULL && (node)->children_list == NULL)

#define quadtree_data(node) ((node)->data)

#define quadtree_children(node) ((node)->children)

#define quadtree_is_root(node) ((node)->parent == NULL)

#define quadtree_is_visited(node) ((node)->visited == 1)

#endif
