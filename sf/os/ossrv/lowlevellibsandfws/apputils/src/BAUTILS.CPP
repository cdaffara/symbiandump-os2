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
// Started by DWW, November 1995
// BAFL utilities static class
//
//

#include "BaUtilsImp.h"
#include <e32hal.h>
#include <bautils.h>
#include <baflpan.h>
#include <baliba.h>
#include <hal.h>
#include <hal_data.h>
#include <utf.h>

/**
Mimimum length of a filename and mimimum length of a suffix.
Note these two values are tied together.
*/
const TInt KInvNameAndMinSuffixLength = 2;

#define ISDIGIT(c) (c-'0' >= 0 && c-'0' <= 9)

_LIT(KAllDrives, "YXWVUTSRQPONMLKJIHGFEDCBAZ");
const TInt KDriveAndPathLength = 3;

// screen calibration stuff
_LIT(KScreenCalibrationFolder,"\\System\\Data\\");
_LIT(KScreenCalibrationFileName, "Screen.DAT");
const TInt KScreenCalibrationPathLength = 23;	// folder + filename


// #define DO_PROFILING

#if defined(DO_PROFILING)
#pragma message ("------------ N.B. profiling of \"BaflUtils::NearestLanguageFile\" is enabled")
#include <e32svr.h>
#define FIRST_PROFILE_INDEX	50
#define PROFILE_INDEX_1		(FIRST_PROFILE_INDEX+0)
#define PROFILE_COUNT		1
#endif

// BaflUtils

class BaflDir : public CDir
	{
public:
	void RemoveSystem();
	TInt SortByTable(CBaflFileSortTable* aTable);
private:
	TInt MinEntrySize(const TEntry & aEntry);
	};


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

