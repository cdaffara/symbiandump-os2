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
#include <glib/gprintf.h>
#include <stdlib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define	C2P(c)		((gpointer) ((long) (c)))
#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GPOINTER_TO_INT(p)	((gint)   (p))
#define TESTPASS	1
#define TESTFAIL	0


//Test for g_strcanon
void tg_strcanon()
{
	gchar string1[]="aabbccdd";
	gchar string2[]="aabbccdd";
	gchar* valid;
	gchar* teststring1;
	gchar* teststring2;
  
 	//Testing for a valid char available in the string
 	//The invalid chars must be replaced
 	valid="cc";
 	teststring1=g_strcanon (string1, valid, 'x');
 	g_assert(!strcmp(teststring1,"xxxxccxx"));

	//Testing for a valid char not-available in the string
	//All the chars must be replaced
	valid="nn";
	teststring2=g_strcanon(string2,valid,'x');
	g_assert(!strcmp(teststring2,"xxxxxxxx"));
}

//Test for g_strcasecmp
void tg_strcasecmp()
{
 	gint strcasecmp_eq,strcasecmp_gr8,strcasecmp_less;
 	//Testing for equal strings,zero must be returned
 	strcasecmp_eq=g_strcasecmp ("abcd123","abcd123");
 	g_assert(strcasecmp_eq==0);

 	//Testing for un-equal strings,left greater,positive value must be returned
 	strcasecmp_gr8=g_strcasecmp ("abcd123","abcd");
 	g_assert(strcasecmp_gr8>0);
 	
 	//Testing for un-equal strings,right greater,negative value must be returned
 	strcasecmp_less=g_strcasecmp ("abcd","abcd123");
 	g_assert(strcasecmp_less<0);
 	
 }
 
 //Test for g_strdown
 void tg_strdown()
 {
 	gchar input[]="ABCDef";
 	gchar* upperToLower=g_strdown(input);
 	g_assert(!strcmp(upperToLower,"abcdef"));	
 }
 
 //Test for g_string_append_c
 void tg_string_append_c()
 {
	GString* obj=(GString*)malloc(sizeof(GString));
	gchar ip[]="abcd";
  	obj->str=ip;
 	obj->len=strlen(ip);
 	obj->allocated_len=10;
 	obj=g_string_append_c(obj,'e');
 	g_assert(!strcmp((obj->str),"abcde"));
 }
 
 //Test for g_string_ascii_down
 void tg_string_ascii_down()
 {
 	GString* obj=(GString*)malloc(sizeof(GString));
 	gchar ip[]="ABc12DeF";
 	obj->str=ip;
 	obj->len=strlen(ip);
 	obj->allocated_len=10;
 	obj=g_string_ascii_down(obj);
 	g_assert(!strcmp((obj->str),"abc12def"));
 }
 
 //Test for g_string_ascii_up
 void tg_string_ascii_up()
 {
 	GString* obj=(GString*)malloc(sizeof(GString));
 	gchar ip[]="ABc12DeF";
 	obj->str=ip;
 	obj->len=strlen(ip);
 	obj->allocated_len=10;
 	obj=g_string_ascii_up(obj);
 	g_assert(!strcmp((obj->str),"ABC12DEF"));
 }
 
 //Test for g_string_down
 void tg_string_down()
 {
 	GString* obj=(GString*)malloc(sizeof(GString));
 	gchar ip[]="ABc12DeF";
 	obj->str=ip;
 	obj->len=strlen(ip);
 	obj->allocated_len=10;
 	obj=g_string_down(obj);
 	g_assert(!strcmp((obj->str),"abc12def"));
 }

 //Test for g_string_hash
 void tg_string_hash()
 {
 	GString* obj=(GString*)malloc(sizeof(GString));
 	guint g_string_hash_result1,g_string_hash_result2;
 	guint g_string_hash_result3,g_string_hash_result4;
 	gchar ip1[]="ABC12";
 	gchar ip2[]="abc12";
 	obj->allocated_len=10;
 	
 	obj->str=ip1;
 	obj->len=strlen(ip1);
 	g_string_hash_result1=g_string_hash(obj);
 	g_string_hash_result2=g_string_hash(obj);
 	g_assert(g_string_hash_result1==g_string_hash_result2);
 	
 	obj->str=ip2;
 	obj->len=strlen(ip2);
 	g_string_hash_result3=g_string_hash(obj);
 	g_string_hash_result4=g_string_hash(obj);
 	g_assert(g_string_hash_result3==g_string_hash_result4);
 	
 	g_assert(g_string_hash_result1 != g_string_hash_result3);
 	
 }
 
 
 //Test for g_string_prepend_c
  void tg_string_prepend_c()
 {
	GString* obj=(GString*)malloc(sizeof(GString));
	gchar ip[]="abcd";
  	obj->str=ip;
 	obj->len=strlen(ip);
 	obj->allocated_len=10;
 	obj=g_string_prepend_c(obj,'e');
 	g_assert(!strcmp((obj->str),"eabcd"));
 }
 
 
 //Test for g_string_up
 void tg_string_up()
 {
 	GString* obj=(GString*)malloc(sizeof(GString));
 	gchar ip[]="ABc12DeF";
 	obj->str=ip;
 	obj->len=strlen(ip);
 	obj->allocated_len=10;
 	obj=g_string_up(obj);
 	g_assert(!strcmp((obj->str),"ABC12DEF"));
 }
 
 
 //Test for g_string_prepend_unichar
 void tg_string_prepend_unichar()
 {
	GString* obj=(GString*)malloc(sizeof(GString));
	gchar ip[]="abcd";
  	obj->str=ip;
 	obj->len=strlen(ip);
 	obj->allocated_len=10;
 	obj=g_string_prepend_unichar(obj,'e');
 	g_assert(!strcmp((obj->str),"eabcd"));
 }
 
 //Test for g_strip_context
 void tg_strip_context()
 {
 	gchar msgid[]="abc|defgh";
 	gchar msgval[]="abc|defgh";
 	const gchar* op;
 	op=g_strip_context(msgid,msgid);
 	g_assert(!strcmp(op,"defgh"));	
 }
 
 //Test for g_strjoin
 void tg_strjoin()
 {
 	gchar sep[]="#&#";
	gchar a[]="abc";
	gchar b[]="def";
 	gchar* op=g_strjoin(sep,a,b,NULL);
 	g_assert(!strcmp(op,"abc#&#def"));
 }
 
 
 //Test for g_strncasecmp
