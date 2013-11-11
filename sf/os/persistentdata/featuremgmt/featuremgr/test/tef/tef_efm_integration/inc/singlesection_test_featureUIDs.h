// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @internalComponent
*/
namespace NFeature {
	// default present
	/**
	Feature = TestDSRUid1
	This feature is defined as "unsupported", "modifiable" and "persisted" in the xml file
	Also this feature is included into DSR.
	Feature manager should return the status for this feature as defined by the individual feature specification 
	*/
	const TUid KTestDSRUid1 = {0x10281801};


	// default not present
	/**
	Feature = TestBlacklistedUid
	This feature is defined as "modifiable" in the xml file
	*/
	const TUid KTestBlacklistedUid = {0x104};

	/**
	Feature = TestModifiableUid,  
	This feature is defined as "modifiable" in the xml file
	*/
	const TUid KTestModifiableUid = {0x103};

	/**
	Feature = TestOverwrittenByIBYUid
	This feature is defined as "unsupported" in the xml file, and then its status changed to 
	"supported" in the IBY file. 
	*/
	const TUid KTestOverwrittenByIBYUid = {0x109};

	/**
	Feature = TestPersistedUid
	This feature is defined as "persisted" and "modifiable" in the xml file
	*/
	const TUid KTestPersistedUid = {0x107};

	/**
	Feature = TestSupportedUid,  
	This feature is defined as "supported" in the xml file
	*/
	const TUid KTestSupportedUid = {0x101};

	/**
	Feature = TestUninitialisedUid
	This feature is defined as "uninitialised" and "modifiable" in the xml file
	*/
	const TUid KTestUninitialisedUid = {0x105};

	/**
	Feature = TestUnsupportedUid
	This feature is defined as "unsupported" in the xml file
	*/
	const TUid KTestUnsupportedUid = {0x108};

	/**
	Feature = TestUpgradableUid,  
	This feature is defined as "upgradable" in the xml file
	*/
	const TUid KTestUpgradableUid = {0x102};

}
