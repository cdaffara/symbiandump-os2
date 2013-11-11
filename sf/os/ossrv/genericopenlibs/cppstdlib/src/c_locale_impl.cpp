// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Name        : c_locale_libc.cpp
// Part of     : libstdcpp
// Adaptation layer to get locale functionality.
// Version     : 
// This material, including documentation and any related 
// computer programs, is protected by copyright controlled by 
// Nokia Corporation. All rights are reserved. Copying, 
// including reproducing, storing, adapting or translating, any 
// or all of this material requires the prior written consent of 
// Nokia Corporation. This material also contains confidential 
// information which may not be disclosed to others without the 
// prior written consent of Nokia Corporation.
//



//#include "stlport_prefix.h" 
#include <limits.h>
#include <locale.h>
#include <langinfo.h>
#include <string.h>
#include <stdlib.h>
#include <wctype.h>
#include "c_locale.h"
#include "e32cmn.h"
#include "e32std.h" 

#ifdef	_MESSAGE_CATALOG_
#include <libintl.h>
#endif

#define LOCALE_SYSTEM_DEFAULT	"C"
#define	MAX_NAME_LEN	30	

#if (defined (__GNUC__) || defined (_KCC) || defined(__ICC)) && (!defined (__SYMBIAN32__))
typedef unsigned short int _Locale_mask_t;
#else
typedef unsigned int _Locale_mask_t;
#endif

