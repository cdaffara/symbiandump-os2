// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CURIANDAUTHORITYTEST_H__
#define __CURIANDAUTHORITYTEST_H__

// System includes
//
#include <e32base.h>

// Local includes
//
#include "IpuTestUtils.h"
#include <uri8.h>
#include <uri16.h>
#include <authority8.h>
#include <authority16.h>
#include <delimitedpath8.h>
#include <delimitedpath16.h>
#include "ctestbase.h"

enum TUriMatchAndPresenceFlags
	{
	ESchemeFlag			= 0x01,
	EUserinfoFlag		= 0x02,
	EHostFlag			= 0x04,
	EPortFlag			= 0x08,
	EPathFlag			= 0x10,
	EQueryFlag			= 0x20,
	EFragmentFlag		= 0x40
	};

// CUriAndAuthorityTest - test class for the Uri and Authority family of classes
//
class CUriAndAuthorityTest : public CTestBase
	{
public:

	// Static factory c'tor. Leaves pointer to created object on the cleanup stack.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CUriAndAuthorityTest* NewLC(CIpuTestHarness* aTestHarness);

	// Static factory c'tor.
	//
	// Rtn: pointer to newly created object - ownership transfered to caller.
	//
	static CUriAndAuthorityTest* NewL(CIpuTestHarness* aTestHarness);

	// D'tor
	//
	~CUriAndAuthorityTest();

	// Runs the defined tests.
	//
	void DoTestsL();

//
//
//	Uri Tests
//
//

	// Tests the parsing of TUriParser components
	//
	// In:
	// aUri			- the uri to be parsed
	// aScheme		- what the scheme should be
	// aUserInfo	- what the userinfo should be
	// aHost		- what the host should be
	// aPort		- what the port should be
	// aPath		- what the path should be
	// aQuery		- what the query should be
	// aFragment	- what the fragment should be
	//
	void TestUriComponentExtractionL(const TDesC16& aUri, const TDesC16& aScheme,
									   const TDesC16& aUserinfo, const TDesC16& aHost, 
									   const TDesC16& aPort, const TDesC16& aPath, 
									   const TDesC16& aQuery, const TDesC16& aFragment) const;


	void TestUriRemoveComponentL(const TDesC& aUri_NoScheme, const TDesC& aUri_NoHost, const TDesC& aUri_NoUserinfo, 
								const TDesC& aUri_NoPort, const TDesC& aUri_NoPath, const TDesC& aUri_NoQuery, 
								const TDesC& aUri_NoFragment, const TDesC& aUri_Whole) const;

	void TestUriSetComponentL(const TDesC& aUri_Scheme, const TDesC& aUri_Host, const TDesC& aUri_Userinfo, 
							const TDesC& aUri_Port, const TDesC& aUri_Path, const TDesC& aUri_Query, 
							const TDesC& aUri_Fragment, const TDesC& aScheme, const TDesC& aHost, 
							const TDesC& aUserinfo, const TDesC& aPort, const TDesC& aPath, 
							const TDesC& aQuery, const TDesC& aFragment) const;
private:	// Methods

	// Default c'tor.
	//
	CUriAndAuthorityTest(CIpuTestHarness* aTestHarness);

	// Non-trivial c'tor. Second part of 2-phase construction - does all allocation.
	//
	void ConstructL();

	// Tests uri resolving.
	//
	// In:
	// aBase		- base uri
	// aRelative	- referene uri to be resolved
	// aExpected	- expected result
	//
	void TestResolveL(const TDesC& aBase, const TDesC& aReference, const TDesC& aExpected) const;

	// Tests comparison of 2 uri's
	//
	// In:
	// aUri1		- a uri
	// aUri2		- a uri
	// aMatchFlags	- set of flags specifying which components match
	//
	void TestUriCompareL(const TDesC& aUri1, const TDesC& aUri2, TInt aMatchFlags) const;

	// Tests the presence of components in a uri
	//
	// In:
	// aUri				- the uri to test
	// aPresenceFlags	- set of flags specifying which components are present
	//
	void TestUriComponentPresenceL(const TDesC& aUri, TInt aPresenceFlags) const;

	// Tests the UriDes() function
	//
	// In:
	// aUri	- the uri
	//
	void TestUriDesL(const TDesC& aUri) const;

	// Tests scheme validation
	//
	// In:
	// aUri			- the uri
	// aValidScheme	- boolean value stating whether scheme is valid or not
	//
	void TestUriSchemeValidiyL(const TDesC& aUri, TBool aValidScheme) const;

	// Tests retrieving the fragmentless uri
	//
	// In:
	// aUri			- the uri
	// aExpected	- the expeced fragmentless uri
	//
	void TestUriWithoutFragmentL(const TDesC& aUri, const TDesC& aExpected);

//
//
//	Authority Tests
//
//
	
	// Tests the parsing of TAuthorityParser components
	//
	// In:
	// aAuthority	- the authority to be parsed
	// aHost		- what the host should be
	// aUserInfo	- what the userinfo should be
	// aPort		- what the port should be
	//
	void TestAuthorityComponentExtractionL(const TDesC16& aAuthority, const TDesC16& aHost, 
										   const TDesC16& aUserinfo, const TDesC16& aPort) const;

	// Tests comparison of 2 authorities
	//
	// In:
	// aAuthority1	- an authority
	// aAuthority2	- an authority
	// aMatchFlags	- set of flags specifying which components match
	//
	void TestAuthorityCompareL(const TDesC& aAuthority1, const TDesC& aAuthority2, TInt aMatchFlags) const;
					
	// Tests the presence of components in an authority
	//
	// In:
	// aAuthority		- the authority to test
	// aPresenceFlags	- set of flags specifying which components are present
	//
	void TestAuthorityComponentPresenceL(const TDesC& aAuthority, TInt aPresenceFlags) const;

	// Tests the AuthorityDes() function
	//
	// In:
	// aAuthority	- the authority
	//
	void TestAuthorityDesL(const TDesC& aAuthority);

	void TestAuthoritySetComponentL(const TDesC& aAuthority_Host, const TDesC& aAuthority_Userinfo, const TDesC& aAuthority_Port, 
									const TDesC& aHost, const TDesC& aUserinfo, const TDesC& aPort);

	void TestAuthoritySetAndEscapeComponentL(const TDesC& aAuthority_Host, const TDesC& aAuthority_Userinfo, const TDesC& aAuthority_Port, 
											const TDesC& aHost, const TDesC& aUserinfo, const TDesC& aPort) const;

	void TestAuthorityRemoveComponentL(const TDesC& aAuthority_NoHost, const TDesC& aAuthority_NoUserinfo, 
									   const TDesC& aAuthority_NoPort, const TDesC& aAuthority_Whole) const;

private:	// Attributes

	// Test harness
	//
	CIpuTestHarness*	iTestHarness;

	};

#endif	// __CURIANDAUTHORITYTEST_H__
