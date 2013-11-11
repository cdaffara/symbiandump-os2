// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name         : strcoll.cpp
// Part of      : MRT
// Implementation for wcpcpy API
// Version      : 1.0
//

#include <stdlib.h>
#include <string.h>
#include <e32std.h>
#include <charconv.h>
#include <e32des16.h>
#include <f32file.h> 
#include <wchar.h>
#include <langinfo.h>
#include <e32debug.h>
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
#include <collate.h>
#include "localeinfo.h"
#endif

#ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
#include <utf.h> 
#endif

#if (defined(__SYMBIAN32__) && (defined(__WINSCW__) || defined(__WINS__)))
#include "libc_wsd_defs.h"
#endif


#ifdef __SYMBIAN32__
#define MAX_COL_LEVEL 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SYMBIAN32__
	extern "C" char* LC_COLLATE_LocaleName[30];
#ifdef EMULATOR

char *GET_WSD_VAR_NAME(LC_COLLATE_LocaleName, g)();
#define LC_COLLATE_LocaleName (GET_WSD_VAR_NAME(LC_COLLATE_LocaleName, g)())
#endif //EMULATOR

#endif //__SYMBIAN32__

#ifdef __cplusplus
}
#endif

static int DoConvertionToUnicode(const char* s1, TUint16* aUnicodeText);

EXPORT_C
int strcoll(const char *s1, const char *s2)
{
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
    TInt retVal;
	char *ret=NULL;
#endif
	if((strcmp("C",(const char*) LC_COLLATE_LocaleName)==0) ||(strcmp("POSIX", (const char*) LC_COLLATE_LocaleName)==0)) 
		return strcmp (s1,s2);
	else	  	
	{
		TUint16* text1 = new TText16[strlen(s1) + 1];
		TUint16* text2 = new TText16[strlen(s2 ) + 1];
		
		if((DoConvertionToUnicode(s1, text1) == -1) || (DoConvertionToUnicode(s2, text2) == -1))
			{
		  	delete [] text1;
		 	delete [] text2;
			return -1;
			}

		const TPtrC leftString((TText*) text1);
		const TPtrC rightString((TText*) text2);
		
		TInt left = leftString.Length();
		TInt right = rightString.Length();
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
		ret=(char*)LC_COLLATE_LocaleName;
		if(*ret==NULL)
		{
#endif
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
		retVal = Mem :: CompareC(text1,left,text2,right, MAX_COL_LEVEL,NULL);	
#else
	   	TInt retVal = Mem :: CompareC(text1,left,text2,right, MAX_COL_LEVEL,NULL);
#endif
#ifdef SYMBIAN_OE_ENHANCED_LOCALE_SUPPORT
        }
        else
        {
        CLocale* loc = CLocale::GetInstance();
        TExtendedLocale elocale;
        const TText* text;
        
        #ifdef SYMBIAN_DISTINCT_LOCALE_MODEL
        text = loc->GetCollateLocaleName();
        TPtrC localenew(text);
        retVal = elocale.LoadLocaleAspect(localenew); 
        if ( retVal == KErrNotFound ) // load old collate dll
            {
            text = loc->GetLocaleName();
            TPtrC locale(text);
            retVal = elocale.LoadLocaleAspect(ELocaleCollateSetting,locale);  
            }
        #else
        text = loc->GetLocaleName();
        TPtrC locale(text);
        retVal = elocale.LoadLocaleAspect(ELocaleCollateSetting,locale);  
        #endif
        if( retVal == KErrNone )
            {    
            TCollationMethod chmethod=elocale.GetPreferredCollationMethod(0);
            retVal = Mem :: CompareC(text1,left,text2,right, MAX_COL_LEVEL,&chmethod);
            }
        }
#endif
	  	delete [] text1;
	 	delete [] text2;
		return retVal;
	}	
		 
}

