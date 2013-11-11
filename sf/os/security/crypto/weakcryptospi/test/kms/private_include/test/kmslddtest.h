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
* Variables and structures used by KMS LDD test code.
*
*/




/**
 @file
*/

#ifndef KMSLDDTEST_H
#define KMSLDDTEST_H

#include <e32ldr.h>
#include <e32ldr_private.h>
#include <e32test.h>
#include <e32def_private.h>

#include "kmsldd.h"
#include "kmsextrldd.h"

#include "kmslddcommon.h"	// require access to request values and transfer constructs

#include "kmstest.h"

// ---- test channel ----

class RKmsChannelTest : public RBusLogicalChannel
/**
	Test device driver sends invalid descriptors to kernel-side
	channel. These descriptors are normally created by the kmsldd dll,
	and so hidden from the RKmsChannel user.
 */
	{
public:
	TInt Open();
	
	TInt Open(const TVersion& aReqVer);
	TInt SendControl(TInt aControlCode);
	TInt SendRequest(TInt aRequestCode);
	
	void TestInvalidGenArgsDescriptor(TRequestStatus& aReqStat);
	void TestInvalidStoreArgsDescriptor(TRequestStatus& aStatus);
	void TestAddUsageBadArgDescriptor(TRequestStatus& aStatus);
	void TestDeleteUsageBadArgDescriptor(TRequestStatus& aStatus);
	void TestUsageAllowedBadArgDescriptor(TRequestStatus& aStatus);
	};

/**
	Identifies a function on the custom channel which will cause the current
	thread to be panicked.
 */
typedef void (RKmsChannelTest::* TTestChannelPanicFunc)(TRequestStatus&);

void TestPanicTestChannel(TTestChannelPanicFunc aFunc, TInt aExpectedReason);

void TestSecondProcess(
	TUint32 aCapabilityMask, TSecureId aSecureId,
	TInt (*aTestFunc)(RProcess), TInt aExpectedResult);

/**
 * Test cases implement a function with this signature.  On entry
 * the channel has already been opened, and running in context of
 * the thread which should be panicked.  The implementation should
 * call the relevant channel function, passing in the invalid parameter.
 */
typedef void (*TPanicFunc)(RKmsChannel&, TRequestStatus&);

void TestPanic(TPanicFunc aFunc, TInt aExpectedReason);


/**
	Secure ID for a second executable.  Defined to test hardware
	store's SID policing.

	@see KSidTestProc
 */
_LIT_SECURE_ID(KSidSecondProc, 0x102833C1);

/**
	Security policy policies on SID KSidTestProc.

	@see KSidTestProc
	@see KSecPolSidSecondProc
 */
_LIT_SECURITY_POLICY_S0(KSecPolSidCurrentProc, KmsLddImpl::KKmsServerUidValue);

/**
	Security policy policies on SID KSidTestProc.

	@see KSidSecondProc
	@see KSecPolSidCurrentProc
 */
_LIT_SECURITY_POLICY_S0(KSecPolSidSecondProc, 0x102833C1);

const TInt KOperationX = MULTICHAR4("OP_X");	///< Arbitrary operation identifier, used to construct usage.
const TInt KOperationY = MULTICHAR4("OP_Y");	///< Arbitrary operation identifier, used to construct usage.

void RunOomTest(void (*aAllocFunc)(TRequestStatus&), void (*aFreeFunc)());

// ---- top-level tests ----

void TestDup();
void TestOpenClose();
void TestGenerateKey();
void TestStoreKey();
void TestDeleteKey();
void TestUsage();
void TestExtraction();

void TestGenerateSingleKey(TInt aLength, RProcess aOwner, TKeyHandle& aHandle, TInt aExpectedResult);
void TestGenerateSingleKey(TInt aLength, TKeyHandle& aHandle, TInt aExpectedResult);

void TestStoreSingleKey(const TDesC8& aData, TKeyHandle& aHandle, TInt aExpectedResult);

TInt DeleteSingleKeySync(TKeyHandle aHandle);
void TestDeleteSingleKey(TKeyHandle aHandle, TInt aExpectedResult);

void AddUsage(RProcess aSetter, TKeyHandle aHandle, TInt aOperation, const TSecurityPolicy& aSecPol, TInt aExpectedResult);

RThread LaunchThreadAndWaitForRendezvous(TThreadFunction aEntrypoint, TAny* aData);
void ConfirmPanicReason(RThread aThread, TInt aExpectedReason);

void DeleteAllKeys();

// ---- globals ----

extern RTest test;						///< Test object asserts conditions and prints status.

extern RKmsChannel KmsChannel;					///< Channel object declared at file level for convenience.
extern RKmsExtrChannel ExtrChannel;				///< Channel object declared at file level for convenience.

extern TKeyHandle GenKeyHandle0;				///< When used, refers to generated key.
extern TKeyHandle GenKeyHandle1;				///< When used, refers to second generated key.

extern TKeyHandle StoreKeyHandle0;				///< When used, refers to stored key.
extern TKeyHandle StoreKeyHandle1;				///< When used, refers to second stored key.

extern RProcess CurrentProcess;					///< Handle to current process, declared at file level for convenience.


#endif	// #ifndef KMSLDDTEST_H

