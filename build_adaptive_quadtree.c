/**
 * Program to construct the quadtree
 */
#include "dlist.c"
#include "queue.c"
#include "stack.c"
#include "adaptive_quadtree.c"
#include <stdio.h>
#include <math.h>

#define MAXCELLS 15


double round_up(double x){

  return ceil(x);

}


double roud_down(double x){

  return floor(x);

}


int next_pos(double x, double m){

  double y = 0.0;
  double z = 0.0;

  y = x/m;

  double r = modf(y, &z);

  if(r > 0.5)
     z = z + 1;

  return z;

}


QuadTreeNode* init_child(QuadTreeNode* root, int pos){

  QuadTreeNode* child = (QuadTreeNode*)malloc(sizeof(QuadTreeNode));
  child->rect = (Rectangle*)malloc(sizeof(Rectangle));
  child->parent = root;
  child->visited = 0;
  int x1=0, y1=0, x2=0, y2=0;
  if(pos == 0){
    x1 = root->rect->pt1.x;
    y1 = root->rect->pt1.y;
    x2 = ( root->rect->pt1.x + root->rect->pt2.x )/2;
    y2 = ( root->rect->pt1.y + root->rect->pt2.y )/2;
  }else if(pos == 1){
    x1 = ( root->rect->pt1.x + root->rect->pt2.x )/2;
    y1 = root->rect->pt1.y;
    x2 = root->rect->pt2.x;
    y2 = ( root->rect->pt1.y + root->rect->pt2.y )/2;
  }else if(pos == 2){
    x1 = ( root->rect->pt1.x + root->rect->pt2.x )/2;
    y1 = ( root->rect->pt1.y + root->rect->pt2.y )/2;
    x2 = root->rect->pt2.x;
    y2 = root->rect->pt2.y;
  }else if(pos == 3){
    x1 = root->rect->pt1.x;
    y1 = ( root->rect->pt1.y + root->rect->pt2.y )/2;
    x2 = ( root->rect->pt1.x + root->rect->pt2.x )/2;
    y2 = root->rect->pt2.y;
  }
  child->rect->pt1.x = x1;
  child->rect->pt1.y = y1;
  child->rect->pt2.x = x2;
  child->rect->pt2.y = y2;
  child->data = NULL;
  child->mass = 0.0;
  child->cm.x= 0;
  child->cm.y= 0;
  return child;
}


struct point_t get_middle_point(Rectangle rect){

  int x1 = rect.pt1.x;
  int y1 = rect.pt1.y;
  int x2 = rect.pt2.x;
  int y2 = rect.pt2.y;

  int m = (x1 + x2)/2;
  int n = (y1 + y2)/2;

  struct point_t middle_point = {m,n};
  return middle_point;
}


int get_position(void* data, struct point_t middle_point){

  int pos = -1;
  int m = middle_point.x;
  int n = middle_point.y;
  struct cell_t *cell = (struct cell_t *)data;

  if( (cell->x <= m) && (cell->y <= n) ){
    pos = 0;
  }else if ( (cell->x > m) && (cell->y <= n) ){
    pos = 1;
  }else if ( (cell->x > m) && (cell->y > n) ){
    pos = 2;
  }else if ( (cell->x <= m) && (cell->y > n) ){
    pos = 3;
  }

  printf("get_position: position:%d, m:%d, n:%d\n", pos, m, n);
  return pos;
}


int get_position_forward(QuadTreeNode* node, struct point_t middle_point){

  int pos = -1;
  pos = get_position(node->data, middle_point);
  printf("get_position_forward: position:%d, m:%d, n:%d\n", pos, middle_point.x, middle_point.y);
  return pos;

}



void quadtree_insert(void* element, QuadTreeNode* node){
//... Try to insert particle i at node n in quadtree
//     ... By construction, each leaf will contain either
//    ... 1 or 0 particles

  struct point_t middle_point = {0, 0};
  //printf("middle_point: %d\n", middle_point.x);
  //printf("middle_point: %d\n", middle_point.y);
  if(quadtree_is_leaf(node)){
    if(node->data != NULL){
      //just_one particle
      //add n's four children to the Quadtree
      node->children = (QuadTreeNode **)malloc(NUM_CHILDREN * sizeof(QuadTreeNode*));
      //printf("quadtree_insert: %d\n", ((struct cell_t*)(node->data))->x);
      //printf("quadtree_insert: %d\n", ((struct cell_t*)(node->data))->y);
      int i;
      for(i=0; i < NUM_CHILDREN; i++){
	QuadTreeNode* child = init_child(node, i);
	node->children[i] = child;
      }
      //move the particle already in n into the child
      //in which it lies
      // struct point_t middle_point = get_middle_point(node->rect);

      middle_point = get_middle_point(*node->rect);
      int p = get_position_forward(node, middle_point);
      //printf("quadtree_insert: %d\n", ((struct cell_t*)(node->data))->x);
      //printf("quadtree_insert: %d\n", ((struct cell_t*)(node->data))->y);
      printf("quadtree_insert over leaf node (%d, %d). Recursive call...\n", ((struct cell_t*)(node->data))->x, ((struct cell_t*)(node->data))->y);
      node->children[p]->data = node->data;
      node->data = NULL;

      //let c be child in which particle i lies
      //QuadInsert(i,c)
      int q = get_position(element, middle_point);
      quadtree_insert(element, node->children[q]);
    }else{
      //... n is a leaf
      //store particle i in node n
      node->data = element;
      printf("quadtree_insert over empty leaf. Element (%d, %d) finding its place.\n", ((struct cell_t*)(node->data))->x, ((struct cell_t*)(node->data))->y);
      //printf("quadtree_insert:ya %d\n", ((struct cell_t*)(node->data))->x);
      //printf("quadtree_insert:ya %d\n", ((struct cell_t*)(node->data))->y);
    }
  }else{
    //more than one nodes in the subtree
    //determine which child c of node n particle i lies in
    //QuadInsert(i,c)
    middle_point = get_middle_point(*node->rect);
    int q = get_position(element, middle_point);
    quadtree_insert(element, node->children[q]);
  }
}


