// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TEST__FILE_URI_SERVER_H__
#define __TEST__FILE_URI_SERVER_H__

// Epoc Include
// For accessing TEF classes 
#include <testexecuteserverbase.h>

/*@{*/
// Literal constants for INI file field-names
_LIT(KIniFileName, 			"filename"			);
_LIT(KIniFileUri, 			"fileuri"			);
_LIT(KIniExpectedFileName, 	"expectedfilename"	);
_LIT(KIniExpectedFileUri, 	"expectedfileuri"	);
_LIT(KIniFileType, 			"filetype"			);
_LIT(KIniCharacterSet, 		"characterset"		);
_LIT(KIniDrive, 			"drive"				);
/*@}*/

/*@{*/
// Literal constants used by some test steps
_LIT(KFileTypePrivate, 		"private"			);
_LIT(KFileTypePublic, 		"public"			);
_LIT(KExtMedia, 			"ext-media"			);
_LIT(KDrivePlaceHolder, 	"<drive>"			);
_LIT(KDriveSeparator, 		":"					);
_LIT(KBackSlash, 			"\\"				);
/*@}*/

// constants used
const TInt KCharSet8		= 8 ;
const TInt KCharSet16		= 16;
const TInt KLetterA 		= 97; 	// ASCII of 'a'

/**
The test server. 
@internalTechnology
*/
class CTestFileUriServer : public CTestServer
	{
public:
	// Construction
	static	CTestFileUriServer* 	NewL();
	
	// Base class pure virtual
	virtual CTestStep* 				CreateTestStep(const TDesC& aStepName);
	
	// Common static functions used by the test steps
	static	void 					GetDriveNumber(const TDesC& aDrive, TDriveNumber& aDriveNum);
	static 	TInt 					IsRemovableDrive(const TDriveNumber& aDrive, TBool& aResult);
	static	HBufC16*				CheckAndFillDriveNameL(const TPtrC& aFileUri, const TPtrC& aDrive);
	static	TInt 					FirstRemovableDriveWithSameFileName(const TDesC& aFileName, TBuf<1>& aCorrectDrive);
	
	static	TInt 					CreateFullyQualifiedName(const TPtrC& aRelativeName, const TPtrC& aDrive, TFileName& aFullyQualifiedName);
	
	private:
	// Private function
	static 	TInt 					PopulateRemovableDrivesBuf(const RFs& aFs);
	
	private:
	// To idebtify the removable drive letters
	static TBuf<KMaxDrives> 		iRemovableDrives;
	};

#endif		// __TEST__FILE_URI_SERVER_H__

