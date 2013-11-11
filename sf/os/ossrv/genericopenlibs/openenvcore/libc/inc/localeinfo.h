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
* Description:  Contains the class declartion for locale related functions
 *     
 *
*/

 
#include <e32base.h>
#include <e32std.h>
#include <e32ldr.h>
#include <charconv.h>
#include <f32file.h>
#include <locale.h>
#include <langinfo.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "lmonetary.h"
#include "lnumeric.h"
#include "timelocal.h"
#include "sysif.h"

// CONSTANTS
const TInt KNumLocaleExports = 22;

class CLocale:public CBase
{
private:	
	CLocale();
	static CLocale* New();
	TInt ConstructL();
	
public:	
	static CLocale* GetInstance();
	static void DeleteInstance();
	
	static struct lc_numeric_T* GetCurrentNumericLocale(void);
	static struct lc_monetary_T* GetCurrentMonetaryLocale(void);
	static struct lc_time_T* GetCurrentTimeLocale(void);
	
	TText*  SetLocale(TDesC& aLocale);
	TInt LoadLocale(TDesC& aLocale);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	TInt SyncLocale(TLocaleAspect aspect);
    #endif
	const TText* nl_langinfo(nl_item aItem);
	struct lconv * localeconv();
	
   
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
	TInt  GetSystemLocale(TDesC& aLanDllName,TDesC& aRegDllName);
	const TText* GetCollateLocaleName(void);
#endif
	TInt  GetSystemLocale(TDesC& aDllName);
    const TText* GetLocaleName(void);
#else
  TText* GetLocaleName(void);
#endif

    

	TInt MonetaryLoadLocaleL(const char* localeName);
	TInt NumericLoadLocale(const char* localeName);
	TInt TimeLoadLocaleL(const char* localeName);
	
	TInt ConvertToMultiByteL(TUint8* aNarrowCharString, const TText16* aWideCharString, TInt  aCharsetUID);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	void ConvertToMultiByteCustom(TUint8* aNarrowCharString, const TText16* aWideCharString);
    #endif
	TInt GetCharcaterSetIDL(const char* aLocaleName);
	/*******************************************************************
	Overloading new and delete operators so that they will
	allocate and deallocare memory from/to the private heap of backend
	********************************************************************/
	inline TAny* operator new(TUint aSize, TAny* aBase) __NO_THROW
		{
		Mem::FillZ(aBase, aSize); return aBase;
		}
		
	inline TAny* operator new(TUint aSize) __NO_THROW
		{
		return Backend()->Alloc(aSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave)
		{
		TAny* ptr = Backend()->Alloc(aSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
		
	inline TAny* operator new(TUint aSize, TUint aExtraSize) __NO_THROW
		{
		return Backend()->Alloc(aSize + aExtraSize);
		}
		
	inline TAny* operator new(TUint aSize, TLeave, TUint aExtraSize)
		{
		TAny* ptr = Backend()->Alloc(aSize + aExtraSize);
		if (ptr == NULL)
			{
			User::Leave(KErrNoMemory);
			}
		return ptr;
		}
	
	//Corresponding overloaded delete operators
	inline void operator delete(TAny *aPtr, TAny*) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
		
	inline void operator delete(TAny *aPtr) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
		
	inline void operator delete(TAny *aPtr, TLeave) 
		{
		Backend()->Free( aPtr );
		}
		
	inline void operator delete(TAny *aPtr, TUint) __NO_THROW
		{
		Backend()->Free( aPtr );
		}
	inline void operator delete(TAny *aPtr, TLeave, TUint) 
		{
		Backend()->Free( aPtr );
		}

private:
	static CLocale* iNewLocale;
	static struct lc_monetary_T* iMonetary_locale;
	static struct lc_numeric_T* iNumeric_locale;
	static struct lc_time_T* iTime_locale;
	
	
	SLocaleLanguage iLanguageSettings;
	SLocaleLocaleSettings iLocaleSettings;
	SLocaleTimeDateFormat iLocaleTimeDateFormat;
	TLocale iLocale;
    TLibraryFunction iData[KNumLocaleExports];
	TChar iThousandsSeparator;
	TChar iDecimalSeparator;
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
	TLibraryFunction iDataRegion[KNumLocaleExports];// will contain Monetary , numeric and time info
    TLibraryFunction iDataLanguage[KNumLocaleExports];// will contain month table and Day Table   details
    TBuf<30> iRegDllName;
    TBuf<30> iLanDllName;
	TBool iOldDllPresent;
	TInt LoadNewLocaleDll( TDesC& aLocale );
	TInt ParseNewLocaleFile( TDesC& aLocale );
	TInt TimeLoadNewLocaleL( const char* localeName );
	
#endif
};
//end of file



