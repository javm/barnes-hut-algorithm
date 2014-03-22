/*
 * Program:
		stack
	Description:
	        Implementation of a queue
	Copyright (C) 2009 José Antonio Villarreal

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
*  -------------------------------- stack.c --------------------------------  *
*                                                                            *
*****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include "stack.h"
/*#include "dlist.c"*/


/*****************************************************************************
*                                                                            *
*  ------------------------------- push ------------------------------    *
*                                                                            *
*****************************************************************************/

int push(Stack *stack, const void *data) {

   return dlist_ins_prev(stack, dlist_head(stack), data);
}


int pop(Stack *stack, void **data){

   return dlist_remove(stack, stack->head, data);
}