/**
This function gets the list of languages that are 'equivalent' to the
given language. We say language L1 is equivalent to language L2 if 
speakers of L2 can readily understand L1 without intentional study 
or extraordinary effort.

The equivalence relationship is defined in a static table. Please refer 
to the definition of 'KEquivalentLists' for details.
Each row in the table is formatted like this:
@code
L1, L2, L3, ..., Ln, ELangNone
@codeend
In which L2, ..., Ln are equivalents of L1, and ELangNone marks the end of an
entry. The list is ordered. Compared with L3, L2 is nearer to L1. When choosing
an equivalent of L1, L2 shall be preferred over L3, L3 shall be preferred 
over L4, and so on.  
L1 is always returned as the ‘nearest equivalent’ of L1 itself.

BaflUtils::NearestLanguageFileV2 searches language specific resource files 
according to the 'equivalent' relationship returned by this function.
 
@param aLang The language whose equivalents needs to be found out.
@param aEquivalents On return, this array contains the ordered list of 
       languages that are equivalent to the given language. If there is no 
       entry for the given language in the table, this array will contain 
       two elements on return: the first is the given language itself 
       and the 2nd one is ELangNone. For any language that has equivalents 
       defined, content of he corresponding entry is returned.     
       
@see BaflUtils::NearestLanguageFileV2
*/ 
EXPORT_C void 
BaflUtils::GetEquivalentLanguageList(TLanguage aLang, TLanguagePath& aEquivalents) 
	{
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

/**
NearestLanguageFileV2 is very similar to the existing 'NearestLanguageFile'
function. The only difference between NearestLanguageFile and
NearestLanguageFileV2 is the order in which language specific 
resource files are searched for. 
NearestLanguageFile searches language specific resource files in the 
order defined by the 'downgrade path' of the given language. Content of the 
downgrade path is dependent on the current active locale, and parts of 
it is runtime configurable.
NearestLanguageFileV2 searches for language specific resource files 
in the order defined by the 'language equivalence table', which is a  
static data table fixed at build time. There is one entry in the table for 
each language that has one or more equivalents.

@param aFs An active file server session.
@param aName Name of the language-neutral resource file name which consist of
an optional drive specification, followed by an optional path name,
followed by basename for filename, followed by a period and extension.
On return, in case of a match, this is replaced by the language-specific version
which consists of the last two characters of the extension plus any preceding
numeric characters being replaced by the language code. Remains unchanged when there's no match 
@param aLanguage On return, in case of a match, this is replaced by the corresponding language.
  In case of no match, it is set to ELangNone.  

@see TLanguage
@see BaflUtils::GetEquivalentLanguageList
 */
EXPORT_C void 
BaflUtils::NearestLanguageFileV2(const RFs& aFs,TFileName& aName, TLanguage& aLanguage)
	{
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

// TLibAssocBase

EXPORT_C TLibAssocBase::TLibAssocBase(const RLibrary& aLib,TAny* aPtr)
	: iLibrary(aLib),iPtr(aPtr)
/**
Constructs the object taking the specified DLL and a class instance.

@param aLib A reference to a DLL that has already been opened.
@param aPtr An untyped pointer to an object to be associated with the DLL.
            Typically, this object will have been created using
            the ordinal 1 function from that DLL. */	
	{}




EXPORT_C void TLibAssocBase::Set(const RLibrary& aLib,TAny* aPtr)
/**
Implements TLibAssoc::Set().

@param aLib   A reference to a DLL that has already been opened.
@param aClass A pointer to an object to be associated with the DLL.
              Typically, this object will have been created using
              the ordinal 1 function from that DLL.

@see TLibAssoc::Set */
	{
	__ASSERT_ALWAYS(iLibrary.Handle()==KNullHandle&&iPtr==NULL,Panic(EBafPanicLibAssocAlreadySet));
	iLibrary=aLib;
	iPtr=aPtr;
	}




EXPORT_C void TLibAssocBase::DoUnload(TAny* aThis)
/**
Calls Close() on the associated DLL.

@param aThis An untyped pointer to a TLibAssoc type.
*/
	{
	TLibAssocBase& l=*(TLibAssocBase*)aThis;
	l.iPtr=NULL;
	l.iLibrary.Close();
	}

//
// class BaflUtils
//
EXPORT_C void BaflUtils::CopyWithTruncation(TDes& aDest,const TDesC& aSrc,TChar aTruncationSymbol)
/** Copies a descriptor, abbreviating it to fit the destination descriptor.

If aSrc is less than the maximum length of aDest, then the string is simply 
copied to aDest.

If this is not so, then the left-most characters of aSrc are copied to aDest, 
up to aDest's maximum length-1. aDest's final character is set to be aTruncationSymbol.

@param aDest On return, the truncated string
@param aSrc The string to truncate
@param aTruncationSymbol The truncation character to add */
	{ // static
	TInt maxLength=aDest.MaxLength();
	if (aSrc.Length()<=maxLength)
		aDest.Copy(aSrc);
	else
		{
		aDest.Copy(aSrc.Left(maxLength-1));
		aDest.Append(aTruncationSymbol);
		}
	}

EXPORT_C TBool BaflUtils::FileExists(const RFs& aFileSession,const TDesC& aFileName)
/** Checks if the specified file exists.
	
@param aFs File server session
@param aFileName File to check
@return ETrue if the file exists, otherwise EFalse */
	{ // static
	TEntry entry;
	return(aFileSession.Entry(aFileName,entry)==KErrNone);
	}

EXPORT_C TBool BaflUtils::PathExists(RFs& aFs,const TDesC& aPath)
/** Tests whether a path exists.

The path should contain a drive letter and a directory, or EFalse is returned. 
EFalse is also returned if it contains a filename or filename extension.

If the path is badly formed, for instance if it contains illegal characters, 
or any directory name consists of a single or double dot, or any directory 
name includes wildcard characters, the function returns EFalse.

@param aFs A connected session with the file server.
@param aPath The path to test for. It should end in a backslash.
@return ETrue if the path exists, EFalse if not. EFalse is also returned if the 
specified path is badly formed. */
	{ // static
	TParse parse;
	TInt retcode;
	retcode = parse.Set(aPath, NULL, NULL);
	if (retcode != KErrNone)
		return EFalse;
	if ((! parse.DrivePresent()) || (parse.NameOrExtPresent()))
		return EFalse;
	if (parse.Path().Length() == 0)
		return EFalse;
	TFileName dirName = parse.DriveAndPath();
	if (dirName.Length() > KMaxFileName)
		return(EFalse);
    RDir dir;
    retcode = dir.Open(aFs,dirName,0);
	if (retcode == KErrNone)
		dir.Close();
	return (retcode == KErrNone);
	}

EXPORT_C void BaflUtils::EnsurePathExistsL(RFs& aFileSession,const TDesC& aFileName)
/** Makes one or more directories, if they do not already exist. 
	
Any valid path component in the specified path that does not already exist 
is created as a directory. If the specified path already exists, the function 
returns normally.

@param aFs File server session
@param aFileName Path to ensure exists
@see RFs::MkDirAll() */
	{ // static
	TInt error=aFileSession.MkDirAll(aFileName);
	if (error!=KErrAlreadyExists)
		User::LeaveIfError(error);
	}

EXPORT_C TPtrC BaflUtils::ExtractAppNameFromFullName(const TFullName &aName)
/** Gets the application name from a full thread name.

@param aName Thread name
@return Application name
@see RThread */
	{
	// static - return the app name (after first :: before next ::, if any) from a full thread name
	TChar delimiter=':';
	TInt start=aName.Locate(delimiter);
	if (start<0)
		start=0; // should never happen
	else if (aName.Length()>start+2)
		start+=2;
	TPtrC rest=aName.Mid(start);
	TInt end=rest.Locate(delimiter);
	return end<0 ? rest : rest.Left(end);
	}

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
@param			aFilename The filename to parse
				
@return			Count of the numeric digits at the end of the name passed, 
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
@param			aFilename The filename to parse
				
@return			Count of the numeric digits at the end of the suffix, 
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
	return BaflUtils::FileExists(iFs, aFileName);
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



/** Get the value of the system drive.

The system drive can be set to one of the built-in read/write drives. Which 
drive is used is hardware-dependent. On some hardware, there may not be a 
system drive. The system drive is used as the drive on which localisable files 
are searched for. This enables a phone to be localised dynamically, using 
files not in the ROM.

@param aDriveNumber On return, contains the drive number of the system drive.
@return KErrNone is always returned.

@deprecated This method has been replaced by (and now internally calls) 
RFs:GetSystemDrive, which always returns a valid drive number.

@see BaflUtils::NearestLanguageFile
@see RFs::GetSystemDrive
*/
EXPORT_C TInt BaflUtils::GetSystemDrive(TDriveNumber& aDriveNumber)
	{
	aDriveNumber = RFs::GetSystemDrive();
	return KErrNone;
	}


/** Set most appropriate extension language code for filename and set corresponding language.

Symbian uses numeric values to identify natural languages as specified by the TLanguage enumeration
defined in e32const.h. These values are used at the end of filename extensions to identify the
languages pertaining to files which have language specific variants such as resource files.
For instance filename.r01 and filename.r02 would be the English and French versions of the
resource file filename.rsc. Language codes can be between 2 to 5 digits in length.

Starting from Symbian OS v7.0 this function constructs and uses a language downgrade path which 
consists of up to sixteen TLanguage values the first of which is the ideal language followed by 
the language of the current locale. Up to the next three can be customised using 
TLocale::SetLanguageDowngrade(). The rest of the language downgrade path is based on a 
table of language near equivalence which is internal to Symbian.

This function searches the custom resource drive (if set, retrieved from HAL)
and then searches the optional drive specified in aName or 'Z:' if none is 
specified in aName. The custom resource drive is retrieved from the HAL 
attribute ECustomResourceDrive if set, if not set it will retrieve the legacy 
value set in the legacy HAL attribute ESystemDrive. No custom resource drive 
is searched if neither are set.  
Note - setting the custom resource drive will reduce the performance of this 
routine which will adversely affect device performance e.g. at boot up.
On NAND Flash based devices use of a composite Z: drive file system made up of multiple 
ROM images is the preferred mechanism for customising language resources on devices in 
Symbian OS 9.2 onwards, see Developer Library » Base Porting Guide » Porting: background 
information » NAND flash » NAND Flash image format. Thus use of the custom resource drive 
HAL attribute is effectively obsolete.
  
The last two characters of aName are removed along with any digits which appear before them.
Then language codes specified in the constructed language downgrade path are appended in turn to 
aName as a match is searched for in the file system. In case no match is found using the 
constructed language downgradepath then files in the specified directory are searched for a 
suitable extension with preference given to the one specified if present. In cases where a 
match takes place the aName and aLanguage arguments are updated otherwise aName is left 
unchanged and aLanguage is set to ELangNone.

Here are some examples of correct and incorrect function usage with different aName inputs, 
file system state and downgrade paths as follows: 

@code
Following files exist:
C:\\abc.rsc  - Language Neutral resource file.
C:\\abc.r01  - Resource file for the English language.
C:\\abc.r10  - Resource file for the American-English language.
C:\\abc.r160 - Resource file for the English as appropriate in Japan.

Constructed Downgrade Language Path cases:
- Case 1. (ELangAmerican -> ELangEnglish -> ELangNone).
- Case 2. (ELangEnglish_Japan -> ELangEnglish -> ELangNone).
- Case 3. Same as case 1, However "C:\\abc.r10" is deleted prior to the function call.
- Case 4. Same as case 1, However both "C:\\abc.r01" and "C:\\abc.r10" are deleted prior to the function call.
@endcode

@code
Input aName . . . . Output aName. . . aLanguage . . . . . Description
--------------------------------------------------------------------------------------------------------------------
"C:\\abc.rsc" . . . "C:\\abc.r10" . . ELangAmerican . . . Match on first language (Case 1)
"C:\\abc.r10" . . . "C:\\abc.r10" . . ELangAmerican . . . Match, However it's not the intended use of 
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . the function (Case 1)
"C:\\abc.r" . . . . "C:\\abc.r" . . . ELangNone . . . . . The no. of characters in the suffix is less than 
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . KInvNameAndMinSuffixLength(2)(Case 1)
"C:\\abc.". . . . . "C:\\abc.". . . . ELangNone . . . . . Invalid Suffix: The filename ends with a period(Case 1)
"C:\\abc.r123456" . "C:\\abc.r123456" ELangNone . . . . . Invalid Suffix: The no. of digits in the suffix is greater 
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . than KMaxSuffixLength(5) (Case 1)
"C:\\abc.10". . . . "C:\\abc.10 . . . ELangNone . . . . . Invalid Suffix: There's no proceeding alphabetical 
. . . . . . . . . . . . . . . . . . . . . . . . . . . . . characters in the suffix (Case 1)
"\\abc.rsc" . . . . "\\abc.rsc" . . . ELangNone . . . . . No drive so Z: search, no match (Case 1)
"C:\\abc.rsc" . . . "C:\\abc.r160". . ELangEnglish_Japan. Match for language file 3 digits long (Case 2)
"C:\\abc.rsc" . . . "C:\\abc.r01" . . ELangEnglish. . . . Match on second language (Case 3)
"C:\\abc.rsc" . . . "C:\\abc.rsc" . . ELangNone . . . . . No corresponding langauge file match found (Case 4)
---------------------------------------------------------------------------------------------------------------------
@endcode

@param aFs File server session.
@param aName Optional drive specification, followed by optional path name,
followed by basename for filename, followed by period and extension.
On return, in case of a match, this is replaced by the language-specific version
which consists of the last two characters of the extension plus any preceding
numeric characters being replaced by the language code. Remains unchanged when there's no match 
@param aLanguage On return, in case of a match, this is replaced by the corresponding language.
In case of no match it's set to ELangNone.
@see TLanguage
@see BaflUtils::GetDowngradePathL
@see TLocale::SetLanguageDowngrade
*/
EXPORT_C void BaflUtils::NearestLanguageFile(const RFs& aFs,TFileName& aName, TLanguage& aLanguage)
	{
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
	
/** Searches for the file with the correct language extension for the language 
of the current locale, or failing this, the best matching file.

@param aFs File server session.
@param aName File name as it would be without a language-specific extension. 
On return, this is changed to the language-specific version. If no such file 
is found, the name is unchanged.
@see BaflUtils::NearestLanguageFile(const RFs& aFs,TFileName& aName, TLanguage& aLanguage) */
EXPORT_C void BaflUtils::NearestLanguageFile(const RFs& aFs,TFileName& aName)
	{
	TLanguage language;
	
	NearestLanguageFile( aFs, aName, language);
	
	(void)language;
	}

/** Set the ideal language for the thread. 
This interface is intended for the use of UIKON only.

@param aLanguage Ideal language.
@return KErrNone, if successful; KErrNoMemory if there is not enough memory @see TLanguage
@see BaflUtils::NearestLanguageFile() 
@internalAll */
EXPORT_C TInt BaflUtils::SetIdealLanguage(TLanguage aLanguage)
	{
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
	
/** Get the ideal language of the thread. 
This interface is intended for the use of UIKON only.

@return Ideal language if set, ELangNone if not set
@see BaflUtils::NearestLanguageFile() 
@internalAll */
EXPORT_C TLanguage BaflUtils::IdealLanguage()
	{
	TLanguage* langPtr=(TLanguage*)Dll::Tls();
	
	if( langPtr==NULL)
		{
		return(ELangNone);
		}

	return(*langPtr);
	}
	
EXPORT_C void BaflUtils::ReleaseIdealLanguage()
/** Releases the ideal language store if it has been allocated. 
This interface is intended for the use of UIKON only.

@internalAll */
	{
	TLanguage* aLanguage=(TLanguage*)Dll::Tls();
	if( aLanguage==NULL)
		return;
	
	delete aLanguage;
	Dll::FreeTls();
	}

EXPORT_C TInt BaflUtils::IsFolder(const RFs& aFs, const TDesC& aFullName, TBool& aIsFolder)
/** Checks if the specified item is a folder.

@param aFs File server session
@param aFullName Name to check
@param aIsFolder ETrue if aFullName is a folder, otherwise EFalse
@return KErrNone if successful, otherwise another of the system-wide error 
codes */
	{
	TParsePtrC parse(aFullName);
	if ((parse.DriveAndPath().Length() == KDriveAndPathLength) && (aFullName.Length() == KDriveAndPathLength))	
		{
		aIsFolder = ETrue;
		return(KErrNone);
		}
	TEntry entry;
	TInt retcode = aFs.Entry(aFullName, entry);
	if (retcode == KErrNone)
		aIsFolder = ((entry.iAtt & KEntryAttDir)==KEntryAttDir);

	return(retcode);
	}


EXPORT_C TBool BaflUtils::FolderExists(RFs& aFs, const TDesC& aFolderName)
/** Tests whether a folder exists.

The folder is specified in a path. The path can omit the drive letter, in 
which case the drive letter is taken from the session path.

If the path is badly formed, for instance if it contains illegal characters, 
or any directory name consists of a single or double dot, or any directory 
name includes wildcard characters, the function returns EFalse.

If a filename is included in the path, it is ignored (the existence 
of the file will not be checked).  However if included, it must not
be badly formed - this will cause the function to return EFalse.  
If no filename is specified, the path should end in a backslash.

Examples of valid paths (returning ETrue):
C:\; \; C:\Readme.txt; C:\system\data\; \system\data\Anyfile.dat

Examples of invalid paths (returning EFalse):
C:\FolderDoesntExist\; ..\system\; C:\Wild*card\; C:\system\data\Bad>File.txt

@param aFs A connected session with the file server.
@param aFolderName A path specifying the folder to test for.
@return ETrue if the folder specified in aFolderName exists, EFalse if not. 
EFalse is also returned if the specified path is badly formed. */
	{	
	if (aFolderName.Length()==0)
		{return EFalse;}
	
	TParse parse;
		 	
 	TInt retcode = parse.SetNoWild(aFolderName, NULL, NULL);
 	
 	if (retcode != KErrNone)
 		{return EFalse;}
 			
 	if (parse.NameOrExtPresent())
 		if (!aFs.IsValidName(aFolderName))
 			{return EFalse;}
 	
 	TPtrC dirName = parse.DriveAndPath();
 	RDir dir;
    retcode = dir.Open(aFs,dirName,0);
 	if (retcode == KErrNone)
 		{dir.Close();}
 	return (retcode == KErrNone);
	}


EXPORT_C TFileName BaflUtils::FolderNameFromFullName(const TDesC& aFullName) 
/** Gets the folder name from a path.

A valid full name should have a drive associated with it
e.g ("a:\\" - "z:\\")("a:" - "z:")("c:\\system\data\file.txt")
Invalid entry will have no drive and cause a panic EBafPanicBadOpenArg
For example, if the path is "c:\documents\word\mydoc1", then "word" is returned.
							"c:"						then "c:" is returned
							"c:\\"						then "c:\" is returned
							"c:\\mydoc1.txt				then "c:\" is returned

@param aFullName A path.
@return The folder name. */
	{
	TParsePtrC parse(aFullName);
	
	__ASSERT_ALWAYS(parse.DrivePresent(),Panic(EBafPanicBadOpenArg));
	
	TFileName folderName = parse.Path();
	//If the path name has no associated path(e.g "c:") or path='\'(e.g "c:\\", "c:\\file.txt")
	//then the folder name is just equal to drivename
	
	TBuf<1> pathSeparator;
	pathSeparator.Append(KPathDelimiter);

	if (folderName.Length()==0 || folderName==pathSeparator)
		return (parse.DriveAndPath());
	//else just get the foldername
	TInt len = folderName.Length();
	TInt pos = --len;
	while (--pos)
		if (folderName.Mid(pos, 1)==pathSeparator)
			break;
	folderName.Delete(len, 1);
	folderName.Delete(0, pos+1);
	return(folderName);
	}


EXPORT_C TFileName BaflUtils::DriveAndPathFromFullName(const TDesC& aFullName) 
/** Gets the drive letter and path from a file name. 

This is in the form: drive-letter:\\path\\. The drive letter is folded using 
class TCharF. 

@param aFullName File name
@return The drive and path */
	{
	TParsePtrC parse(aFullName);
	return (parse.DriveAndPath());
	}


EXPORT_C TFileName BaflUtils::RootFolderPath(const TBuf<1> aDriveLetter)
/** Gets the root folder for the specified drive.

If aDriveLetter is an alphabet(lowercase or uppercase) then it will return
the TFileName which is simply the drive letter plus ":\"
If this is not the case, the function will panic with panic code EBafPanicBadOpenArg

@param aDriveLetter Drive letter
@return Root folder */
	{
	TChar driveLetter(aDriveLetter[0]);
	driveLetter.LowerCase();
	TInt aDriveNumber=driveLetter-TChar('a');
	__ASSERT_ALWAYS(aDriveNumber>= EDriveA && aDriveNumber <= EDriveZ,Panic(EBafPanicBadOpenArg));
	
	TFileName rootFolderPath = aDriveLetter;
	rootFolderPath.Append(_L(":\\"));
	return rootFolderPath;
	}


EXPORT_C void BaflUtils::AbbreviateFileName(const TFileName& aOriginalFileName, TDes& aAbbreviatedFileName)
/** Abbreviates a file name.

If aOriginalFileName is less than the maximum length of aAbbreviatedFileName, 
then the name is simply copied to aAbbreviatedFileName.

If this is not so, then the left-most characters of aOriginalFileName are 
copied to aAbbreviatedFileName, up to aAbbreviatedFileName's maximum length-1. 
aAbbreviatedFileName's first character is set to be an ellipsis.

@param aOriginalFileName Original file name
@param aAbbreviatedFileName On return, abbreviated file name */
	{
	TInt maxWidthInChars = aAbbreviatedFileName.MaxLength();
	if (aOriginalFileName.Length() <= maxWidthInChars)
		{
		aAbbreviatedFileName = aOriginalFileName;
		return;
		}
	TChar ellipsis(0x2026);
	--maxWidthInChars;  // since the ellipsis will be the first char in aAbbreviatedFileName
	aAbbreviatedFileName.Zero();
	aAbbreviatedFileName.Append(ellipsis);
	aAbbreviatedFileName.Append(aOriginalFileName.Mid((aOriginalFileName.Length() - 1) - maxWidthInChars + 1, maxWidthInChars));
	}


EXPORT_C TBool BaflUtils::UidTypeMatches(const TUidType& aFileUid, const TUidType& aMatchUid)
/** Tests whether two UID types match.

A match is made if each UID in aMatchUid is either identical to the corresponding 
one in aFileUid, or is KNullUid.

@param aFileUid The UID type to match
@param aMatchUid The UID type to match against
@return ETrue if the UIDs match, otherwise EFalse */
	{
	for (TInt i=0; i<KMaxCheckedUid; i++)
		{
		if (aMatchUid[i] == KNullUid)
			continue;
		if (aMatchUid[i] != aFileUid[i])
			return(EFalse);
		}
	return(ETrue);
	}


EXPORT_C TInt BaflUtils::Parse(const TDesC& aName)
/** Checks if a specified file name can be parsed.

@param aName Name to parse
@return KErrNone if successful, otherwise another of the system-wide error 
codes */
	{ // keeps a TParse on the stack for the minimum time possible
	TParse parse;
	return parse.Set(aName,NULL,NULL);
	}


EXPORT_C TInt BaflUtils::ValidateFolderNameTypedByUserL(const RFs& aFs, const TDesC& aFolderNameTypedByUser, const TDesC& aCurrentPath, TFileName& aNewFolderFullName)
/** Checks if a folder name (without drive or path) is valid and returns the full 
name of the folder.

@param aFs File server session
@param aFolderNameTypedByUser Folder name to check
@param aCurrentPath Path to which to add the folder
@param aNewFolderFullName aFolderNameTypedByUser appended to aCurrentPath
@return KErrNone if successful, otherwise another of the system-wide error 
codes */
	{
	if (aFolderNameTypedByUser.Length() == 0)
		return KErrArgument;	// R_EIK_TBUF_NO_FOLDERNAME_SPECIFIED;

	TParse* targetParse = new(ELeave) TParse;
	CleanupStack::PushL(targetParse);

	TInt retcode = targetParse->Set(aFolderNameTypedByUser, NULL, NULL);
	User::LeaveIfError(retcode);
	if (targetParse->DrivePresent() || targetParse->PathPresent())
		{
		 CleanupStack::PopAndDestroy();
		 return KErrBadName;	// R_EIK_TBUF_INVALID_FOLDER_NAME;
		}

	if (!(aFs.IsValidName(aFolderNameTypedByUser)))
		{
		 CleanupStack::PopAndDestroy();
		 return KErrBadName;	// R_EIK_TBUF_INVALID_FOLDER_NAME;
		}
		 

	if ((aCurrentPath.Length() + aFolderNameTypedByUser.Length() + 1) > KMaxFileName)
		{
		 CleanupStack::PopAndDestroy();
		 return KErrTooBig;		//R_EIK_TBUF_FOLDERNAME_TOO_LONG;
		}
		 

	//TFileName newFolderFullName = aCurrentPath;
	aNewFolderFullName = aCurrentPath;
	if ((aNewFolderFullName.Length() + aFolderNameTypedByUser.Length() + 1) <= KMaxFileName)
		{
		aNewFolderFullName.Append(aFolderNameTypedByUser);
		aNewFolderFullName.Append(KPathDelimiter);
		}
	else
		{
		CleanupStack::PopAndDestroy();
		return KErrOverflow;
		}

	retcode = targetParse->Set(aNewFolderFullName, NULL, NULL);
	if (retcode != KErrNone)
		{
		CleanupStack::PopAndDestroy();
		return KErrBadName;	// R_EIK_TBUF_INVALID_FOLDER_NAME;
		}
	
	CleanupStack::PopAndDestroy();

	return(KErrNone);
	}

void BaflUtils::DoCopyFileL(RFs& aFs, const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch)
	{
	CFileMan* fileMan=CFileMan::NewL(aFs);
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Copy(aSourceFullName,aTargetFullName,aSwitch));
	CleanupStack::PopAndDestroy(); // fileMan
	}


EXPORT_C TInt BaflUtils::CopyFile(RFs& aFs, const TDesC& aSourceFullName, const TDesC& aTargetFullName, TUint aSwitch)
/** Copies one or more files.

For more details, 
@see CFileMan::Copy()
@since     5.1
@param     aFs File server session
@param     aSourceFullName Path indicating the file(s) to be copied. Any path
components that are not specified here will be taken from the session path.
@param     aTargetFullName Path indicating the directory into which the file(s)
are to be copied
@param     aSwitch=CFileMan::EOverWrite Set this to zero for no overwriting and 
no recursion; CFileMan::EOverWrite to overwrite files with the same name; or 
CFileMan::ERecurse for recursion.
@return   KErrNone if successful, otherwise another of the system-wide error 
codes.*/
	{
	TRAPD(err,DoCopyFileL(aFs,aSourceFullName,aTargetFullName,aSwitch));
	return err;
	}

void BaflUtils::DoDeleteFileL(RFs& aFs, const TDesC& aSourceFullName, TUint aSwitch)
	{
	CFileMan* fileMan=CFileMan::NewL(aFs);
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Delete(aSourceFullName,aSwitch));
	CleanupStack::PopAndDestroy(); // fileMan
	}


EXPORT_C TInt BaflUtils::DeleteFile(RFs& aFs, const TDesC& aSourceFullName, TUint aSwitch)
/** Deletes one or more files.

For more details,
@see CFileMan::Delete().
@since 5.1
@param aFs File server session
@param aSourceFullName Path indicating the file(s) to be deleted. May either
be a full path, or relative to the session path. Use wildcards to specify 
more than one file.
@param aSwitch=0  Specify CFileMan::ERecurse for recursion,
zero for no recursion.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */	
    {
	TRAPD(err,DoDeleteFileL(aFs,aSourceFullName,aSwitch));
	return err;
	}

void BaflUtils::DoRenameFileL(RFs& aFs, const TDesC& aOldFullName, const TDesC& aNewFullName, TUint aSwitch)
	{
	CFileMan* fileMan=CFileMan::NewL(aFs);
	CleanupStack::PushL(fileMan);
	User::LeaveIfError(fileMan->Rename(aOldFullName,aNewFullName,aSwitch));
	CleanupStack::PopAndDestroy(); // fileMan
	}


EXPORT_C TInt BaflUtils::RenameFile(RFs& aFs, const TDesC& aOldFullName, const TDesC& aNewFullName, TUint aSwitch)
/**  Renames or moves one or more files or directories.

It can be used to move one or more files by specifying different
destination and source directories.
For more details, 
@see CFileMan::Rename().
@since 5.1
@param aFs File server session
@param aOldFullName Path specifying the file(s) to be renamed.
@param aNewFullName Path specifying the new name for the files and/or the
new directory. Any directories specified in this path that do not exist will 
be created.
@param aSwitch=CFileMan::EOverWrite  Specify zero for no overwriting, or
CFileMan::EOverWrite to overwrite files with the same name. This 
function cannot operate recursively.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */	
    {
	TRAPD(err,DoRenameFileL(aFs,aOldFullName,aNewFullName,aSwitch));
	return err;
	}


EXPORT_C TInt BaflUtils::CheckWhetherFullNameRefersToFolder(const TDesC& aFullName, TBool& aIsFolder)
/** Checks if a string is a valid folder name.

@param aFullName String to check
@param aIsFolder ETrue if aFullName is a valid folder name, otherwise EFalse
@return KErrNone if successful, otherwise another of the system-wide error 
codes (probably because aFullName cannot be parsed). */
	{
	aIsFolder = EFalse;
	TInt retcode = BaflUtils::Parse(aFullName);
	if (retcode != KErrNone)
		return(retcode);
	TParsePtrC parse(aFullName);
	if (! parse.NameOrExtPresent())
		aIsFolder = ETrue;
	return(KErrNone);
	}

EXPORT_C TInt BaflUtils::MostSignificantPartOfFullName(const TDesC& aFullName, TFileName& aMostSignificantPart)
/** Gets the folder name if the specified item is a valid folder name, otherwise 
gets the file name.

@param aFullName Item to parse
@param aMostSignificantPart Folder or file name
@return KErrNone if successful, otherwise another of the system-wide error 
codes */
	{
	TBool entryIsAFolder;
	TInt retcode = CheckWhetherFullNameRefersToFolder(aFullName, entryIsAFolder);
	if (retcode != KErrNone)
		return(retcode);
	if (entryIsAFolder)
		{
		aMostSignificantPart = FolderNameFromFullName(aFullName);
		return (KErrNone);
		}
	// assume aFullName refers to a file
	TParsePtrC parse(aFullName);
	aMostSignificantPart = parse.NameAndExt();
	return(KErrNone);
	}

EXPORT_C TInt BaflUtils::CheckFolder(RFs& aFs, const TDesC& aFolderName)
/** Checks that the specified folder can be opened.

@param aFs File server session
@param aFolderName Folder to check
@return KErrNone if successful, otherwise another of the system-wide error 
codes */
	{
    RDir dir;
    TInt retcode = dir.Open(aFs, aFolderName, 0);
	if (retcode == KErrNone)
		dir.Close();
	return (retcode);
	}

/**
Checks if the specified drive is read-only.
Checks that the KMediaAttWriteProtected and EMediaRom flags are both set.

@param aFs File server session
@param aFullName File name, including drive
@param aIsReadOnly On return, ETrue if the drive is read-only, otherwise EFalse
@return KErrNone if successful, otherwise another of the system-wide error codes
@see BaflUtils::DriveIsReadOnlyInternal
*/
EXPORT_C TInt BaflUtils::DiskIsReadOnly(RFs& aFs, const TDesC& aFullName, TBool& aIsReadOnly)
	{
	TInt retcode=BaflUtils::Parse(aFullName);
	if (retcode!=KErrNone)
		return retcode;
	TParsePtrC parse(aFullName);
	if (!parse.DrivePresent())
		return KErrBadName;
	TBuf<1> drive=parse.Drive().Left(1);
	TChar driveLetter=drive[0];
	TInt driveId=0;
	retcode=RFs::CharToDrive(driveLetter,driveId);
	if (retcode!=KErrNone)
		return retcode;
	TVolumeInfo volInfo;
	retcode=aFs.Volume(volInfo,driveId);
	if (retcode!=KErrNone)
		return retcode;
	aIsReadOnly=(volInfo.iDrive.iMediaAtt&KMediaAttWriteProtected || volInfo.iDrive.iType==EMediaRom);
	return KErrNone;
	}

/** 
Checks if the specified drive is read-only and is an internal drive i.e. non-removable.
Checks that the KMediaAttWriteProtected and KDriveAttInternal flags are both set.

@param aFs File server session
@param aFullName File name, including drive
@param aIsReadOnlyInternal On return, ETrue if the drive is read-only and internal, otherwise EFalse
@return KErrNone if successful, otherwise another of the system-wide errors codes 
*/
EXPORT_C TInt BaflUtils::DriveIsReadOnlyInternal(RFs& aFs, const TDesC& aFullName, TBool& aIsReadOnlyInternal)
	{
	TInt retcode=BaflUtils::Parse(aFullName);
	if (retcode!=KErrNone)
		return retcode;
	TParsePtrC parse(aFullName);
	if (!parse.DrivePresent())
		return KErrBadName;
	TBuf<1> drive=parse.Drive().Left(1);
	TChar driveLetter=drive[0];
	TInt driveId=0;
	retcode=RFs::CharToDrive(driveLetter,driveId);
	if (retcode!=KErrNone)
		return retcode;
	TVolumeInfo volInfo;
	retcode=aFs.Volume(volInfo,driveId);
	if (retcode!=KErrNone)
		return retcode;
	aIsReadOnlyInternal=((volInfo.iDrive.iMediaAtt&KMediaAttWriteProtected)&&(volInfo.iDrive.iDriveAtt&KDriveAttInternal));
	return KErrNone;
	}

EXPORT_C void BaflUtils::GetDiskListL(const RFs& aFs,CDesCArray& aArray)
/** Retrieves a list of all drives on the system.

The file server is interrogated for a list of the drive letters for all available 
drives. 

On emulator:
The removable media is represented by drive X: .

On hardware:
The removable media is represented by drives D: E: F: and G: .

@param aFs A connected session with the file server.
@param aArray On return, contains the drive letters that correspond to the 
available drives. The drive letters are uppercase and are in alphabetical 
order. */
	{ // static
	aArray.Reset();
	TDriveList driveList;
	User::LeaveIfError(aFs.DriveList(driveList));
	for (TInt ii=0;ii<KMaxDrives;ii++)
		{
		if (driveList[ii])
			{
			TChar drive;
			User::LeaveIfError(aFs.DriveToChar(ii,drive));
			drive.UpperCase();
			TBuf<1> buf;
			buf.Append(drive);
			aArray.AppendL(buf);
			}
		}
	}

EXPORT_C void BaflUtils::UpdateDiskListL(const RFs& aFs,CDesCArray& aArray,TBool aIncludeRom,TDriveNumber aDriveNumber)
/** Retrieves a list of all drives present on the system.

The file server is interrogated for a list of the drive letters for all available 
drives. The drive letter that corresponds to aDriveNumber is added to the 
list regardless of whether it is present, or is corrupt. Also, the C: drive 
is forced onto the list, even if corrupt or not present.

On emulator:
The removable media is represented by drive X: and is forced onto the list 
unless removed (F5,F4).

On hardware:
The removable media is represented by drives D: E: F: and G: and is forced 
onto the list regardless of whether it is present, or is corrupt.

@param aFs A connected session with the file server.
@param aArray On return, contains the drive letters that correspond to the 
available drives. The drive letters are uppercase and are in alphabetical 
order.
@param aIncludeRom Specify ETrue if the ROM drive should be included in the 
list, EFalse if not.
@param aDriveNumber The drive to force into the list, e.g. the drive in the 
default path. */
	{ // static
	aArray.Reset();
	TDriveList driveList;
	User::LeaveIfError(aFs.DriveList(driveList));
	for (TInt ii=0;ii<KMaxDrives;ii++)
		{
		if (driveList[ii] || ii==aDriveNumber)
			{
			TVolumeInfo vInfo;
			const TInt err=aFs.Volume(vInfo,TDriveUnit(ii));
			if (err==KErrNone || err==KErrCorrupt || ii==aDriveNumber || BaflUtils::IsFirstDriveForSocket(TDriveUnit(ii)))
				{
				if (ii==EDriveZ && vInfo.iDrive.iDriveAtt&KDriveAttRom && !aIncludeRom)
					continue;
				TChar drive;
				User::LeaveIfError(aFs.DriveToChar(ii,drive));
				drive.UpperCase();
				TBuf<1> buf;
				buf.Append(drive);
				aArray.AppendL(buf);
				}
			}
		}
	}

EXPORT_C TBool BaflUtils::IsFirstDriveForSocket(TDriveUnit aDriveUnit)
/** Tests whether the specified drive corresponds to the primary partition in a 
removable media slot.

The function assumes that the D: drive corresponds to the primary partition 
on socket 0, and that the E: drive corresponds to the primary partition on 
socket 1 (a socket is a slot for removable media). This mapping may not always 
be the case because it is set up in the variant layer of the Symbian OS.

This function assumes that the drive mappings are contiguous, starting 
from drive D: .

On emulator:
The removable media is represented by drive X: only.

@param aDriveUnit The drive to check.
@return ETrue if the drive is the primary partition in a removable media slot. 
ETrue is also returned if the drive is A, B or C:. EFalse is returned otherwise. */
	{ // static
	TDriveInfoV1Buf buf;
	UserHal::DriveInfo(buf);

#ifdef __EPOC32__
	return ((aDriveUnit-EDriveC)<=buf().iTotalSockets);
#else // emulator
    return (aDriveUnit==EDriveX || (aDriveUnit-EDriveC)<=buf().iTotalSockets);
#endif
	}

EXPORT_C void BaflUtils::RemoveSystemDirectory(CDir& aDir)
/** Removes "System" from a list of directory entries. 

@param aDir Array of directory entries. */
	{ // static
	STATIC_CAST(BaflDir&,aDir).RemoveSystem();
	}

EXPORT_C TInt BaflUtils::SortByTable(CDir& aDir,CBaflFileSortTable* aTable)
/** Sorts files by UID.

The caller supplies a table which specifies the order in which files are to 
be sorted. The files whose UID3 is the first UID in the table appear first. 
The files whose UID3 is the UID specified second appear next, and so on. Files 
whose UID3 is not specified in the table, and directories, appear at the end 
of the list, with directories preceding the files, and with files sorted in 
ascending order of UID3.

This function is used for customising how lists of application files are sorted.

@param aDir The array of files and directories to sort.
@param aTable A sort order table containing the UIDs to use in the sort.
@return KErrNone if successful, otherwise one of the standard error codes. */
	{ // static
	return STATIC_CAST(BaflDir&,aDir).SortByTable(aTable);
	}

EXPORT_C void BaflUtils::GetDowngradePathL(const RFs& aFs, const TLanguage aCurrentLanguage, RArray<TLanguage>& aLanguageArray)
/** Gets the full language downgrade path for a particular locale.

@param aFs A connected session with the file server.
@param aCurrentLanguage The language of the locale for which the language downgrade 
path is required. This language will always be returned as the first language 
in aLanguageArray. To get the downgrade path for the language of the current 
locale, specify the language returned by User::Language(). 
@param aLanguageArray On return, contains the language downgrade path.
@see BaflUtils::NearestLanguageFile() */
 	{
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

EXPORT_C void BaflUtils::PersistLocale()
/** Saves the locale settings in TLocale and the currency symbol to file. 
@deprecated 9.1
Persistence and initialisation of system locale data will be performed 
transparently by a separate executable (InilialiseLocale.exe) wich should 
be loaded as part of the system start procedure.
*/
	{
// Replaced by new repository based locale initialisation mechanism.
	}



EXPORT_C TInt BaflUtils::PersistHAL()
/** Saves the HAL settings to file. 
This will start a new executable and saves HAL attributes to a file, 
little delay because of the creation of new process
@return KErrNone if suceessful, otheriwse system wide error code.
*/
	{
	RProcess process;
	_LIT(KHALSettings, "HALSettings.exe");
	_LIT(KCommandLine, "PERSIST");
	TInt result = process.Create(KHALSettings, KCommandLine);
	if(result != KErrNone )
		return result;
	TRequestStatus status;
	process.Logon(status);
	if ( status != KRequestPending)
		{
		process.Kill(0);		// abort 
		}
	else
		{
		process.Resume();	// logon OK
		}
	User::WaitForRequest(status);
	
	// we can't use the 'exit reason' if the exe panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	result = process.ExitType() == EExitPanic ? KErrGeneral : status.Int();
	process.Close();
	return result;
	}

EXPORT_C void BaflUtils::PersistScreenCalibration(const TDigitizerCalibration& aScreenCalibration)
	{
	
	RFs	fs;
	TInt err = fs.Connect();
	if (err == KErrNone)
		{
		// Setting up drive to store Screen data
		TDriveUnit systemDrive(static_cast<TInt>(RFs::GetSystemDrive()));
		TBuf<KMaxDriveName+KScreenCalibrationPathLength>  ScreenFileNameWithDriveAndPath(systemDrive.Name());
		ScreenFileNameWithDriveAndPath.Append(KScreenCalibrationFolder);
		
		// Ensure directory \System\Data exists in target drive
		TRAP(err, EnsurePathExistsL(fs, ScreenFileNameWithDriveAndPath));
		if(err == KErrNone)
			{
			ScreenFileNameWithDriveAndPath.Append(KScreenCalibrationFileName);	
			
			RFile file;
			err = file.Replace(fs,ScreenFileNameWithDriveAndPath,EFileWrite|EFileShareExclusive);
			if (err == KErrNone)
				{
				// Write aScreenCalibration to file. 
				TPtrC8 calptr((const TUint8*)&aScreenCalibration, sizeof(TDigitizerCalibration));
				err = file.Write(calptr);
				}
			file.Close();
			}
		}
	fs.Close();
	}

EXPORT_C void BaflUtils::InitialiseScreenCalibration(RFs& aFs)
	{
	TFindFile ff(aFs);
	if (ff.FindByDir(KScreenCalibrationFileName, KScreenCalibrationFolder)==KErrNone)
		{
		RFile file; 
		if (file.Open(aFs,ff.File(),EFileRead) == KErrNone )
			{
			TDigitizerCalibration calibrationSetting;
			TPtr8 scrcal((TUint8*)&calibrationSetting, sizeof(TDigitizerCalibration));
			if( file.Read(scrcal, sizeof( TDigitizerCalibration )) == KErrNone )
				UserHal::SetXYInputCalibration(calibrationSetting);

			}
		file.Close();
		}
	}

EXPORT_C void BaflUtils::InitialiseHAL(RFs&)
/** Initialise the HAL settings from. 
@deprecated 9.1
This function is empty
*/
	{
	}

EXPORT_C void BaflUtils::InitialiseLocale(RFs& /* aFs */) 
	{
// Replaced by new repository based locale initialisation mechanism.
	}


//
// class CEikFileSortTable
// 

/**
*/	
EXPORT_C CBaflFileSortTable::CBaflFileSortTable()
	: CArrayFixFlat<TUid>(EArrayGranularity)
	{}

/**
 * Loads the CBaflFileSortTable using the UIDs read from the TResourceReader supplied in aReader. 
 * @param aReader TResourceReader from which UIDS are read.
 * @leave KErrNoMemory if there is insufficient memory available or one of the system wide error codes.
 */	
EXPORT_C void CBaflFileSortTable::ConstructFromResourceL(TResourceReader& aReader)
	{
	const TInt count=aReader.ReadInt16();
	for (TInt i=0;i<count;i++)
		AppendL(TUid::Uid(aReader.ReadInt32()));
	}

//
// class BaflDir
//

#define KSystemDirName _L("System") // Name for System directory


void BaflDir::RemoveSystem()
/*  Remove "System" if in list and it's a directory. */	
{
	TInt index;
	TEntry entry;
	entry.iName=KSystemDirName;
	TKeyArrayPak key(_FOFF(TEntry,iName),ECmpFolded);
	if (iArray->Find(entry,key,index)==KErrNone)
		{
		entry=(*iArray)[index];
		if (entry.IsDir())
			iArray->Delete(index);
		}
	};

/*
This function gets the element at position "aPos" of aEntries array and inserts 
it to poition "aNewPos". The element size is "aSize". After the operation the array size
grows by 1, the element at "aPos" position  moves one position forward.
This function must be called only from BaflDir::SortByTable() and the insert position
is always less or equal than the position of the element to be inserted.
*/
static void InsertL(CArrayPakFlat<TEntry>* aEntries, TInt aPos, TInt aNewPos, TInt aSize)
	{
	__ASSERT_DEBUG(aPos >= aNewPos, User::Invariant());
	//Expand the array adding one empty entry at "aNewPos" position.
	aEntries->ExpandL(aNewPos, aSize);
	//After successfull "ExpandL" call "aPos" must be incremented by 1.
	//Copy the entry from "aPos + 1" position to "aNewPos" position
	(*aEntries)[aNewPos] = (*aEntries)[++aPos];
	}

/**
Copied from f32file.inl (now moved to f32file_private.h)
Returns the minimum uncompressed size of the TEntry object, including the valid 
portion of the name string. The returned value is aligned to 4-byte boundary and
length includes private members.

@internalTechnology
@return minimum uncompressed size of TEntry object
*/

TInt BaflDir::MinEntrySize(const TEntry & aEntry)
	{
	return(sizeof(TUint)+sizeof(TInt)+sizeof(TTime)+sizeof(TInt)+sizeof(TUidType)+
         Align4(aEntry.iName.Size()) + 2*sizeof(TInt));
	}

TInt BaflDir::SortByTable(CBaflFileSortTable* aTable)
/**
Sort into order from given table.
Any file with uid[2] matching an entry in the table will be sorted relative to 
others in the table and before any files with no matching uid. 
For Example: Assume UID table is filled with below 2 UID's
table[0] =0x10003a64 and table[1] =0x10003a5c. Then file with UID[2]=0x10003a64 
will be sorted first in the list followed by file with UID[2]=0x10003a5c. Rest
files will be sorted in the ascending order of UID[2] with directories preceding
the files.

@param aTable A sort order table containing the UIDs to use in the sort. 
@return KErrNone if suceessful, otheriwse another system-wide error code.
*/
	{
	TInt r=this->Sort(EDirsFirst|ESortByUid);
	if(r != KErrNone)
		{
		return r;
		}
	const TInt tableCount=aTable->Count();
	const TInt count=iArray->Count();
	TInt sortedInsertionPoint = 0;
	for (TInt i=0;i<tableCount;i++)	
		{
		TUid	tableUid;
		// get each UID in the table
		tableUid = aTable->At(i);
		for (TInt j=sortedInsertionPoint;j<count;j++)
			{
			//parse files in the array list
			TEntry* pEntry=&(*iArray)[j];
			// check table UID for match with UID[2] of the file in the list
			// if found, move the file at the top in the list, followed by next matching UID
			if (tableUid == pEntry->iType[2])
				{
				TRAPD(insertErr, ::InsertL(iArray, j, sortedInsertionPoint++, MinEntrySize(*pEntry)));
				if(insertErr!=KErrNone)
					{
					return insertErr;
					}
				iArray->Delete(j+1);
				}
			}
		}
		iArray->Compress();
		return KErrNone;
	}
