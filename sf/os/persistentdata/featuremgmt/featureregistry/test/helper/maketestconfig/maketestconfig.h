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
// Constants used in test configuration files.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef MAKETESTCONFIG_H
#define MAKETESTCONFIG_H

#include <e32cmn.h>

// Naming scheme for feature ID constants in small valid config test:
//
// UNFeat = unsupported feature not in default=supported range (need not be listed in config file)
// URFeat = unsupported feature in default=supported range (must be listed in config file)
// SNFeat = supported feature not in default=supported range (must be listed in config file)
// SRFeat = supported feature in default=supported range (need not be listed in config file)
// LoFeat = low-feature-Id of default=supported range
// HiFeat = high-feature-Id of default=supported range
// Id = TInt identifier
// St = status word

const TUint32 KFeatRegTest1_01_UNFeatId = 0x12345679;
const TUint32 KFeatRegTest1_02_SNFeatId = 0x12345678;
const TUint32 KFeatRegTest1_02_SNFeatSt = 0x41; // low-bit set = supported, other bits to retrieve through API
const TUint32 KFeatRegTest1_03_LoFeatId = 0x20000000;
const TUint32 KFeatRegTest1_04_HiFeatId = 0x20000FFF;
const TUint32 KFeatRegTest1_05_URFeatId = 0x20000100;
const TUint32 KFeatRegTest1_05_URFeatSt = 0x2;	// low-bit not set = unsupported, 2nd bit set = upgradable
const TUint32 KFeatRegTest1_06_SRFeatId = 0x20000123;
const TUint32 KFeatRegTest_DummyRangeId = 0x2FFFFFFF;

// Parameters for large valid config test:
// start, stop & increment of features added to config file
// plus "not supported" bit to alternate support of queried features.
const TUint32 KFeatRegTest2_01_FirstId	= 0x00000000;
const TUint32 KFeatRegTest2_02_LastId	= 0xFF000000;
const TUint32 KFeatRegTest2_03_IncrId	= 0x00010000;
const TUint32 KFeatRegTest2_04_numTestFeatures = 0x100;
// listed features are not supported if following bit set (alternately with above increment):
const TUint32 KFeatRegTest2_05_NotSupportedBit = 0x01000000;
// default=supported ranges have following bit set:
const TUint32 KFeatRegTest2_06_DefSupportedBit = 0x20000000;
const TUint32 KFeatRegTest2_07_rangeCount = 4;

#endif