typedef struct _Locale_ctype { 
	char name[MAX_NAME_LEN];
	_Locale_mask_t	ctable[257];	
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

size_t _Locale_strxfrm(L_collate_t* lcol, char* pDestStr, size_t destLen,
                        const char* pSrcStr, size_t srcLen);

size_t _Locale_strwxfrm(L_collate_t* lcol, wchar_t* pDestStr, size_t destLen,
                          const wchar_t* pSrcStr, size_t srcLen);

void _Locale_init()
	{}

void _Locale_final()
	{}

/* Gets the system locale name */
static const char* Locale_common_default( char* name)
{
	if(name != NULL)
	{
		strcpy(name,LOCALE_SYSTEM_DEFAULT);
	}
	return LOCALE_SYSTEM_DEFAULT;
}


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

   
void* _Locale_ctype_create(const char * name, struct _Locale_name_hint* /*hint*/)
{ 
   unsigned char buffer[256];
   int i;
   _Locale_mask_t* ctable;	

   L_ctype_t *plocCType = new (L_ctype_t); 	
   if (!plocCType)
      return NULL;

   strcpy(plocCType->name, name);
   char* ptemp = setlocale(LC_CTYPE, name); 
   if (!ptemp)
      return NULL;

   ctable = plocCType->ctable;

   /* Partial implementation for ANSI code page, need to implement for DBCS code pages*/

   /* Make table with all characters. */
   for(i = 0; i < 256; i++) 
   { 
      buffer[i] = i;  
   }    
   for (i=0;i<256;i++)
   {
      ctable[i+1] = Get_locale_char_ctype(buffer[i]);	
   }	
   ctable[0] = 0; //EOF
   return plocCType;
}
  
void* _Locale_numeric_create(const char *name, struct _Locale_name_hint* /*hint*/)
{ 
   L_numeric_t *plocNumeric = new (L_numeric_t);
   struct lconv *plconv;

   if (!plocNumeric)
      return NULL; 	

   strcpy(plocNumeric->name, name); 	
   char* ptemp = setlocale(LC_NUMERIC, name); 
   if (!ptemp)
      return NULL;

   plconv = localeconv();

   //copy locale numeric data to local structure
   plocNumeric->grouping = __getString(plconv->grouping);
   strcpy(plocNumeric->decimal_point, plconv->decimal_point); 	
   strcpy(plocNumeric->thousands_sep, plconv->thousands_sep);

   return plocNumeric; 
}
 
void*_Locale_time_create(const char * name, struct _Locale_name_hint* /*hint*/)
{ 
   L_time_t *plocTime = new(L_time_t);

   if (!plocTime )
      return NULL;

   strcpy(plocTime ->name, name); 	
   char* ptemp = setlocale(LC_TIME, name); 
   if (!ptemp)
      return NULL;

   int i;

   //Get all month names
   int index = (MON_1);   
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
 
void* _Locale_collate_create(const char *name, struct _Locale_name_hint* /*hint*/)
{ 
   L_collate_t *plocCollate = new(L_collate_t);

   if (!plocCollate)
      return NULL;

   char* ptemp = setlocale(LC_COLLATE, name); 
   if (!ptemp)
	   return NULL;
   
   strcpy(plocCollate->name, name); 	
   return plocCollate;
}
 
void* _Locale_monetary_create(const char * name, struct _Locale_name_hint* /*hint*/)
{ 
 	L_monetary_t *plocMonetary = new(L_monetary_t);
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
  	strcpy(plocMonetary->int_curr_symbol, plconv->int_curr_symbol); 
  	strcpy(plocMonetary->curr_symbol, plconv->currency_symbol);
  	strcpy(plocMonetary->negative_sign, plconv->negative_sign);
  	strcpy(plocMonetary->positive_sign, plconv->positive_sign);
  	plocMonetary->grouping		  = __getString(plconv->mon_grouping);
  	plocMonetary->frac_digits	  = plconv->frac_digits;
  	plocMonetary->int_frac_digits = plconv->int_frac_digits;
 	
 	return plocMonetary;
}

void* _Locale_messages_create(const char *name, struct _Locale_name_hint* /*hint*/)
{ 
 	L_messages_t *plocMessages= new(L_messages_t);
 	
 	if (!plocMessages)
		return NULL; 	
 	
 	char* ptemp = setlocale(LC_MESSAGES, name); 
 	if (!ptemp)
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

char const* _Locale_ctype_name(const void* cat, char* buff)
{ 
	strcpy(buff, ((L_ctype_t*)cat)->name);
	return buff;
}

char const* _Locale_numeric_name(const void* cat, char* buff)
{ 
	strcpy(buff, ((L_numeric_t*)cat)->name);
	return buff;	
}

char const* _Locale_time_name(const void* cat, char* buff)
{ 
	strcpy(buff, ((L_time_t*)cat)->name); 
	return buff;
}

char const* _Locale_collate_name(const void* cat, char* buff)
{
	strcpy(buff, ((L_collate_t*)cat)->name); 
	return buff; 
}

char const* _Locale_monetary_name(const void* cat, char* buff)
{ 
	strcpy(buff, ((L_monetary_t*)cat)->name); 
	return buff;  
}

char const* _Locale_messages_name(const void* cat, char* buff)
{
	strcpy(buff, ((L_messages_t*)cat)->name); 
	return buff; 
}

void _Locale_ctype_destroy(void* locale)
{
 	delete((L_ctype_t*)locale);
}
 
void _Locale_numeric_destroy(void* locale)
{
 	delete ((L_numeric_t*)locale)->grouping;
 	delete (L_numeric_t*)locale;
}
 
void _Locale_time_destroy(void* locale)
{
 	int i; //count
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
 
char const* _Locale_extract_ctype_name(const char* cname, char* buf, struct _Locale_name_hint* /*__hint*/) 
{ 
 	return Locale_extract_name(cname, buf, LC_CTYPE);
}
 
char const* _Locale_extract_numeric_name(const char* cname, char* buf, struct _Locale_name_hint* /*__hint*/)
{ 
 	return Locale_extract_name(cname, buf, LC_NUMERIC);
}
 
char const* _Locale_extract_time_name(const char* cname, char* buf, struct _Locale_name_hint* /*__hint*/)
{ 
 	return Locale_extract_name(cname, buf, LC_TIME);
}
 
char const* _Locale_extract_collate_name(const char* cname, char* buf, struct _Locale_name_hint* /*__hint*/) 
{ 
 	return Locale_extract_name(cname, buf, LC_COLLATE); 
}
 
char const* _Locale_extract_monetary_name(const char* cname, char* buf, struct _Locale_name_hint* /*__hint*/) 
{ 
 	return Locale_extract_name(cname, buf, LC_MONETARY);
}
 
char const* _Locale_extract_messages_name(const char* cname, char* buf, struct _Locale_name_hint* /*__hint*/) 
{ 
 	return Locale_extract_name(cname, buf, LC_MESSAGES);
}

const char* _Locale_compose_name(char* buf,
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

struct _Locale_name_hint* _Locale_get_ctype_hint(struct _Locale_ctype* /*ctype*/)
{ return 0; }
struct _Locale_name_hint* _Locale_get_numeric_hint(struct _Locale_numeric* /*numeric*/)
{ return 0; }
struct _Locale_name_hint* _Locale_get_time_hint(struct _Locale_time* /*time*/)
{ return 0; }
struct _Locale_name_hint* _Locale_get_collate_hint(struct _Locale_collate* /*collate*/)
{ return 0; }
struct _Locale_name_hint* _Locale_get_monetary_hint(struct _Locale_monetary* /*monetary*/)
{ return 0; }
struct _Locale_name_hint* _Locale_get_messages_hint(struct _Locale_messages* /*messages*/)
{ return 0; }

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

_Locale_mask_t  _Locale_wchar_ctype(L_ctype_t* /*lctype*/, wint_t c, _Locale_mask_t  which_bits)
{ 
 	_Locale_mask_t mask = Get_locale_wchar_ctype(c,which_bits); 	
 	return mask & which_bits; 
}

wint_t _Locale_wchar_tolower(L_ctype_t* /*lctype*/, wint_t c)
{ 
 	return towlower(c); 
}

wint_t _Locale_wchar_toupper(L_ctype_t* /*lctype*/, wint_t c)
{ 
 	return towupper(c); 
}
 
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

size_t _Locale_mbtowc(struct _Locale_ctype *lctype,
                      wchar_t *to,			/* size_t n1 - removed in 5.x?  */
                      const char *from, size_t n,
                      mbstate_t * /*shift_state*/)
{
	setlocale(LC_CTYPE, lctype->name);
	return (size_t) mbtowc(to, from,n); 
}

size_t _Locale_wctomb(L_ctype_t* lctype,
                                 char *to, size_t /*n*/,
                                 const wchar_t c,
                                 mbstate_t */*shift_state*/) 
{
	setlocale(LC_CTYPE, lctype->name);
  	return (size_t) wctomb(to, c);
}

 
size_t _Locale_unshift(L_ctype_t * /*lctype*/,
                                  mbstate_t */*st*/,
                                  char *buff, size_t /*n*/, char ** next) 
{
	//Conversion is not required, because iconv will not do partial conversion.
	*next = buff;
	return 0;
}

/* Collate */

int _Locale_strcmp(L_collate_t* lcol,
                    const char* pStr1, size_t len1, const char* pStr2,
                    size_t len2) 
{

	char *ptempStr1 = NULL, *ptempStr2 = NULL;
	int tempLen1, tempLen2;

	tempLen1 = _Locale_strxfrm(lcol, NULL, 0, pStr1, len1);
	tempLen2 = _Locale_strxfrm(lcol, NULL, 0, pStr2, len2);
	ptempStr1 = (char*) new char[tempLen1+1];
	ptempStr2 = (char*) new char[tempLen2+1];
	int ret;
	int minN = tempLen1 < tempLen2 ? tempLen1 : tempLen2;	
	setlocale(LC_COLLATE, lcol->name);
	_Locale_strxfrm(lcol, ptempStr1, tempLen1, pStr1, len1);
	_Locale_strxfrm(lcol, ptempStr2, tempLen2, pStr2, len2);
	ret = strncmp(ptempStr1, ptempStr2, minN);

	delete [] ptempStr1;
	delete [] ptempStr2;

	if (ret == 0) 
	{
		if (len1 < len2) 
			return -1;
		else if (len1 > len2) 
			return 1;
		else 
			return 0;
	} 
	else
		return ret;

}

int _Locale_strwcmp(L_collate_t* lcol,
                     const wchar_t* pStr1, size_t len1,
                     const wchar_t* pStr2, size_t len2) 
{
	wchar_t *ptempStr1 = NULL, *ptempStr2 = NULL;
	int tempLen1,tempLen2;

	tempLen1 = _Locale_strwxfrm(lcol, NULL,0,pStr1,len1);
	tempLen2 = _Locale_strwxfrm(lcol, NULL,0,pStr2,len2);
	ptempStr1 = (wchar_t*) new wchar_t[tempLen1+1];
	ptempStr2 = (wchar_t*) new wchar_t[tempLen2+1];
	int ret;
	int minN = tempLen1 < tempLen2 ? tempLen1 : tempLen2;
	setlocale(LC_COLLATE, lcol->name);
	_Locale_strwxfrm(lcol, ptempStr1,tempLen1+1,pStr1,len1);
	_Locale_strwxfrm(lcol, ptempStr2,tempLen2+1,pStr2,len2);
	ret = wcsncmp(ptempStr1, ptempStr2, minN);
	
	delete [] ptempStr1;
	delete [] ptempStr2;
	
	if (ret == 0) 
	{
		if (len1 < len2) 
			return -1;
		else if (len1 > len2) 
			return 1;
		else 
			return 0;
	} 
	else
		return ret;	
}

size_t _Locale_strxfrm(L_collate_t* lcol,
                        char* pDestStr, size_t destLen,
                        const char* pSrcStr, size_t srcLen) 
{
	size_t n;
	setlocale(LC_COLLATE, lcol->name);

	char* ptemp = (char*) new char[srcLen+1];
	if(ptemp == NULL)
		return 0;
	strncpy(ptemp, pSrcStr, srcLen);
	*(ptemp + srcLen) = 0;
	
	n = strxfrm(pDestStr, ptemp, destLen);
	
	delete []ptemp;
	
	if ((pDestStr == NULL) || (destLen ==0) )
		return n;
	else if(n > destLen)
		return (size_t)-1;
	
	pDestStr[n] = 0;
	return n;
}

size_t _Locale_strwxfrm(L_collate_t* lcol,
                          wchar_t* pDestStr, size_t destLen,
                          const wchar_t* pSrcStr, size_t srcLen)
{
	size_t n;
	setlocale(LC_COLLATE, lcol->name);
	wchar_t* ptemp = (wchar_t*) new wchar_t[srcLen+1];
	if(ptemp == NULL)
		return 0;
	wcsncpy(ptemp, pSrcStr, srcLen);
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
 
char _Locale_mon_decimal_point(L_monetary_t * lmon)
{ 
 	return lmon->decimal_point[0];
}
 
char _Locale_mon_thousands_sep(L_monetary_t * lmon)
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
 
char _Locale_int_frac_digits(L_monetary_t * lmon)
{  	 	
 	return lmon->int_frac_digits;
}
 
char _Locale_frac_digits(L_monetary_t * lmon)
{  	 	
 	return lmon->frac_digits;
}
 
int _Locale_p_cs_precedes(L_monetary_t * lmon)
{ 
 	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv(); 	 	
 	return plconv->p_cs_precedes;
}
 
int _Locale_p_sep_by_space(L_monetary_t * lmon)
{ 
 	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv(); 		 	
 	return plconv->p_sep_by_space;
}
 
int _Locale_p_sign_posn(L_monetary_t * lmon)
{ 
	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv(); 	 	 	
 	return plconv->p_sign_posn;
}
 
int _Locale_n_cs_precedes(L_monetary_t * lmon)
{ 
	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv(); 	
 	return plconv->n_cs_precedes;
}
 
int _Locale_n_sep_by_space(L_monetary_t * lmon)
{ 
	struct lconv* plconv;
 	setlocale(LC_MONETARY, lmon->name);
 	plconv = localeconv(); 	 	 	 	
 	return plconv->n_sep_by_space; 
}
 
int _Locale_n_sign_posn(L_monetary_t * lmon)
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
 	return nl_langinfo(D_T_FMT); 
}
 
const char* _Locale_long_d_fmt(L_time_t* ltime)
{ 
	setlocale(LC_TIME, ltime->name);
	return nl_langinfo(D_FMT); 
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

/* Messages */ 
 
nl_catd_type _Locale_catopen(struct _Locale_messages* lmessage, const char* catalogName)
{ 
   lmessage->domain = __getString((char*)catalogName);
   return 1;  
}
 
void _Locale_catclose(struct _Locale_messages* lmessage, nl_catd_type /*catalog_desc*/) 
{
 	delete(lmessage->domain);
 	lmessage->domain = NULL;
}

const char* _Locale_catgets(struct _Locale_messages* lmessage, nl_catd_type /*catalog_desc*/,
                             int /*set*/, int /*message*/,
                             const char * /*dfault*/)
{ 
 	char* locale = setlocale(LC_ALL, lmessage->name);
#ifdef _MESSAGE_CATALOG_ 	
 	textdomain(lmessage->domain);	
	
 	return gettext(dfault);
#else
	return NULL;
#endif

}

