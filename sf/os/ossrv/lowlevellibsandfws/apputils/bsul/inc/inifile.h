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
// Ini file parser header file
// 
//

/**
 @file
 @internalAll
*/

#ifndef __INIFILE_H__
#define __INIFILE_H__

#include <e32std.h>
#include <e32base.h>

//Forward declaration
class RFs;

namespace BSUL
	{

class CIniDocument8;
class CIniDocument16;

class CIniSecIter8Impl;
/** 
Section iterator for 8 bit ini file. This class provides an iterator
to navigate through the settings within a section inside an 8 bit ini file.
*/
NONSHARABLE_CLASS(CIniSecIter8): public CBase
{
public:
	IMPORT_C static CIniSecIter8* NewL(const TDesC8& aSectionName,const CIniDocument8* aIniDocument);
	IMPORT_C ~CIniSecIter8();
		
	IMPORT_C TBool Next(TPtrC8& aKey,TPtrC8& aValue);
	IMPORT_C TBool End();
	IMPORT_C void Reset();
private:
	CIniSecIter8();
	CIniSecIter8Impl* iImpl;
};

class CIniDocument8Impl;
/**
Dom parser for 8 bit ini file. This class provides the functionality to read
and write settings of an 8 bit ini file.
*/
NONSHARABLE_CLASS(CIniDocument8): public CBase
{
public:
	IMPORT_C static CIniDocument8* NewL(RFs& aFs,const TDesC& aFileName);
	IMPORT_C ~CIniDocument8();
	IMPORT_C TInt Externalise(const TDesC& aFileName);

	//read api
	IMPORT_C TInt GetSectionList(RArray<TPtrC8>& aSectionList) const;
	IMPORT_C TInt GetKeyValue(const TDesC8& aSectionName,const TDesC8& aKey,TPtrC8& aValue) const;

	//write api
	IMPORT_C TInt AddSection(const TDesC8& aSectionName);
	IMPORT_C TInt RemoveSection(const TDesC8& aSectionName);
	IMPORT_C TInt SetKey(const TDesC8& aSectionName,const TDesC8& aKey,const TDesC8& aValue);
	IMPORT_C TInt RemoveKey(const TDesC8& aSectionName,const TDesC8& aKey);
	IMPORT_C TBool CompareDocs(CIniDocument8& aDoc);
private:
	friend class CIniSecIter8Impl;
	CIniDocument8();
	CIniDocument8Impl* iImpl;
};

class CIniSecIter16Impl;
/** 
Section iterator for 16 bit ini file. This class provides an iterator
to navigate through the settings within a section inside an 16 bit ini file.
*/
NONSHARABLE_CLASS(CIniSecIter16): public CBase
{
public:
	IMPORT_C static CIniSecIter16* NewL(const TDesC16& aSectionName,const CIniDocument16* aIniDocument);
	IMPORT_C ~CIniSecIter16();
		
	IMPORT_C TBool Next(TPtrC16& aKey,TPtrC16& aValue);
	IMPORT_C TBool End();
	IMPORT_C void Reset();
private:
	CIniSecIter16();
	CIniSecIter16Impl* iImpl;
};

class CIniDocument16Impl;
/**
Dom parser for 16 bit ini file. This class provides the functionality to read
and write settings of an 16 bit ini file.
*/
NONSHARABLE_CLASS(CIniDocument16): public CBase
{
public:
	IMPORT_C static CIniDocument16* NewL(RFs& aFs,const TDesC& aFileName);
	IMPORT_C ~CIniDocument16();
	IMPORT_C TInt Externalise(const TDesC& aFileName);

	//read api
	IMPORT_C TInt GetSectionList(RArray<TPtrC16>& aSectionList) const;
	IMPORT_C TInt GetKeyValue(const TDesC16& aSectionName,const TDesC16& aKey,TPtrC16& aValue) const;

	//write api
	IMPORT_C TInt AddSection(const TDesC16& aSectionName);
	IMPORT_C TInt RemoveSection(const TDesC16& aSectionName);
	IMPORT_C TInt SetKey(const TDesC16& aSectionName,const TDesC16& aKey,const TDesC16& aValue);
	IMPORT_C TInt RemoveKey(const TDesC16& aSectionName,const TDesC16& aKey);
	IMPORT_C TBool CompareDocs(CIniDocument16& aDoc);
private:
	friend class CIniSecIter16Impl;
	CIniDocument16();
	CIniDocument16Impl* iImpl;
};

class CIniFile8Impl;
/**
A light weight parser for reading setting values from an 8 bit ini file. Note that this does not 
guarantee checking that the ini file is wellformed. See CIniFile16 for a class that handles both 
8 and 16 bit files.  
*/
NONSHARABLE_CLASS(CIniFile8): public CBase
{
public:
	IMPORT_C static CIniFile8* NewL(RFs& aFs,const TDesC& aFileName);
	IMPORT_C ~CIniFile8();
	IMPORT_C TInt FindVar(const TDesC8& aSectionName,const TDesC8& aKeyName,TPtrC8& aValue)	const;
private:
	CIniFile8();
	CIniFile8Impl* iImpl;
};

class CIniFile16Impl;
/**
A light weight parser for reading setting values from a 16 or 8 bit ini file. The aConvert8To16
parameter of NewL controls whether or not 8 bit files are accepted. Note that this does
not guarantee checking that the ini file is wellformed.
*/
NONSHARABLE_CLASS(CIniFile16): public CBase
{
public:
	IMPORT_C static CIniFile16* NewL(RFs& aFs,const TDesC& aFileName);
	IMPORT_C static CIniFile16* NewL(RFs& aFs,const TDesC& aFileName,TBool aConvert8To16);
	IMPORT_C ~CIniFile16();
	IMPORT_C TInt FindVar(const TDesC16& aSectionName,const TDesC16& aKeyName,TPtrC16& aValue) const;
private:
	CIniFile16();
	CIniFile16Impl* iImpl;
};

}//namespace BSUL

#endif
