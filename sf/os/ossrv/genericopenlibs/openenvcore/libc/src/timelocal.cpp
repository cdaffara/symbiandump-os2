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
* Description:  Contains the source for LC_TIME category related functions
 *     
 *
*/

 
#include <stddef.h>
#include "localeinfo.h"
#include "timelocal.h"

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif

extern int __nlocale_changed;
#ifdef EMULATOR
int *GET_WSD_VAR_NAME(__nlocale_changed, g)();
#define __nlocale_changed (*GET_WSD_VAR_NAME(__nlocale_changed, g)())
#endif //EMULATOR

#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
extern unsigned char __clocale_set;
#ifdef EMULATOR
unsigned char *GET_WSD_VAR_NAME(__clocale_set, g)();
#define __clocale_set (*GET_WSD_VAR_NAME(__clocale_set, g)())
#endif //EMULATOR
#endif
#ifndef EMULATOR
int _time_using_locale = 0;
#else //EMULATOR

GET_GLOBAL_VAR_FROM_TLS(_time_using_locale, int)
#define _time_using_locale (*GET_WSD_VAR_NAME(_time_using_locale, g)())
#endif //EMULATOR

static const struct lc_time_T	_C_time_locale = {
	{
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	}, {
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	}, {
		"Sun", "Mon", "Tue", "Wed",
		"Thu", "Fri", "Sat"
	}, {
		"Sunday", "Monday", "Tuesday", "Wednesday",
		"Thursday", "Friday", "Saturday"
	},

	/* X_fmt */
	"%H:%M:%S",

	/*
	 * x_fmt
	 * Since the C language standard calls for
	 * "date, using locale's date format," anything goes.
	 * Using just numbers (as here) makes Quakers happier;
	 * it's also compatible with SVR4.
	 */
	"%m/%d/%y",

	/*
	 * c_fmt
	 */
	"%a %b %e %H:%M:%S %Y",

	/* am */
	"AM",

	/* pm */
	"PM",

	/* date_fmt */
	"%a %b %e %H:%M:%S %Z %Y",
	
	/* alt_month
	 * Standalone months forms for %OB
	 */
	{
		"January", "February", "March", "April", "May", "June",
		"July", "August", "September", "October", "November", "December"
	},

	/* md_order
	 * Month / day order in dates
	 */
	"md",

	/* ampm_fmt
	 * To determine 12-hour clock format time (empty, if N/A)
	 */
	"%I:%M:%S %p"
};

struct lc_time_T *
__get_current_time_locale(void)
{
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
    TInt ret;
#endif
	//retrieve time(LC_TIME category) related information
	if(_time_using_locale)
	{
		return (CLocale::GetCurrentTimeLocale());
	}
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
    else if(__clocale_set)	
#endif
	//Retrieve POSIX locale time(LC_TIME) information
	return (struct lc_time_T *)&_C_time_locale;
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	else
	{
		CLocale *l=CLocale::GetInstance();
		ret=l->SyncLocale(ELocaleLanguageSettings);
    	if(ret==-1)
    	return NULL;
    	TRAPD(result, l->TimeLoadLocaleL(NULL));
    	if((result != KErrNone))
		return NULL;
		return (CLocale::GetCurrentTimeLocale());
	}
#endif
}

int
__time_load_locale(const char* localeName)
{

	CLocale* locale = CLocale::GetInstance();	
	//Set time(LC_TIME category) related information
	TInt ret=KErrNone;
	TRAPD(result,ret = locale->TimeLoadLocaleL(localeName));
	if((result != KErrNone) || (ret == -1))
	{
		return -1;
	}
	_time_using_locale = 1;
	return 0;

}
