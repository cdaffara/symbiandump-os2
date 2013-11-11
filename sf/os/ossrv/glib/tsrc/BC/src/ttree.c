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

//Support for gtree
static gint my_compare (gconstpointer a,gconstpointer b,gpointer data)
{
  const char *cha = a;
  const char *chb = b;

  return *cha - *chb;
}

static gint my_traverse (gpointer key,gpointer value,gpointer data)
{
  char *ch = key;
  char *ch1 = value;
  g_assert (key == value);
  return FALSE;
}

//Tests for gtree
void tg_tree_tests ()
{

	GTree *tree;
	char chars[62];
	char c='a';
	char err='1';
	gpointer d,op;
	int i,j;
	char del='c';
	char key='f';
	char val='z';
		
  	tree = g_tree_new_with_data (my_compare,&c);
  	i = 0;
	for (j = 0; j < 26; j++, i++)
    {
      chars[i] = 'A' + j;
      g_tree_insert (tree, &chars[i], &chars[i]);
    }
    
    for (j = 0; j < 26; j++, i++)
    {
      chars[i] = 'a' + j;
      g_tree_insert (tree, &chars[i], &chars[i]);
    }
    
    //Test for g_tree_lookup...srch for value for 'a'
	g_assert(*(gchar*)( g_tree_lookup (tree ,&c)) == 'a');
	g_assert(g_tree_lookup (tree ,&err) == NULL);

	//Test for g_tree_lookup_extended...srch for value for 'a'
	g_assert( g_tree_lookup_extended(tree ,&c ,&d ,&op));
	g_assert( !g_tree_lookup_extended(tree ,&err ,&d ,&op));

	//Test for g_tree_replace...replace f/f with f/z
	g_tree_replace(tree,&key,&val);
	g_assert(*(gchar*)( g_tree_lookup (tree ,&key)) == 'z');

	//Test for g_tree_steal...remove f/z
	g_assert(g_tree_steal(tree ,&key));
	
	//Test for g_tree_traverse...logging occurs in the traversal function
	g_tree_traverse(tree,my_traverse,G_IN_ORDER,&del);

}

int main (int argc,char *argv[])
{

	#ifdef SYMBIAN
 
 	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
 	tg_tree_tests();
 #ifdef SYMBIAN
  testResultXml("ttree");
#endif /* EMULATOR */
 	return 0;
}