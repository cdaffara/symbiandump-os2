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
#include <unistd.h>
#include <glib/gstdio.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define	C2P(c)		((gpointer) ((long) (c)))
#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GPOINTER_TO_INT(p)	((gint)   (p))
#define TESTPASS	1
#define TESTFAIL	0

//Support for scanner
void hash_func(gpointer key,gpointer value,gpointer user_data)
{
	
}

void err_func(GScanner* scanner,gchar* message, gboolean error)
{
	char s[] = "unexpected string constant \"efgh\", expected number (float) - error";
	g_assert(!strcmp(s,message));
	
}

//Test for scanner
void tg_scanner_tests()
{
	GScanner *scanner;
	int i,j,fd;
	const gint TOKEN_OPEN = G_TOKEN_LEFT_BRACE;
	const gint TOKEN_CLOSE = G_TOKEN_RIGHT_BRACE;
	const gint TOKEN_INT = G_TOKEN_INT;
	const gint TOKEN_CH = G_TOKEN_STRING;
	
	gint cur_pos,err;
	GTokenValue val;
	GTokenType token;
	gchar* g_scanner_cur_value_str;
	gpointer op;
	char* g_scanner_peek_next_token_str;
	gchar ip_text[]="(30)";
	
	err = TRUE;
	scanner = g_scanner_new(NULL);
	fd = g_open ("c:\scanfile.txt", O_RDONLY, 0);
	g_scanner_input_file (scanner, fd);
	scanner->config->symbol_2_token = TRUE;
	scanner->msg_handler = err_func;
	
	g_scanner_scope_add_symbol(scanner,0,"leftbrace",GUINT_TO_POINTER (TOKEN_OPEN));
	g_scanner_scope_add_symbol(scanner,0,"rightbrace",GUINT_TO_POINTER (TOKEN_CLOSE));
	g_scanner_scope_add_symbol(scanner,0,"intval",GUINT_TO_POINTER (TOKEN_INT));
	g_scanner_scope_add_symbol(scanner,0,"strval",GUINT_TO_POINTER (TOKEN_CH));

	for(i=0;i<10;i++)
	{
	token = g_scanner_get_next_token (scanner);
	cur_pos = g_scanner_cur_position (scanner);
	}
	
	//Tests begin here:
	
	g_assert(g_scanner_cur_line(scanner) == 2);  //g_scanner_cur_line test
	g_assert(g_scanner_cur_position(scanner) == 11);	//g_scanner_cur_position test
	g_assert(g_scanner_cur_token (scanner) == G_TOKEN_STRING); //g_scanner_cur_token test
	val = g_scanner_cur_value (scanner); //g_scanner_cur_value test
	g_scanner_cur_value_str = val.v_string;
	g_assert(!strcmp(g_scanner_cur_value_str,"efgh"));
	g_assert(g_scanner_peek_next_token(scanner) == G_TOKEN_LEFT_BRACE);	//g_scanner_peek_next_token test
	g_assert(g_scanner_scope_lookup_symbol(scanner,0,"intval") == GUINT_TO_POINTER (TOKEN_INT));	//g_scanner_scope_lookup_symbol test
	g_assert(g_scanner_lookup_symbol(scanner,"intval") == GUINT_TO_POINTER (TOKEN_INT));	//g_scanner_lookup_symbol test
	g_assert(g_scanner_set_scope(scanner,14) == 0);	//g_scanner_set_scope..set to 14
	
	g_scanner_scope_remove_symbol(scanner,14,"leftbrace");
	g_assert(g_scanner_lookup_symbol(scanner,"leftbrace") == NULL);	//g_scanner_scope_remove_symbol test
	
	g_scanner_scope_foreach_symbol(scanner,14,hash_func,GUINT_TO_POINTER(G_TOKEN_LEFT_PAREN));
	g_assert(g_scanner_lookup_symbol(scanner,"intval") == NULL);	//g_scanner_scope_foreach_symbol test
	
	
	g_scanner_unexp_token(scanner,G_TOKEN_FLOAT,NULL,NULL,"float","error",err);
	
	//Test for g_scanner_input_text
	g_scanner_set_scope(scanner,0);
	g_scanner_input_text(scanner,ip_text,strlen(ip_text));
	for(i=0;i<3;i++)
	{
	token = g_scanner_get_next_token (scanner);
	}
	g_assert(g_scanner_cur_token (scanner) == G_TOKEN_RIGHT_PAREN); //g_scanner_input_text test
	
	close(fd);
	g_scanner_destroy (scanner);
	
}



int main (int argc,char *argv[])
{

	#ifdef SYMBIAN
 
 	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
 	tg_scanner_tests();
 #ifdef SYMBIAN
  testResultXml("tscanner");
#endif /* EMULATOR */
 	return 0;
}