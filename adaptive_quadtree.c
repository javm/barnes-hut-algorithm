/*

	Program:
		quadtree
	Description:
	        Definitions for a quadtree

	Copyright (C) 2014 José Antonio Villarreal

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
*  ------------------------------ quadtree.c ------------------------------  *
*                                                                            *
*****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "cell.h"
#include "adaptive_quadtree.h"

/*****************************************************************************
*                                                                            *
*  ------------------------------ quadtree_init ---------------------------- *
*                                                                            *
*****************************************************************************/

void quadree_init(QuadTree *tree, void (*destroy)(void *data)) {

/*****************************************************************************
*                                                                            *
*  Initialize the quadtree.                                                  *
*                                                                            *
*****************************************************************************/

tree->size = 0;
tree->destroy = destroy;
tree->root = NULL;

 return;

}

/*****************************************************************************
*                                                                            *
*  --------------------------- quadtree_destroy ---------------------------  *
*                                                                            *
*****************************************************************************/

void quadtree_destroy(QuadTree *tree) {

/*****************************************************************************
*                                                                            *
*  Remove all the nodes from the tree.                                       *
*                                                                            *
*****************************************************************************/

quadtree_rem_child(tree, NULL, 0);

/*****************************************************************************
*                                                                            *
*  No operations are allowed now, but clear the structure as a precaution.   *
*                                                                            *
*****************************************************************************/

memset(tree, 0, sizeof(QuadTree));

return;

}




/*****************************************************************************
*                                                                            *
*  ---------------------------- quadtree_child ---------------------------  *
*                                                                            *
*****************************************************************************/

QuadTreeNode* quadtree_child(QuadTreeNode *node, int pos){

	if( (pos < 0) && (pos >= NUM_CHILDREN))
		return NULL;
	else
		return node->children[pos];

}


/*****************************************************************************
*                                                                            *
*  ---------------------------- quadtree_ins_node -------------------------  *
*                                                                            *
*****************************************************************************/

int quadtree_ins_node(QuadTree *tree, QuadTreeNode *node, const void *data, int pos) {

QuadTreeNode         *new_node,
                     **position;

/*****************************************************************************
*                                                                            *
*  Determine where to insert the node.                                       *
*                                                                            *
*****************************************************************************/

if((pos < 0) || (pos >= NUM_CHILDREN))
	return -1;


if (node == NULL) {

   /**************************************************************************
   *                                                                         *
   *  Allow insertion at the root only in an empty tree.                     *
   *                                                                         *
   **************************************************************************/

   if (quadtree_size(tree) > 0)
      return -1;

   position = &tree->root;

   }

else {

   /**************************************************************************
   *                                                                         *
   *  Normally allow insertion only at the end of a branch.                  *
   *                                                                         *
   **************************************************************************/

	if(!quadtree_is_leaf(node)){
  	 if (quadtree_child(node, pos) != NULL)
    	return -1;

   	position = &node->children[pos];
	}else{
	  return -1;
	}

}

/*****************************************************************************
*                                                                            *
*  Allocate storage for the node.                                            *
*                                                                            *
*****************************************************************************/

if ((new_node = (QuadTreeNode *)malloc(sizeof(QuadTreeNode))) == NULL)
   return -1;

/*****************************************************************************
*                                                                            *
*  Insert the node into the tree.                                            *
*                                                                            *
*****************************************************************************/

new_node->data = (void *)data;

//Initialize the array of childrens to NULL because it doesn't has children

new_node->children =  NULL;
new_node->children_list = NULL;
new_node->visited = 0;
new_node->mass = 0.0;
new_node->cm.x = 0;
new_node->cm.y = 0;

*position = new_node;

/*****************************************************************************
*                                                                            *
*  Adjust the size of the tree to account for the inserted node.             *
*                                                                            *
*****************************************************************************/

tree->size++;

return 0;

}





/*****************************************************************************
*                                                                            *
*  ---------------------------- quadtree_rem_child ---------------------------  *
*                                                                            *
*****************************************************************************/

void quadtree_rem_child(QuadTree *tree, QuadTreeNode *node, int pos) {

QuadTreeNode         **position;

/*****************************************************************************
*                                                                            *
*  Do not allow removal from an empty tree.                                  *
*                                                                            *
*****************************************************************************/

if (quadtree_size(tree) == 0)
   return;

/*****************************************************************************
*                                                                            *
*  Determine where to remove nodes.                                          *
*                                                                            *
*****************************************************************************/

if (node == NULL)
   position = &tree->root;
else if (!quadtree_is_leaf(node))
	position = &node->children[pos];
else
	return;

/*****************************************************************************
*                                                                            *
*  Remove the nodes.                                                         *
*                                                                            *
*****************************************************************************/

if(*position != NULL){

	int i;

	for(i=0; i< NUM_CHILDREN; i++){
	
		quadtree_rem_child(tree, *position, i); 
	}

		
    if (tree->destroy != NULL) {

      /***********************************************************************
      *                                                                      *
      *  Call a user-defined function to free dynamically allocated data.    *
      *                                                                      *
      ***********************************************************************/

      tree->destroy((*position)->data);

    } 

 	free(*position);
    *position = NULL;
	

   /**************************************************************************
   *                                                                         *
   *  Adjust the size of the tree to account for the removed node.           *
   *                                                                         *
   **************************************************************************/

   tree->size--;

}

return;

}


void quadtree_mark(QuadTreeNode* node){
	node->visited = 1;
}

