/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains the source for setting the locale
 *     
 *
*/

 
#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <_ansi.h>
#include "lmonetary.h"	
#include "lnumeric.h"	
#include "timelocal.h" 
#include "localeinfo.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

#define LOCALE_LENGTH 30
extern int  _time_using_locale;
extern int _numeric_using_locale;
extern int _monetary_using_locale;
extern int __nlocale_changed;
extern int __mlocale_changed;
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
extern unsigned char __clocale_set;
extern unsigned char __applocale_changed;
#endif

#ifdef EMULATOR
int *GET_WSD_VAR_NAME(_monetary_using_locale, g)();
int *GET_WSD_VAR_NAME(_numeric_using_locale, g)();
int *GET_WSD_VAR_NAME(_time_using_locale, g)();
GET_GLOBAL_VAR_FROM_TLS(__mlocale_changed, int)
GET_GLOBAL_VAR_FROM_TLS(__nlocale_changed, int)
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
GET_GLOBAL_VAR_FROM_TLS(__clocale_set, unsigned char)
GET_GLOBAL_VAR_FROM_TLS(__applocale_changed, unsigned char)
#endif

#define	_monetary_using_locale (*GET_WSD_VAR_NAME(_monetary_using_locale, g)())
#define _numeric_using_locale (*GET_WSD_VAR_NAME(_numeric_using_locale, g)())
#define _time_using_locale (*GET_WSD_VAR_NAME(_time_using_locale, g)())
#define __mlocale_changed (*GET_WSD_VAR_NAME(__mlocale_changed, g)())
#define __nlocale_changed (*GET_WSD_VAR_NAME(__nlocale_changed, g)())
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
#define __clocale_set (*GET_WSD_VAR_NAME(__clocale_set, g)())
#define __applocale_changed (*GET_WSD_VAR_NAME(__applocale_changed, g)())
#endif
#endif //EMULATOR

__BEGIN_DECLS
#ifndef EMULATOR
char LC_CTYPE_LocaleName[LOCALE_LENGTH] = {'\0'};
char LC_COLLATE_LocaleName[LOCALE_LENGTH] = {'\0'};
char LC_NUMERIC_LocaleName[LOCALE_LENGTH] = {'\0'};
char LC_MONETARY_LocaleName[LOCALE_LENGTH] = {'\0'};
char LC_TIME_LocaleName[LOCALE_LENGTH] ={'\0'};
char LC_ALL_LocaleName[LOCALE_LENGTH] = {'\0'};

#else //EMULATOR

GET_GLOBAL_ARRAY_FROM_TLS(LC_CTYPE_LocaleName, char)
GET_GLOBAL_ARRAY_FROM_TLS(LC_COLLATE_LocaleName, char)
GET_GLOBAL_ARRAY_FROM_TLS(LC_NUMERIC_LocaleName, char)
GET_GLOBAL_ARRAY_FROM_TLS(LC_MONETARY_LocaleName, char)
GET_GLOBAL_ARRAY_FROM_TLS(LC_TIME_LocaleName, char)
GET_GLOBAL_ARRAY_FROM_TLS(LC_ALL_LocaleName, char)

#define LC_CTYPE_LocaleName (GET_WSD_VAR_NAME(LC_CTYPE_LocaleName, g)())
#define LC_COLLATE_LocaleName (GET_WSD_VAR_NAME(LC_COLLATE_LocaleName, g)())
#define LC_NUMERIC_LocaleName (GET_WSD_VAR_NAME(LC_NUMERIC_LocaleName, g)())
#define LC_MONETARY_LocaleName (GET_WSD_VAR_NAME(LC_MONETARY_LocaleName, g)())
#define LC_TIME_LocaleName (GET_WSD_VAR_NAME(LC_TIME_LocaleName, g)())
#define LC_ALL_LocaleName (GET_WSD_VAR_NAME(LC_ALL_LocaleName, g)())
#endif //EMULATOR


__END_DECLS