/* Constructs an adaptive quadtree from a quadtree
 * The quadtree children array is converted in a list 
 */

QuadTree* construct_adaptive_quadtree(QuadTree* tree){

  Queue queue;
  dlist_init(&queue, &free);
  enqueue(&queue, tree->root);
  //QuadTreeNode* current_node = malloc(sizeof(QuadTreeNode));
  QuadTreeNode** current_data = malloc(sizeof(QuadTreeNode*));

  while(dequeue(&queue, (void**)current_data) != -1){
    if(quadtree_is_leaf(*current_data)){
      if((*current_data)->data != NULL){
	printf("(%d, %d) %s\n",((struct cell_t*)(*current_data)->data)->x, ((struct cell_t*)(*current_data)->data)->y, " is a leaf");
      }else{
	printf("(%d, %d) %s\n", 0, 0, " is a leaf" );
      }
    }else{
      printf("(%d, %d) %s\n", 0, 0," has children");
      QuadTreeNode** children = (*current_data)->children;
      Queue* children_queue = malloc(sizeof(Queue));
      dlist_init(children_queue, &free);
      int i;
      for(i=0; i<NUM_CHILDREN; i++){
	enqueue(&queue, children[i]);
	if(quadtree_is_leaf(children[i])){
	  if((children[i])->data != NULL)
	    enqueue(children_queue, children[i]);
	}else
	  enqueue(children_queue, children[i]);
      }

      (*current_data)->children_list = children_queue;

      (*current_data)->children = NULL;
    }
  }
  return tree;
}



QuadTree* construct_quadtree(void** elements, int num_elements, Rectangle* rect){

  QuadTree* tree = (QuadTree*)malloc(sizeof(QuadTree));
  tree->root = (QuadTreeNode*)malloc(sizeof(QuadTreeNode));

  int i;
  tree->root->data = (void*) elements[0];
  tree->root->rect = rect;
  for(i=1; i < MAXCELLS; i++){
    quadtree_insert(elements[i], quadtree_root(tree));
  }
  return construct_adaptive_quadtree(tree);
}


DList* postorder(QuadTree* tree){

  Stack *s = malloc(sizeof(Stack));
  dlist_init(s, &free);

  Queue *q = malloc(sizeof(Queue));
  dlist_init(q, &free);
  push(s, tree->root);
  QuadTreeNode** current_data = malloc(sizeof(QuadTreeNode*));

  while(pop(s, (void**)current_data) != -1){
    if(quadtree_is_visited(*current_data)){
      enqueue(q, *current_data);
    }else{
      quadtree_mark(*current_data);
      push(s, *current_data);
      if(!quadtree_is_leaf(*current_data)){
	DList* children = (*current_data)->children_list;
	DListElmt* current_elem = dlist_head(children);

	while(current_elem != NULL){
	  push(s, current_elem->data);
	  current_elem = dlist_next(current_elem);

	}
      }
    }
  }
  return q;
}

