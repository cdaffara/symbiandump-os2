/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


# include "stlport_prefix.h" 
#include <limits.h>

//madhu - 18/8/06
#include "locale.h"
#include "langinfo.h"
#include <string.h>
#include <stdlib.h>
#include <cwctype>
#include "c_locale.h"
#define	CODECVT_ICONV //Define CODECVT_ICONV to use ICONV APIs for code conversion.

//#define _MESSAGE_CATALOG_
#ifdef	_MESSAGE_CATALOG_
#include <libintl.h>
#endif

# ifdef _STLP_REAL_LOCALE_IMPLEMENTED

// here, we'll put C locale implementation for those compilers where
// it has to be done in C++

# else

/* This is a "stub" implementation of the "c_locale.h" interface,
   intended for operating systems where we have not yet written
   a real implementation.  A C++ library using this stub implementation
   is still standard-conforming, since the C++ standard does not require
   that any locales other than "C" be supported.
*/

#if 0 //Madhu - 18/8/06
/* Framework functions */

struct _Locale_ctype    /* { } */ ;
struct _Locale_numeric  /* { } */;
struct _Locale_time      /* { } */;
struct _Locale_collate   /*{ } */;
struct _Locale_monetary  /* { } */;
struct _Locale_messages  /* { } */;

#endif //#if 0


#ifdef CODECVT_ICONV
#include <iconv.h>
#include <errno.h>
#define	UNICODESTR	"UCS2"
struct node;
enum CONVWAY
{
	
	IN = 0,
	OUT

};
typedef struct node
{
	mbstate_t	*state;
	iconv_t		in_iconvstate;
	iconv_t		out_iconvstate;
	struct node  *next;
}StateList;

void BEtoLE(wchar_t* src, int size)
{
	int i = 0;
	
	for (i = 0;i<size;i++)
		src[i] = (src[i]>>8) | (src[i]<<8);
}

void ExtractCodeset(char* name, char* codeset)
{
	char* temp = strstr(name,".");
	if( temp != NULL)
		{
		temp++;
		while ( (*temp)&&(*temp!='=') )
		*codeset++ = *temp++;
		}
}

int AddState(StateList **list, iconv_t icnvstate, mbstate_t* state, enum CONVWAY way)
{
	StateList *temp = *list;
	StateList *newnode = NULL;
	while(temp)
	{
		if (temp->state == state)	
			break;
		temp = temp->next;
	}
	
	if (!temp)
	{		
		newnode = new StateList;
		if (newnode == NULL)
			return 0;
		newnode->next = NULL;
		newnode->state = state;
		newnode->in_iconvstate = newnode->out_iconvstate = NULL;
	}
	if(newnode)
		temp = newnode;
	if (!way)
		temp->in_iconvstate = icnvstate;
	else
		temp->out_iconvstate = icnvstate;
	
	if (*list == NULL)
	{
		*list = temp;
		return 1;	
	}
	else if(newnode)
	{
			
		StateList *move = *list;
		while(move->next)
			move = move->next;
			
		move->next = temp;
	}
	return 1;	
	
}

iconv_t	GetState(StateList *list, mbstate_t* state, enum CONVWAY way)
{
	while(list)
	{
		if (list->state == state)
		{
			if (!way)
				return (iconv_t)list->in_iconvstate;
			else
				return (iconv_t)list->out_iconvstate;
		}
			
		list = list->next;
	}
	return NULL;
}

void DeleteAllStates(StateList *list)
{	
	while(list)
	{
		StateList *temp =list ;
		iconv_close(temp->in_iconvstate);
		iconv_close(temp->out_iconvstate);
		list = list->next;
		delete temp;
	}
}


#endif



//Madhu - locale function definitions - START
//Madhu - temporarily hardcoded system default locale - 18/8/06
#define LOCALE_SYSTEM_DEFAULT	"C"
#define	MAX_NAME_LEN	64	
char	presentLocaleName[64]; // Stores present locale name.
 


#if defined (__GNUC__) || defined (_KCC) || defined(__ICC)
typedef unsigned short int _Locale_mask_t;
#else
typedef unsigned int _Locale_mask_t;
#endif

typedef struct _Locale_ctype { 
	char name[MAX_NAME_LEN];
	_Locale_mask_t	ctable[257];	
#ifdef CODECVT_ICONV
	StateList *statelist;
	~_Locale_ctype() 
	{
		DeleteAllStates(statelist);
	}
#endif
  
} L_ctype_t;


typedef struct _Locale_numeric	{  
  char name[MAX_NAME_LEN];
  char decimal_point[4];
  char thousands_sep[4];
  char *grouping;
} L_numeric_t;