extern "C" {

EXPORT_C char * setlocale(int category, const char* locale)
{
	CLocale* loc = CLocale::GetInstance();
	
	//Query the current locale
	if(NULL == locale)	
	{
		char* ret = NULL;
		switch(category)
		{
			case LC_CTYPE: 	if(!*LC_CTYPE_LocaleName)
							{
								LC_CTYPE_LocaleName[0] = 'C';
								LC_CTYPE_LocaleName[1] = '\0';
							}
							ret = (char*) LC_CTYPE_LocaleName;
							break;
						
			case LC_COLLATE:if(!*LC_COLLATE_LocaleName)
							{
								LC_COLLATE_LocaleName[0] = 'C';
								LC_COLLATE_LocaleName[1] = '\0';
							} 
							ret = (char*) LC_COLLATE_LocaleName;
							break;
							
			case LC_TIME:	if(!*LC_TIME_LocaleName)
							{
								LC_TIME_LocaleName[0] = 'C';
								LC_TIME_LocaleName[1] = '\0';
								_time_using_locale = 0;
							} 
							ret = (char*) LC_TIME_LocaleName;
							break;	
						
			case LC_NUMERIC:if(!*LC_NUMERIC_LocaleName)
							{
								LC_NUMERIC_LocaleName[0] = 'C';
								LC_NUMERIC_LocaleName[1] = '\0';
								_numeric_using_locale = 0;
							}  
							ret = (char*) LC_NUMERIC_LocaleName;
							break;
							
			case LC_MONETARY:if(!*LC_MONETARY_LocaleName)
							{
								LC_MONETARY_LocaleName[0] = 'C';
								LC_MONETARY_LocaleName[1] = '\0';
								_monetary_using_locale = 0;
							}  
							ret = (char*) LC_MONETARY_LocaleName;
							break;
				
			case LC_MESSAGES:
						break;
			
			case LC_ALL :if(!*LC_ALL_LocaleName)
							{
								_time_using_locale = 0;
								_numeric_using_locale = 0;
								_monetary_using_locale = 0;
								
								LC_CTYPE_LocaleName[0] = 'C';
								LC_CTYPE_LocaleName[1] = '\0';
								LC_COLLATE_LocaleName[0] = 'C';
								LC_COLLATE_LocaleName[1] = '\0';
								LC_TIME_LocaleName[0] = 'C';
								LC_TIME_LocaleName[1] = '\0';
								LC_NUMERIC_LocaleName[0] = 'C';
								LC_NUMERIC_LocaleName[1] = '\0';
								LC_MONETARY_LocaleName[0] = 'C';
								LC_MONETARY_LocaleName[1] = '\0';
								LC_ALL_LocaleName[0] =  'C';
								LC_ALL_LocaleName[1] = '\0';
							}  
						 ret = (char*) LC_ALL_LocaleName;
						 break;
			default:
				errno = EINVAL;
				return (NULL);
		}
		return ret;
	}

	//If locale is  "" set each category(LC_CTYPE, LC_COLLATE, LC_NUMERIC, LC_TIME, LC_MONETARY) to C locale
	if(strcmp(locale, "") == 0 )
	{
		_time_using_locale = 0;
		_numeric_using_locale = 0;
		_monetary_using_locale = 0;
		__nlocale_changed = 1;
		__mlocale_changed = 1;
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
		__clocale_set = 1;
		__applocale_changed = 1;
#endif
		
		LC_CTYPE_LocaleName[0] = 'C';
		LC_CTYPE_LocaleName[1] = '\0';
		LC_COLLATE_LocaleName[0] = 'C';
		LC_COLLATE_LocaleName[1] = '\0';
		LC_TIME_LocaleName[0] = 'C';
		LC_TIME_LocaleName[1] = '\0';
		LC_NUMERIC_LocaleName[0] = 'C';
		LC_NUMERIC_LocaleName[1] = '\0';
		LC_MONETARY_LocaleName[0] = 'C';
		LC_MONETARY_LocaleName[1] = '\0';
		LC_ALL_LocaleName[0] =  'C';
		LC_ALL_LocaleName[1] = '\0';
				
		return "C";
	}
	
	if (strcmp(locale, "C") == 0 || strcmp(locale, "POSIX") == 0)
	{
		if(LC_ALL == category)
		{
			_time_using_locale = 0;
			_numeric_using_locale = 0;
			_monetary_using_locale = 0;
			__nlocale_changed = 1;
			__mlocale_changed = 1;
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
			__clocale_set = 1;
			__applocale_changed = 1;
#endif
			
			strcpy((char*) LC_ALL_LocaleName, locale);
			LC_ALL_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_CTYPE_LocaleName, locale);
			LC_CTYPE_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_COLLATE_LocaleName, locale);
			LC_COLLATE_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_NUMERIC_LocaleName, locale);
			LC_NUMERIC_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_MONETARY_LocaleName, locale);
			LC_MONETARY_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_TIME_LocaleName, locale);
			LC_TIME_LocaleName[strlen(locale)] = '\0';
			return (char*) locale;	
		}
		
		switch(category)
		{
			case LC_CTYPE: 	strcpy((char*) LC_CTYPE_LocaleName, locale);
							LC_CTYPE_LocaleName[strlen(locale)] = '\0';
							break;
						
			case LC_COLLATE: strcpy((char*) LC_COLLATE_LocaleName, locale);
							 LC_COLLATE_LocaleName[strlen(locale)] = '\0';
							 break;
							
			case LC_TIME:	_time_using_locale = 0;
							strcpy((char*) LC_TIME_LocaleName, locale);
							LC_TIME_LocaleName[strlen(locale)] = '\0';
							break;	
						
			case LC_NUMERIC: _numeric_using_locale = 0;
							 __nlocale_changed = 1;
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
							 __applocale_changed = 1;
#endif
							 strcpy((char*) LC_NUMERIC_LocaleName, locale);
							 LC_NUMERIC_LocaleName[strlen(locale)] = '\0';
							 break;
							
			case LC_MONETARY: _monetary_using_locale = 0;
							  __mlocale_changed = 1;
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
							  __applocale_changed = 1;
#endif
							  strcpy((char*) LC_MONETARY_LocaleName, locale);
							  LC_MONETARY_LocaleName[strlen(locale)] = '\0';	
							  break;

			case LC_MESSAGES:
			default:
				errno = EINVAL;
				return (NULL);	
		}
		return ((char*) locale);
	}

	//If locale is other than C or POSIX
	TPtrC8 src((const TUint8*)locale);
	TBuf16 <LOCALE_LENGTH> localeString;
	localeString.Copy(src);
	TText* retVal = loc->SetLocale(localeString);
	if(NULL == retVal)
	{
		return NULL;
	}
			
	if(LC_ALL == category)
	{
		TInt retVal =0;
		
		if( __numeric_load_locale(locale) == -1)
		{
			retVal = -1;
		}
		if(__monetary_load_locale(locale) == -1)
		{
			retVal = -1;
		}
		if (__time_load_locale(locale) == -1)
		{
			retVal = -1;
		}	
				
		if(!retVal)
		{
			strcpy((char*) LC_ALL_LocaleName, locale);
			LC_ALL_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_CTYPE_LocaleName, locale);
			LC_CTYPE_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_COLLATE_LocaleName, locale);
			LC_COLLATE_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_NUMERIC_LocaleName, locale);
			LC_NUMERIC_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_MONETARY_LocaleName, locale);
			LC_MONETARY_LocaleName[strlen(locale)] = '\0';
			strcpy((char*) LC_TIME_LocaleName, locale);
			LC_TIME_LocaleName[strlen(locale)] = '\0';
			return (char*) locale;
		}
		else
		{
			return NULL;
		}
	}

	switch (category)
	{
		case LC_CTYPE: 	strcpy((char*) LC_CTYPE_LocaleName, locale);
						LC_CTYPE_LocaleName[strlen(locale)] = '\0';
						break;
						
		case LC_COLLATE: strcpy((char*) LC_COLLATE_LocaleName, locale);
						 LC_COLLATE_LocaleName[strlen(locale)] = '\0';
						 break;
						
		case LC_TIME:	if(!__time_load_locale(locale))
						{
							strcpy((char*) LC_TIME_LocaleName, locale);
							LC_TIME_LocaleName[strlen(locale)] = '\0';
						}
						else
						{
							return NULL;	
						}	
						break;	
					
		case LC_NUMERIC: if(!__numeric_load_locale(locale))
						 {
						 	strcpy((char*) LC_NUMERIC_LocaleName, locale);
							LC_NUMERIC_LocaleName[strlen(locale)] = '\0';
						 }
						 else
						 {
						 	return NULL;	
						 }	
						 break;
						
		case LC_MONETARY: if(!__monetary_load_locale(locale))
						 {
						 	strcpy((char*) LC_MONETARY_LocaleName, locale);
							LC_MONETARY_LocaleName[strlen(locale)] = '\0';	
						 }
						 else
						 {
						 	return NULL;
						 }	
						 break;

		case LC_MESSAGES:
		default:
			errno = EINVAL;
			return (NULL);
	}
	return ((char*) locale);
}


EXPORT_C wchar_t* wsetlocale(int category, const wchar_t* locale)
{
	char LocaleName[LOCALE_LENGTH];	
	wchar_t *wideLocaleName = new wchar_t[LOCALE_LENGTH + 1];
	char *result = NULL;
	
	if (locale != NULL)
	{
		if ((size_t)-1 != (wcstombs((char*)LocaleName, locale, LOCALE_LENGTH*sizeof(wchar_t))))
		{
			result = setlocale(category, LocaleName);
		}
	}
	else
	{
		result = setlocale(category, NULL);
	}

	if (result != NULL)
	{
		if ((size_t)-1  != (mbstowcs(wideLocaleName, result, LOCALE_LENGTH*sizeof(wchar_t))))
		{
			return wideLocaleName;
		}
	}

	delete []wideLocaleName;
	return NULL;
}

} //extern "C"
