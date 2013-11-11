// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <f32file.h>
#include "logservsecurity.h"
#include "LogServResourceInterpreter.h"
#include <logeng.h>
#include <logengevents.h>
#include "t_logsecuritypolicy.h"
#include "t_logutil.h"

RTest TheTest(_L("t_logsecurity"));
static RFs TheFileSess;

///////////////////////////////////////////////////////////////////////////////////////
//

//Check if supplied aPolicy parameter has aCapability capability.
static TBool HasCapability(const TCompiledSecurityPolicy& aPolicy, TCapability aCapability)
	{
	TInt maxCount = 0;
	if(aPolicy.Type() == TSecurityPolicy::ETypeS3 || 
	   aPolicy.Type() == TSecurityPolicy::ETypeV3 ||
	   aPolicy.Type() == TSecurityPolicy::ETypeC3)
		{
		maxCount = 3;	
		}
	else if(aPolicy.Type() == TSecurityPolicy::ETypeC7)
		{
		maxCount = 7;	
		}
	else
		{
		TEST(0);
		}
	for(TInt i=0;i<maxCount;++i)
		{
		if(aPolicy.Capability(i) == aCapability)
			{
			return ETrue;	
			}
		}
	return EFalse;
	}

///////////////////////////////////////////////////////////////////////////////////////
//

//Checks that all supported LogEngServer event types have the expected capabilities.
//See LogWrap.rss file
static void CheckEventSecurityPolicies(CLogServSecurity& aLogServSecurity)
	{
	TSecurityPolicy plc1r = aLogServSecurity.SecurityPolicy(KLogCallEventTypeUid, EReadOp);
	TEST(::HasCapability(plc1r, ECapabilityReadUserData));
	
	TSecurityPolicy plc1w = aLogServSecurity.SecurityPolicy(KLogCallEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc1w, ECapabilityWriteUserData));
	
	TSecurityPolicy plc2r = aLogServSecurity.SecurityPolicy(KLogDataEventTypeUid, EReadOp);
	TEST(::HasCapability(plc2r, ECapabilityReadUserData));
	
	TSecurityPolicy plc2w = aLogServSecurity.SecurityPolicy(KLogDataEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc2w, ECapabilityWriteUserData));
	
	TSecurityPolicy plc3r = aLogServSecurity.SecurityPolicy(KLogFaxEventTypeUid, EReadOp);
	TEST(::HasCapability(plc3r, ECapabilityReadUserData));
	
	TSecurityPolicy plc3w = aLogServSecurity.SecurityPolicy(KLogFaxEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc3w, ECapabilityWriteUserData));
	
	TSecurityPolicy plc4r = aLogServSecurity.SecurityPolicy(KLogShortMessageEventTypeUid, EReadOp);
	TEST(::HasCapability(plc4r, ECapabilityReadUserData));
	
	TSecurityPolicy plc4w = aLogServSecurity.SecurityPolicy(KLogShortMessageEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc4w, ECapabilityWriteUserData));
	
	TSecurityPolicy plc5r = aLogServSecurity.SecurityPolicy(KLogMailEventTypeUid, EReadOp);
	TEST(TCompiledSecurityPolicy(plc5r).Type() == TSecurityPolicy::EAlwaysPass);
	
	TSecurityPolicy plc5w = aLogServSecurity.SecurityPolicy(KLogMailEventTypeUid, EWriteOp);
	TEST(TCompiledSecurityPolicy(plc5w).Type() == TSecurityPolicy::EAlwaysPass);
	
	TSecurityPolicy plc6r = aLogServSecurity.SecurityPolicy(KLogTaskSchedulerEventTypeUid, EReadOp);
	TEST(::HasCapability(plc6r, ECapabilityReadUserData));
	
	TSecurityPolicy plc6w = aLogServSecurity.SecurityPolicy(KLogTaskSchedulerEventTypeUid, EWriteOp);
	TEST(!::HasCapability(plc6w, ECapabilityWriteUserData));
	
	TSecurityPolicy plc7r = aLogServSecurity.SecurityPolicy(KLogPacketDataEventTypeUid, EReadOp);
	TEST(::HasCapability(plc7r, ECapabilityReadUserData));
	
	TSecurityPolicy plc7w = aLogServSecurity.SecurityPolicy(KLogPacketDataEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc7w, ECapabilityWriteUserData));
	
	
	TSecurityPolicy plc8r = aLogServSecurity.SecurityPolicy(KLogLbsSelfLocateEventTypeUid, EReadOp);
	TEST(::HasCapability(plc8r, ECapabilityReadDeviceData));
	
	TSecurityPolicy plc8w = aLogServSecurity.SecurityPolicy(KLogLbsSelfLocateEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc8w, ECapabilityWriteDeviceData));
	
	TSecurityPolicy plc9r = aLogServSecurity.SecurityPolicy(KLogLbsExternalLocateEventTypeUid, EReadOp);
	TEST(::HasCapability(plc9r, ECapabilityReadDeviceData));
	
	TSecurityPolicy plc9w = aLogServSecurity.SecurityPolicy(KLogLbsExternalLocateEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc9w, ECapabilityWriteDeviceData));
	
	TSecurityPolicy plc10r = aLogServSecurity.SecurityPolicy(KLogLbsTransmitLocationEventTypeUid, EReadOp);
	TEST(::HasCapability(plc10r, ECapabilityReadDeviceData));
	
	TSecurityPolicy plc10w = aLogServSecurity.SecurityPolicy(KLogLbsTransmitLocationEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc10w, ECapabilityWriteDeviceData));
	
	TSecurityPolicy plc11r = aLogServSecurity.SecurityPolicy(KLogLbsNetworkLocateEventTypeUid, EReadOp);
	TEST(::HasCapability(plc11r, ECapabilityReadDeviceData));
	
	TSecurityPolicy plc11w = aLogServSecurity.SecurityPolicy(KLogLbsNetworkLocateEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc11w, ECapabilityWriteDeviceData));
	
	TSecurityPolicy plc12r = aLogServSecurity.SecurityPolicy(KLogLbsAssistanceDataEventTypeUid, EReadOp);
	TEST(::HasCapability(plc12r, ECapabilityReadDeviceData));
	
	TSecurityPolicy plc12w = aLogServSecurity.SecurityPolicy(KLogLbsAssistanceDataEventTypeUid, EWriteOp);
	TEST(::HasCapability(plc12w, ECapabilityWriteDeviceData));
	}


