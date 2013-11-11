// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <bafl/langutil.h>
#include <hal.h>
#include <hal_data.h>
#include "LangUtilImpl.h"


/**
Mimimum length of a filename and mimimum length of a suffix.
Note these two values are tied together.
*/
const TInt KInvNameAndMinSuffixLength = 2;

#define ISDIGIT(c) (c-'0' >= 0 && c-'0' <= 9)

_LIT(KAllDrives, "YXWVUTSRQPONMLKJIHGFEDCBAZ");

LOCAL_C const TLanguage dp0[] = { ELangCanadianEnglish, ELangAmerican,ELangEnglish, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangAustralian,ELangNewZealand,ELangInternationalEnglish,ELangSouthAfricanEnglish,ELangNone };
LOCAL_C const TLanguage dp1[] = { ELangAmerican, ELangEnglish,ELangCanadianEnglish, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangAustralian,ELangNewZealand,ELangInternationalEnglish,ELangSouthAfricanEnglish, ELangNone };
LOCAL_C const TLanguage dp2[] = { ELangAustralian, ELangEnglish, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangNewZealand,ELangInternationalEnglish,ELangSouthAfricanEnglish, ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp3[] = { ELangSouthAfricanEnglish, ELangEnglish, ELangAustralian, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangNewZealand,ELangInternationalEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp4[] = { ELangInternationalEnglish, ELangEnglish, ELangAustralian, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangNewZealand,ELangSouthAfricanEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp5[] = { ELangEnglish_Apac, ELangEnglish, ELangAustralian, ELangAmerican,ELangInternationalEnglish,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangNewZealand,ELangSouthAfricanEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp6[] = { ELangEnglish_Taiwan, ELangEnglish, ELangAustralian, ELangAmerican, ELangEnglish_Apac,ELangInternationalEnglish,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangNewZealand,ELangSouthAfricanEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp7[] = { ELangEnglish_HongKong, ELangEnglish, ELangAustralian, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangInternationalEnglish,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangNewZealand,ELangSouthAfricanEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp8[] = { ELangEnglish_Prc, ELangEnglish, ELangAustralian, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangInternationalEnglish,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangNewZealand,ELangSouthAfricanEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp9[] = { ELangEnglish_Japan, ELangEnglish, ELangAustralian, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangInternationalEnglish,ELangEnglish_Thailand,ELangEnglish_India,ELangNewZealand,ELangSouthAfricanEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp10[] = { ELangEnglish_Thailand, ELangEnglish, ELangAustralian, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangInternationalEnglish,ELangEnglish_India,ELangNewZealand,ELangSouthAfricanEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp11[] = { ELangEnglish_India, ELangEnglish, ELangAustralian, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangInternationalEnglish,ELangNewZealand,ELangSouthAfricanEnglish,ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp12[] = { ELangNewZealand, ELangEnglish, ELangAmerican, ELangEnglish_Apac,ELangEnglish_Taiwan,ELangEnglish_HongKong,ELangEnglish_Prc,ELangEnglish_Japan,ELangEnglish_Thailand,ELangEnglish_India,ELangAustralian,ELangInternationalEnglish,ELangSouthAfricanEnglish, ELangCanadianEnglish,ELangNone };
LOCAL_C const TLanguage dp13[] = { ELangInternationalFrench,ELangFrench,ELangSwissFrench,ELangBelgianFrench,ELangCanadianFrench,ELangNone };
LOCAL_C const TLanguage dp14[] = { ELangBelgianFrench, ELangFrench,ELangInternationalFrench,ELangSwissFrench,ELangCanadianFrench,ELangNone };
LOCAL_C const TLanguage dp15[] = { ELangCanadianFrench, ELangFrench,ELangInternationalFrench,ELangSwissFrench,ELangBelgianFrench,ELangNone };
LOCAL_C const TLanguage dp16[] = { ELangFrench,ELangInternationalFrench,ELangSwissFrench,ELangBelgianFrench,ELangCanadianFrench,ELangNone };
LOCAL_C const TLanguage dp17[] = { ELangSwissFrench,ELangFrench,ELangInternationalFrench,ELangBelgianFrench,ELangCanadianFrench,ELangNone };
LOCAL_C const TLanguage dp18[] = { ELangSwissGerman,ELangGerman,ELangAustrian,ELangNone };
LOCAL_C const TLanguage dp19[] = { ELangAustrian,ELangGerman,ELangSwissGerman,ELangNone };
LOCAL_C const TLanguage dp20[] = { ELangGerman,ELangSwissGerman,ELangAustrian,ELangNone };
LOCAL_C const TLanguage dp21[] = { ELangSerbian,ELangCroatian,ELangNone };
LOCAL_C const TLanguage dp22[] = { ELangCroatian,ELangSerbian,ELangNone };
LOCAL_C const TLanguage dp23[] = { ELangRomanian,ELangMoldavian,ELangNone };
LOCAL_C const TLanguage dp24[] = { ELangMoldavian,ELangRomanian,ELangNone };
LOCAL_C const TLanguage dp25[] = { ELangBelgianFlemish,ELangDutch,ELangNone };
LOCAL_C const TLanguage dp26[] = { ELangDutch,ELangBelgianFlemish,ELangNone };
LOCAL_C const TLanguage dp27[] = { ELangAfrikaans,ELangDutch,ELangNone };
LOCAL_C const TLanguage dp28[] = { ELangMalay_Apac,ELangMalay,ELangNone };
LOCAL_C const TLanguage dp29[] = { ELangIndonesian_Apac,ELangIndonesian,ELangNone };
LOCAL_C const TLanguage dp30[] = { ELangSpanish,ELangInternationalSpanish,ELangLatinAmericanSpanish,ELangNone };
LOCAL_C const TLanguage dp31[] = { ELangLatinAmericanSpanish,ELangSpanish,ELangInternationalSpanish,ELangNone };
LOCAL_C const TLanguage dp32[] = { ELangInternationalSpanish,ELangSpanish,ELangLatinAmericanSpanish,ELangNone };
LOCAL_C const TLanguage dp33[] = { ELangCyprusGreek,ELangGreek,ELangNone };
LOCAL_C const TLanguage dp34[] = { ELangGreek,ELangCyprusGreek,ELangNone };
LOCAL_C const TLanguage dp35[] = { ELangSwissItalian,ELangItalian,ELangNone };
LOCAL_C const TLanguage dp36[] = { ELangItalian,ELangSwissItalian,ELangNone };
LOCAL_C const TLanguage dp37[] = { ELangBrazilianPortuguese,ELangPortuguese,ELangNone };
LOCAL_C const TLanguage dp38[] = { ELangPortuguese,ELangBrazilianPortuguese,ELangNone };
LOCAL_C const TLanguage dp39[] = { ELangFinlandSwedish,ELangSwedish,ELangNone };
LOCAL_C const TLanguage dp40[] = { ELangSwedish,ELangFinlandSwedish,ELangNone };
LOCAL_C const TLanguage dp41[] = { ELangCyprusTurkish,ELangTurkish,ELangNone };
LOCAL_C const TLanguage dp42[] = { ELangTurkish,ELangCyprusTurkish,ELangNone };
LOCAL_C const TLanguage dp43[] = { ELangHongKongChinese, ELangTaiwanChinese, ELangPrcChinese,ELangNone };
LOCAL_C const TLanguage dp44[] = { ELangTaiwanChinese, ELangHongKongChinese,ELangPrcChinese,ELangNone };
LOCAL_C const TLanguage dp45[] = { ELangPrcChinese, ELangHongKongChinese, ELangTaiwanChinese,ELangNone };
LOCAL_C const TLanguage * const KEquivalentLists[] = { dp0,  dp1,  dp2,  dp3,  dp4,  dp5,  dp6,  
		dp7,  dp8,  dp9,  dp10,  dp11,  dp12,  dp13,  dp14,  dp15,  dp16,  dp17,
		dp18,  dp19,  dp20,  dp21,  dp22,  dp23,  dp24,  dp25,  dp26,  dp27,  
		dp28,  dp29,  dp30,  dp31,  dp32,  dp33,  dp34,  dp35,  dp36,  dp37,  
		dp38,  dp39,  dp40,  dp41,  dp42,  dp43,  dp44,  dp45};



LOCAL_C TBool IsLanguageExtended(const TLanguage aLanguage)
	{
	// For compatibility reasons, ELangNone is 0xFFFF. However, it's not an extended language.
	if ((aLanguage==ELangNone) || ((static_cast<TUint>(aLanguage))<=KDialectMask))
		return EFalse;
	else
		return ETrue;
	}


LOCAL_C TLanguage BaseLanguage(const TLanguage aLanguage)
	{
	if (IsLanguageExtended(aLanguage))
		return static_cast<TLanguage>(aLanguage & KDialectMask);
	else
		return aLanguage;
	}

LOCAL_C TLanguage NextLanguage(TLanguage aLanguage)
/** Returns the next best language to use after aLanguage,
based on Symbian's base table of language near-equivalence.
@internalAll */
	{
	switch (aLanguage)
		{
		case ELangAustralian:
		case ELangNewZealand:
		case ELangSouthAfricanEnglish:
		case ELangInternationalEnglish:
		case ELangAmerican:
		case ELangEnglish_Apac:
		case ELangEnglish_Taiwan:
		case ELangEnglish_HongKong:
		case ELangEnglish_Prc:
		case ELangEnglish_Japan:
		case ELangEnglish_Thailand:
			return ELangEnglish;
		case ELangCanadianEnglish:
			return ELangAmerican;	// 2-stage downgrade
		case ELangSwissFrench:
		case ELangBelgianFrench:
		case ELangInternationalFrench:
		case ELangCanadianFrench:
			return ELangFrench;
		case ELangSwissGerman:
		case ELangAustrian:
			return ELangGerman;
		case ELangInternationalSpanish:
		case ELangLatinAmericanSpanish:
			return ELangSpanish;
		case ELangSwissItalian:
			return ELangItalian;
		case ELangFinlandSwedish:
			return ELangSwedish;
		case ELangCyprusTurkish:
			return ELangTurkish;
		case ELangBelgianFlemish:
			return ELangDutch;
		case ELangHongKongChinese:
			return ELangTaiwanChinese;
		case ELangCyprusGreek:
			return ELangGreek;
		case ELangMalay_Apac:
			return ELangMalay;
		case ELangBrazilianPortuguese:
			return ELangPortuguese;
		default:
			return ELangNone;	
		}
	}


void AddLanguage(TLanguagePath& aPath, TLanguage aNewLanguage)
/** Add language to the language path if there is space.
The first empty slot must have "ELangNone" in it. This will also be true
on exit. */ 
    {
    TLanguage *p = aPath;
    const TLanguage *end = &(aPath[KMaxDowngradeLanguages]);
    while (p != end)
        {
        if (*p == aNewLanguage)
            // language already in list
            break;
        if (*p == ELangNone)
            {
            // found the end of the list
            p[0] = aNewLanguage;
            p[1] = ELangNone;
            break;
            }
        ++p;
        }
    return;
    }

void MakeLanguageDowngradePath(TLanguagePath& aPath,
    TLanguage aCurrent, TLanguage aIdeal, const TLocale& aLocale)
    {
    TInt j = 0;
    if( aIdeal != ELangNone)
        {
        aPath[j++]=aIdeal;  
        }
    aPath[j++] = aCurrent;
    aPath[j++] = ELangNone;

    if (aCurrent & ~KDialectMask)
        AddLanguage(aPath, static_cast<TLanguage>(aCurrent & KDialectMask));

    for (TInt i=0;i<=2;i++)
        {
        AddLanguage(aPath, aLocale.LanguageDowngrade(i));
        AddLanguage(aPath, BaseLanguage(aLocale.LanguageDowngrade(i)));
        }

    while (ELangNone != (aCurrent = NextLanguage(BaseLanguage(aCurrent))))  
        AddLanguage(aPath, aCurrent);
    }



//EXPORT_C 
void LangUtil::GetDowngradePathL(const RFs& aFs, const TLanguage aCurrentLanguage, RArray<TLanguage>& aLanguageArray){

 	TLocale currentLocale; 
  	TNearestLanguageFileFinder languageDowngradePath(aFs);
  	TLanguage idealLanguage=IdealLanguage();
  	MakeLanguageDowngradePath(languageDowngradePath.iPath,aCurrentLanguage,idealLanguage, currentLocale);
 	aLanguageArray.Reset();
  	const TLanguage* p=languageDowngradePath.iPath;
  	while (*p != ELangNone)
  		{
		User::LeaveIfError(aLanguageArray.Append(*p));
  		++p;
  		}

}


//EXPORT_C 
void LangUtil::GetEquivalentLanguageList(TLanguage aLang, TLanguagePath& aEquivalents){

    aEquivalents[0] = aLang;
	aEquivalents[1] = ELangNone;
	const TInt len = sizeof(KEquivalentLists) / sizeof(KEquivalentLists[0]);
	for (TInt i = 0; i < len; ++i)
		{
		const TLanguage *ptr = KEquivalentLists[i];
		if (ptr[0] == aLang)
			{
			TInt index = 1;
			while (ELangNone != *ptr)
				{
				aEquivalents[index++] = (TLanguage)*(++ptr);
				}
			aEquivalents[index] = ELangNone;
			break;
			} // end if ptr[0]
		} // end for i

}



//EXPORT_C 
TLanguage LangUtil::IdealLanguage(){

		TLanguage* langPtr=(TLanguage*)Dll::Tls();
	
	if( langPtr==NULL)
		{
		return(ELangNone);
		}

	return(*langPtr);

}


//EXPORT_C 
void LangUtil::NearestLanguageFile(const RFs& aFs, TFileName& aName){
	
	TLanguage language;
	
	NearestLanguageFile( aFs, aName, language);
	
	(void)language;

}


//EXPORT_C 
void LangUtil::NearestLanguageFile(const RFs& aFs, TFileName& aName, TLanguage& aLanguage){
#if defined(DO_PROFILING)
	RDebug::ProfileReset(FIRST_PROFILE_INDEX, PROFILE_COUNT);
	RDebug::ProfileStart(PROFILE_INDEX_1);
#endif
	TNearestLanguageFileFinder finder(aFs);
	TBool goodSuffix=finder.SetFileName(aName);
	
	// Only continue if the suffix is good.
	if(goodSuffix)
		{
		// add preset customised resource drive to drive list  
		// Note that errors returned from AddCustomResourceDrive are ignored. This is because if 
		// a custom resource drive has not been found we still want to continue on with searching 
		// other drives according to our algorithm
		finder.AddCustomResourceDrive();
		
		TLocale locale;
		TLanguage idealLanguage;
		idealLanguage = IdealLanguage();
		MakeLanguageDowngradePath(finder.iPath, User::Language(), idealLanguage, locale);
		if (!finder.FindLanguageAndDrive()
			&& KErrNone != finder.FindFirstLanguageFileAndDrive())
			finder.RepairFileName();
		aLanguage = finder.Language();
		}
		
#if defined(DO_PROFILING)
	RDebug::ProfileEnd(PROFILE_INDEX_1);
	TProfile profile[PROFILE_COUNT];
	RDebug::ProfileResult(&profile[0], FIRST_PROFILE_INDEX, PROFILE_COUNT);
	if(goodSuffix)
		{
		RDebug::Print(_L("BaflUtils::NearestLanguageFile profile info: %d.%03ds"), profile[PROFILE_INDEX_1-FIRST_PROFILE_INDEX].iTime/1000000, profile[PROFILE_INDEX_1-FIRST_PROFILE_INDEX].iTime%1000000);
		}
	else
		{
		RDebug::Print(_L("BaflUtils::NearestLanguageFile (bad suffix ) profile info: %d.%03ds"), profile[PROFILE_INDEX_1-FIRST_PROFILE_INDEX].iTime/1000000, profile[PROFILE_INDEX_1-FIRST_PROFILE_INDEX].iTime%1000000);
		}
#endif

}


//EXPORT_C 
void LangUtil::NearestLanguageFileV2(const RFs& aFs, TFileName& aName, TLanguage& aLanguage){
	
	TNearestLanguageFileFinder finder(aFs);
	TBool goodSuffix=finder.SetFileName(aName);
	
	// Continue only if the suffix is good.
	if(goodSuffix)
		{
		// add preset customised resource drive to drive list  
		// Note that errors returned from AddCustomResourceDrive are ignored. This is because if 
		// a custom resource drive has not been found we still want to continue on with searching 
		// other drives according to our algorithm
		finder.AddCustomResourceDrive();
		
		GetEquivalentLanguageList(User::Language(), finder.iPath);
		if (!finder.FindLanguageAndDrive()
			&& KErrNone != finder.FindFirstLanguageFileAndDrive())
			finder.RepairFileName();
		aLanguage = finder.Language();
		}
	else
		{
		aLanguage = ELangNone;
		}

}


//EXPORT_C 
void LangUtil::ReleaseIdealLanguage(){

	TLanguage* aLanguage=(TLanguage*)Dll::Tls();
	if( aLanguage==NULL)
		return;
	
	delete aLanguage;
	Dll::FreeTls();

}


//EXPORT_C
TInt LangUtil::SetIdealLanguage(TLanguage aLanguage){
TLanguage* langPtr=(TLanguage*)Dll::Tls();
	if( langPtr==NULL)
		{
		langPtr=(TLanguage*)User::Alloc(sizeof(TLanguage));
		
		if(!langPtr) 
			return(KErrNoMemory);
		
		TInt ret=Dll::SetTls(langPtr);
		
		if(ret!=KErrNone)
			return(ret);
		}
	*langPtr=aLanguage;
	return(KErrNone);
	
}

TInt RRealDirectoryScanner::Open(RFs& aFs, const TDesC& aMatchPattern)
    {
    return iDir.Open(aFs, aMatchPattern,
        KEntryAttReadOnly | KEntryAttHidden | KEntryAttSystem | KEntryAttArchive);
    }

TInt RRealDirectoryScanner::Next(TEntry& aOut)
    {
    return iDir.Read(aOut);
    }

void RRealDirectoryScanner::Close()
    {
    iDir.Close();
    }

/**
Simply counts the number of numerical characters at the end of the name passed.

@internalComponent
@param          aFilename The filename to parse
                
@return         Count of the numeric digits at the end of the name passed, 
                e.g. x.r491 gives 3.
*/
TInt TNearestLanguageFileFinder::CountDigitsFromEnd(const TDesC& aFilename)
    {
    TInt digitCount = 0;
    
    for (TInt idx = aFilename.Length()-1; idx>=0 && ISDIGIT (aFilename [idx]); --idx)
        {
        ++digitCount;
        }
        
    return digitCount;
    }


/**
Counts the number of digits at the end of a filename.

@internalComponent
@param          aFilename The filename to parse
                
@return         Count of the numeric digits at the end of the suffix, 
                e.g. x.r491 gives 3.
                0 if no numeric end of suffix,
                KErrBadName for an invalid filename,
                KErrNotSupported if the filename (minus path) is less
                than or equal to KInvNameAndMinSuffixLength in length
*/
TInt TNearestLanguageFileFinder::CountDigitsFromEndInSuffix (const TDesC& aFilename)
    {
    TInt digitCount = 0;
    TInt slashIdx = 0;
    TInt len = aFilename.Length ();
    
    // NOTE: We didn't use TChar here as they are too slow.
    // We also didn't use TParse as they are too large.
    
    // don't work on the path
    for (slashIdx=len-1; slashIdx >= 0 && aFilename[slashIdx] != '\\'; --slashIdx)
    {/*do nothing*/};
    
    // Get new length
    if (slashIdx>=0) {len = len-slashIdx-1;}

    // Initial test to see if filename legal size.
    if (len > KInvNameAndMinSuffixLength)
        {
        digitCount = CountDigitsFromEnd(aFilename);

        // Can't store something bigger or we'll panic!
        if (digitCount > KMaxSuffixLength)
            {
            digitCount = KErrBadName;
            }
        else
        // numeric filename, e.g. "1234". 
        // No preceeding alpha character
        if (!(len-digitCount))
            {
            digitCount = KErrBadName;
            }
        }
    else
        {
        digitCount = KErrNotSupported;
        }
        
    return digitCount;
    }

RDirectoryScanner& TNearestLanguageFileFinder::DirectoryScanner()
    {
    return iDirScanner;
    }

TBool TNearestLanguageFileFinder::FileExists(const TDesC& aFileName) const
    {
    //return BaflUtils::FileExists(iFs, aFileName);
    TEntry entry;
    return(iFs.Entry(aFileName,entry)==KErrNone);
    
    }

TBool TNearestLanguageFileFinder::FindDrive()
    {
    ASSERT(iFileName);
    TBool found=EFalse;
    TInt driveLength=iDrives.Length();
    for (TInt drive = 0; drive!=driveLength; ++drive)
        {
        (*iFileName)[0] = iDrives[drive];
        if (FileExists(*iFileName))
            {
            found=ETrue;
            break;
            }
        }
    return found;
    }

TBool TNearestLanguageFileFinder::AppendLanguageCode(TLanguage aLanguage)
    {
    TInt rest = static_cast<TInt>(aLanguage);
#ifdef _DEBUG
    _LIT(KErrorMessage, "Bafl");
#endif
    __ASSERT_DEBUG(0 <= rest, User::Panic(KErrorMessage,KErrArgument));
    iFileName->SetLength(iBaseLength);
    const TInt remaining = iFileName->MaxLength() - iBaseLength;
    TInt soFar = 0;
    TBuf<1> num;
    num.Append('0');
    TBool appendLangSuccess = ETrue;
    TInt digitCount = 0;
    TInt digit = 0;
    while (rest)
        {
        if (remaining == soFar)
            {
            // no more room in descriptor- return rather than panic,
            // file cannot exist.
            iFileName->SetLength(iBaseLength);
            appendLangSuccess= EFalse;
            break;
            }
        // Convert the number to ASCII by consistantly getting the base 10 remainder to convert.
        // The number is updated minus the remainder for the next iteration.
        // eg (rest = 123) -> (12, r3) -> (1, r2) -> (0, r1)
        // Then insert the ASCII representation of the remainder into the filename end
        // so it appears the correct way round.
        // eg (filename.r) -> (filename.r3) -> (filename.r23) -> (filename.r123)
        digit = rest % 10;
        digitCount++;
        rest /= 10;
        num[0] = static_cast<TText16>(digit + '0');
        iFileName->Insert(iBaseLength, num);

        // Minimum suffix length is KInvNameAndMinSuffixLength
        // so we have to insert zeros to make this up.
        while (!rest && digitCount < KInvNameAndMinSuffixLength)
            {
            num[0] = static_cast<TText16>('0');
            iFileName->Insert(iBaseLength, num);
            ++digitCount;
            }
            
        ++soFar;
        }
        
    return appendLangSuccess;
    }


TBool TNearestLanguageFileFinder::FindLanguageAndDrive()
/** Search for files across all drives in all languages in the path plus the
language-neutral file. */
    {
    ASSERT(iFileName);
    // No point appending if the suffix is bad
    for (const TLanguage* currentLang = iPath; *currentLang != ELangNone; ++currentLang)
        {
        if (AppendLanguageCode(*currentLang) && FindDrive())
            {
            iLanguage = *currentLang;
            return ETrue;
            }
        }
    // search for language-neutral file
    iFileName->SetLength(iBaseLength);
    iFileName->Append(iSuffix);
    return FindDrive();
    }

TInt TNearestLanguageFileFinder::LanguageNumberFromFile(const TDesC& aFileName, const TDesC& aStem)
    {
    TInt lang = 0;
    TInt multiplier = 1;
    TInt leadingZeroCount = 0;
    TInt languageNumber = KErrNotFound;
    const TText* firstChar = aFileName.Ptr();
    const TText* lastChar = firstChar + aFileName.Length() - 1;
    const TText* currentChar = lastChar;
    // string cannot contain only numbers, because it must have a ':' in it
    while ('0' <= *currentChar && *currentChar <= '9')
        {
        if (*currentChar == '0')
            leadingZeroCount++;
        else
            {
            leadingZeroCount = 0;
            lang += multiplier * (*currentChar - '0');
            }
        multiplier *= 10;
        --currentChar;
        }
    TInt along=lastChar - currentChar;
    if (2 <= along)
        {
        // We have at least 2 digits at the end.
        // trim of bad leading zeros
        TInt maxTrim = along - 2;
        if (maxTrim < leadingZeroCount)
            {
            leadingZeroCount = maxTrim;
            }
        currentChar += leadingZeroCount;
        // we have at least 2 digits at the end but does the rest of it match the stem?
        TPtrC foundStem(firstChar, currentChar - firstChar + 1);
        //foundStem.CompareF(aStem.Right(foundStem.Length()))
        if (0 == foundStem.CompareF(aStem))
            {
            languageNumber=lang;
            }
        }
    return languageNumber;
    }

TInt TNearestLanguageFileFinder::FindFirstLanguageFile(RFs& aFs)
    {
    ASSERT(iFileName);
    iFileName->SetLength(iBaseLength);
    TPtrC name(*iFileName);
    TParsePtrC nameToParse(name);
    TPtrC nameStem(nameToParse.NameAndExt());
    iFileName->Append('*');
    TInt bestLanguageMatch = KMaxTInt;
    RDirectoryScanner& scanner = DirectoryScanner();
    TInt err = scanner.Open(aFs, *iFileName);
    if (err != KErrNone)
        {
        return err;
        }
    TEntry entry;
    while (KErrNone == scanner.Next(entry))
        {
        TInt lang = LanguageNumberFromFile(entry.iName, nameStem);
        if (0 < lang && lang < bestLanguageMatch)
            {
            bestLanguageMatch = lang;
            }
        }
    scanner.Close();
    if (bestLanguageMatch != KMaxTInt)
        {
        iLanguage = static_cast<TLanguage>(bestLanguageMatch);
        AppendLanguageCode(static_cast<TLanguage>(bestLanguageMatch));
        return KErrNone;
        }
    return KErrNotFound;
    }

// Try each drive for any language files
// iFileName must have a directory specifier
TInt TNearestLanguageFileFinder::FindFirstLanguageFileAndDrive()
    {
    ASSERT(iFileName);
    TInt findFirstResult=KErrNotFound;
    TInt driveLength=iDrives.Length();
    for (TInt drive = 0; drive != driveLength; ++drive)
        {
        (*iFileName)[0] = iDrives[drive];
        TInt err = FindFirstLanguageFile(CONST_CAST(RFs&,iFs));
        if (err == KErrNone || err == KErrNoMemory)
            {
            findFirstResult=err;
            break;
            }
        }
    return findFirstResult;
    }

/**
Invalid filenames are any filename whose length (minus path) must be greater
than KInvNameAndMinSuffixLength, and whose form is purely numerical, i.e. '1234' 
*/
TBool TNearestLanguageFileFinder::SetFileName(TFileName& aFileName)
    {
    iDrives.Zero();
    iFileName = &aFileName;
    iOriginalBaseLength = iFileName->Length();
    
    TInt suffixLength = CountDigitsFromEndInSuffix (aFileName);
    
    // No point trying for filenames thats are badly formed
    // or that are too large.
    if (suffixLength >= 0 && 
        KInvNameAndMinSuffixLength < iOriginalBaseLength)
        {
        if (suffixLength > 0)
            {
            // all of suffix to be replaced 
            iSuffix = iFileName->Right(suffixLength);
            iOriginalBaseLength -= suffixLength;
            iFileName->SetLength(iOriginalBaseLength);
            }
        else
            { 
            // No numerical part to suffix
            TInt periodIdx = 0;
            
            // Search for the period within range KInvNameAndMinSuffixLength 
            // from the end. As this must work for all values of
            // KInvNameAndMinSuffixLength
            for (TInt i = iOriginalBaseLength-1; 
                 !periodIdx && i >= (iOriginalBaseLength-KInvNameAndMinSuffixLength-1);
                 --i)
                {
                if ((*iFileName) [i] == '.')
                    {
                    periodIdx = i;
                    }
                }
            
            // Don't handle files ending in a period.
            // This is because the behaviour is different between Windows
            // and Symbian Fs. In Windows it strips the period off.
            //
            // However, and this shouldn't happen as it is not shown
            // (in the documentation) to be valid.
            // Just try our best.
            if (periodIdx == iOriginalBaseLength-1)
                {
                iSuffix.Zero();
                return EFalse;
                }
            else
            if (periodIdx)
                {
                // If there are KInvNameAndMinSuffixLength chars after the period
                // simply replace them.
                TInt right = iOriginalBaseLength-periodIdx-1;
                iSuffix = iFileName->Right(right);
                iOriginalBaseLength -= right;
                iFileName->SetLength(iOriginalBaseLength);                  
                }
            else
                {
                // Make the suffix start from KInvNameAndMinSuffixLength 
                // from the right
                TInt right = KInvNameAndMinSuffixLength;
                iSuffix = iFileName->Right(right);
                iOriginalBaseLength -= right;
                iFileName->SetLength(iOriginalBaseLength);                  
                }
            }
        }
    else
        {
        // bad or no suffix - treat the same
        iSuffix.Zero();
        return EFalse;
        }

    // For filenames with no drive letter prefix and also for filenames
    // shorter than the drive letter length, i.e. with no drive
    // information, insert it.
    // Handles if the user simply enters the drive, e.g. "c:".
    if (iOriginalBaseLength < KMaxDriveName || (*iFileName)[1] != ':')
        {
        // Set up the default if none supplied and make room in the filename 
        // array to contain a drive specification. Set initial drive letter to -1
        // so the iFileName is repaired before exited 
        iInitialDriveLetter = -1;
        iFileName->Insert(0, _L("_:")); 
        iDrives.Append('Z');
        }
    else
        {
       // Use the drive supplied inthe aName to NearestLanguageFile()
        iInitialDriveLetter = (*iFileName)[0];
        iDrives.Append(iInitialDriveLetter);
        }
    
    iBaseLength = iFileName->Length();
    
    return ETrue;
    }


TLanguage TNearestLanguageFileFinder::Language()
    {
    return iLanguage;
    }

TNearestLanguageFileFinder::TNearestLanguageFileFinder(
    const RFs& aFs)
    : iFs(aFs), iFileName(0), iLanguage(ELangNone)
    {
    }

void TNearestLanguageFileFinder::RepairFileName()
    {
    ASSERT(iFileName);
    iFileName->SetLength(iBaseLength);
    if (iInitialDriveLetter == -1)
        iFileName->Delete(0, 2);
    else
        (*iFileName)[0] = static_cast<TText>(iInitialDriveLetter);
    iFileName->SetLength(iOriginalBaseLength);
    iFileName->Append(iSuffix);
    }


/**
Add the custom resource drive to the start of the iDrives string.

The custom resource drive is a preset writeable drive on which customised 
resource files may be present. This drive takes priority over the other 
drives when searching for language files.

@return KErrNone if iDrives string was successfully modified; KErrAlreadyExists 
if the drive is already present in the string; otherwise one of 
the other system-wide error codes (iDrives will be unmodified). 
*/
TInt TNearestLanguageFileFinder::AddCustomResourceDrive()
    {
    TInt drive = GetCustomResourceDriveNumber();
    if (drive<0)
        return drive;
    
    // if drive not already in drive list
    if (iDrives.LocateF('A' + drive) < 0)
        {
        // add it
        _LIT(KDrivePlaceholder, "_");
        iDrives.Insert(0, KDrivePlaceholder);
        iDrives[0] = 'A' + drive;
        return KErrNone;
        }
    else
        return KErrAlreadyExists;
    }


void TNearestLanguageFileFinder::AddAllDrives()
    {
    ASSERT(iDrives.Length() < 2);
    if (iDrives.Length() == 0)
        {
        iDrives = KAllDrives;
        return;
        }
    TInt pos = KAllDrives().LocateF(iDrives[0]);
    if (pos < 0)
        {
        iDrives = KAllDrives;
        return;
        }
    iDrives.Append(KAllDrives().Left(pos));
    iDrives.Append(KAllDrives().Mid(pos + 1));
    }


/**
Get the value of the custom resource drive.

The custom resource drive is a preset writeable drive on which customised language resource 
files can reside. The drive number is accessed via the HAL attribute ECustomResourceDrive. 
It is then returned if it has been defined as a valid drive no.
Otherwise for backward compatibility reasons an attempt is then made to access the system 
drive HAL attribute instead. This drive number is returned if it has been defined as a valid 
drive number.  
Otherwise if neither a valid ECustomResourceDrive or ESystemDrive exists then KErrNotFound 
is returned.
 
Note that the ESystemDrive HAL attribute has been deprecated. It is accessed here to cater 
for existing implementations which still expect it to be used.
 
@return The drive number (corresponding to a TDriveNumber value) if successful; 
KErrNotFound if neither a valid ECustomResourceDrive or a valid ESystemDrive HAL attribute 
is defined;  
 
@see HAL::ECustomResourceDrive
@see HAL::ESystemDrive
*/
TInt TNearestLanguageFileFinder::GetCustomResourceDriveNumber() const
    {
    TInt drive = KErrNotFound;
    
    // access custom resource drive attribute  
    if (HAL::Get(HAL::ECustomResourceDrive, drive) == KErrNone)
        {
        // check that drive is valid
        if (drive>=EDriveA && drive<=EDriveZ)
            return drive;    
        }
                        
    // access system drive attribute  
    // (Note that ESystemDrive is deprecated. It is checked here 
    // solely for backward compatibility reasons.)      
    if (HAL::Get(HAL::ESystemDrive, drive) == KErrNone)
        {
        // check that drive is valid
        if (drive>=EDriveA && drive<=EDriveZ)
                return drive;
        }       
 
    return KErrNotFound;
    }


