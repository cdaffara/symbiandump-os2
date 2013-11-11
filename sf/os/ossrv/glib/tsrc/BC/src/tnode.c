/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330,
* Boston, MA 02111-1307, USA.
*
* Description:  ?Description
*
*/


#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN


#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <fcntl.h>
#include <goption.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define	C2P(c)		((gpointer) ((long) (c)))
#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GPOINTER_TO_INT(p)	((gint)   (p))
#define TESTPASS	1
#define TESTFAIL	0


//Support func for node tests
void myFun(GNode *node,gpointer data)
{
	node->data=data;	
}

gpointer cpy_func(gconstpointer src, gpointer data)
{
	return data;
}

// g_node_tests
void tg_node_tests()
{
	GNode *root;
	GNode *node_B;
	GNode *node_D;
	GNode *node_F;
	GNode *node_G;
	GNode *node_J;
	GNode *node_first_sibling;
	GNode *node_get_root;
	GNode *node_insert_after;
	GNode *node_last_sibling;
	GNode *copy_deep;
	gint node_children_foreach;
	gpointer dat;
	
	//All allocations done thro the allocators..Hence they also get tested!
	GAllocator* alloc = g_allocator_new ("node_alloc",5000);
	g_node_push_allocator (alloc);
	
	
	root = g_node_new (C2P ('A'));
	
	//deep_copy test
	copy_deep = g_node_copy_deep (root, cpy_func, C2P ('P'));
	g_assert (copy_deep->data==C2P('P'));
	
	node_B = g_node_new (C2P ('B'));
  	g_node_append (root, node_B);
  	g_node_append_data (node_B, C2P ('E'));
 	g_node_prepend_data (node_B, C2P ('C'));
 	node_D = g_node_new (C2P ('D'));
 	g_node_insert (node_B, 1, node_D); 
	node_F = g_node_new (C2P ('F'));
  	g_node_append (root, node_F);
 	node_G = g_node_new (C2P ('G'));
 	g_node_append (node_F, node_G);
	node_J = g_node_new (C2P ('J'));
	g_node_prepend (node_G, node_J);
	g_node_insert (node_G, 42, g_node_new (C2P ('K')));
	g_node_insert_data (node_G, 0, C2P ('H'));
 	g_node_insert (node_G, 1, g_node_new (C2P ('I')));
 	
 	
   /* we have built:                   A
   *                                 /   \
   *                               B       F
   *                             / | \       \
   *                           C   D   E       G
   *                                         / /\ \
   *                                       H  I  J  K
   */
   
   //Test for g_node_child_index
 	g_assert(g_node_child_index(node_B,C2P ('E'))==2);	
 	g_assert(g_node_child_index(root,C2P ('E'))==-1);	
 	g_assert(g_node_child_index(node_G,C2P ('K'))==3);	

	//Test for g_node_children_foreach
	//G_TRAVERSE_ALL test..sets C,D,E to Z
	g_node_children_foreach(node_B,G_TRAVERSE_ALL,myFun,C2P ('Z'));
	node_children_foreach=g_node_child_index(node_B,C2P ('Z'));
	g_assert(node_children_foreach==0);
	//G_TRAVERSE_LEAVES test..tries to set F to Y but fails cause its not a leaf
	g_node_children_foreach(node_F,G_TRAVERSE_LEAVES,myFun,C2P ('Y'));
	node_children_foreach=g_node_child_index(node_F,C2P ('Y'));
	g_assert(node_children_foreach==-1);
	//G_TRAVERSE_NON_LEAVES test..tries to set G to Z but fails cause its a leaf
	g_node_children_foreach(node_G,G_TRAVERSE_NON_LEAVES,myFun,C2P ('Z'));
	node_children_foreach=g_node_child_index(node_G,C2P ('Z'));
	g_assert(node_children_foreach!=0);
	
	 	
   /* now we have:	                   A
   *                                 /   \
   *                               B       F
   *                             / | \       \
   *                           	Z  Z   Z       G
   *                                         / /\ \
   *                                       H  I  J  K
   */
	
	//Test for g_node_first_sibling
	node_first_sibling=g_node_first_sibling(node_D->next);
	g_assert(node_first_sibling->data==C2P('Z')); 	
	
	//Test for g_node_get_root
	node_get_root=g_node_get_root(node_J);
	g_assert(node_get_root->data==C2P('A'));
	 
	//Test for g_node_insert_after
	node_insert_after = g_node_new (C2P ('X'));
	g_node_insert_after(node_B,node_D,node_insert_after);
	g_assert(g_node_child_index(node_B,C2P ('X'))==2);	
	
	
   /* now we have:	                   A
   *                                 /   \
   *                               B       F
   *                            /  | \ \    \
   *                           Z   Z  X  Z    G
   *                                        / /\ \
   *                                       H I  J K
   */
	
	//Test for g_node_is_ancestor
	g_assert(g_node_is_ancestor(root,node_G)); //Grandparent
	g_assert(g_node_is_ancestor(node_G,node_J)); //Parent
	g_assert(!g_node_is_ancestor(node_F,node_B)); //Sibling-negative test
	
	//Test for g_node_last_sibling
	node_last_sibling=g_node_last_sibling(node_D);
	g_assert(node_last_sibling->data==C2P('Z')); //Last sibling for D

	
	g_node_destroy (root);
	g_node_pop_allocator ();
}


int main (int argc,char *argv[])
{

	#ifdef SYMBIAN
 
 	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
 	tg_node_tests();
 
#ifdef SYMBIAN
  testResultXml("tnode");
#endif /* EMULATOR */
 	return 0;
}