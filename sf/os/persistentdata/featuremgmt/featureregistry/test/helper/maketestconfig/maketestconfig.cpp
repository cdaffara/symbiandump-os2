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
// Puts some test feature registry configuration files - some corrupt, others legal -
// in the C: private data cage of the setup executable. Sets up test RProperties.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#include <e32cmn.h>
#include <e32base.h>
#include <f32file.h>
#include <e32property.h>
#include <featreg.h>
#include "featregcmn.h"
#include "maketestconfig.h"

// names of test configurations that can be set up by this exe, specified as
// command-line option:
_LIT(KFeatRegTestConfig_force_setup,				"force_setup");
_LIT(KFeatRegTestConfig_reset,						"reset");
_LIT(KFeatRegTestConfig_corrupt_missing,			"corrupt_missing");
_LIT(KFeatRegTestConfig_corrupt_noheader,			"corrupt_noheader");
_LIT(KFeatRegTestConfig_corrupt_incompleteheader,	"corrupt_incompleteheader");
_LIT(KFeatRegTestConfig_corrupt_invalidtypeprefix,	"corrupt_invalidtypeprefix");
_LIT(KFeatRegTestConfig_corrupt_badversionnumber,	"corrupt_badversionnumber");
_LIT(KFeatRegTestConfig_corrupt_toomuchdata,		"corrupt_toomuchdata");
_LIT(KFeatRegTestConfig_corrupt_toolittledata,		"corrupt_toolittledata");
_LIT(KFeatRegTestConfig_corrupt_entryoutoforder,	"corrupt_entryoutoforder");
_LIT(KFeatRegTestConfig_corrupt_entryrepeated,		"corrupt_entryrepeated");
_LIT(KFeatRegTestConfig_corrupt_badrange,			"corrupt_badrange");
_LIT(KFeatRegTestConfig_valid_nofeatures,			"valid_nofeatures");
_LIT(KFeatRegTestConfig_valid_small,				"valid_small");
_LIT(KFeatRegTestConfig_valid_large,				"valid_large");
_LIT(KFeatRegTestConfig_valid_perf,				    "valid_perf_");


TConfigFileName ConfigFileName;


static TInt DefineTestFlagProperty()
	{
	RProcess thisProcess;
	// sanity check that feature property category in common header equals SID of this process
	ASSERT(KFeaturePropCat == thisProcess.SecureId());
	TSecurityPolicy readPolicy(TSecurityPolicy::EAlwaysPass);
	TSecurityPolicy writePolicy(thisProcess.SecureId());
	TInt result = RProperty::Define(KFeatRegConfigTestKey, RProperty::EInt, readPolicy, writePolicy);
	if (result == KErrAlreadyExists)
		{
		result = KErrNone;
		}
	return result;
	}

