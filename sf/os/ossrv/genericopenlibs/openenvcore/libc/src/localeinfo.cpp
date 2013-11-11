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
* Description:  Contains the source for all locale related functionalities
 *     
 *
*/


#include <stdlib.h>
#include <f32file.h>
#include "localeinfo.h"
#include "sysif.h"
#include <e32std_private.h>
#include <e32ldr_private.h>

typedef void (*TLibFn)(TLocale*);

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#include "localetlsinfo.h"
#else

#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
#define NEW_LANG_VARIANT_DLL_NAME 30
#endif

#endif
#define LC_TIME_MEMORY 1024
#define LC_MONETARY_MEMORY 50
#define LC_NUMERIC_MEMORY 20
#define MONTHS	12
#define WEEKDAYS 7
#define DECIMAL_THOUSAND_DATE_TIME_SEPARATOR 2
#define EURO_CURRENCY 4
#define LOCALE_LENGTH 50
#define CHARACTER_SET_NAME 32
#define TIME_FORMAT_LENGTH 32
#define NON_UNICODE_TEXT_BUFFER_LENGTH 256
#define LANG_VARIANT_DLL_NAME 10

#define MONTH_STR	"%m"
#define DAY_STR		"%d"
#define YEAR_STR 	"%Y"
#define TWELVE_HOUR_STR 	"%I"
#define TWENTY_FOUR_HOUR_STR	"%H"
#define MINUTE_STR	"%M"
#define SECOND_STR	"%S"
#define AMPM_SPACE	" %p"
#define AMPM_NO_SPACE	"%p"


#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL


_LIT(KLanDllName, "elocl_lan.");
_LIT(KRegDllName, "elocl_reg.");
_LIT(KColDllName, "elocl_col.");

#endif

_LIT(KDllName, "elocl");


#ifndef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
_LIT(KEuro, "\x20AC");
#endif

_LIT8(KAMPMSpace12hr, "B");
_LIT8(KAMPMSpace, "A");
_LIT8(KAMPMNoSpace12hr, "*B");
_LIT8(KAMPMNoSpace, "*A");

#ifndef EMULATOR

CLocale* CLocale::iNewLocale = NULL;
lc_monetary_T* CLocale::iMonetary_locale = NULL;
lc_numeric_T* CLocale::iNumeric_locale =NULL;
lc_time_T* CLocale::iTime_locale = NULL;

TText8*  numericLocale =  NULL;
TText8* monetaryLocale = NULL;
TText8* timeLocale =  NULL;

static TBuf<LANG_VARIANT_DLL_NAME> DllName;

#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
static TBuf<NEW_LANG_VARIANT_DLL_NAME> ColDllName;
#endif

#else //EMULATOR

#define iNewLocale ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(iNewLocale,s))
#define iMonetary_locale ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(iMonetary_locale,s))
#define iNumeric_locale ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(iNumeric_locale,s))
#define iTime_locale ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(iTime_locale,s))

#define numericLocale ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(numericLocale,s))
#define monetaryLocale ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(monetaryLocale,s))
#define timeLocale ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(timeLocale,s))

#define DllName ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(DllName,s))

#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL

#define ColDllName ((GetGlobals()->localeptr)->GET_WSD_VAR_NAME(ColDllName,s))
#endif

#endif //EMULATOR

CLocale::CLocale()
	{
	iMonetary_locale = NULL;
	iNumeric_locale = NULL;
	iTime_locale = NULL;

	numericLocale =  NULL;
    monetaryLocale = NULL;
	timeLocale =  NULL;
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL    
     iOldDllPresent = 0;
#endif
	}
	


CLocale* CLocale::New()
	{
	CLocale* self = new CLocale();
	if(self)
		{
		TInt ret=KErrNone;
		TRAPD(result,ret = self->ConstructL());
		if(KErrNone != (result | ret))
			{
			delete self;
			self = NULL;
			}
		}
	return self;
	}
	
TInt CLocale::ConstructL()
	{
	iMonetary_locale = new  lc_monetary_T;
	if(!iMonetary_locale)
		{
		return KErrNoMemory;
		}
	iNumeric_locale = new lc_numeric_T;
	if(!iNumeric_locale)
		{
		return KErrNoMemory;
		}
	iTime_locale = new (ELeave) lc_time_T;
	if(!iTime_locale)
		{
		return KErrNoMemory;
		}
	
	for(TInt i =0 ; i < MONTHS; i++)
		{
		iTime_locale->mon[i] = NULL;
		iTime_locale->month[i] = NULL;
		iTime_locale->alt_month[i] = NULL;	
		}
	

	for(TInt i =0 ; i < WEEKDAYS; i++)
		{
		iTime_locale->wday[i] = NULL;
		iTime_locale->weekday[i] = NULL;	
		}

	iTime_locale->X_fmt = NULL;
	iTime_locale->x_fmt = NULL;
	iTime_locale->c_fmt = NULL;
	iTime_locale->am = NULL;
	iTime_locale->pm = NULL;
	iTime_locale->date_fmt = NULL;
	iTime_locale->md_order = NULL;
	iTime_locale->ampm_fmt = NULL;
	
	iMonetary_locale->int_curr_symbol = NULL;
	iMonetary_locale->currency_symbol  = NULL;
	iMonetary_locale->mon_decimal_point = NULL;
	iMonetary_locale->mon_thousands_sep = NULL;
	iMonetary_locale->mon_grouping = NULL;
	iMonetary_locale->positive_sign = NULL;
	iMonetary_locale->negative_sign = NULL;
	iMonetary_locale->int_frac_digits = iMonetary_locale->frac_digits = NULL;
	iMonetary_locale->p_cs_precedes = iMonetary_locale->int_p_cs_precedes = NULL;
	iMonetary_locale->p_sep_by_space = iMonetary_locale->int_p_sep_by_space = NULL;
	iMonetary_locale->n_cs_precedes = iMonetary_locale->int_n_cs_precedes = NULL;
	iMonetary_locale->n_sep_by_space = iMonetary_locale->int_n_sep_by_space = NULL;
	iMonetary_locale->p_sign_posn = iMonetary_locale->int_p_sign_posn = NULL;
	iMonetary_locale->n_sign_posn = iMonetary_locale->int_n_sign_posn = NULL;

	iNumeric_locale->decimal_point = NULL;
	iNumeric_locale->thousands_sep = NULL;
	iNumeric_locale->grouping = NULL;
	
	return KErrNone;
	}

CLocale* CLocale::GetInstance()
	{
	// Switch to the backend heap
	RHeap* oldheap = User::SwitchHeap(Backend()->Heap());
	
	if(NULL == iNewLocale)	
		{
		iNewLocale = New();
		}
		
	if(numericLocale == NULL )
		{
		numericLocale =  new TText8 [LC_NUMERIC_MEMORY];	
		}
	
	if(monetaryLocale == NULL)
		{
		monetaryLocale = new TText8[LC_MONETARY_MEMORY];
		}
	
	if(timeLocale == NULL)
		{
		timeLocale =  new TText8[LC_TIME_MEMORY];
		}
	

				
	//Set back the default user heap
	User::SwitchHeap(oldheap);
	return iNewLocale;	
	}

	
