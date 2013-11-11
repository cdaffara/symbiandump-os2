/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* KMS LDD tests which cover opening and closing the channel.
*
*/


/**
 @file
*/

#include "kmslddtest.h"


TInt RKmsChannelTest::Open(const TVersion& aReqVer)
/**
	Attempt to open the channel with the supplied version.

	@param	aReqVer			Requested version of device driver.
	@return					Symbian OS error code.  This is the return code
							from DoCreate, which is called with the supplied version.
 */
	{
	// device driver factory name.  This is not the LDD filename.
	const TDesC& name = RKmsChannel::FactoryName();
	
	return DoCreate(
		name,
		aReqVer,
		KNullUnit,			// aUnit: units not supported (KDeviceAllowUnit flag not set)
		NULL,				// aDriver: PDD name, not supported (KDeviceAllocPhysicalDevice flag not set)
		NULL,				// anInfo: allow info not supported (KDeviceAllowInfo flag not set)
		EOwnerThread,		// aType: handle is owned by thread, not process
		EFalse);			// aProtected: channel cannot be shared with other processes
	}

TInt RKmsChannelTest::SendControl(TInt aControlCode)
/**
	Send the supplied control function ID to the channel with no arguments.
	This function is provided to test that the LDD handles unrecognized control
	function IDs.
	
	@param	aControlCode	Control function ID to send via DoControl.
 */
	{
	return DoControl(aControlCode);
	}

TInt RKmsChannelTest::SendRequest(TInt aRequestCode)
/**
	Send the supplied request function ID to the channel with no arguments.
	This function is provided to test that the LDD handles unrecognized request
	function IDs.

	@param	aRequestCode	Request identifier to send to the LDD.
	@return					Symbian OS error code, returned by DoRequest.
	@see RBusLogicalChannel::DoRequest
 */
	{
	TRequestStatus rs;
	DoRequest(aRequestCode, rs);
	User::WaitForRequest(rs);
	return rs.Int();
	}

void TestOpenClose()
/**
	Open and close a channel, but do not do anything with it.
 */
	{
	test.Start(_L(" @SYMTestCaseID:SEC-CRYPTOSPI-OPENCLOSE-0001 TestOpenClose "));
	RKmsChannel kmsldd;
	
	TInt r = kmsldd.Open();
	test(r == KErrNone);
	kmsldd.Close();
	
	TVersion reqVer = KmsLddImpl::Version();
	
	// request lower version than factory provides
	--reqVer.iMajor;	
	RKmsChannelTest testChannel;
	r = testChannel.Open(reqVer);
	test(r == KErrNone);
	testChannel.Close();
	
	// request same version as factory provides
	++reqVer.iMajor;	
	r = testChannel.Open(reqVer);
	test(r == KErrNone);
	
	// send invalid function codes
	r = testChannel.SendControl(10);
	test(r == KErrNotSupported);
	r = testChannel.SendRequest(10);
	test(r == KErrNotSupported);
	testChannel.Close();
	
	// request greater version than factory provides
	++reqVer.iMajor;	
	r = testChannel.Open(reqVer);
	test(r == KErrNotSupported);
	testChannel.Close();
	
	test.End();
	}