static void DoOOMTestL()
	{
  CLogServResourceInterpreter* logServRsc = CLogServResourceInterpreter::NewL(TheFileSess);
	CleanupStack::PushL(logServRsc);
	
	CLogServSecurity* logServSecurity = CLogServSecurity::NewL(*logServRsc);
	CleanupStack::PushL(logServSecurity);

	::CheckEventSecurityPolicies(*logServSecurity);

	CleanupStack::PopAndDestroy(logServSecurity);
	CleanupStack::PopAndDestroy(logServRsc);
	}

// CLogServSecurity instance creation - OOM test
static void OOMTest()
	{
	for(TInt count=1;;++count)
		{
		__UHEAP_FAILNEXT(count);
		__UHEAP_MARK;
		TRAPD(err, ::DoOOMTestL());

		if(err == KErrNoMemory)
			{
			__UHEAP_MARKEND;
			}
		else if(err == KErrNone)
			{
			__UHEAP_MARKEND;
			TheTest.Printf(_L("The test succeeded at heap failure rate=%d.\n"), count);
			break;
			}
		else 
			{
			__UHEAP_MARKEND;
			TEST2(err, KErrNone);
			}
		}
	__UHEAP_RESET;
	}
	
///////////////////////////////////////////////////////////////////////////////////////
//

static void MainL()
	{
	TheTest.Start(_L(" @SYMTestCaseID:SYSLIBS-LOGENG-LEGACY-T_LOGSECURITY-0001 OOM test "));
	::OOMTest();
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* tc = CTrapCleanup::New();
	TEST(tc != NULL);
	
	TEST2(TheFileSess.Connect(), KErrNone);

	TheTest.Title();
	TRAPD(err, ::MainL());
	TEST2(err, KErrNone);
	
	TheTest.End();
	TheTest.Close();

	TheFileSess.Close();
	
	delete tc;

	__UHEAP_MARKEND;

	User::Heap().Check();
	return KErrNone;
	}