/*	

int main(int argc, char *argv[]){
	
	struct cell_t** cells = ( struct cell_t ** ) malloc( (MAXCELLS) * sizeof(struct cell_t*) );
	struct cell_t *cell;
	
	int x=0, y=0;
	int num_cells = 0;
	int i;

	
	for(i=0; i < MAXCELLS; i++){
	
  		cell = (struct cell_t *) malloc( sizeof(struct cell_t) );	
  		cell->x = x;
  		cell->y = y;
  	  	
		cells[num_cells++] = cell;
	}
	
	struct point_t p1 = {0,0};
	struct point_t p2 = {20, 20};
	
	Rectangle rect = {p1, p2};
	
	cells[0]->x = 1;
	cells[0]->y = 0;
	
	cells[1]->x = 2;
	cells[1]->y = 0;

	cells[2]->x = 3;
	cells[2]->y = 0;
	
	cells[3]->x = 2;
	cells[3]->y = 1;
	
	cells[4]->x = 3;
	cells[4]->y = 1;
	
	cells[5]->x = 4;
	cells[5]->y = 1;
	
	cells[6]->x = 10;
	cells[6]->y = 1;
	
	cells[7]->x = 13;
	cells[7]->y = 1;
	
	cells[8]->x = 12;
	cells[8]->y = 5;
	
	cells[9]->x = 13;
	cells[9]->y = 5;
	
	cells[10]->x = 1;
	cells[10]->y = 5;
	
	cells[11]->x = 13;
	cells[11]->y = 6;
	
	cells[12]->x = 14;
	cells[12]->y = 6;
	
	cells[13]->x = 1;
	cells[13]->y = 10;
	
	cells[14]->x = 13;
	cells[14]->y = 9;

		
	QuadTree* tree = construct_quadtree((void **)cells, MAXCELLS, &rect);	

	
	
	Queue queue;
	dlist_init(&queue, &free);
	
	
	enqueue(&queue, tree->root);
	
	QuadTreeNode* current_node = malloc(sizeof(QuadTreeNode));
	
	QuadTreeNode** current_data = malloc(sizeof(QuadTreeNode*));
	
	int level = 0;
	
	while(dequeue(&queue, (void**)current_data) != -1){
		
	if(quadtree_is_leaf(*current_data)){
		if((*current_data)->data != NULL){
			printf("(%d, %d) %s\n",((struct cell_t*)(*current_data)->data)->x, ((struct cell_t*)(*current_data)->data)->y, " is a leaf, main function");
		}else{
			printf("(%d, %d) %s\n", 0, 0, " is a leaf , main function\n" );
		}
		
	}else{
		
		printf("(%d, %d) %s\n", 0, 0," has children, main function\n");
		DList* children = (*current_data)->children_list;
		
		DListElmt* current_elem = dlist_head(children);
		
		int s = children->size;
		printf("size adaptive list: %d, main function\n",s);
	
			while(current_elem != NULL){
					
			//printf("%d\n", *((int*)list_data(current_elem) ));

			enqueue(&queue, current_elem->data);
				
			current_elem = dlist_next(current_elem);					
		
			}
	
		}	
	}

	
	printf("\n\n%s\n", "Postorder Traversal:");
	
	DList *l = postorder(tree);
	
	DListElmt* current_elem = dlist_head(l);
	
			while(current_elem != NULL){
					
			//printf("%d\n", *((int*)list_data(current_elem) ));
	
			QuadTreeNode* node = (QuadTreeNode*)current_elem->data;
			
			if((node)->data != NULL){
				printf("(%d, %d) %s\n",((struct cell_t*)(node)->data)->x, ((struct cell_t*)(node)->data)->y, " is a leaf, main function: postorder");
				
			//We createa a new point to record the center of mass
			//and copy the coordinates of the cell to initialize it.
			//This established the initial condition for the cm calculation
			
		 	//node->cm = malloc(sizeof(struct point_t));
			(node->cm).x = ((struct cell_t*)(node->data))->x;
			(node->cm).y = ((struct cell_t*)(node->data))->y;
			node->mass = 1.0;
			 
				
			}else{
				printf("(%d, %d) %s\n", 0, 0, " is a leaf , main function: postorder\n" );
			
				double mass = 0.0;
				struct point_t cm = {0, 0};
				
				DList* children = node->children_list;
		
				DListElmt* current_child = dlist_head(children);
	
				while(current_child != NULL){				
							
					mass = mass + ((QuadTreeNode*)(current_child->data))->mass;
					cm.x = cm.x + (((QuadTreeNode*)(current_child->data))->mass)*( (((QuadTreeNode*)(current_child->data))->cm).x);
					cm.y = cm.y + (((QuadTreeNode*)(current_child->data))->mass)*( (((QuadTreeNode*)(current_child->data))->cm).y);
					
					current_child = dlist_next(current_child);					
				}
			
				node->mass = mass;
				//(node->cm).x = (cm.x / mass);
				(node->cm).x = next_pos(cm.x, mass);
				//(node->cm).y = (cm.y / mass);
				(node->cm).y = next_pos(cm.y , mass);
				
			}

			//enqueue(&queue, current_elem->data);
				
			current_elem = dlist_next(current_elem);					
		
			}
			
			
			
			printf("\n\n%s\n", "Postorder Traversal Results:");
	
			current_elem = dlist_head(l);
	
			while(current_elem != NULL){
					
				//printf("%d\n", *((int*)list_data(current_elem) ));
	
				QuadTreeNode* node = (QuadTreeNode*)current_elem->data;
			
				printf("Center of mass: (%d, %d)\n",((node)->cm).x, ((node)->cm).y);
				printf("Mass: %f \n",(node)->mass);	
			
				//enqueue(&queue, current_elem->data);
				
				current_elem = dlist_next(current_elem);					
		
			}
	
	
}
*/
