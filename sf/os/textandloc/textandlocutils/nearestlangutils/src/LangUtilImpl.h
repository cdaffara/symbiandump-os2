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

#ifndef __LANGUTILIMPL_H__
#define __LANGUTILIMPL_H__


#include <f32file.h>
#include <e32std.h>

/**
Specifies the maximum length of the numerical part of the suffix.

If this is changed the documentation of the following functions 
must also be updated:

	LangUtil::NearestLanguageFile()
	TNearestLanguageFileFinder::CountDigitsFromEndInValidSuffix()
	TNearestLanguageFileFinder::SetFileName()
*/
const TInt KMaxSuffixLength = 5;

class RDirectoryScanner
/**
@internalAll
*/
    {
public:
    virtual TInt Open(RFs& aFs, const TDesC& aMatchPattern) = 0;
    virtual TInt Next(TEntry& aOut) = 0;
    virtual void Close() = 0;
    virtual ~RDirectoryScanner() {}
    };

NONSHARABLE_CLASS(RRealDirectoryScanner) : public RDirectoryScanner
/**
@internalAll
*/
    {
public:
    virtual TInt Open(RFs& aFs, const TDesC& aMatchPattern);
    virtual TInt Next(TEntry& aOut);
    virtual void Close();
private:
    RDir iDir;
    };


/**
 * Add a language to the end of the language path, unless it is already
 * present. On entry, the language path must have an ELangNone entry at its
 * end. This will be true on exit also.
 * @internalAll
 */
void AddLanguage(TLanguagePath& aPath, TLanguage aNewLanguage);

/**
 * Create a language path from the current language, ideal language and locale. 
 * The path may have up to eight entries in it.
 * @internalAll
 */

void MakeLanguageDowngradePath(TLanguagePath& aPath,
	TLanguage aCurrent, TLanguage aIdeal, const TLocale& aLocale);


/**
 * This class contains all the functions for working out the nearest language
 * file. It can be derived from for test code purposes.
 * @internalAll
 */
NONSHARABLE_CLASS(TNearestLanguageFileFinder)
	{
public:
	TNearestLanguageFileFinder(const RFs& aFs);
	TBool SetFileName(TFileName& aFileName);
	TLanguage Language();
	// put back the original suffix and drive letter
	void RepairFileName();
	// add the preset custom resource drive, if any, to iDrives.
	TInt AddCustomResourceDrive();
	// add all remaining drives to iDrives. iDrives must not have more than one
	// drive in it on entry.
	void AddAllDrives();
	// Tries to append the language code to iFileName. This is 00..09 or just the number.
	// If there was not enough space, EFalse is returned.
	TBool AppendLanguageCode(TLanguage aLanguage);
	// Search the drives in iDrives for the file named iFileName.
	// iFileName must have a drive specifier, which will be overwritten.
	TBool FindDrive();
	// Search for files across all drives in all languages in the path plus the
	// language-neutral file.
	// On entry, iFileName should be the original name with a drive specifier
	// minus the suffix. On return, iFileName will be untouched if EFalse is
	// returned, but contain the result if ETrue is returned.
	TBool FindLanguageAndDrive();
	// Test whether the filename passed in matches the stem given + numbers
	// added to the end. Returns the number if it does, or KErrNotFound if not.
	// aFileName must not end in a digit.
	static TInt LanguageNumberFromFile(const TDesC& aFileName, const TDesC& aStem);
	// Find lowest numbered file that matches iFileName, which must be without
	// its suffix.
	TInt FindFirstLanguageFile(RFs&);
	// Try each drive for any language files that match iFileName.
	// iFileName must have a directory specifier and be without its suffix.
	// returns KErrNotFound, KErrNone or error code.
	TInt FindFirstLanguageFileAndDrive();

	virtual TInt GetCustomResourceDriveNumber() const;
	
	virtual TInt FileExists(const TDesC& aFileName) const;
	// return our member that is our directory scanning class
	virtual RDirectoryScanner& DirectoryScanner();
	virtual ~TNearestLanguageFileFinder() {}
	
private:
	TInt CountDigitsFromEnd(const TDesC& aFilename);
	TInt CountDigitsFromEndInSuffix (const TDesC& aFilename);

public:
	const RFs& iFs;
	TFileName* iFileName;
	TLanguage  iLanguage;
	TBuf<26> iDrives;
	TLanguagePath iPath;
	TBuf<KMaxSuffixLength> iSuffix;
	TInt iInitialDriveLetter;
	// length minus the removed suffix
	TInt iOriginalBaseLength;
	// length plus added drive letter minus removed suffix
	TInt iBaseLength;
	RRealDirectoryScanner iDirScanner;
	};

#endif // __LANGUTILIMPL_H__

