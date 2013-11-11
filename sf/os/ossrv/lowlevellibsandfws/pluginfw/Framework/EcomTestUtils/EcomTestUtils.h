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
// Helper class for ECom test code. Allows various file manipulation operations
// that require higher Capabilities than shoud be given to test harnesses.
// 
//


#ifndef __ECOMTESTUTILS_H__
#define __ECOMTESTUTILS_H__

#include <e32test.h>

// System includes
#include <f32file.h>

// Wait or sleep macros for current thread
#define WAIT_FOR1s User::After(1000000)
#define WAIT_FOR3s User::After(3000000)
#define WAIT_FOR5s User::After(5000000)

// Common ECOM Server strings
_LIT(KEComZDriveDatFile, 		"C:\\private\\10009D8F\\ECom.ROM.Z.dat");
_LIT(KEComCDriveDatFile, 		"C:\\private\\10009D8F\\ECom.dat");
_LIT(KEComEDriveDatFile, 		"E:\\private\\10009D8F\\ECom.dat");


_LIT(KEComServerProcessName,    "ecomserver");

/**
This enum is used to represent the rom build option i.e. NAND or default rom build
*/
enum TRomBuildOption
{
	EDefaultRomBuild,
	ENandRomBuild
};

/**
Enum type to represent possible configurations of the hardware running the tests
*/
enum THardwareConfiguration
{
	EPlatformWINSCW,
	EPlatformH2RAM,
	EPlatformH2NAND,
	EPlatformH2NANDDP,
	EPlatformH4RAM,
	EPlatformH4MMC,//New hardware configuration for WDP enabled ROM on H4
	EPlatformH4NAND,
	EPlatformH4NANDDP,
	EPlatformH6RAM,
	EPlatformH6NAND,
	EPlatformH6NANDDP,
	EPlatformUnknown,
};

class EComTestUtils
	{
public:
	IMPORT_C static void FileManCopyFileL(const TDesC& anOld,const TDesC& aNew);
	IMPORT_C static void FileManDeleteFileL(const TDesC& aFile);
	IMPORT_C static void MakeFileWriteableL(const TDesC& aFile);
	IMPORT_C static void MakeFileReadOnlyL(const TDesC& aFile);
	IMPORT_C static void RfsDeleteFileL(const TDesC& aFile);
	IMPORT_C static void RfsReplaceFileL(const TDesC& anOld,const TDesC& aNew);
	IMPORT_C static void KillProcessL(const TDesC& aProcessName);
	IMPORT_C static void FileManRenameL(const TDesC& anOld,const TDesC& aNew);
	IMPORT_C static void FileManDeleteDirL(const TDesC& aPath);
	IMPORT_C static void SwitchToLanguageL(TLanguage aLang);
	IMPORT_C static void RLoaderDeleteFileL(const TDesC& aFile);
	IMPORT_C static TRomBuildOption RomBuildType(const RFs& aRfs);
	IMPORT_C static THardwareConfiguration GetHardwareConfiguration();
	};

inline void KillEComServerL(RTest &aRTest)
	{
	aRTest.Printf(_L("Killing ECOM Server, waiting 1s...\n"));
	TRAPD(err, EComTestUtils::KillProcessL(KEComServerProcessName));
	if ((err != KErrNone) && (err != KErrNotFound))
		User::Leave(err);
	// Precaution delay, give the system a chance to settle afterwards
	WAIT_FOR1s;	
	}
#define ECOMTU_KILL_SERVER_L(rt) KillEComServerL((rt))


inline void Validate(RTest& aRTest, TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected integer/error: %d, got: %d\r\n"), aExpected, aValue);
		aRTest(EFalse, aLine);
		}
	}

#define TESTC(aRTest, aValue, aExpected) ::Validate(aRTest, aValue, aExpected, __LINE__)


#endif