static int DoConvertionToUnicode( const char* s1, TUint16* aUnicodeText)
{

    TInt r = KErrNone;
    RFs fileSession;
    r = fileSession.Connect();
    if (r != KErrNone)
        {
            return r;
        }
    CleanupClosePushL(fileSession);
        
    CCnvCharacterSetConverter* conv = CCnvCharacterSetConverter::NewL() ;
    CleanupStack::PushL(conv);
        
    CArrayFix<CCnvCharacterSetConverter::SCharacterSet> *charSet = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL(fileSession);
    CleanupStack::PushL( charSet );
    
    TInt i = 0;
    TInt count = charSet->Count();
    TUint charSetUID = 0 ;
    while(i < count)
    {
        CCnvCharacterSetConverter::SCharacterSet characterSet = charSet->At(i); 
        i++;
        charSetUID = characterSet.Identifier();
       // const TBufC<25> charSetName = characterSet.NameIsFileName()?TParsePtrC(characterSet.Name()).Name():characterSet.Name();                             
        const TBufC<50> charSetName = characterSet.NameIsFileName()?TParsePtrC(characterSet.Name()).Name():characterSet.Name();
        const int CHAR_SET_NAME_SIZE = 100; //worst case size
        TBuf8<CHAR_SET_NAME_SIZE> aCharSetName;
       // if(wcstombs((char *)aCharSetName.Ptr(),(wchar_t*)(const_cast<TBufC<25>*>(&charSetName))->Des().PtrZ(),CHAR_SET_NAME_SIZE) == (size_t)-1)
        if(wcstombs((char *)aCharSetName.Ptr(),(wchar_t*)(const_cast<TBufC<50>*>(&charSetName))->Des().PtrZ(),CHAR_SET_NAME_SIZE) == (size_t)-1)
            {
                CleanupStack::PopAndDestroy(3); 
                return -1;
            }
        if(!aCharSetName.Compare(TPtrC8((const TText8*) LC_COLLATE_LocaleName)))
        {
            break;;
        }
        HBufC8*  stdInterName = conv->ConvertCharacterSetIdentifierToStandardNameL(charSetUID, fileSession);
        if(NULL != stdInterName)
        {
            if(!stdInterName->Compare(TPtrC8((const TText8*) LC_COLLATE_LocaleName)))
            {
                delete stdInterName;
                stdInterName = NULL;
                break;
            }
            
            delete stdInterName;
            stdInterName = NULL;
        }

	}
	if(!charSetUID)	
		{
				CleanupStack::PopAndDestroy(3); 
				return -1;
		}

	TPtrC8 remainderOfForeignText((const TText8*) s1);
	TInt length = 0;
	while(remainderOfForeignText.Length() > 0)
	{
		TBuf16<300> unicodeText;
		TInt retVal = KErrNone;
					
		CCnvCharacterSetConverter::TAvailability  avail = conv->PrepareToConvertToOrFromL(charSetUID, fileSession);
		if(CCnvCharacterSetConverter::ENotAvailable == avail)
			{
				CleanupStack::PopAndDestroy(3); 
				return -1;
			}
					
			TInt state = CCnvCharacterSetConverter::KStateDefault;
			TInt aNumberOfUnconvertibleCharacters = 0;
			TInt aIndexOfFirstByteOfFirstUnconvertibleCharacter = 0;

			retVal = conv->ConvertToUnicode(unicodeText, remainderOfForeignText, state, aNumberOfUnconvertibleCharacters, aIndexOfFirstByteOfFirstUnconvertibleCharacter);
			length += unicodeText.Length();
			
			wcsncpy((wchar_t*)aUnicodeText, (wchar_t*) unicodeText.Ptr(), unicodeText.Length() );
			
			if(retVal < 0 && (retVal != CCnvCharacterSetConverter::EErrorIllFormedInput))
			{
				CleanupStack::PopAndDestroy(3); 
				return -1;
			}
					
			if(aNumberOfUnconvertibleCharacters)
			{
				
				CleanupStack::PopAndDestroy(3); 
				return -1;
			}
			
			if(retVal == CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
				
				CleanupStack::PopAndDestroy(3); 
				return -1;
			}
			
			remainderOfForeignText.Set(remainderOfForeignText.Right(retVal));

	}
	aUnicodeText[length] = '\0';
	CleanupStack::PopAndDestroy(3); 
	return 0;

}		