typedef struct _Locale_time	{  
  char name[MAX_NAME_LEN];
  char *month[12];
  char *abbrev_month[12];
  char *dayofweek[7];
  char *abbrev_dayofweek[7];
} L_time_t;

typedef struct _Locale_collate	{
  char name[MAX_NAME_LEN];
} L_collate_t;

typedef struct _Locale_monetary	{  
  char name[MAX_NAME_LEN]; 
  char decimal_point[4];
  char thousands_sep[4];
  char *grouping;
  char int_curr_symbol[5]; // 3+1+1
  char curr_symbol[6];
  char negative_sign[5];
  char positive_sign[5];
  int frac_digits; 
  int int_frac_digits;
} L_monetary_t;

typedef struct _Locale_messages	{
  char name[MAX_NAME_LEN];
  char* domain;
} L_messages_t;


char* __getString(char* str)
{
	int len = strlen(str)+1;
	char* temp = (char*) new char[len];
	strcpy(temp, str);
	
	return temp;
	
}

/* Common function to set locale information */
#if 0
static void* Locale_Common_Create(const char* locale)
{
		
	setlocale(LC_ALL, locale);
	
	return (void*) localeconv();
	
		
}
#endif

/* Gets the system locale name */
static const char* Locale_common_default( char* name)
{
#if 0
	char* presentLocalename = NULL;
	char* defaultLocalename = NULL;
	
	//Madhu - going round to get default system locale, need to find alternative - 18/8/96
	presentLocalename = setlocale(LC_ALL, NULL);
	defaultLocalename = setlocale(LC_ALL, "");
	setlocale(LC_ALL, presentLocalename);
#endif

	if(name!=NULL)
		strcpy(name,LOCALE_SYSTEM_DEFAULT);
	
	return LOCALE_SYSTEM_DEFAULT;
	
}

#if 0
/* Gets the present locale name */ 
static char* Locale_common_name(const void* cat, char* name)
{
#if 0
	char* presentLocalename = NULL;
	
	presentLocalename = setlocale(LC_ALL, NULL);
	strcpy(name,presentLocalename);
	
	return presentLocalename;
#endif	
	L_ctype_t* locale = (L_ctype_t*) cat;
	strcpy(name,locale->name);
	return name;
}
#endif
/* */
static char * Locale_extract_name ( const char *cname, char *into, int category )
{
  int i = 0;
  const char * end;
const char* strstar = "*", *strnull = "";
  
  if ( cname[0] != '/' )
  {
  	if (strcmp(cname,strnull) == 0)
  		return strcpy(into, strstar);
  	else
  		return strcpy(into, cname); /* simple locale name */
  }
  
  for ( i = 0; i <= category; i ++ ) {
    while ( *cname != '\0' && *cname != '/' )
      cname++;
    if ( *cname == '\0' )
      return into;
    cname++;
  }
  
  if ( *cname == '\0' )
    return into;
  
  end = cname;
  while ( *end != '\0' && *end != '/' )
    end++;
  
  strncpy ( into, cname, end - cname );
  into [ end - cname ] = '\0';
  
  return into;
}