void CLocale::DeleteInstance()
	{
	// Switch to the backend heap so that deletes happen from there
	RHeap* oldheap = User::SwitchHeap(Backend()->Heap());

	delete [] timeLocale;
	timeLocale =NULL;

	delete []  numericLocale;
	numericLocale = NULL;

	delete [] monetaryLocale;
	monetaryLocale = NULL;

	delete  iMonetary_locale;
	iMonetary_locale = NULL;

	delete iNumeric_locale;
	iNumeric_locale = NULL;

	delete iTime_locale;
	iTime_locale = NULL;
	
	delete iNewLocale;
	iNewLocale = NULL;
	

	
	//Set back to the default user heap
	User::SwitchHeap(oldheap);
	}
		

TText* CLocale::SetLocale(TDesC& aLocale)	
	{
		//Check whether locale is supported
	
		TText* retVal = NULL;
		TInt r;
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL 
		
		// load the new locale dll if not present check for the old dll
		r = LoadNewLocaleDll( aLocale );
        if ( r  ==  KErrNone ) 
        {
            retVal = (TText*) aLocale.Ptr();
            iOldDllPresent = 0;
            return retVal;
        }
#endif

    if(!LoadLocale(aLocale))
    {
        return NULL;
    }
    RLoader loader;
    r = loader.Connect();
    if(KErrNone == r)
        {
            //Load the language variant DLL 
            TInt size = KNumLocaleExports * sizeof(TLibraryFunction);
            TPtr8 functionListBuf((TUint8*) iData, size, size);
            r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&DllName, &functionListBuf) );
            if(KErrNone == r)
            {
                ((TLibFn)iData[FnLocaleData])(&iLocale);
                retVal = (TText*) aLocale.Ptr();
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL 
                iOldDllPresent = 1;
#endif
            }
            loader.Close();
        }
    return retVal;
}

#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT

#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
TInt  CLocale::GetSystemLocale(TDesC& aLanDllName,TDesC& aRegDllName)
    {
        RLoader loader;
        TInt retVal = -1;
        TInt r = loader.Connect();
        if(KErrNone == r)
            {
            TInt size = KNumLocaleExports * sizeof(TLibraryFunction);
            TPtr8 functionListBuf1((TUint8*) iDataLanguage, size, size);
            r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&aLanDllName, &functionListBuf1) );
            if(KErrNone != r)
                return retVal;
            TPtr8 functionListBuf2((TUint8*) iDataRegion, size, size);
            r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&aRegDllName, &functionListBuf2) );
            ((TLibFn)iDataRegion[FnLocaleDataV2])(&iLocale);
            if(KErrNone != r)
                return retVal;
            retVal = 0;
            loader.Close();
            }
        return retVal;
    
    }
#endif
//Load the system dll settings
TInt CLocale::GetSystemLocale(TDesC& aDllName)	
	{
		RLoader loader;
		TInt retVal = -1;
		TInt r = loader.Connect();
		if(KErrNone == r)
			{
			//Load the language variant DLL 
			TInt size = KNumLocaleExports * sizeof(TLibraryFunction);
          	TPtr8 functionListBuf((TUint8*) iData, size, size);
          	r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&aDllName, &functionListBuf) );
			if(KErrNone == r)
				{
            	((TLibFn)iData[FnLocaleData])(&iLocale);
            	retVal = 0;
				}
			loader.Close();
			}
		return retVal;
	}

#endif