void tg_strncasecmp()
{
 	gint strncasecmp_eq,strncasecmp_gr8,strncasecmp_less;
 	//Testing for equal strings,zero must be returned
 	strncasecmp_eq=g_strncasecmp ("abcd123","abcd123",10);
 	g_assert(strncasecmp_eq==0);

 	//Testing for un-equal strings,left greater,positive value must be returned
 	strncasecmp_gr8=g_strncasecmp ("abcd123","abcd",4);
 	g_assert(strncasecmp_gr8==0);
 	
 	//Testing for un-equal strings,right greater,negative value must be returned
 	strncasecmp_less=g_strncasecmp ("abcd","abcd123",6);
 	g_assert(strncasecmp_less<0);
 	
 }
 
 
 //Test for g_strnfill
 void tg_strnfill()
 {
 	gchar fill='x';
	gsize size=10;
	gchar* strnfill_buf_null;
	gchar* strnfill_buf=g_strnfill(size,fill);
 	g_assert(!strcmp(strnfill_buf,"xxxxxxxxxx"));
 
 	size=0;
	strnfill_buf_null=g_strnfill(size,fill);
 	g_assert(!strcmp(strnfill_buf_null,""));
 
 }
 
 //Test for g_strreverse
 void tg_strreverse()
 {
 	gchar ip[]="abCdeF123";
	gchar* strreverse_op=g_strreverse(ip);
 	g_assert(!strcmp(strreverse_op,"321FedCba"));
 
 }
 
 
 //Test for g_strup
 void tg_strup()
 {
 	gchar ip[]="Abc12deF";
 	gchar* strup=g_strup(ip);
 	g_assert(!strcmp(strup,"ABC12DEF"));
 }
 
 
 //Test for g_pattern_match_string
 void tg_pattern_match_string()
 {
 	gchar pattern_str[]="abcdefghijklmnopqrstuvwxyz";
 	gchar match_str_pos[]="abcdefghijklmnopqrstuvwxyz"; //Proper a-z
 	gchar match_str_neg[]="abcdefghjiklmnopqrstuvwxyz";	//i and j interchanged
 	
 	GPatternSpec* spec=g_pattern_spec_new(pattern_str);
 	g_assert(g_pattern_match_string(spec,match_str_pos));
 	g_assert(!g_pattern_match_string(spec,match_str_neg));
 	
 }

 //Called by g_printf_string_upper_bound for a va-list
 void test_arg_string_upper_bound(gchar* fmt,...)
 {
 	va_list ap;
	va_start(ap,fmt);
	g_assert(g_printf_string_upper_bound(fmt,ap)==27);
	va_end(ap);
 }
 //Test for g_printf_string_upper_bound
 void tg_printf_string_upper_bound()
 {
 	test_arg_string_upper_bound("%d\n%s\t%f",9999999,"Abcd#%@",999.999999);
 }


 //Test for g_sprintf
 void tg_sprintf()
 {
 	gchar sprintf_converted[100];
 	gint sprintf_return_val=g_sprintf(sprintf_converted,"%d%s%f",9999999,"Abcd#%@",999.999999);
 	g_assert(sprintf_return_val==24);
 }



int main (int argc,char *argv[])
{

	#ifdef SYMBIAN
  	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
 	tg_strcanon();
	tg_strcasecmp();
	tg_strdown();
	tg_string_append_c();
	tg_string_ascii_down();
	tg_string_ascii_up();
	tg_string_down();
	tg_string_hash();
	tg_string_prepend_c();
	tg_string_prepend_unichar();
	tg_string_up();
	tg_strip_context();
	tg_strjoin();
	tg_strncasecmp();
	tg_strnfill();
	tg_strreverse();
	tg_strup();
	tg_pattern_match_string();
	tg_printf_string_upper_bound();
	tg_sprintf();
	
	#ifdef SYMBIAN
  testResultXml("tgstring");
#endif /* EMULATOR */
 	return 0;
}