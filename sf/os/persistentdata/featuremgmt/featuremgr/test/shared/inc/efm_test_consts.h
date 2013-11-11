// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Constants definitions for Feature Manager test code
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/
#ifndef __EFMTESTCONSTS__
#define __EFMTESTCONSTS__

_LIT( KTargetFeatureFile, "C:\\private\\102836E5\\features.dat");
_LIT( KFeatureFileDefault, "Z:\\EFM\\data\\features_invalid_header.dat");
_LIT( KFeatureFileInvalidHeader, "Z:\\EFM\\data\\features_invalid_header.dat");
_LIT( KFeatureFileRunTime, "C:\\private\\102836E5\\runtime\\features.dat");
_LIT( KPluginSourcePath, "Z:\\EFM\\plugins\\reconciliation.rsc");
_LIT( KPluginDestinationPath, "c:\\resource\\Plugins\\reconciliation.rsc");

//use UIDs from non-allocated range (0x00000000 -  1000484F)
const TUid KDefaultSupportedUid = { 0x01000001 };
const TUid KDefaultUnsupportedUid = { 0x01000002 };
const TUid KModifiableUid = { 0x01000003 };
const TUid KUnmodifiableUid = { 0x01000004 };
const TUid KUnknownUid = { 0x01000005 };
const TUid KUninitializedUid = { 0x01000006 };
const TUid KBlacklistedUid = { 0x01000007 };
const TUid KNotPersistedUid = { 0x01000008 };
const TUid KSupportedUpgradeableUid = { 0x01000101 };
const TUid KSupportedUpgradeableBlacklistedUid = { 0x01000102 };
const TUid KSupportedUpgradeableModifiableUid = { 0x01000103 };
const TUid KSupportedUpgradeableModifiableUninitialisedUid = { 0x01000104 };
const TUid KSupportedUpgradeableModifiablePersistedUid = { 0x01000105 } ;
const TUid KModifiablePersistedUid = { 0x01000106 } ;
const TUid KModifiableUninitialisedPersistedUid = { 0x01000107 } ;
const TUid KPersistedUid = { 0x01000108 };
const TUid KNonmodifiableUninitialisedUid = { 0x01000109 };
const TUid KHangingPluginUid = { 0x0100010A };
const TUid KSlowStartPluginUid = { 0x0100010B };
const TUid KFeatureAUid = { 0x0100010C };
const TUid KFeatureBUid = { 0x0100010D };
const TUid KFeatureCUid = { 0x0100010E };
const TUid KOODUid = { 0x0100010F };
const TUid KPluginIgnoringUid = { 0x01000110 };
const TUid KPropertyCat1 = {0x01000111};
const TUid KPropertyCat2 = {0x01000112};
const TUid KAccessTestPropertyCat = {0x01000113};


const TUid KNewUid = { 0x01000009 };
const TUid KNewUid2 = { 0x0100000A };
const TUid KNewUid1 = { 0x0100000B };
const TUid KNewUid3 = { 0x0100000C };
const TUid KNewUid4 = { 0x0100000D };

const TUid KDSR1Uid = { 0x10000001 }; // Uid written in test feature file to DSR
const TUid KDSR2Uid = { 0x10000006 }; // Another Uid written in test feature file to DSR
const TUid KDSR3Uid = { 0x1000000A }; // This should not be written to DSR in test feature file

const TUid KDSRLowBound1= { 0x100DCBA9 };
const TUid KDSRHighBound1 = { 0x100DEF01 };

// These UIDs are used by the 'MergePluginFeatures' OOM test.
const TUid KMergeUid1 = { 0x01000011 };
const TUid KMergeUid2 = { 0x01000012 };
const TUid KMergeUid3 = { 0x01000013 };
const TUid KMergeUid4 = { 0x01000014 };
const TUid KMergeUid5 = { 0x01000015 };
                                       
// Feature data used in test cases.
const TInt KDefaultData1 = { 0xabcd };
const TInt KChangeData = { 0xfafa };
const TInt KWaitDelay = { 2000000 };
const TInt KPropertyKey = 1;

enum { EReadDeviceDataCap = 1, 
	   ENonReadDeviceDataCap };

#endif//__EFMTESTCONSTS__