# ifdef __cplusplus
_STLP_BEGIN_NAMESPACE //Madhu - moved to here
extern "C" {

_Locale_mask_t  Get_locale_wchar_ctype(wint_t wc, _Locale_mask_t  /*mask*/)

{
	_Locale_mask_t  ret = 0;
	
	if (iswcntrl(wc))  ret |= _Locale_CNTRL;
	if (iswupper(wc))  ret |= _Locale_UPPER;
	if (iswlower(wc))  ret |= _Locale_LOWER;
	if (iswdigit(wc))  ret |= _Locale_DIGIT;
	if (iswxdigit(wc)) ret |= _Locale_XDIGIT;
	if (iswpunct(wc))  ret |= _Locale_PUNCT;
	if (iswspace(wc))  ret |= _Locale_SPACE;
	if (iswprint(wc))  ret |= _Locale_PRINT;
	if (iswalpha(wc))  ret |= _Locale_ALPHA;
	
	return ret;
  
}

_Locale_mask_t  Get_locale_char_ctype(unsigned char c)

{
	_Locale_mask_t  ret = 0;
	
	if (iscntrl(c))  ret |= _Locale_CNTRL;
	if (isupper(c))  ret |= _Locale_UPPER;
	if (islower(c))  ret |= _Locale_LOWER;
	if (isdigit(c))  ret |= _Locale_DIGIT;
	if (isxdigit(c)) ret |= _Locale_XDIGIT;
	if (ispunct(c))  ret |= _Locale_PUNCT;
	if (isspace(c))  ret |= _Locale_SPACE;
	if (isprint(c))  ret |= _Locale_PRINT;
	if (isalpha(c))  ret |= _Locale_ALPHA;
	
	return ret;
  
}
//Madhu - locale function definitions - END


//# ifdef __cplusplus
//_STLP_BEGIN_NAMESPACE

# define __DUMMY_PAR
# define __DUMMY_PAR1
# define __DUMMY_PAR2
# define __DUMMY_PAR3
# define __DUMMY_PAR4
# define __DUMMY_PAR5
# define __DUMMY_PAR6
# define __DUMMY_PAR7
# define __DUMMY_PAR8
# endif
    
 void* _Locale_ctype_create(const char * name)
 { 
 
 	unsigned char buffer[256];
	int i;
	_Locale_mask_t* ctable;
	
 	L_ctype_t *plocCType = (L_ctype_t*) new (L_ctype_t);
 	
 	if (!plocCType)
		return NULL;
 	
 	strcpy(plocCType->name, name);
	char* ptemp = setlocale(LC_CTYPE, name); 
	if (!ptemp)
		return NULL;
 	ctable = plocCType->ctable;
 	
	/* Partial implementation for ANSI code page, need to implement for DBCS code pages*/
	
	/* Make table with all characters. */
    for(i = 0; i < 256; i++) buffer[i] = i;
    
    for (i=0;i<256;i++)
    {
    	
    	ctable[i+1] = Get_locale_char_ctype(buffer[i]);	
    }	
 	ctable[0] = 0; //EOF
#ifdef CODECVT_ICONV
	plocCType->statelist = NULL;	
#endif 	
 	return plocCType;
 }
  
 void* _Locale_numeric_create(const char *name)
 { 
 	L_numeric_t *plocNumeric = (L_numeric_t *)new (L_numeric_t);
 	struct lconv *plconv;
 	
 	if (!plocNumeric)
		return NULL; 	
 	strcpy(plocNumeric->name, name);
 	
	char* ptemp = setlocale(LC_NUMERIC, name); 
	if (!ptemp)
		return NULL;
 	plconv = localeconv();
 	
 	//copy locale numeric data to local structure
 	strcpy(plocNumeric->decimal_point, plconv->decimal_point);
 	plocNumeric->grouping = __getString(plconv->grouping);
 	strcpy(plocNumeric->thousands_sep, plconv->thousands_sep);
 	
  	return plocNumeric; 
 }
 
 void*_Locale_time_create(const char * name)
 { 
 	L_time_t *plocTime = (L_time_t *)new(L_time_t);
 	//struct lconv *plconv;
 	
 	if (!plocTime )
		return NULL; 	
 	strcpy(plocTime ->name, name);
 	
	char* ptemp = setlocale(LC_TIME, name); 
	if (!ptemp)
		return NULL;
 	//plconv = localeconv();
 	
 	int i;
 	int index = (MON_1);
 	//Get all month names
 	for (i = 0;i<12;i++,index++)
 		plocTime->month[i] = __getString(nl_langinfo(index)); 	
 	
 	//Get all abbrevated month names
 	index = (ABMON_1);
 	for (i = 0;i<12;i++,index++)
 		plocTime->abbrev_month[i] = __getString(nl_langinfo(index));
 		
  	//Get all weekday names
 	index = (DAY_1);
 	for (i = 0;i<7;i++,index++)
 		plocTime->dayofweek[i] = __getString(nl_langinfo(index));
 	
  	//Get all weekday names
 	index = (ABDAY_1);
 	for (i = 0;i<7;i++,index++)
 		plocTime->abbrev_dayofweek[i] = __getString(nl_langinfo(index));
 	
  	 	
 	return plocTime;
 }
 
 void* _Locale_collate_create(const char *name)
 { 
 	L_collate_t *plocCollate = (L_collate_t *)new(L_collate_t);
 	
 	if (!plocCollate)
		return NULL; 	
 	strcpy(plocCollate->name, name);
 	
 	return plocCollate;
 }
 
 void* _Locale_monetary_create(const char * name)
 { 
 	L_monetary_t *plocMonetary = (L_monetary_t *)new(L_monetary_t);
 	struct lconv *plconv;
 	
 	if (!plocMonetary)
		return NULL; 	
 	strcpy(plocMonetary->name, name);
 	
	char* ptemp = setlocale(LC_MONETARY, name); 
	if (!ptemp)
		return NULL;
 	plconv = localeconv();
 	
 	strcpy(plocMonetary->decimal_point,plconv->mon_decimal_point);
  	strcpy(plocMonetary->thousands_sep, plconv->mon_thousands_sep);
  	plocMonetary->grouping = __getString(plconv->mon_grouping);
  	strcpy(plocMonetary->int_curr_symbol, plconv->int_curr_symbol); // 3+1+1
  	strcpy(plocMonetary->curr_symbol, plconv->currency_symbol);
  	strcpy(plocMonetary->negative_sign, plconv->negative_sign);
  	strcpy(plocMonetary->positive_sign, plconv->positive_sign);
  	plocMonetary->frac_digits = plconv->frac_digits;
  	plocMonetary->int_frac_digits = plconv->int_frac_digits;
 	
 	return plocMonetary;
 }
 void* _Locale_messages_create(const char *name)
 { 
 	L_messages_t *plocMessages= (L_messages_t *)new(L_messages_t);
 	
 	if (!plocMessages)
		return NULL; 	
 	strcpy(plocMessages->name, name);
 	
 	return plocMessages;
 }

 const char* _Locale_ctype_default(char* buff)
 { 
 	return Locale_common_default(buff);
 }
 
 const char* _Locale_numeric_default(char *buff)
 { 
 	return Locale_common_default(buff);
 }
 
 const char* _Locale_time_default(char* buff)
 { 
 	return Locale_common_default(buff);
 }
 
 const char* _Locale_collate_default(char* buff)
 { 
 	return Locale_common_default(buff); 
 }
 
 const char* _Locale_monetary_default(char* buff)
 { 
 	return Locale_common_default(buff);
 }
 
 const char* _Locale_messages_default(char* buff) 
 { 
 	return Locale_common_default(buff);
 }

 char* _Locale_ctype_name(const void* cat, char* buff)
 { 
 	L_ctype_t* locale = (L_ctype_t*) cat;
	strcpy(buff,locale->name);
	return buff;
 	//return Locale_common_name(locale,buff);
 }
 
 char* _Locale_numeric_name(const void* cat, char* buff)
 { 
 	L_numeric_t* locale = (L_numeric_t*) cat;
	strcpy(buff,locale->name);
	return buff;
 	//return Locale_common_name(locale,buff);
 }
 
 char* _Locale_time_name(const void* cat, char* buff)
 { 
 	L_time_t* locale = (L_time_t*) cat;
	strcpy(buff,locale->name);
	return buff;
 	//return Locale_common_name(locale,buff);
 }
 
 char* _Locale_collate_name(const void* cat, char* buff)
 { 
 	L_collate_t* locale = (L_collate_t*) cat;
	strcpy(buff,locale->name);
	return buff;
 	//return Locale_common_name(locale,buff);
 }
 
 char* _Locale_monetary_name(const void* cat, char* buff)
 { 
 	L_monetary_t* locale = (L_monetary_t*) cat;
	strcpy(buff,locale->name);
	return buff;
 	//return Locale_common_name(locale,buff);
 }
 
 char* _Locale_messages_name(const void* cat, char* buff)
 { 
 	L_messages_t* locale = (L_messages_t*) cat;
	strcpy(buff,locale->name);
	return buff;
 	//return Locale_common_name(locale,buff);
 }

 void _Locale_ctype_destroy(void* locale)
 {
 #ifdef	CODECVT_ICONV
 	DeleteAllStates(((L_ctype_t*)locale)->statelist);
#endif 	
 	delete((L_ctype_t*)locale);
 }
 
 void _Locale_numeric_destroy(void* locale)
 {
 	delete ((L_numeric_t*)locale)->grouping;
 	delete (L_numeric_t*)locale;
 }
 
 void _Locale_time_destroy(void* locale)
 {
 	int i;
 	L_time_t* plocTime =(L_time_t*)locale; 
 	//delete months
 	for (i = 0;i<12;i++)
 		delete (plocTime->month[i]);
 	//delete abbrevated months
 	for (i = 0;i<12;i++)
 		delete (plocTime->abbrev_month[i]);
 	//delete week day
 	for (i = 0;i<7;i++)
 		delete (plocTime->dayofweek[i]);
 	//delete abbrevated week day
 	for (i = 0;i<7;i++)
 		delete (plocTime->abbrev_dayofweek[i]);
 	 		
 	delete((L_time_t*)locale);
 }
 
 void _Locale_collate_destroy(void* locale)
 {
 	delete((L_collate_t*)locale);
 }
 
 void _Locale_monetary_destroy(void* locale)
 {
 	delete ((L_monetary_t*)locale)->grouping;
 	delete((L_monetary_t*)locale);
 }
 
 void _Locale_messages_destroy(void* locale)
 {
 	delete((L_messages_t*)locale);
 }

 char* _Locale_extract_ctype_name(const char* cname, char* buf) 
 { 
 	return Locale_extract_name(cname, buf, LC_CTYPE);
 }
 
 char* _Locale_extract_numeric_name(const char* cname, char* buf) 
 { 
 	return Locale_extract_name(cname, buf, LC_NUMERIC);
 }
 
 char* _Locale_extract_time_name(const char* cname, char* buf) 
 { 
 	return Locale_extract_name(cname, buf, LC_TIME);
 }
 
 char* _Locale_extract_collate_name(const char* cname, char* buf) 
 { 
 	return Locale_extract_name(cname, buf, LC_COLLATE); 
 }
 
 char* _Locale_extract_monetary_name(const char* cname, char* buf) 
 { 
 	return Locale_extract_name(cname, buf, LC_MONETARY);
 }
 
 char* _Locale_extract_messages_name(const char* cname, char* buf) 
 { 
 	return Locale_extract_name(cname, buf, LC_MESSAGES);
 }

 char* _Locale_compose_name(char* buf,
			     const char* ctype, const char* numeric,
			     const char* time, const char* collate,
			     const char* monetary, const char* messages,
			     const char* /*default_name*/)
 { 
 
 	if ( !strcmp ( ctype, numeric ) &&
	 !strcmp ( ctype, time ) &&
	 !strcmp ( ctype, collate ) &&
	 !strcmp ( ctype, monetary ) &&
	 !strcmp ( ctype, messages ) )
	return strcpy ( buf, ctype );

    strcpy ( buf, "/" );
    strcat ( buf, ctype );

    strcat ( buf, "/" );
    strcat ( buf, numeric );

    strcat ( buf, "/" );
    strcat ( buf, time );

    strcat ( buf, "/" );
    strcat ( buf, collate );

    strcat ( buf, "/" );
    strcat ( buf, monetary );

    strcat ( buf, "/" );
    strcat ( buf, messages );

    return buf;
 }


/* ctype */

const  _Locale_mask_t* _Locale_ctype_table(struct _Locale_ctype* lctype)
{ 
	    
	return lctype->ctable; 
}

int _Locale_toupper(struct _Locale_ctype* /*lctype*/, int c)
{ 
	return toupper(c); 
}

int _Locale_tolower(L_ctype_t* /*lctype*/, int c) 
{ 
	return tolower(c); 
}

#ifndef _STLP_NO_WCHAR_T

 _Locale_mask_t  _Locale_wchar_ctype(L_ctype_t* /*lctype*/, wint_t c, _Locale_mask_t  which_bits)
 { 
 	_Locale_mask_t mask = Get_locale_wchar_ctype(c,which_bits);
 	
 	return mask & which_bits; 
 }
 
 wint_t _Locale_wchar_tolower(L_ctype_t* lctype, wint_t c)
 { 
 	setlocale(LC_CTYPE, lctype->name);
 	return towlower(c); 
 }
 
 wint_t _Locale_wchar_toupper(L_ctype_t* /*lctype*/, wint_t c)
 { 
 	return towupper(c); 
 }
 
# endif

# ifndef _STLP_NO_MBSTATE_T

int _Locale_mb_cur_max (L_ctype_t * /*lctype*/) 
{ 
	return MB_CUR_MAX;
}

int _Locale_mb_cur_min (L_ctype_t * /*lctype*/) 
{ 
	return 1; 
}

int _Locale_is_stateless (L_ctype_t * /*lctype*/) 
{ 
	return (MB_CUR_MAX == 1)?1:0; 
}

#ifndef _STLP_NO_WCHAR_T
wint_t _Locale_btowc(L_ctype_t * lctype, int c) 
{ 
	setlocale(LC_CTYPE, lctype->name);
	return btowc(c); 
}

int _Locale_wctob(L_ctype_t * lctype, wint_t wc) 
{ 
	setlocale(LC_CTYPE, lctype->name);
	return wctob(wc); 
}

size_t _Locale_mbtowc(L_ctype_t *lctype,
                                 wchar_t *to, size_t n1,
                                 const char *from, size_t n, int* chars_write,
                                 mbstate_t * shift_state) 
{
#ifdef CODECVT_ICONV
	char	codeset[64] = 
	{
		0
	};
	iconv_t state = GetState(lctype->statelist, shift_state, IN);
	if (state == NULL)
	{
			ExtractCodeset(lctype->name, codeset);
			state= iconv_open(UNICODESTR,codeset);
			if (state==NULL)
				return (size_t)-1;
			if (AddState(&lctype->statelist,state,shift_state,IN) == 0)
				return (size_t)-1;
					
	}
	char* outbuf = (char*)to;
	const char* inbuf = (char*)from;
	unsigned int inbytes = n;
	unsigned int outbytes = n1*sizeof(wchar_t);
	unsigned int chars_read = iconv((void*)state,&inbuf,&inbytes,&outbuf,&outbytes);
	*chars_write = (wchar_t*)outbuf - to; 
	if (chars_read == (size_t)-1)
	{
		if ( (errno == EINVAL) || (outbuf!=(char*)to) )
		{
			BEtoLE(to,(wchar_t*)outbuf-to);
			return inbuf-from;
		}			
		
		return (size_t)-1;	
		 
	}
	BEtoLE(to,(wchar_t*)outbuf-to);
	//return (wchar_t*)outbuf-to;
	return inbuf-from;

#else
	setlocale(LC_CTYPE, lctype->name);
  return (size_t) mbtowc(to, from,n); 
#endif
}

size_t _Locale_wctomb(L_ctype_t* lctype,
                                 char *to, size_t n,
                                 const wchar_t c,
                                 mbstate_t *shift_state) 
{
#ifdef CODECVT_ICONV
  	char	codeset[64] = 
	{
		0
	};
	iconv_t state = GetState(lctype->statelist, shift_state,OUT);
	if (state == NULL)
	{
			ExtractCodeset(lctype->name, codeset);
			state= iconv_open(codeset, UNICODESTR);
			if (state==NULL)
				return (size_t)-1;
			if (AddState(&lctype->statelist,state,shift_state,OUT) == 0)
				return (size_t)-1;
					
	}
	char* outbuf = to;
	const char* inbuf = (char*)&c;
	unsigned int inbytes = 2;
	unsigned int outbytes = n;
	unsigned int chars_read = iconv((void*)state,&inbuf,&inbytes,&outbuf,&outbytes);
	if (chars_read == (size_t)-1)
	{
		if (errno == EINVAL)
			return outbuf-to;
		else
			return (size_t)-1;	
		
	}
	//BEtoLE(to,(wchar_t*)outbuf-to);
	return outbuf-to;
#else
	setlocale(LC_CTYPE, lctype->name);
  	return (size_t) wctomb(to, c);

#endif
}

# endif

//Madhu - Need to double check the implementation. 
size_t _Locale_unshift(L_ctype_t * /*lctype*/,
                                  mbstate_t *st,
                                  char *buff, size_t n, char ** next) 
{
#if 0	
    if(*st == 0)
    {
		*next = buff;
		return 0;
    }
    else
    {
		if(n < 1) 
		{ 
			*next = buff; 
			return (size_t)-2; 
		}

		*next = buff + 1;
		return 1;
    }
#endif
//Conversion is not required, because iconv will not do partial conversion.
	*next = buff;
	return 0;
}

# endif /*  _STLP_NO_MBSTATE_T */


/* Collate */
 int _Locale_strcmp(L_collate_t* lcol,
                    const char* pStr1, size_t len1, const char* pStr2,
                    size_t len2) 
{

	char *ptempStr1, *ptempStr2;
	int tempLen1,tempLen2;

	tempLen1 = _Locale_strxfrm(lcol, NULL,0,pStr1,len1);
	tempLen2 = _Locale_strxfrm(lcol, NULL,0,pStr2,len2);
	ptempStr1 = (char*) new char[tempLen1];
	ptempStr2 = (char*) new char[tempLen2];
	int ret;
	int minN = tempLen1 < tempLen2 ? tempLen1 : tempLen2;	
	setlocale(LC_COLLATE, lcol->name);
	_Locale_strxfrm(lcol, ptempStr1,tempLen1,pStr1,len1);
	_Locale_strxfrm(lcol, ptempStr2,tempLen2,pStr2,len2);
	ret = strncmp(ptempStr1, ptempStr2, minN);
	if (ret == 0) 
	{
		if (len1 < len2) 
			return -1;
		else if (len1 > len2) 
			return 1;
		else 
			return 0;
	} else
		return ret;

}

# ifndef _STLP_NO_WCHAR_T

 int _Locale_strwcmp(L_collate_t* lcol,
                     const wchar_t* pStr1, size_t len1,
                     const wchar_t* pStr2, size_t len2) 
{
	wchar_t *ptempStr1, *ptempStr2;
	int tempLen1,tempLen2;

	tempLen1 = _Locale_strwxfrm(lcol, NULL,0,pStr1,len1);
	tempLen2 = _Locale_strwxfrm(lcol, NULL,0,pStr2,len2);
	ptempStr1 = (wchar_t*) new wchar_t[tempLen1+1];
	ptempStr2 = (wchar_t*) new wchar_t[tempLen2+1];
	int ret;
	int minN = tempLen1 < tempLen2 ? tempLen1 : tempLen2;
	setlocale(LC_COLLATE, lcol->name);
	_Locale_strwxfrm(lcol, ptempStr1,tempLen1,pStr1,len1);
	_Locale_strwxfrm(lcol, ptempStr2,tempLen2,pStr2,len2);
	ret = wcsncmp(ptempStr1, ptempStr2, minN);
	if (ret == 0) 
	{
		if (len1 < len2) 
			return -1;
		else if (len1 > len2) 
			return 1;
		else 
			return 0;
	} else
		return ret;

	
}

# endif

 size_t _Locale_strxfrm(L_collate_t* lcol,
                        char* pDestStr, size_t destLen,
                        const char* pSrcStr, size_t srcLen) 
{
	size_t n;
	setlocale(LC_COLLATE, lcol->name);

	char* ptemp = (char*) new char[srcLen+1];
	if(ptemp == NULL)
		return 0;
	memmove(ptemp,pSrcStr,srcLen);
	*(ptemp+srcLen) = 0;
	
	n = strxfrm(pDestStr, ptemp, destLen);
	
	delete []ptemp;
	
	if ((pDestStr == NULL) || (destLen ==0) )
		return n;
	else if(n > destLen)
		return (size_t)-1;
	
	pDestStr[n] = 0;
	return n;
}

# ifndef _STLP_NO_WCHAR_T

  size_t _Locale_strwxfrm(L_collate_t* lcol,
                          wchar_t* pDestStr, size_t destLen,
                          const wchar_t* pSrcStr, size_t srcLen)
{
	size_t n;
	setlocale(LC_COLLATE, lcol->name);
	wchar_t* ptemp = (wchar_t*) new wchar_t[srcLen+1];
	if(ptemp == NULL)
		return 0;
	memmove(ptemp,pSrcStr,srcLen*sizeof(wchar_t));
	*(ptemp+srcLen) = 0;
	n = wcsxfrm(pDestStr, ptemp, destLen);
	
	delete []ptemp;
	if ((pDestStr == NULL) || (destLen ==0) )
		return n;
	else if(n > destLen)
		return (size_t)-1;
	
	pDestStr[n] = 0;
	return n;
}

# endif

/* Numeric */
  
 char _Locale_decimal_point(L_numeric_t* lnum)
 { 
 	
 	return lnum->decimal_point[0];
 }
 
 char _Locale_thousands_sep(L_numeric_t* lnum) 
 { 
 	 	
 	return lnum->thousands_sep[0]; 
 }
 const char* _Locale_grouping(L_numeric_t*lnum) 
 { 
 	 	
 	return lnum->grouping;
 }

 const char * _Locale_true(L_numeric_t * /*lnum*/)  
 { 
 	const char* __true_name="true"; //glib and NT doing the same
  	
	
 	return __true_name; 
 }
 
 const char * _Locale_false(L_numeric_t * /*lnum*/) 
 {
 	const char* __false_name="false"; //glib and NT doing the same
 	
 	return __false_name;
 }


/* Monetary */

 const char* _Locale_int_curr_symbol(L_monetary_t * lmon)
 { 
 	 	
 	return lmon->int_curr_symbol;
 }
 
 const char* _Locale_currency_symbol(L_monetary_t * lmon)
 { 
 	 	
 	return lmon->curr_symbol;
 }
 
 char   _Locale_mon_decimal_point(L_monetary_t * lmon)
 { 
 	return lmon->decimal_point[0];
 }
 
 char    _Locale_mon_thousands_sep(L_monetary_t * lmon)
 { 
 	 	
 	return lmon->thousands_sep[0];
 }
 
 const char* _Locale_mon_grouping(L_monetary_t * lmon)
 { 
 	 	
 	return lmon->grouping;
 }
 
 const char* _Locale_positive_sign(L_monetary_t * lmon)
 { 
 	 	
 	return lmon->positive_sign;
 }
 
 const char* _Locale_negative_sign(L_monetary_t * lmon)
 { 
 	 	
 	return lmon->negative_sign;
 }
 
 char   _Locale_int_frac_digits(L_monetary_t * lmon)
 { 
 	 	
 	return lmon->int_frac_digits;
 }
 
 char        _Locale_frac_digits(L_monetary_t * lmon)
 { 
 	 	
 	return lmon->frac_digits;
 }
 
 int         _Locale_p_cs_precedes(L_monetary_t * lmon)
 { 
 	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv();
 	 	
 	return plconv->p_cs_precedes;
 }
 
 int         _Locale_p_sep_by_space(L_monetary_t * lmon)
 { 
 	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv();
 		 	
 	return plconv->p_sep_by_space;
 }
 
 int         _Locale_p_sign_posn(L_monetary_t * lmon)
 { 
	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv();
 	 	 	
 	return plconv->p_sign_posn;
 }
 
 int         _Locale_n_cs_precedes(L_monetary_t * lmon)
 { 
	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv();
 	
 	return plconv->n_cs_precedes;
 }
 
 int          _Locale_n_sep_by_space(L_monetary_t * lmon)
 { 
	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv();
 	 	 	 	
 	return plconv->n_sep_by_space; 
 }
 
 int          _Locale_n_sign_posn(L_monetary_t * lmon)
 { 
 	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv();
 	
 	return plconv->n_sign_posn; 
 }


/* Time */
 const char * _Locale_full_monthname(L_time_t * ltime, int month)
 { 
 	 	
 	return ltime->month[month];
 }
 
 const char * _Locale_abbrev_monthname(L_time_t * ltime, int month)
 { 
 	 	
 	return ltime->abbrev_month[month];
 }
 
 const char * _Locale_full_dayofweek(L_time_t * ltime, int day)
 { 
 	 	
 	return ltime->dayofweek[day];
 }
 
 const char * _Locale_abbrev_dayofweek(L_time_t * ltime, int day)
 { 
 	 	
 	return ltime->abbrev_dayofweek[day];
 }

 const char* _Locale_d_t_fmt(L_time_t* ltime)    
 { 
 	setlocale(LC_TIME, ltime->name);
 	return nl_langinfo(D_T_FMT);
 }
 
 const char* _Locale_d_fmt(L_time_t* ltime)
 {
 	setlocale(LC_TIME, ltime->name); 
	return nl_langinfo(D_FMT);
 }
 
 const char* _Locale_t_fmt(L_time_t* ltime)
 { 
 	setlocale(LC_TIME, ltime->name);
 	return nl_langinfo(T_FMT);
 }
 
 const char* _Locale_long_d_t_fmt(L_time_t* ltime)
 { 
 	setlocale(LC_TIME, ltime->name);
 	return nl_langinfo(D_T_FMT); //Madhu--now same as D_T_FMT, need to check
 }
 
const char* _Locale_long_d_fmt(L_time_t* ltime)
{ 
	setlocale(LC_TIME, ltime->name);
	return nl_langinfo(D_FMT); //Madhu--now same as D_FMT, need to check
}
const char* _Locale_am_str(L_time_t* ltime)     
{ 
	setlocale(LC_TIME, ltime->name);
	return nl_langinfo(AM_STR);
}

const char* _Locale_pm_str(L_time_t* ltime)     
{ 
	setlocale(LC_TIME, ltime->name);
	return nl_langinfo(PM_STR);
}

const char* _Locale_t_fmt_ampm(L_time_t* ltime) 
{ 
	setlocale(LC_TIME, ltime->name);
	return nl_langinfo(T_FMT_AMPM);
}

/* Messages */ // Madhu - support after libc team supported.

 

 int _Locale_catopen(L_messages_t* lmessage, const char* catalogName)
 { 
 	lmessage->domain = __getString((char*)catalogName);
 	return 1;//catopen(catalogName,/*NL_CAT_LOCALE*/1); 
 }
 
 void _Locale_catclose(L_messages_t* lmessage, int /*catalog_desc*/) 
 {
 	//setlocale(LC_MESSAGES, lmessage->name);
 	delete(lmessage->domain);
 	lmessage->domain = NULL;
 	//catclose(catalog_desc);
 
 }
 
 const char* _Locale_catgets(L_messages_t* lmessage, int /*catalog_desc*/,
                                       int /*set*/, int /*message*/,
                                       const char *dfault)
 { 
 	char* locale = setlocale(LC_ALL, lmessage->name);
#ifdef _MESSAGE_CATALOG_ 	
 	textdomain(lmessage->domain);	
	
 	return gettext(dfault);
#else
	return NULL;
#endif

 }

#ifdef __cplusplus    
} /* extern C */
_STLP_END_NAMESPACE
#endif

#endif /* real locale */
