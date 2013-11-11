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
* Description:
*
*/



#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <stdio.h>
#include <string.h>
#include "glib.h"

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

struct data
{
	char a[50],b[50],c[50];
};

void function(GQuark key_id,gpointer data,gpointer user_data)
{
	int *i = (int *)user_data;
	(*i)++;
}

void dataset_test()
{
	struct data *d = g_malloc(sizeof(struct data));
	gchar *str1,*str2,*str3,*str4,*str5,*str6;
	int user_data = 0;
	
	GQuark q1,q2,q3;
	
	strcpy(d->a,"test1");
	strcpy(d->b,"test2");
	strcpy(d->c,"test3");
	
	q1 = g_quark_from_string(d->a);
	q2 = g_quark_from_string(d->b);
	q3 = g_quark_from_string(d->c);
	
	g_dataset_id_set_data_full(d,q1,d->a,NULL);
	g_dataset_id_set_data_full(d,q2,d->b,NULL);
	g_dataset_id_set_data_full(d,q3,d->c,NULL);
	
	str1 = g_dataset_id_get_data(d,q1);
	str2 = g_dataset_id_get_data(d,q2);
	str3 = g_dataset_id_get_data(d,q3);
	
	// These assertions will check whether g_dataset_id_set_data_full & 
	// g_dataset_id_get_data is sucessful or not
	g_assert(!strcmp(str1,d->a));
	g_assert(!strcmp(str2,d->b));
	g_assert(!strcmp(str3,d->c));
	
	str4 = g_dataset_id_remove_no_notify(d,q1);
	str5 = g_dataset_id_get_data(d,q1);
	
	//This assertion will check if g_dataset_id_remove_no_notify is sucessful or not
	g_assert(!strcmp(str4,d->a));
	g_assert(str5 == NULL);
	
	g_dataset_foreach(d,function,&user_data);
	
	
	//This assertion will check if g_dataset_foreach is sucessful or not
	g_assert(user_data == 2);
	
	g_dataset_destroy(d);	
	
	str6 = g_dataset_id_get_data(d,q2);
	
	//This assertion will check if g_dataset_destroy is successful or not
	g_assert(str5 == NULL);
	
}


int main (int   argc,
      char *argv[])
{
	#ifdef SYMBIAN
	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
	#endif /*SYMBIAN*/
	
	dataset_test();
	
	#if SYMBIAN
  	testResultXml("dataset_test");
  	#endif /* EMULATOR */
	
	return 0;
}