TInt CLocale::MonetaryLoadLocaleL(const char* localeName)
{
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        TInt charsetID=-1;
        if(localeName)
        {
            charsetID = GetCharcaterSetIDL(localeName);
            if(charsetID == -1)
            {
                return -1;
            }
        }
    #else
        TInt charsetID = GetCharcaterSetIDL(localeName);
        if(charsetID == -1)
        {
            return -1;
        }
    #endif
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
	    else
        {
        //update the iLocale with the system locale settings
            iLocale.Refresh();	
        }
    #endif
    #ifndef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        //Retreive monetary information from language variant DLL
        Mem::Copy(&iLocaleSettings.iCurrencySymbol[0], (const TAny*) iData[FnCurrencySymbol](), sizeof(TText) * (KMaxCurrencySymbol+1));	
    #endif
        TLocalePos aCurrencySymbolPosition = iLocale.CurrencySymbolPosition();
        TLocale::TNegativeCurrencyFormat aNegativeCurrencyFormat = iLocale.NegativeCurrencyFormat();
        TBool aCurrencySpaceBetween = iLocale.CurrencySpaceBetween();
        TBool aNegativeCurrencySymbolOpposite = iLocale.NegativeCurrencySymbolOpposite();
        TBool aNegativeLoseSpace = iLocale.NegativeLoseSpace();
        TBool aCurrencyTriadsAllowed = iLocale.CurrencyTriadsAllowed();
        
        TInt currencyDecimalPlaces = iLocale.CurrencyDecimalPlaces();
        iThousandsSeparator= iLocale.ThousandsSeparator();
        iDecimalSeparator = iLocale.DecimalSeparator();
            
        TText8* temp = monetaryLocale;
        
        //Set the decimal separator
        const TText* dec_point = (const TText*) &iDecimalSeparator;
        if(dec_point)
        {
            const TPtrC dec(dec_point);
            TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> decimal;
            decimal.Copy(dec);
            strncpy((char*) monetaryLocale, (char*) decimal.Ptr(), decimal.Length());
            monetaryLocale[decimal.Length()] = '\0';
            iMonetary_locale->mon_decimal_point = (const char*) monetaryLocale;
            monetaryLocale += decimal.Length() + 1;
        }
        
        //Set thousand separator
        const TText* thous_sep = (const TText*) &iThousandsSeparator;
        if(thous_sep)
        {
            const TPtrC thousand(thous_sep);
            TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> thousand_sep;
            thousand_sep.Copy(thousand);
            strncpy((char*) monetaryLocale, (char*) thousand_sep.Ptr(), thousand_sep.Length());
            monetaryLocale[thousand_sep.Length()] = '\0';
            iMonetary_locale->mon_thousands_sep = (const char*) monetaryLocale;
            monetaryLocale += thousand_sep.Length() + 1;
        }
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        TBuf<KMaxCurrencySymbol> CurrencySymbol;
        if(localeName)
        {
#ifdef  SYMBIAN_DISTINCT_LOCALE_MODEL
        if( iOldDllPresent ) // old style dll is loaded
            {
                Mem::Copy(&iLocaleSettings.iCurrencySymbol[0], (const TAny*) iData[FnCurrencySymbol](), sizeof(TText) * (KMaxCurrencySymbol+1));
            }
        else
            {
                Mem::Copy(&iLocaleSettings.iCurrencySymbol[0], (const TAny*) iDataRegion[FnCurrencySymbolV2](), sizeof(TText) * (KMaxCurrencySymbol+1));
            }
#else
        Mem::Copy(&iLocaleSettings.iCurrencySymbol[0], (const TAny*) iData[FnCurrencySymbol](), sizeof(TText) * (KMaxCurrencySymbol+1));
#endif
        //Set Currency information
        const TText16* cur = iLocaleSettings.iCurrencySymbol;
        const TPtrC currency(cur);
        CurrencySymbol.Copy(currency);
        }
        else
        {
        TCurrencySymbol cur;
        const TPtrC currency(cur);
        CurrencySymbol.Copy(currency);
        }
        strncpy((char*) monetaryLocale, (char*) CurrencySymbol.Ptr(), CurrencySymbol.Length());
        monetaryLocale[CurrencySymbol.Length()] = '\0';
            iMonetary_locale->currency_symbol = (const char*) monetaryLocale;
            monetaryLocale += strlen((char*) monetaryLocale) + 1;
    #else	
        //Set Currency information
        const TText16* currency = iLocaleSettings.iCurrencySymbol;
        if(currency)
        {
            TBuf<EURO_CURRENCY> euroSymbol(currency);
            char* temp = strchr(localeName, '.');
            temp = temp + 1;
            
            if(!euroSymbol.Compare(KEuro) && !strcmp(temp, "ISO-8859-1"))
            {
                strcpy((char*) monetaryLocale,"\x80");
                monetaryLocale[1] = '\0';					
            }
            else
            {
                ConvertToMultiByteL((TText8*)monetaryLocale, currency, charsetID);	
            }
            iMonetary_locale->currency_symbol = (const char*) monetaryLocale;
            monetaryLocale += strlen((char*) monetaryLocale) + 1;
        }
    #endif
        
        //Set grouping information
        strcpy((char*) monetaryLocale,(aCurrencyTriadsAllowed? "\003":""));
        monetaryLocale[strlen((char*) monetaryLocale)] = '\0';
        iMonetary_locale->mon_grouping = (const char*) monetaryLocale;
        monetaryLocale += strlen((char*) monetaryLocale) + 1;
        
        //Set currency decimal places
        iMonetary_locale->int_frac_digits =	iMonetary_locale->frac_digits = (const char*) monetaryLocale;
        *monetaryLocale++ = currencyDecimalPlaces;
        *monetaryLocale++ = '\0';
            
        //Set the currency symbol position for a monetary quantity with non-negative value 
        iMonetary_locale->p_cs_precedes = (const char*) monetaryLocale;
        iMonetary_locale->int_p_cs_precedes = iMonetary_locale->p_cs_precedes;
        *monetaryLocale++ = (ELocaleBefore == aCurrencySymbolPosition)?1:0;
        *monetaryLocale++ = '\0';
        
        //Set the currency symbol position for a monetary quantity with negative value		
        iMonetary_locale->n_cs_precedes = (const char*) monetaryLocale;
        iMonetary_locale->int_n_cs_precedes = iMonetary_locale->n_cs_precedes;
        *monetaryLocale++ = aNegativeCurrencySymbolOpposite?0:1;
        *monetaryLocale++ ='\0';
        
        //Set the currency symbol separated by space or not for a monetary quantity with non-negative value
        iMonetary_locale->p_sep_by_space = (const char*) monetaryLocale;
        iMonetary_locale->int_p_sep_by_space = iMonetary_locale->p_sep_by_space;
        *monetaryLocale++ = aCurrencySpaceBetween?1:0;	
        *monetaryLocale++ ='\0';
        
        //Set the currency symbol separated by space or not for a monetary quantity with negative value
        iMonetary_locale->n_sep_by_space = (const char*) monetaryLocale;
        iMonetary_locale->int_n_sep_by_space = iMonetary_locale->n_sep_by_space;
        *monetaryLocale++ = aNegativeLoseSpace?1:0;
        *monetaryLocale++ = '\0';
        
        //Set the positioning of the negative and positive for a monetary quantity 
        if(TLocale::ELeadingMinusSign == aNegativeCurrencyFormat)				
        {
            iMonetary_locale->n_sign_posn = (const char*) monetaryLocale;
            *monetaryLocale++ = 1;
            *monetaryLocale++ = '\0';
        }
        if(TLocale::EInBrackets == aNegativeCurrencyFormat)
        {
            iMonetary_locale->n_sign_posn = (const char*) monetaryLocale;
            *monetaryLocale++ = 0;
            *monetaryLocale++ = '\0';
        }
        
        if(TLocale::ETrailingMinusSign == aNegativeCurrencyFormat)
        {
            iMonetary_locale->n_sign_posn = (const char*) monetaryLocale;
            *monetaryLocale++ = 2;
            *monetaryLocale++ = '\0';
        }
        
        if(TLocale::EInterveningMinusSign == aNegativeCurrencyFormat)
        {
            iMonetary_locale->n_sign_posn = (const char*) monetaryLocale;
            *monetaryLocale++ = 3;
            *monetaryLocale++ = '\0';
        }
        
        iMonetary_locale->int_n_sign_posn = iMonetary_locale->n_sign_posn;
        iMonetary_locale->p_sign_posn = 	iMonetary_locale->int_p_sign_posn = 	iMonetary_locale->n_sign_posn;
        //Set the positive sign and negative sign for monetary quantity
        iMonetary_locale->positive_sign = iMonetary_locale->negative_sign = "";
        //Set the internationla currency symbol
        iMonetary_locale->int_curr_symbol = "";
        monetaryLocale = temp;
        return 0;
        
	
}
/* Forward declaration of ReadALine */
inline TInt ReadALine(RFile& aFile, TInt& aPos, TDes8& aDesc);
extern void GetInstallationDataDir(TFileName& aPath);
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL

TInt CLocale::ParseNewLocaleFile( TDesC& aLocale )
{
// open and parse the file
    TPtrC src(aLocale);
    TBuf8<LOCALE_LENGTH> locale_name;
    locale_name.Copy(src);
    char locale[LOCALE_LENGTH];
    strncpy(locale, (char*) locale_name.Ptr(), locale_name.Length());
    locale[locale_name.Length()] = '\0';
    
    if (NULL == strchr(locale, '.'))
        {
        return 0;
        }
    RFile newfile;
#ifndef __EPOC32__
    _LIT(localeFileNew,"\\resource\\openc\\locales_new.txt" );
#else
    TFileName localeFileNew;
    GetInstallationDataDir(localeFileNew);
    localeFileNew.Append(_L("locales_new.txt" ));
#endif
           
    int retValNew = newfile.Open(Backend()->FileSession(), localeFileNew, EFileRead);
    if( KErrNone != retValNew )
    {
        return 0;
    }
    int pos = 0;
    TBuf8<LOCALE_LENGTH> localeInfoNew;
    retValNew = ReadALine(newfile, pos, localeInfoNew);
    char* buffer = (char*) localeInfoNew.Ptr();
    int  i = 0;
    int  flag = 0;
    while( retValNew > 0)
    {
        char str1[LOCALE_LENGTH];
        while(buffer[i] != '=')
        {
            str1[i] = buffer[i];
            i++;
        }
        str1[i] = '\0';
        if(!strcmp(str1,locale))
        {
            flag = 1;
            break;
        }
        i = 0;
        retValNew = ReadALine(newfile, pos, localeInfoNew);
        buffer = (char*) localeInfoNew.Ptr();
    }
    if(flag)
    {
        
        iLanDllName.Copy(KLanDllName);
        iRegDllName.Copy(KRegDllName);
        ColDllName.Copy(KColDllName);
        i++;
        int len = localeInfoNew.Length();
        TBuf<50> localeids;
        while(i < len )
        {
        localeids.Append(buffer[i]);  
        i++;
        }
        TLex lex( localeids );
        TChar ch;
        TInt k = 0;
        TBuf<10> token[3];
        while((ch = lex.Get()) != 0 )
        {
        while (ch != ',' && ch != 0 && ch != '\t' && ch != '\n' && ch != '\r') 
           {
           lex.Inc();
           ch = lex.Peek();
           }
        token[k].Copy(lex.MarkedToken());
        k++;
        lex.Inc();
        lex.Mark();
        }
        iLanDllName.Append(token[0]);
        iRegDllName.Append(token[1]);
        ColDllName.Append(token[2]);
        newfile.Close();
        return 1;
       
     }
    newfile.Close();
    return 0;
}

TInt CLocale::LoadNewLocaleDll( TDesC& aLocale  )
    {
    
    if(!ParseNewLocaleFile( aLocale ))
        {
            return -1;
        }
    RLoader loader;
    TInt r = loader.Connect();
    if( KErrNone != r)
       return -1;
    TInt size = KNumLocaleExports * sizeof(TLibraryFunction);
    TPtr8 functionListBuf((TUint8*) iDataRegion, size, size );
    r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&iRegDllName, &functionListBuf) );
    if( KErrNone != r)
       return -1;
    ((TLibFn)iDataRegion[FnLocaleDataV2])(&iLocale);
    // language dll loading
    TPtr8 functionListBuf_lan((TUint8*) iDataLanguage, size, size);
    r = loader.SendReceive(ELoadLocale, TIpcArgs(0, (const TDesC*)&iLanDllName, &functionListBuf_lan) );
    if( KErrNone != r)
       return -1;
   
    return r;
    
    }

#endif



#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
TInt CLocale::NumericLoadLocale(const char* localeName)
#else
TInt CLocale::NumericLoadLocale(const char* /*localeName*/)
#endif
{
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        if(!localeName)
        {
            //update the iLocale with the system settings
            iLocale.Refresh();
            
        }
    #endif
        //Retrive numeric information from language variant DLL
        TBool aCurrencyTriadsAllowed = iLocale.CurrencyTriadsAllowed();
        iThousandsSeparator= iLocale.ThousandsSeparator();
        iDecimalSeparator = iLocale.DecimalSeparator();
        
        TText8* temp = numericLocale;
        //Set the decimal separator
        const TText* dec_point = (const TText*) &iDecimalSeparator;
        if(dec_point)
        {
            const TPtrC dec(dec_point);
            TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> decimal;
            decimal.Copy(dec);
            strncpy((char*) numericLocale, (char*) decimal.Ptr(), decimal.Length());
            numericLocale[decimal.Length()] ='\0';
            iNumeric_locale->decimal_point = (const char*) numericLocale;
            numericLocale += decimal.Length() + 1;
        }
        
        //Set thousand separator
        const TText* thous_sep = (const TText*) &iThousandsSeparator;
        if(thous_sep)
        {
            const TPtrC thousand(thous_sep);
            TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> thousand_sep;
            thousand_sep.Copy(thousand);
            strncpy((char*) numericLocale, (char*) thousand_sep.Ptr(), thousand_sep.Length());
            numericLocale[thousand_sep.Length()] = '\0';
            iNumeric_locale->thousands_sep = (const char*) numericLocale;
            numericLocale += thousand_sep.Length() + 1;
        }
        
        //Set grouping information
        strcpy((char*) numericLocale,(aCurrencyTriadsAllowed? "\003":""));
        numericLocale[strlen((char*) numericLocale)] = '\0';
        iNumeric_locale->grouping = (const char*) numericLocale;
        numericLocale += strlen((char*) numericLocale) + 1;
                
        numericLocale = temp;
        return 0;
	
}

#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL


