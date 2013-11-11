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
#include <wchar.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define SIZE	30

void tg_utf8_prev_char()
{
	FILE* fp;
	int i = 0;
	gchar ip[SIZE];
	gchar *g_utf8_prev_char_op;
	gchar *input;
	
	fp = fopen ("c:\\utf8_hindi.txt","r");
 	while ( L'\n' != (ip[i] = fgetwc (fp)))
 		i++;
 	fclose (fp);
 	input = &ip[2];
 	
 	g_utf8_prev_char_op = g_utf8_prev_char (input);
	g_assert ( g_utf8_prev_char_op[0] == 0x15);
}

void tg_utf8_offset_to_pointer()
{
 	FILE *fp;
 	int i = 0;
	gchar ip[SIZE];
	gchar *g_utf8_offset_to_pointer_op;
	gchar *input;
	
 	fp = fopen ("c:\\utf8_hindi.txt","r");
 	while ( L'\n' != (ip[i] = fgetwc (fp)))
 		i++;
 	fclose (fp);
 	
 	input = &ip[1];
	g_utf8_offset_to_pointer_op = g_utf8_offset_to_pointer (input ,6);
	g_assert ( g_utf8_offset_to_pointer_op[0] == 0x24);
}

void tg_utf8_pointer_to_offset()
{
 	FILE *fp;
 	int i = 0;
	gchar ip[SIZE];
	glong g_utf8_pointer_to_offset_op;
	gchar *input;
	gchar *feed;
	
 	fp = fopen ("c:\\utf8_hindi.txt","r");
 	while ( L'\n' != (ip[i] = fgetwc (fp)))
 		i++;
 	fclose (fp);
 	
 	input = &ip[1];
 	feed = &ip[7];
	g_utf8_pointer_to_offset_op = g_utf8_pointer_to_offset (input ,feed);
	g_assert ( g_utf8_pointer_to_offset_op == 6);
}

void tg_utf8_strncpy()
{
 	FILE* fp;
	int i = 0;
 	gchar ip[SIZE];
 	gchar feed[15];
	unsigned int op[]=
	{
		0x15,0x24,0x1A,0x41,0x30
	};
 	fp = fopen ("c:\\utf8_hindi.txt","r");
 	while ( L'\n' != (ip[i] = fgetwc (fp)))
 		i++;
 	fclose (fp);
 	
	g_utf8_strncpy (feed ,&ip[1] ,5);
	
	for(i = 0;i < 5;i++)
	{
		g_assert (feed[i] == op[i]);
	}
}

void tg_utf8_strrchr()
{
	FILE* fp;
	int i = 0;
	gchar ip[SIZE];
	gchar *input;
	gchar *g_utf8_strrchr_op;
	
 	fp = fopen ("c:\\utf8_hindi.txt","r");
 	while ( L'\n' != (ip[i] = fgetwc (fp)))
 		i++;
 	fclose (fp);
 	
 	input = &ip[1];
 	g_utf8_strrchr_op = g_utf8_strrchr (input ,-1 , 0x1A);
	g_assert (g_utf8_strrchr_op[1] == 0x41);
}

void tg_utf8_collate_key_for_filename()
{
	gchar input[] = "ptr.txt";
	gchar *g_utf8_collate_key_for_filename_op;
	g_utf8_collate_key_for_filename_op = g_utf8_collate_key_for_filename (input ,6);
	g_assert (g_utf8_collate_key_for_filename_op[0] == 'A');
}	

void tg_filename_from_utf8()
{
	FILE* fp;
	int i = 0;
	gchar ip[SIZE];
	gchar *input;
	gsize bytes_read = 0;
    gsize bytes_written = 0;
    gchar *g_filename_from_utf8_op;
    unsigned int op[]=
	{
		0x15,0x24,0x1A,0x41,0x30
	};
	
    fp = fopen ("c:\\utf8_hindi.txt","r");
 	while ( L'\n' != (ip[i] = fgetwc (fp)))
 		i++;
 	fclose (fp);

 	input = &ip[1];
 	g_filename_from_utf8_op = g_filename_from_utf8 (input ,5 ,&bytes_read ,&bytes_written ,NULL);
	
	for(i = 0;i < 5;i++)
	{
		g_assert (g_filename_from_utf8_op[i] == op[i]);
	}
}	


void tg_filename_to_utf8()
{
	FILE* fp;
	int i = 0;
	gchar ip[SIZE];
	gchar *input;
	gsize bytes_read = 0;
    gsize bytes_written = 0;
    gchar *g_filename_to_utf8_op;
    unsigned int op[]=
	{
		0x15,0x24,0x1A,0x41,0x30
	};
	
    fp = fopen ("c:\\utf8_hindi.txt","r");
 	while ( L'\n' != (ip[i] = fgetwc (fp)))
 		i++;
 	fclose (fp);

 	input = &ip[1];
 	g_filename_to_utf8_op = g_filename_to_utf8 (input ,5 ,&bytes_read ,&bytes_written ,NULL);
	
	for(i = 0;i < 5;i++)
	{
		g_assert (g_filename_to_utf8_op[i] == op[i]);
	}
}
	
int main (int argc,char *argv[])
{

	#ifdef SYMBIAN
 
 	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
	tg_utf8_prev_char ();
	tg_utf8_offset_to_pointer();
	tg_utf8_pointer_to_offset();
 	tg_utf8_strncpy();
 	tg_utf8_strrchr();
 	tg_utf8_collate_key_for_filename();
 	tg_filename_from_utf8();
 	tg_filename_to_utf8();
 	#ifdef SYMBIAN
  testResultXml("tutf8");
#endif /* EMULATOR */
 	return 0;
}