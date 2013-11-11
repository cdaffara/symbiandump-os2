/**
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



/**
 @file
*/

#if (!defined __TE_CENTREP_BUR_STEP_BASE__)
#define __TE_CENTREP_BUR_STEP_BASE__
#include <test/testexecutestepbase.h>

#include <centralrepository.h>

_LIT(KIniNumOfSettings, "NumSettings");
_LIT(KIniExpectedGetResult, "ExpectedGetResult");
_LIT(KIniExpectedGetValue, "ExpectedGetValue");

const TUid KUidBURTestRepository = {0xBACBACBA};
const TUid KUidBURTestDefaultRepository = {0xBAC2BAC2};
const TUid KUidBURDefTestRepository = {0xBAEBAEBA};

const TInt KNumBackupSettings = 18;
const TInt KNumDefaultBackupSettings = 3;
const TInt KNumNonBackupSettings = 16;
const TInt KNumDefBackupSettings = 11;

// Original Key with no meta
const TUint32 KOrigNoMetaInt = 0x01;
const TInt KOrigNoMetaIntValue = 1;

// Original Key with metadata 0x00000000
const TUint32 KOrigZeroMetaInt = 0x02;
const TInt KOrigZeroMetaIntValue = 2;

// Original Key with metadata 0x01000000
const TUint32 KOrigBackupMetaInt = 0x03;
const TInt KOrigBackupMetaIntValue = 3;


// Keys used to test behaviour of deleted settings

// Original Key with metadata 0x00000000 (no backup)
const TUint32 KNoBackupDeleteTestInt = 0x04;
const TInt KNoBackupDeleteTestIntValue = 4;
const TInt KNoBackupDeleteTestIntModifiedValue = 99;

// Original Key with metadata 0x01000000 (backup)
const TUint32 KBackupDeleteTestInt = 0x05;
const TInt KBackupDeleteTestIntValue = 5;
const TInt KBackupDeleteTestIntModifiedValue = 66;

// Keys to be backed up

const TUint32 KNewBackupInt1 = 0x100;
const TInt KIntBackupValue1 = 1234;
const TUint32 KNewBackupInt2 = 0x101;
const TInt KIntBackupValue2 = 4321;
const TUint32 KNewBackupInt3 = 0x102;
const TInt KIntBackupValue3= 9999;
const TUint32 KNewBackupInt4 = 0x103;
const TInt KIntBackupValue4 = 8621;

const TUint32 KNewBackupReal1 = 0x110;
const TReal KRealBackupValue1 = 1234.5;
const TUint32 KNewBackupReal2 = 0x111;
const TReal KRealBackupValue2 = 5432.1;
const TUint32 KNewBackupReal3 = 0x112;
const TReal KRealBackupValue3 = 9999.0;
const TUint32 KNewBackupReal4 = 0x113;
const TReal KRealBackupValue4 = 7864.23;

const TUint32 KNewBackupString81 = 0x120;
_LIT8(KString8BackupValue1, "ABCDEF");
const TUint32 KNewBackupString82 = 0x121;
_LIT8(KString8BackupValue2, "abcdef");
const TUint32 KNewBackupString83 = 0x122;
_LIT8(KString8BackupValue3, "vwxyz");
const TUint32 KNewBackupString84 = 0x123;
_LIT8(KString8BackupValue4, "A1B2C3D4");

const TUint32 KNewBackupString161 = 0x130;
_LIT(KString16BackupValue1, "ghijklmn");
const TUint32 KNewBackupString162 = 0x131;
_LIT(KString16BackupValue2, "GHIJKLMN");
const TUint32 KNewBackupString163 = 0x132;
_LIT(KString16BackupValue3, "G1H2I3J4");
const TUint32 KNewBackupString164 = 0x133;
_LIT(KString16BackupValue4, "STUVWXYZ");

const TUint32 KDefaultBackupInt1 = 0x2;
const TInt KIntDefaultValue1 = 4;
const TUint32 KDefaultBackupString1 = 0x3;
_LIT(KString16DefaultValue1, "");
const TUint32 KModifyBackupString1 = 0x5;
const TUint32 KModifyBackupInt1 = 0x6;

// Keys not to be backed up

const TUint32 KNewInt1 = 0x201;
const TInt KIntValue1 = 567;
const TUint32 KNewInt2 = 0x202;
const TInt KIntValue2 = 765;
const TUint32 KNewInt3 = 0x203;
const TInt KIntValue3= 555;
const TUint32 KNewInt4 = 0x204;
const TInt KIntValue4 = 777;

const TUint32 KNewReal1 = 0x210;
const TReal KRealValue1 = 567.8;
const TUint32 KNewReal2 = 0x211;
const TReal KRealValue2 = 765.4;
const TUint32 KNewReal3 = 0x212;
const TReal KRealValue3 = 777.0;
const TUint32 KNewReal4 = 0x213;
const TReal KRealValue4 = 444.23;

const TUint32 KNewString81 = 0x220;
_LIT8(KString8Value1, "AAAAA");
const TUint32 KNewString82 = 0x221;
_LIT8(KString8Value2, "BBBBB");
const TUint32 KNewString83 = 0x222;
_LIT8(KString8Value3, "CCCCC");
const TUint32 KNewString84 = 0x223;
_LIT8(KString8Value4, "DDD11");

const TUint32 KNewString161 = 0x230;
_LIT(KString16Value1, "eeeeeee");
const TUint32 KNewString162 = 0x231;
_LIT(KString16Value2, "FFFFFFF");
const TUint32 KNewString163 = 0x232;
_LIT(KString16Value3, "GgGgGgG");
const TUint32 KNewString164 = 0x233;
_LIT(KString16Value4, "hhhhHHH");

/////////////////////////////////////////

_LIT(KPersistsFiles, 				"c:\\private\\10202BE9\\persists\\*.txt");


/////////////////////////////////////////

class CTe_centrep_BURSuiteStepBase : public CTestStep
	{
public:
	CTe_centrep_BURSuiteStepBase(){}
	};

#endif
