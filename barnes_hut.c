/**
 * Program that implements barnes_hut algorithm
 */
#include "build_adaptive_quadtree.c"

#include <stdio.h>
#include <math.h>

#define MAXCELLS 15
#define THETA 1.0

void compute_mass(QuadTree* tree){
  DList *l = postorder(tree);
  //Calculation of mass and center of mass
  DListElmt* current_elem = dlist_head(l);
  while(current_elem != NULL){
    QuadTreeNode* node = (QuadTreeNode*)current_elem->data;
    if((node)->data != NULL){
      (node->cm).x = ((struct cell_t*)(node->data))->x;
      (node->cm).y = ((struct cell_t*)(node->data))->y;
      node->mass = 1.0;
    }else{
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
      (node->cm).x = next_pos(cm.x, mass);
      (node->cm).y = next_pos(cm.y , mass);
    }
    current_elem = dlist_next(current_elem);
  }
}



double force(struct cell_t *cell, QuadTreeNode* node){

  double f = 0.0;
  double d = 0.0;
  double dx = cell->x - node->cm.x;
  double dy = cell->y - node->cm.y;

  d = sqrt(pow( dx, 2) + pow(dy, 2));

  printf("node->cm: (%d, %d)\n", node->cm.x, node->cm.y);
  printf("dx: %lf\n", dx);
  printf("dy: %lf\n", dy);
  printf("d: %lf\n", d);

  if (d != 0.0)
    //This is the potential function
    //f = (pow( (1.0/d), 12) - pow( (1.0/d), 6) )  + (0.2*exp(-d/2.0)/(d/2.0));	
    f = (1/d);
  printf("force: 1/d: %lf\n",f);
  return f;
}


double tree_force(struct cell_t* cell, QuadTreeNode* node){

  double f = 0.0;
  if(quadtree_is_leaf(node))
    f = force(cell, node);
  else{

    double d = 0.0;
    double dx = cell->x - node->cm.x;
    double dy = cell->y - node->cm.y;

    d = sqrt(pow( dx, 2) + pow(dy, 2));

    printf("tree_force: not a leaf\n");
    printf("node->cm: (%d, %d)\n", node->cm.x, node->cm.y);
    printf("dx: %lf\n", dx);
    printf("dy: %lf\n", dy);
    printf("d: %lf\n", d);

    double D = node->rect->pt2.x - node->rect->pt1.x;

    printf("D: %lf\n", D);
    printf("D/d: %lf\n", D/d);

    if( D/d < THETA)
      f = force(cell, node);
    else{

      DList* children = node->children_list;

      DListElmt* current_elem = dlist_head(children);

      while(current_elem != NULL){
	printf("f: %lf\n", f);
	f = f + tree_force(cell, (QuadTreeNode*)(current_elem->data));
	current_elem = dlist_next(current_elem);
      }
    }

  }
  return f;
}


double* global_force(struct cell_t** cells, QuadTree* tree){

  double* forces = malloc(sizeof(double)*MAXCELLS);
  int i;

  for(i=0; i < MAXCELLS; i++){
    printf("Calculating force for cells[%d] = (%d, %d)\n",i, cells[i]->x, cells[i]->y);
    forces[i] = tree_force(cells[i], tree->root);
  }
  return forces;
}


int main(int argc, char *argv[]){

  struct cell_t** cells = ( struct cell_t ** ) malloc( (MAXCELLS) * sizeof(struct cell_t*) );
  struct cell_t *cell;

  int x=0, y=0;
  int num_cells = 0;
  int i;
  //Creation of the cells
  for(i=0; i < MAXCELLS; i++){
    cell = (struct cell_t *) malloc( sizeof(struct cell_t) );
    cell->x = x;
    cell->y = y;
    cells[num_cells++] = cell;
  }
  //Definition of the dimensions of the grid

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
  compute_mass(tree);
  double* forces = global_force(cells, tree);

  for(i=0; i < MAXCELLS; i++)
    printf("force[%d]: %lf\n", i, forces[i]);

}