static TInt WriteTestConfigFile(TUint32* aData, TInt aSize)
	{
	RFs fs;
	TInt result = fs.Connect();
	if (result == KErrNone)
		{
		result = fs.MkDirAll(ConfigFileName);
		if ((result == KErrNone) || (result == KErrAlreadyExists))
			{
			RFile cfgFile;
			result = cfgFile.Replace(fs, ConfigFileName, EFileWrite|EFileStream);
			if (result == KErrNone)
				{
				result = cfgFile.Write(TPtrC8(reinterpret_cast<const TUint8 *>(aData), aSize));
				cfgFile.Close();
				}
			}
		fs.Close();
		}
	if (result == KErrNone)
		{
		result = DefineTestFlagProperty();
		}
	return result;
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	TFileName configName;
	User::CommandLine(configName);

	// construct config filename
	GetSystemDrivePath(ConfigFileName);

	// always delete feature property so featreg.dll re-loads it (note this
	// executable has same UID3 as featreg setup:
	TInt result = RProperty::Delete(KFeaturePropKey);
	if (result == KErrNotFound)
		{
		result = KErrNone;
		}
	if (result != KErrNone)
		{
		goto cleanupReturn;
		}

	//--------------------
	if (configName.Find(KFeatRegTestConfig_force_setup) >= 0)
		{
		// force setup
		// nothing to do; just ensures feature property is deleted so setup is run
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_reset) >= 0)
		{
		// reset
		// delete "test-flag" property so setup looks at true config info
		result = RProperty::Delete(KFeatRegConfigTestKey);
		if (result == KErrNotFound)
			{
			result = KErrNone;
			}
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_missing) >= 0)
		{
		// no header
		RFs fs;
		result = fs.Connect();
		if (result == KErrNone)
			{
			result = fs.Delete(ConfigFileName);
			if ((result == KErrNotFound) || (result == KErrPathNotFound))
				{
				result = KErrNone;
				}
			fs.Close();
			}
		if (result == KErrNone)
			{
			result = DefineTestFlagProperty();
			}
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_noheader) >= 0)
		{
		// no header
		result = WriteTestConfigFile(NULL, 0);
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_incompleteheader) >= 0)
		{
		// header is not complete
		TUint32 fileData1[] =
			{
			validTypePrefix,
			0,	// version number, must be zero
			0	// entry count
				// range count: missing
			};
		result = WriteTestConfigFile(fileData1, sizeof(fileData1));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_invalidtypeprefix) >= 0)
		{
		// invalid type prefix
		TUint32 fileData1[] =
			{
			invalidTypePrefix,	 // should be validTypePrefix
			0,	// version number
			0,	// entry count
			0	// range count
			};
		result = WriteTestConfigFile(fileData1, sizeof(fileData1));
		}

	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_badversionnumber) >= 0)
		{
		// bad file version header
		TUint32 fileData1[] =
			{
			validTypePrefix,
			1,	// version number: not zero
			0,	// entry count
			0	// range count
			};
		result = WriteTestConfigFile(fileData1, sizeof(fileData1));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_toomuchdata) >= 0)
		{
		// corrupt: config file contains more data than required by header
		TUint32 fileData2[] =
			{
			validTypePrefix,
			0,	// version number, must be zero
			0,	// entry count
			0,	// range count
			0,	// unexpected extra data
			};
		result = WriteTestConfigFile(fileData2, sizeof(fileData2));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_toolittledata) >= 0)
		{
		// corrupt: config file contains less data than required by header
		TUint32 fileData3[] =
			{
			validTypePrefix,
			0,	// version number, must be zero
			2,	// entry count
			1	// range count
				// should be 2 entries (2 words each) and 1 default range (2 words each)
			};
		result = WriteTestConfigFile(fileData3, sizeof(fileData3));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_entryoutoforder) >= 0)
		{
		// corrupt: feature entries not ordered from lowest to highest UID
		TUint32 fileData4[] =
			{
			validTypePrefix,
			0,	// version number, must be zero
			3,	// entry count
			0,	// range count
			2,	// Feature UID = 2
			1,	//   status low-bit set -> present
			5,	// Feature UID = 5
			1,	//   status low-bit set -> present
			3,	// Feature UID = 3: Feature UIDs not in strictly increasing order
			1	//   status low-bit set -> present
			};
		result = WriteTestConfigFile(fileData4, sizeof(fileData4));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_entryrepeated) >= 0)
		{
		// corrupt: repeated feature UID
		TUint32 fileData5[] =
			{
			validTypePrefix,
			0,	// version number, must be zero
			3,	// entry count
			0,	// range count
			2,	// Feature UID = 2
			1,	//   status low-bit set -> present
			5,	// Feature UID = 5
			1,	//   status low-bit set -> present
			5,	// Feature UID = 5: Feature UID repeated
			1,	//   status low-bit set -> present
			};
		result = WriteTestConfigFile(fileData5, sizeof(fileData5));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_corrupt_badrange) >= 0)
		{
		// corrupt: default-supported range not listed in order lowUid-highUid
		TUint32 fileData6[] =
			{
			validTypePrefix,
			0,	// version number, must be zero
			1,	// entry count
			2,	// range count
			2,	// Feature UID = 2
			1,	//   status low-bit set -> present
			5,	// Range 1 low UID
			9,	//         high UID
			88,	// Range 2 low UID
			76	//         high UID: not >= lowUID
			};
		result = WriteTestConfigFile(fileData6, sizeof(fileData6));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_valid_nofeatures) >= 0)
		{
#ifndef SYMBIAN_FEATURE_MANAGER
		// valid: no features
				TUint32 fileData7[] =
					{
					validTypePrefix,
					0,	// version number, must be zero
					0,	// entry count
					0	// range count
					};
#else	
		// valid: no features
				TUint32 fileData7[] =
					{
					validTypePrefix,
					0,	// version number, must be zero
					0,	// entry count
					1,	// range count: we have to provide at least one range, otherwise the file will be corrupt from the FeatMgr perspective
					KFeatRegTest_DummyRangeId,
					KFeatRegTest_DummyRangeId
					};		
#endif	
		result = WriteTestConfigFile(fileData7, sizeof(fileData7));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_valid_small) >= 0)
		{
		// valid: few features, one default=supported range
		TUint32 fileData8[] =
			{
			validTypePrefix,
			0,	// version number, must be zero
			2,	// entry count
			1,	// range count
			KFeatRegTest1_02_SNFeatId,	// 1. feature UID
			KFeatRegTest1_02_SNFeatSt,	//    status word; low bit set = supported
			KFeatRegTest1_05_URFeatId,	// 2. feature UID
			KFeatRegTest1_05_URFeatSt,	//    status word; low-bit not set = not supported
			KFeatRegTest1_03_LoFeatId,	// low-feature-UID of default=supported range
			KFeatRegTest1_04_HiFeatId	// high-feature-UID of default=supported range
			};
		result = WriteTestConfigFile(fileData8, sizeof(fileData8));
		}
	//--------------------
	else if (configName.Find(KFeatRegTestConfig_valid_large) >= 0)
		{
		// valid: many features, several default=supported ranges
		TUint32 fileData9[2 + 2*KFeatRegTest2_04_numTestFeatures + 2*KFeatRegTest2_07_rangeCount];
		TInt j = sizeof(TFeatureHeader) / sizeof(TUint32);
		TInt entryCount = 0;
		TUint32 id = KFeatRegTest2_01_FirstId;
		// feature entries
		for (TInt e = KFeatRegTest2_04_numTestFeatures; e > 0; e--)
			{
			TBool featureSupported = !(id & KFeatRegTest2_05_NotSupportedBit);
			TBool inDefaultSupportedRange = id & KFeatRegTest2_06_DefSupportedBit;
			// only need to list feature if support differs from default ranges
			if (featureSupported && (!inDefaultSupportedRange))
				{
				fileData9[j++] = id;
				fileData9[j++] = RFeatureRegistry::EStatusSupportBit;
				entryCount++;
				}
			else if ((!featureSupported) && inDefaultSupportedRange)
				{
				fileData9[j++] = id;
				fileData9[j++] = 0;	// specifically listed as not supported
				entryCount++;
				}
			id += KFeatRegTest2_03_IncrId;
			}
		// default=supported ranges
		TUint32 lowId = KFeatRegTest2_06_DefSupportedBit;
		for (TUint r = 0; r < KFeatRegTest2_07_rangeCount; r++)
			{
			fileData9[j++] = lowId;
			fileData9[j++] = lowId + KFeatRegTest2_06_DefSupportedBit - 1;
			lowId += 2*KFeatRegTest2_06_DefSupportedBit;
			}
		// header
		fileData9[0] = validTypePrefix,
		fileData9[1] = 0;	// version number, must be zero
		fileData9[2] = entryCount;
		fileData9[3] = KFeatRegTest2_07_rangeCount;
		result = WriteTestConfigFile(fileData9, j*sizeof(TUint32));
		}
	else if (configName.Find(KFeatRegTestConfig_valid_perf) >= 0)
		{
		// extract the number of features to put into the file from the
		// arguments supplied to the process from the script
	    TInt digitchar = 0;
	    TChar charItem(configName[digitchar]);
	    while (charItem.IsDigit() == EFalse)
           charItem = configName[++digitchar]; 
	    if (digitchar >= configName.Length())
	    	{
	    	result = KErrArgument;
	    	goto cleanupReturn;
	    	}
	    
		TPtrC16 counter = configName.Mid(digitchar);
		TLex lex1(counter);
		TInt count = 0;
        lex1.Val(count);
		// valid: allocate space for maximum number of features we would expect
		TInt j = sizeof(TFeatureHeader) / sizeof(TUint32);
		TUint32 fileData9[4 + 2*8001]; // j must == 4
		TInt entryCount = 0;
		TUint32 id = KFeatRegTest2_01_FirstId;
		// create 'count' number of feature entries
		for (TInt e = count; e > 0; e--)
			{
			fileData9[j++] = id;
			fileData9[j++] = (e&0x01) ? RFeatureRegistry::EStatusSupportBit : 0;
			entryCount++;
			id += KFeatRegTest2_03_IncrId;
			}
#ifndef SYMBIAN_FEATURE_MANAGER
		// header
		fileData9[0] = validTypePrefix,
		fileData9[1] = 0;	// version number, must be zero
		fileData9[2] = entryCount;
		fileData9[3] = 0;
#else
		//we have to provide at least one range, otherwise the file will be corrupt from the FeatMgr perspective 
		fileData9[j++] = KFeatRegTest_DummyRangeId;
		fileData9[j++] = KFeatRegTest_DummyRangeId;
		// header
		fileData9[0] = validTypePrefix,
		fileData9[1] = 0;	// version number, must be zero
		fileData9[2] = entryCount;
		fileData9[3] = 1;		
#endif		
		result = WriteTestConfigFile(fileData9, j*sizeof(TUint32));		
		}
	else
		{
		result = KErrArgument;
		}

cleanupReturn:
	__UHEAP_MARKEND;

	return result;
	}