TInt CLocale::TimeLoadNewLocaleL( const char* localeName )
{
    TInt charsetID = -1;
    if( localeName )
    {
        charsetID = GetCharcaterSetIDL(localeName);
        if(charsetID == -1)
        {
            return -1;
        }
    }
    else
    {
    //update the iLocale with the system locale settings
        iLocale.Refresh();
    }
    TText8* startAddress =  timeLocale;
    //Set month and abbreviated month name information
    for(TInt aMonth = 0 ; aMonth < MONTHS; aMonth++)
    {

        const TText16* month = ((reinterpret_cast<TText*const*>((const TText*) iDataLanguage[FnMonthTableV2]()))[aMonth]);
        if(month)
        {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            if(!localeName)
            {
                ConvertToMultiByteCustom((TUint8*)timeLocale,  month);  
            }
            else    
            {
#endif
            ConvertToMultiByteL((TUint8*)timeLocale,  month, charsetID);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            }
#endif
            iTime_locale->month[aMonth] = (const char*)  timeLocale;
            iTime_locale->alt_month[aMonth] = (const char*)  timeLocale;
            timeLocale += strlen((char*) timeLocale) + 1;
        }
        const TText16* abbMonth = ((reinterpret_cast<TText*const*>((const TText*) iDataLanguage[FnMonthAbbTableV2]()))[aMonth]);
        if(abbMonth)
        {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            if(!localeName)
            {
                ConvertToMultiByteCustom((TUint8*)timeLocale,  abbMonth);   
            }
            else    
            {
#endif
            ConvertToMultiByteL((TUint8*)timeLocale, abbMonth, charsetID);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            }
#endif
            iTime_locale->mon[aMonth] = (const char*)  timeLocale;
            timeLocale += strlen((char*) timeLocale) + 1;               
        }
                    
    }
    
    //Set day and abbreviated day name information
    const TText16* day = ((reinterpret_cast<TText*const*>((const TText*) iDataLanguage[FnDayTableV2]()))[6]);
    if(day)
    {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        if(!localeName)
        {
            ConvertToMultiByteCustom((TUint8*)timeLocale,  day);    
        }
        else    
        {
#endif
        ConvertToMultiByteL((TUint8*)timeLocale, day, charsetID);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        }
#endif
        iTime_locale->weekday[0] = (const char*)  timeLocale;
        timeLocale += strlen((char*) timeLocale) + 1;               
    }
    const TText16* abbDay = ((reinterpret_cast<TText*const*>((const TText*) iDataLanguage[FnDayAbbTableV2]()))[6]);
    if(abbDay)
    {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        if(!localeName)
        {
            ConvertToMultiByteCustom((TUint8*)timeLocale,  abbDay); 
        }
        else
        {
#endif
        ConvertToMultiByteL((TUint8*)timeLocale,  abbDay, charsetID);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        }
#endif
        iTime_locale->wday[0] = (const char*)  timeLocale;              
        timeLocale += strlen((char*) timeLocale) + 1;
    }
    
    for(TInt aDay = 0 ; aDay < WEEKDAYS - 1; aDay++)
    {
        const TText16* day = ((reinterpret_cast<TText*const*>((const TText*) iDataLanguage[FnDayTableV2]()))[aDay]);
        if(day)
        {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            if(!localeName)
            {
                ConvertToMultiByteCustom((TUint8*)timeLocale,  day);    
            }
            else    
            {
#endif
            ConvertToMultiByteL((TUint8*)timeLocale, day, charsetID);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            }
#endif
            iTime_locale->weekday[aDay + 1] = (const char*)  timeLocale;
            timeLocale += strlen((char*) timeLocale) + 1;               
        }
        const TText16* abbDay = ((reinterpret_cast<TText*const*>((const TText*) iDataLanguage[FnDayAbbTableV2]()))[aDay]);
        if(abbDay)
        {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            if(!localeName)
            {
                ConvertToMultiByteCustom((TUint8*)timeLocale,  abbDay); 
            }
            else    
            {
#endif
            ConvertToMultiByteL((TUint8*)timeLocale,  abbDay, charsetID);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            }  
#endif
            iTime_locale->wday[aDay + 1] = (const char*)  timeLocale;               
            timeLocale += strlen((char*) timeLocale) + 1;
        }
                    
    }
    const TText16* am = ((reinterpret_cast<TText*const*>((const TText*) iDataLanguage[FnAmPmTableV2]()))[0]);
    if(am)
    {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        if(!localeName)
        {
            ConvertToMultiByteCustom((TUint8*)timeLocale,  am); 
        }
        else    
        {
#endif
        ConvertToMultiByteL((TUint8*)timeLocale, am, charsetID);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        } 
#endif
        iTime_locale->am = (const char*)  timeLocale;                   
        timeLocale += strlen((char*) timeLocale) + 1;
    }
    const TText16* pm = ((reinterpret_cast<TText*const*>((const TText*) iDataLanguage[FnAmPmTableV2]()))[1]);
    if(pm)
    {
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        if(!localeName)
        {
            ConvertToMultiByteCustom((TUint8*)timeLocale,  pm); 
        }
        else    
        {
#endif
        ConvertToMultiByteL((TUint8*)timeLocale, pm, charsetID);
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        }  
#endif
        iTime_locale->pm = (const char*)  timeLocale;                   
        timeLocale += strlen((char*) timeLocale) + 1;
    }
            
    Mem::Copy(&iLocaleTimeDateFormat.iTimeFormatSpec[0], (const TAny*) iDataRegion[FnTimeFormatSpecV2](), sizeof(TText) * (KMaxTimeFormatSpec+1));
    TText* timeForm = iLocaleTimeDateFormat.iTimeFormatSpec;
    
    //Set time format(T_FMT) and am/pm time format(T_FMT_AMPM) string
    if(timeForm)
    {
        const TPtrC time(timeForm);
        TBuf8<TIME_FORMAT_LENGTH> timeFormat;
        timeFormat.Copy(time);
        
        TChar timeSeparator = iLocale.TimeSeparator(1);
        TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> timeSeparator1;
        timeSeparator1.Append(timeSeparator);
        
        timeSeparator = iLocale.TimeSeparator(2);
        TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> timeSeparator2;
        timeSeparator2.Append(timeSeparator);
        
        TTimeFormat time12hror24hr = iLocale.TimeFormat();
        
        if(KErrNotFound != timeFormat.Locate('J'))
        {
            //Check for time format(12/24 hour format)
            if(time12hror24hr == ETime12)
            {
                strcpy((char*) timeLocale,TWELVE_HOUR_STR);
            }
            else
            {
                strcpy((char*) timeLocale,TWENTY_FOUR_HOUR_STR);
            }
        }
        
        else if(KErrNotFound != timeFormat.Locate('I'))
        {
            strcpy((char*) timeLocale,TWELVE_HOUR_STR);
        }   
        
        else if(KErrNotFound != timeFormat.Locate('H'))
        {
            strcpy((char*) timeLocale,TWENTY_FOUR_HOUR_STR);
        }   
    
        strncat((char*) timeLocale, (char*) timeSeparator1.Ptr(), timeSeparator1.Length());
        strcat((char*) timeLocale, MINUTE_STR);
        strncat((char*) timeLocale, (char*) timeSeparator2.Ptr(), timeSeparator2.Length());
        strcat((char*) timeLocale, SECOND_STR);
        
        if(time12hror24hr == ETime12)
        {

            if((KErrNotFound != timeFormat.Find(KAMPMSpace12hr)) || (KErrNotFound != timeFormat.Find(KAMPMSpace)) )
            {
                if((KErrNotFound != timeFormat.Find(KAMPMNoSpace12hr)) || (KErrNotFound != timeFormat.Find(KAMPMNoSpace)))
                {
                    strcat((char*) timeLocale, AMPM_NO_SPACE);
                }
                else
                {
                    strcat((char*) timeLocale, AMPM_SPACE);
                }
            }   
        }
                    
        iTime_locale->ampm_fmt =  (const char*)  timeLocale;
        iTime_locale->X_fmt = iTime_locale->ampm_fmt;
        timeLocale += strlen((char*) timeLocale) + 1;
    }
    
    TDateFormat aDateFormat = iLocale.DateFormat();
    //Set date fromat string(D_FMT) 
    TChar dateSeparator = iLocale.DateSeparator(1);
    TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> dateSeparator1;
    dateSeparator1.Append(dateSeparator);
    
    TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> dateSeparator2;
    dateSeparator = iLocale.DateSeparator(2);
    dateSeparator2.Append(dateSeparator);

    //Americal date formt(mm/dd/YYYY)
    if(EDateAmerican == aDateFormat)
    {
        strcpy((char*) timeLocale, MONTH_STR);
        strncat((char*) timeLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
        strcat((char*) timeLocale, DAY_STR);
        strncat((char*) timeLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
        strcat((char*) timeLocale, YEAR_STR);
        iTime_locale->x_fmt = (const char*)  timeLocale;
    }
    
    //European date format(dd/mm/YYYY)
    if(EDateEuropean == aDateFormat)
    {
        strcpy((char*) timeLocale, DAY_STR);
        strncat((char*) timeLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
        strcat((char*) timeLocale, MONTH_STR);
        strncat((char*) timeLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
        strcat((char*) timeLocale, YEAR_STR);
        iTime_locale->x_fmt = (const char*)  timeLocale;
    }
    
    //Japanese date format(YYYY/mm/dd)
    if(EDateJapanese == aDateFormat)
    {
        strcpy((char*) timeLocale, YEAR_STR);
        strncat((char*) timeLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
        strcat((char*) timeLocale, MONTH_STR);
        strncat((char*) timeLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
        strcat((char*) timeLocale, DAY_STR);
        iTime_locale->x_fmt = (const char*)  timeLocale;
    }
    
    iTime_locale->md_order = iTime_locale->c_fmt = iTime_locale->date_fmt = (const char*) L"";
            ;
    timeLocale = startAddress;



return 0;
}
#endif
TInt CLocale::TimeLoadLocaleL(const char* localeName)
{
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL

    if( ! iOldDllPresent)
        {
           TInt ret = TimeLoadNewLocaleL( localeName ); // option 2 to load language and region dll both
           if( ret == -1 )
               return -1;
           return ret;
        }
#endif
    TText8* startAddress =  timeLocale;
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            TInt charsetID = -1;
            if(localeName)
            {
                charsetID = GetCharcaterSetIDL(localeName);
                if(charsetID == -1)
                    {
                        return -1;
                    }
            }
    #else
            TInt charsetID = GetCharcaterSetIDL(localeName);
            if(charsetID == -1)
            {
                return -1;
            }
    #endif
		

    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
            else
            {
                iLocale.Refresh();
            }
    #endif
            //Set month and abbreviated month name information
            for(TInt aMonth = 0 ; aMonth < MONTHS; aMonth++)
            {
   
                const TText16* month = ((reinterpret_cast<TText*const*>((const TText*) iData[FnMonthTable]()))[aMonth]);
                if(month)
                {
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                    if(!localeName)
                    {
                        ConvertToMultiByteCustom((TUint8*)timeLocale,  month);	
                    }
                    else	
                    {
    #endif
                    ConvertToMultiByteL((TUint8*)timeLocale,  month, charsetID);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                    }
    #endif
                    iTime_locale->month[aMonth] = (const char*)  timeLocale;
                    iTime_locale->alt_month[aMonth] = (const char*)  timeLocale;
                    timeLocale += strlen((char*) timeLocale) + 1;
                }
    
                const TText16* abbMonth = ((reinterpret_cast<TText*const*>((const TText*) iData[FnMonthAbbTable]()))[aMonth]);
                if(abbMonth)
                {
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                    if(!localeName)
                    {
                        ConvertToMultiByteCustom((TUint8*)timeLocale,  abbMonth);	
                    }
                    else	
                    {
    #endif
                    ConvertToMultiByteL((TUint8*)timeLocale, abbMonth, charsetID);	
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                    }
    #endif
                    iTime_locale->mon[aMonth] = (const char*)  timeLocale;
                    timeLocale += strlen((char*) timeLocale) + 1;				
                }
                            
            }
            
            //Set day and abbreviated day name information
    
            const TText16* day = ((reinterpret_cast<TText*const*>((const TText*) iData[FnDayTable]()))[6]);
            if(day)
            {
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                if(!localeName)
                {
                    ConvertToMultiByteCustom((TUint8*)timeLocale,  day);	
                }
                else	
                {
    #endif
                ConvertToMultiByteL((TUint8*)timeLocale, day, charsetID);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                }
    #endif
                iTime_locale->weekday[0] = (const char*)  timeLocale;
                timeLocale += strlen((char*) timeLocale) + 1;				
            }
            
    
            const TText16* abbDay = ((reinterpret_cast<TText*const*>((const TText*) iData[FnDayAbbTable]()))[6]);
            if(abbDay)
            {
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                if(!localeName)
                {
                    ConvertToMultiByteCustom((TUint8*)timeLocale,  abbDay);	
                }
                else
                {
    #endif
                
                ConvertToMultiByteL((TUint8*)timeLocale,  abbDay, charsetID);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                }
    #endif
                iTime_locale->wday[0] = (const char*)  timeLocale;				
                timeLocale += strlen((char*) timeLocale) + 1;
            }
            
            for(TInt aDay = 0 ; aDay < WEEKDAYS - 1; aDay++)
            {
    
                const TText16* day = ((reinterpret_cast<TText*const*>((const TText*) iData[FnDayTable]()))[aDay]);
                if(day)
                {
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                    if(!localeName)
                    {
                        ConvertToMultiByteCustom((TUint8*)timeLocale,  day);	
                    }
                    else	
                    {
    #endif
                    ConvertToMultiByteL((TUint8*)timeLocale, day, charsetID);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                    }
    #endif
                    iTime_locale->weekday[aDay + 1] = (const char*)  timeLocale;
                    timeLocale += strlen((char*) timeLocale) + 1;				
                }
                
    
                const TText16* abbDay = ((reinterpret_cast<TText*const*>((const TText*) iData[FnDayAbbTable]()))[aDay]);
                if(abbDay)
                {
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                    if(!localeName)
                    {
                        ConvertToMultiByteCustom((TUint8*)timeLocale,  abbDay);	
                    }
                    else	
                    {
    #endif
                    ConvertToMultiByteL((TUint8*)timeLocale,  abbDay, charsetID);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                    }  
    #endif
                    iTime_locale->wday[aDay + 1] = (const char*)  timeLocale;				
                    timeLocale += strlen((char*) timeLocale) + 1;
                }
                            
            }
            
            //Set AM string
    
            const TText16* am = ((reinterpret_cast<TText*const*>((const TText*) iData[FnAmPmTable]()))[0]);
            if(am)
            {
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                if(!localeName)
                {
                    ConvertToMultiByteCustom((TUint8*)timeLocale,  am);	
                }
                else	
                {
    #endif
                ConvertToMultiByteL((TUint8*)timeLocale, am, charsetID);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                }  
    #endif
                iTime_locale->am = (const char*)  timeLocale;					
                timeLocale += strlen((char*) timeLocale) + 1;
            }
            
            //Set PM string
    
            const TText16* pm = ((reinterpret_cast<TText*const*>((const TText*) iData[FnAmPmTable]()))[1]);
            if(pm)
            {
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                if(!localeName)
                {
                    ConvertToMultiByteCustom((TUint8*)timeLocale,  pm);	
                }
                else	
                {
    #endif
                ConvertToMultiByteL((TUint8*)timeLocale, pm, charsetID);
    #ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
                }  
    #endif
                iTime_locale->pm = (const char*)  timeLocale;					
                timeLocale += strlen((char*) timeLocale) + 1;
            }
                    
    
            Mem::Copy(&iLocaleTimeDateFormat.iTimeFormatSpec[0], (const TAny*) iData[FnTimeFormatSpec](), sizeof(TText) * (KMaxTimeFormatSpec+1));
            TText* timeForm = iLocaleTimeDateFormat.iTimeFormatSpec;
            
            //Set time format(T_FMT) and am/pm time format(T_FMT_AMPM) string
            if(timeForm)
            {
                const TPtrC time(timeForm);
                TBuf8<TIME_FORMAT_LENGTH> timeFormat;
                timeFormat.Copy(time);
                
                TChar timeSeparator = iLocale.TimeSeparator(1);
                TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> timeSeparator1;
                timeSeparator1.Append(timeSeparator);
                
                timeSeparator = iLocale.TimeSeparator(2);
                TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> timeSeparator2;
                timeSeparator2.Append(timeSeparator);
                
                TTimeFormat time12hror24hr = iLocale.TimeFormat();
                
                if(KErrNotFound != timeFormat.Locate('J'))
                {
                    //Check for time format(12/24 hour format)
                    if(time12hror24hr == ETime12)
                    {
                        strcpy((char*) timeLocale,TWELVE_HOUR_STR);
                    }
                    else
                    {
                        strcpy((char*) timeLocale,TWENTY_FOUR_HOUR_STR);
                    }
                }
                
                else if(KErrNotFound != timeFormat.Locate('I'))
                {
                    strcpy((char*) timeLocale,TWELVE_HOUR_STR);
                }	
                
                else if(KErrNotFound != timeFormat.Locate('H'))
                {
                    strcpy((char*) timeLocale,TWENTY_FOUR_HOUR_STR);
                }	
            
                strncat((char*) timeLocale, (char*) timeSeparator1.Ptr(), timeSeparator1.Length());
                strcat((char*) timeLocale, MINUTE_STR);
                strncat((char*) timeLocale, (char*) timeSeparator2.Ptr(), timeSeparator2.Length());
                strcat((char*) timeLocale, SECOND_STR);
                
                if(time12hror24hr == ETime12)
                {
    
                    if((KErrNotFound != timeFormat.Find(KAMPMSpace12hr)) || (KErrNotFound != timeFormat.Find(KAMPMSpace)) )
                    {
                        if((KErrNotFound != timeFormat.Find(KAMPMNoSpace12hr)) || (KErrNotFound != timeFormat.Find(KAMPMNoSpace)))
                        {
                            strcat((char*) timeLocale, AMPM_NO_SPACE);
                        }
                        else
                        {
                            strcat((char*) timeLocale, AMPM_SPACE);
                        }
                    }	
                }
                            
                iTime_locale->ampm_fmt =  (const char*)  timeLocale;
                iTime_locale->X_fmt = iTime_locale->ampm_fmt;
                timeLocale += strlen((char*) timeLocale) + 1;
            }
            
            TDateFormat aDateFormat = iLocale.DateFormat();
            //Set date fromat string(D_FMT)	
            TChar dateSeparator = iLocale.DateSeparator(1);
            TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> dateSeparator1;
            dateSeparator1.Append(dateSeparator);
            
            TBuf8<DECIMAL_THOUSAND_DATE_TIME_SEPARATOR> dateSeparator2;
            dateSeparator = iLocale.DateSeparator(2);
            dateSeparator2.Append(dateSeparator);
    
            //Americal date formt(mm/dd/YYYY)
            if(EDateAmerican == aDateFormat)
            {
                strcpy((char*) timeLocale, MONTH_STR);
                strncat((char*) timeLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
                strcat((char*) timeLocale, DAY_STR);
                strncat((char*) timeLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
                strcat((char*) timeLocale, YEAR_STR);
                iTime_locale->x_fmt = (const char*)  timeLocale;
            }
            
            //European date format(dd/mm/YYYY)
            if(EDateEuropean == aDateFormat)
            {
                strcpy((char*) timeLocale, DAY_STR);
                strncat((char*) timeLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
                strcat((char*) timeLocale, MONTH_STR);
                strncat((char*) timeLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
                strcat((char*) timeLocale, YEAR_STR);
                iTime_locale->x_fmt = (const char*)  timeLocale;
            }
            
            //Japanese date format(YYYY/mm/dd)
            if(EDateJapanese == aDateFormat)
            {
                strcpy((char*) timeLocale, YEAR_STR);
                strncat((char*) timeLocale, (char*) dateSeparator1.Ptr(), dateSeparator1.Length());
                strcat((char*) timeLocale, MONTH_STR);
                strncat((char*) timeLocale, (char*) dateSeparator2.Ptr(), dateSeparator2.Length());
                strcat((char*) timeLocale, DAY_STR);
                iTime_locale->x_fmt = (const char*)  timeLocale;
            }
            
            iTime_locale->md_order = iTime_locale->c_fmt = iTime_locale->date_fmt = (const char*) L"";
                    ;
            timeLocale = startAddress;
            return 0;	
		
}

struct lc_numeric_T* CLocale::GetCurrentNumericLocale(void)
{

	if(NULL != iNumeric_locale)
	{
		return iNumeric_locale;	
	}
	return NULL;
	
}

struct lc_monetary_T* CLocale::GetCurrentMonetaryLocale(void)
{
	if(NULL != iMonetary_locale)
	{
		return iMonetary_locale;	
	}
	return NULL;
}

struct lc_time_T* CLocale::GetCurrentTimeLocale(void)
{
	if(NULL != iTime_locale)
	{
		return iTime_locale;
	}
	
	return NULL;
}


TInt CLocale::LoadLocale(TDesC& aLocale)
{
    TPtrC src(aLocale);
    TBuf8<LOCALE_LENGTH> locale_name;
    locale_name.Copy(src);
    char locale[LOCALE_LENGTH];
    strncpy(locale, (char*) locale_name.Ptr(), locale_name.Length());
    locale[locale_name.Length()] = '\0';
    
    if (NULL == strchr(locale, '.'))
        {
        return 0;
        }

    // Open the file locale mapping(mapping of POSIX locale string to language variant DLL number) file
    RFile file;
    
#ifndef __EPOC32__
    _LIT(localeFile, "\\resource\\openc\\locales.txt");
#else
    TFileName localeFile;
    GetInstallationDataDir(localeFile);
    localeFile.Append(_L("locales.txt"));
#endif //__EPOC32__
    
    int retVal = file.Open(Backend()->FileSession(), localeFile, EFileRead);
    if(KErrNone != retVal)
        {
        return 0;
        }
    
    int pos = 0;
    TBuf8<LOCALE_LENGTH> localeInfo;
    retVal = ReadALine(file, pos, localeInfo);
    char* buffer = (char*) localeInfo.Ptr();
        
    //Check whether locale is supported or not
    int i = 0;
    int flag = 0;
    while( retVal > 0)
    {
        char str[LOCALE_LENGTH];
        while(buffer[i] != '=')
        {
            str[i] = buffer[i];
            i++;
        }
        str[i] = '\0';
        if(!strcmp(str,locale))
        {
            flag = 1;
            break;
        }
        i = 0;
        retVal = ReadALine(file, pos, localeInfo);
        buffer = (char*) localeInfo.Ptr();
    }
    if(flag)
    {
        DllName.Copy(KDllName);
        DllName.Append('.');
        i++;
        int len = localeInfo.Length();
        while(i < len)
            {
            DllName.Append(buffer[i]);  
            i++;
            }
        file.Close();
        return 1;   
            }
        
        file.Close();
        return 0;

}


TInt CLocale::ConvertToMultiByteL(TUint8* aNarrowCharString, const TText16* aWideCharString, TInt  aCharsetUID)
{
	//Create a handle to a file server session and connect to the file server. 
	//Push it on to cleanup stack
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	
	//Allocate and constructs a CCnvCharacterSetConverter object
	CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewLC();

	//Search the character set array  containing all of the character sets for which conversion is available
	CCnvCharacterSetConverter::TAvailability  avail = conv->PrepareToConvertToOrFromL(aCharsetUID, fileSession);
	if(CCnvCharacterSetConverter::ENotAvailable == avail)
		{
			CleanupStack::PopAndDestroy(2);
			*aNarrowCharString = '\0';
			return -1;	
		}
		
	const TPtrC16 unicodeText(aWideCharString);
	TBuf8<NON_UNICODE_TEXT_BUFFER_LENGTH> nativeText;
	
	TInt retVal = KErrNone;
	TInt aNumberOfUnconvertibleCharacters = 0;
	TInt aIndexOfFirstByteOfFirstUnconvertibleCharacter = 0;
	
	//Convert text encoded in the Unicode character set (UCS-2) into other character sets
	retVal = conv->ConvertFromUnicode(nativeText,unicodeText, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
	TInt returnCode = 0;
	//Check for illegal characters in the input
	if(retVal == CCnvCharacterSetConverter::EErrorIllFormedInput)
	{
		*aNarrowCharString = '\0';
		returnCode = -1;
	}
	
	//Check for incomplete characters in the input, set the errno accordingly
	else if(aNumberOfUnconvertibleCharacters)
	{
		*aNarrowCharString = '\0';
		returnCode = -1;
	}
	else
	{
	strncpy((char*) aNarrowCharString, (char*) nativeText.Ptr(), nativeText.Length());
	aNarrowCharString[nativeText.Length()] = '\0';
	returnCode = 0;
	}
	CleanupStack::PopAndDestroy(2); // conv, fileSession
	return returnCode;
	
}

TInt CLocale::GetCharcaterSetIDL(const char* aLocaleName)
{
	//extract the charcate set name  from locale string
	char* temp = strchr(aLocaleName, '.');
	char locale[LOCALE_LENGTH];
	if(NULL == temp)
	{
		return -1;
	}
	temp++;
	int  j= 0;
	while((*temp != '@') && *temp != '\0')
	{
		locale[j++]	 = *temp++;
	}
	locale[j++] = '\0';
	
	//Create a handle to a file server session and connect to the file server. 
	//Push it on to cleanup stack
	RFs fileSession;
	User::LeaveIfError(fileSession.Connect());
	CleanupClosePushL(fileSession);
	
	TInt characterSetUID = 0;
	
	//Allocate and constructs a CCnvCharacterSetConverter object
	CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewLC() ;
		
	//Create an array identifying all the character sets for which conversion is available
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet> *charSet = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL(fileSession);
	CleanupStack::PushL( charSet );
	
	TInt i = 0;
	//Get the number of character sets for which conversion is available
	TInt count = charSet->Count();
	while(i < count)
	{
		CCnvCharacterSetConverter::SCharacterSet characterSet = charSet->At(i);	
		i++;
		//Get the UID of character set for which conversion is available
		TUint charSetUID = characterSet.Identifier();
				
		const TBufC<CHARACTER_SET_NAME> charSetName = characterSet.NameIsFileName()?TParsePtrC(characterSet.Name()).Name():characterSet.Name();								
		TBuf8<CHARACTER_SET_NAME> aCharSetname;
		aCharSetname.Copy(charSetName);
	
		//Assign the character set UID
		if(!aCharSetname.Compare(TPtrC8((const TText8*) locale)))
		{
			characterSetUID = charSetUID;
			break;
		}
					
		HBufC8*  stdInterName = conv->ConvertCharacterSetIdentifierToStandardNameL(charSetUID, fileSession);					
		if(NULL != stdInterName)
		{
			if(!stdInterName->Compare(TPtrC8((const TText8*) locale)))
			{
				characterSetUID = charSetUID;
				delete stdInterName;
				stdInterName = NULL;		
				break;
			}
			delete stdInterName;
			stdInterName = NULL;			
		}
	}
	
	if(!characterSetUID)
	{
		characterSetUID =  -1;
	}
		
	CleanupStack::PopAndDestroy(3); //charSet, conv, fileSession
	return characterSetUID;
}

/* ReadALine : Reads one line at a time from specified file */
inline TInt ReadALine(RFile& aFile, TInt& aPos, TDes8& aDesc)
	{
	TInt ret = aFile.Read(aPos, aDesc);
	TChar delimOne = '\r';
	TChar delimTwo = '\n';
	
	TInt len = aDesc.Length();
	if(!ret && len)
		{
		//Always assumes that each line will end with "\r\n" except the last line
		TInt pos = aDesc.Locate(delimOne);
		pos = aDesc.Locate(delimTwo);
			
		if(KErrNotFound != pos)
			{
			aPos = aPos + pos + 1;
			len = pos - 1;
			aDesc.SetLength(len);
			}
		else
			{
			aPos = aPos + len;
			}
		}
	return len;
	}
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT

void CLocale::ConvertToMultiByteCustom(TUint8* aNarrowCharString, const TText16* aWideCharString)
	{
	const TPtrC unicodeText(aWideCharString);
	TBuf8<NON_UNICODE_TEXT_BUFFER_LENGTH> nativeText;
    nativeText.Copy(unicodeText);
    strncpy((char*) aNarrowCharString, (char*) nativeText.Ptr(), nativeText.Length());
    aNarrowCharString[nativeText.Length()] = '\0';
	}

TInt CLocale::SyncLocale(TLocaleAspect aspect)
	{   
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
	TBuf<NON_UNICODE_TEXT_BUFFER_LENGTH> reg_dllname;
#endif
	TBuf<NON_UNICODE_TEXT_BUFFER_LENGTH> Kdname;
	TExtendedLocale locale;
    locale.LoadSystemSettings();
    locale.GetLocaleDllName(aspect,Kdname);
#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
    // check if the new  dlls is present then load it or go for old dll
    if( Kdname.FindC(_L("elocl.")) == KErrNotFound ) 
        {
        iOldDllPresent = 0;
        locale.GetLocaleDllName(ELocaleTimeDateSettings,reg_dllname);
        return (GetSystemLocale(Kdname,reg_dllname ));
        }
    else
        iOldDllPresent = 1;
#endif
    return (GetSystemLocale(Kdname));
	}

const TText* CLocale::GetLocaleName(void)
	{
	TPtrC dllname(DllName);
	return dllname.Ptr();
	}


#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
    
const TText* CLocale::GetCollateLocaleName(void)
    {
        TPtrC dllname(ColDllName);
        return dllname.Ptr(); 
     }
#endif


#endif // SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT




