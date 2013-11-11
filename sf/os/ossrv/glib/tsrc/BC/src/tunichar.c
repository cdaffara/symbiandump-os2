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
#include <stdlib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/

#define	C2P(c)		((gpointer) ((long) (c)))
#define GINT_TO_POINTER(i)	((gpointer)  (i))
#define GPOINTER_TO_INT(p)	((gint)   (p))
#define TESTPASS	1
#define TESTFAIL	0

//Test for tg_unichar_type
void tg_unichar_type()
{
 	g_assert (g_unichar_type (0x41) == G_UNICODE_UPPERCASE_LETTER);
 	g_assert (g_unichar_type (0x63) == G_UNICODE_LOWERCASE_LETTER);
 	g_assert (g_unichar_type (0x30) == G_UNICODE_DECIMAL_NUMBER);
 	g_assert (g_unichar_type (0x2d) == G_UNICODE_DASH_PUNCTUATION);
 	g_assert (g_unichar_type (0x24) == G_UNICODE_CURRENCY_SYMBOL);
}

//Test for tg_unichar_break_type
void tg_unichar_break_type()
{

	g_assert (g_unichar_break_type (0x0d) == G_UNICODE_BREAK_CARRIAGE_RETURN);
	g_assert (g_unichar_break_type (0x0a) == G_UNICODE_BREAK_LINE_FEED);
	g_assert (g_unichar_break_type (0x20) == G_UNICODE_BREAK_SPACE);
	g_assert (g_unichar_break_type (0x2d) == G_UNICODE_BREAK_HYPHEN);
	g_assert (g_unichar_break_type (0x21) == G_UNICODE_BREAK_EXCLAMATION);
	g_assert (g_unichar_break_type (0x31) == G_UNICODE_BREAK_NUMERIC);
}

//Test for g_unichar_get_mirror_char
void tg_unichar_get_mirror_char()
{
	gunichar* res = (gunichar*)malloc (sizeof (gunichar));
	g_assert (g_unichar_get_mirror_char (0x28,res));
	g_assert (g_unichar_get_mirror_char (0x5b,res));
	g_assert (g_unichar_get_mirror_char (0x3c,res));
	g_assert (!g_unichar_get_mirror_char (0x10,res));
 	free (res);
}


//Test for g_unichar_isdefined
void tg_unichar_isdefined()
{
	g_assert (g_unichar_isdefined (0x5a)); //Valid
	g_assert (!g_unichar_isdefined (0xFFFF)); //Invalid
}

//Test for g_unichar_istitle
void tg_unichar_istitle()
{
	g_assert(g_unichar_istitle(0x01C5));	//LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
	g_assert(g_unichar_istitle(0x01C8));	//LATIN CAPITAL LETTER L WITH SMALL LETTER J
	g_assert(g_unichar_istitle(0x01CB)); 	//LATIN CAPITAL LETTER N WITH SMALL LETTER J
	g_assert(g_unichar_istitle(0x01F2)); 	//LATIN CAPITAL LETTER D WITH SMALL LETTER Z

}

//Test for g_unichar_iswide
void tg_unichar_iswide()
{
	char ip1 = 'a';
	wchar_t ip2 = 0x1101;
	g_assert(!g_unichar_iswide(ip1));
	g_assert(g_unichar_iswide(ip2));
}

//Test for g_unichar_totitle
void tg_unichar_totitle()
{
	g_assert(g_unichar_totitle('A')==65);
	g_assert(g_unichar_totitle('a')==65);
}

void tg_unicode_canonical_ordering()
{
	unsigned int ip[] = 
	{
		0x24,0x28,0x69,0x2c,0x34,0x4a,0x5d
	};
	g_unicode_canonical_ordering (ip ,7);
}

int main (int argc,char *argv[])
{

	#ifdef SYMBIAN

 	g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO |  G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
 	#endif /*SYMBIAN*/
 	
 	tg_unichar_type();
	tg_unichar_break_type();
	tg_unichar_get_mirror_char();
	tg_unichar_isdefined();
	tg_unichar_istitle();
	tg_unichar_iswide();
	tg_unichar_totitle();
 	tg_unicode_canonical_ordering();
 	#ifdef SYMBIAN
  testResultXml("tunichar");
#endif /* EMULATOR */
 	return 0;
}