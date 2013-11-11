/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/



#if !defined(__INIPARSER_H__)
#define __INIPARSER_H__

#include <e32base.h>
#include <f32file.h>

_LIT(KUseSystemDrive, "$:");

class CIniData : public CBase 
/**
 *
 * @publishedPartner
 * @test
 *
 * Defines the interface to acess to ini data file
 *
 * The basic functions, FindVar(), SetValue(), AddValue() and WriteToFileL()
 * Compulsory to call WriteToFileL() after calling any SetValue() or AddValue()
 */
	{
public:
	// Constructor, pass in name of ini file to open
	// Default search path is 'c:\system\data' on target filesystem
	// ie. 'NewL(_L("c:\\system\\data\\ttools.ini"))' is equivalent
	// to 'NewL(_L("ttools.ini"))'
	IMPORT_C static CIniData* NewL(const TDesC& aName);
	IMPORT_C virtual ~CIniData();

	IMPORT_C TBool FindVar(const TDesC &aKeyName,	// Key to look for
						TPtrC &aResult);			// Buffer to store value

	IMPORT_C TBool FindVar(const TDesC &aKeyName, 	// Key to look for
						TInt &aResult);				// Int ref to store result

	IMPORT_C TBool FindVar(const TDesC &aSection,	// Section to look under
						const TDesC &aKeyName,		// Key to look for
						TPtrC &aResult);			// Buffer to store result

	IMPORT_C TBool FindVar(const TDesC &aSection,	// Section to look under
						const TDesC &aKeyName,		// Key to look for
						TInt &aResult);				// Int ref to store result

	IMPORT_C TInt SetValue(const TDesC& aKeyName,// Key to look for
						const TDesC& aValue);  // aValue being modified
	IMPORT_C TInt SetValue(const TDesC& aSection, // Section to look under
						const TDesC& aKeyName,  // Key to look for
						const TDesC& aValue);  // aValue being modified
	IMPORT_C TInt AddValue(const TDesC& aSection,  // Section to look under
						const TDesC& aKeyName,   // Key to look for
						const TDesC& aValue);   // aValue being modified
	IMPORT_C TInt AddValue(const TDesC& aKeyName,   // Key to look for
						const TDesC& aValue);  // aValue being modified
	IMPORT_C void WriteToFileL();
	// Overloaded NewL() added to recieve optional system drive letter from the user
	// And expand ${SYSDRIVE} variable. Else, the variable expands to RFs::GetSystemDrive()
	IMPORT_C static CIniData* NewL(const TDesC& aName, const TDesC& aSysDrive);
protected:
	IMPORT_C CIniData();
	IMPORT_C CIniData(const TDesC& aSysDrive);
	IMPORT_C void ConstructL(const TDesC& aName);
public:
	IMPORT_C TBool FindVar(const TDesC &aSection,	// Section to look under
						const TDesC &aKeyName,		// Key to look for
						TInt64 &aResult);			// Int64 ref to store result

private:
	void UpdateVariablesL();
	TBool FindVar(const TDesC &aSectName,const TDesC &aKeyName,TPtrC &aResult, TPtr& aIniDataPtr);
private:
	HBufC* iName;
	HBufC* iToken;
	HBufC* iIniData;
	TPtr iPtr;
	TPtr iPtrExpandedVars;
	TDriveName iDefaultSysDrive;
	TDriveName iSysDrive;
	};

#endif
