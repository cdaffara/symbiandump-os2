/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/




/**
 @file 
*/
#define __INCLUDE_CAPABILITY_NAMES__
#include <e32capability.h>

#include "captestframeworkstep.h"
#include "captestframework.h"

#include <test/testexecutelog.h>
#include <s32file.h>
#include <f32file.h>


CCapTestFrameworkStep::CCapTestFrameworkStep(TThoroughness aThoroughness)
	: iThoroughness(aThoroughness)
	{
	}

CCapTestFrameworkStep::~CCapTestFrameworkStep()
	{
	iLibrary.Close();
	iFs.Close();	
	}
	
TVerdict CCapTestFrameworkStep::doTestStepPreambleL()
	{
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	
	if (EFalse==GetStringFromConfig(ConfigSection(), _L("DllName"), iDllName))
		{
		return EFail;
		}

	GetBoolFromConfig(ConfigSection(), _L("OmitTCBCapInComplementSet"), iOmitTCBCapInComplementSet);

	SetupFactoryL();
	return EPass;
	}

TVerdict CCapTestFrameworkStep::doTestStepL()
	{
	for (iCurrentTest=0; iCurrentTest < iFactory->NumberOfTests(); ++iCurrentTest)
		{
		INFO_PRINTF1(_L(""));
		INFO_PRINTF2(_L("Running %S"), &iFactory->Test(iCurrentTest)->Name());
		INFO_PRINTF1(_L("Assumed to require:"));
		INFO_PRINTF2(_L("\tSid: %x"), iFactory->Test(iCurrentTest)->SidRequired());
		INFO_PRINTF2(_L("\tVid: %x"), iFactory->Test(iCurrentTest)->VidRequired());
		PrintCapabilitySet(iFactory->Test(iCurrentTest)->CapabilitiesRequired(), _L("\tCapabilities: "));
		RunTestStepL(iFactory->Test(iCurrentTest));
		}
	
	return EPass;
	}

void CCapTestFrameworkStep::RunTestStepL(MCapabilityTest* aTest)
	{
	// figure out capabilities required
	TCapabilitySet capsRequired=aTest->CapabilitiesRequired();
	TUid sidRequired=aTest->SidRequired();
	TUid vidRequired=aTest->VidRequired();
	
	RArray<TTestEnvironment> capTestEnvs;
	CleanupClosePushL(capTestEnvs);
	
	GenerateEnvironmentsL(capsRequired, sidRequired, vidRequired, capTestEnvs);

	// for all cap sets needed
	for (TInt i=0; i < capTestEnvs.Count(); ++i)
		{
		INFO_PRINTF1(_L("With:"));
		INFO_PRINTF2(_L("\tSid: %x"), capTestEnvs[i].iSid);
		INFO_PRINTF2(_L("\tVid: %x"), capTestEnvs[i].iVid);
		PrintCapabilitySet(capTestEnvs[i].iCaps, _L("\tCapabilities: "));

		// Set the capabilities of the helper
		SetupHelperL(capTestEnvs[i]);

		// run the helper and check results
		RunHelperL(iCurrentTest, capTestEnvs[i].iExpectPass);
		}
	
	CleanupStack::PopAndDestroy(&capTestEnvs);
	}

void CCapTestFrameworkStep::RunHelperL(TInt aTestNumber, TBool aShouldPass)
	{
	RProcess p;
	
	//Launching process
	User::LeaveIfError(p.Create(_L("tempcaptestframeworkhelper.exe"), KNullDesC));

	p.SetParameter(KDllNameTransferSlot, iDllName);
	p.SetParameter(KShouldPassTransferSlot, aShouldPass);
	p.SetParameter(KTestNumberTransferSlot, aTestNumber);
	
	_LIT(KLogFileName , "\\captestframework.txt");
	TDriveUnit sysDrive(RFs::GetSystemDrive());
	
	TBuf<128> logFileNameOnSysDrive = sysDrive.Name();
	logFileNameOnSysDrive.Append(KLogFileName);
	p.SetParameter(KLogFileNameTransferSlot, logFileNameOnSysDrive);
	
	// Wait for the test to finish
	TRequestStatus s;
	TRequestStatus& a=s;
	p.Logon(a);
	p.Resume();
	User::WaitForRequest(a);
	p.Close();

	// Extract the info from the logfile
	RFileReadStream logFile;
	logFile.Open(iFs, logFileNameOnSysDrive, 0);
	CleanupClosePushL(logFile);

	while (ETrue)
		{
		TInt32 pass=logFile.ReadInt32L();
		if (pass==ETestsEnded)
			{
			break;	// end of file
			}
		else if (pass==EFileEnd)
			{
			// reached end of file marker, with no success
			SetTestStepResult(EFail);
			break;	
			}
		else if (pass==ETestFailed)
			{
			HBufC* text=HBufC::NewL(logFile, KMaxTInt);
			ERR_PRINTF2(_L("%S"), text);
			delete text;
			}
		else if (pass==ETestPassed)
			{
			HBufC* text=HBufC::NewL(logFile, KMaxTInt);
			INFO_PRINTF2(_L("%S"), text);
			delete text;
			}
		}	

	CleanupStack::PopAndDestroy(&logFile);
	}

void CCapTestFrameworkStep::SetupHelperL(const TTestEnvironment& aEnvironment)
	{
	RProcess p;
// SETCAP source_exe capability [-SID secureId] [-VID vendorId] [destination_path]
	_LIT(KArgsFormat, "captestframeworkhelper.exe  %X -SID %X -VID %X tempcaptestframeworkhelper.exe");

// The 'capability' command line argument is the hexadecimal value of the
// capabilities when they are represented as a bit-field. E.g. the 3 capabilities
// LocalServices, ReadUserData and WriteUserData would together have a value of:
//
// (1<<ECapabilityLocalServices) | (<<ECapabilityReadUserData) | (1<<ECapabilityWriteUserData)
//
// Which in hexadecimal is '1c000'
 
 	const TCapabilitySet& aCapSet=aEnvironment.iCaps;
	TInt caps=0;
	for (TInt c = 0; c < ECapability_Limit; ++c)
		{
		if (aCapSet.HasCapability(TCapability(c)))
			{
			caps+=(1<<c);
			}
		}

	TBuf<512> cmdLine;
	cmdLine.Format(KArgsFormat, caps, aEnvironment.iSid, aEnvironment.iVid);

	User::LeaveIfError(p.Create(_L("setcap.exe"), cmdLine));
	
	// Wait for setcap to finish
	TRequestStatus s;
	TRequestStatus& rs=s;
	p.Logon(rs);

	p.Resume();
	User::WaitForRequest(rs);
	p.Close();
	}

void CCapTestFrameworkStep::PrintCapabilitySet(const TCapabilitySet& aCapSet, const TDesC& aExtra)
	{
	TBuf<512> string;
	string.AppendFormat(_L("%S"), &aExtra);
	for (TInt c = 0; c < ECapability_Limit; ++c)
		{
		if (aCapSet.HasCapability(TCapability(c)))
			{
			string.Append(' ');
			const char *p=CapabilityNames[c];
			
			while(*p)
				{
				string.Append(*p++);
				}
			}
		}
	if (string.Length()==aExtra.Length())
		{
		string.AppendFormat(_L("None"));
		}
	INFO_PRINTF1(string);
	}


void CCapTestFrameworkStep::GenerateEnvironmentsL(const TCapabilitySet& aCapsNeeded, const TUid& aSidNeeded, const TUid& aVidNeeded, RArray<TTestEnvironment>& aEnvironments)
	{
	for (TUid sid={0}; sid.iUid <= aSidNeeded.iUid; sid.iUid+= aSidNeeded.iUid ? aSidNeeded.iUid : 1) // if no sid is needed then only do check once
		{
		TBool shouldPass=ETrue;

		if (sid!=aSidNeeded)
			{
			shouldPass=EFalse;
			}
		
		for (TUid vid={0}; vid.iUid <= aVidNeeded.iUid; vid.iUid+= aVidNeeded.iUid ? aVidNeeded.iUid : 1)
			{
			if (vid!=aVidNeeded)
				{
				shouldPass=EFalse;
				}
			
			//For each IPC, your test should implement the following:
			//
			// If an IPC is controlled by one capability only, the test code should run 
			//first with the required capability only and second with the complement set
			//of the required capability. If the first test is successful and the second 
			//a failure, the IPC value is proven to be controlled by the required capability
			//only. 
			//
			// If an IPC is controlled by n capabilities, the test code should run first with
			//exact n required capabilities and after with any combination of all subsets of 
			//n-1 capabilities with the complement set. 
			//
			// For example, ABC controls an IPC. The full list of capabilities is 
			//A, B, C, D, E and F. Test code with ABC must be successful, while test code
			//with ABDEF, ACDEF, BCDEF must all fail.
			//
			// So, the number of tests to run to validate a IPC is the combination of n 
			//capabilities, n-1 at a time (i.e. n!/(n-1)! )  plus 1 exact positive test,
			//therefore a total of n+1 tests.
			
			// possibly positive case (depends on Sid and Vid settings)
			aEnvironments.Append(TTestEnvironment(aCapsNeeded, sid, vid, shouldPass));

			if (iThoroughness == EBasicChecks) // will fail when cap set is empty, but there is no TCapabilitySet::NotEmpty exported, write a replacement copy
				{
				// Just add one with no caps and expect to fail
				TCapabilitySet noCaps;
				aEnvironments.Append(TTestEnvironment(noCaps, sid, vid, EFalse));
				}
			else
				{
				// Thorough, cap sets as per comment above 
				for (TInt c = 0; c < ECapability_Limit; ++c)
					{
					if (aCapsNeeded.HasCapability(TCapability(c)))
						{
						// Need to add a new one with (aCapsNeeded / c) | ~aCapsNeeded
						TCapabilitySet caps(aCapsNeeded);
						caps.RemoveCapability(TCapability(c)); // take cap c away
						caps.Union(InvertCapSet(aCapsNeeded)); // add in complement

						aEnvironments.Append(TTestEnvironment(caps, sid, vid, EFalse)); // add to sets, should fail
						}
					}
				} // end thorough cap tests
			
			} // end vid loop
		
		} // end sid loop
	}


TVerdict CCapTestFrameworkStep::doTestStepPostambleL()
	{
	return EPass;
	}

// helper functions
TCapabilitySet CCapTestFrameworkStep::InvertCapSet(const TCapabilitySet& aCapSet)
	{
	TCapabilitySet ret;
	ret.SetEmpty();

	for (TInt c = 0; c < ECapability_Limit; ++c)
		{
		if (c == ECapabilityTCB && iOmitTCBCapInComplementSet)
			{
			continue;
			}
		if (!aCapSet.HasCapability(TCapability(c)))
			{
			ret.AddCapability(TCapability(c));
			}
		}
	return ret;
	}

MCapabilityTestFactory* CCapTestFrameworkStep::SetupFactoryL()
	{
	User::LeaveIfError(iLibrary.Load(iDllName));
	
	TLibraryFunction testFactory=iLibrary.Lookup(1);
	iFactory=reinterpret_cast<MCapabilityTestFactory*>(testFactory());
	
	return iFactory;
	}

// TCapSetTestInfo
TTestEnvironment::TTestEnvironment(const TCapabilitySet& aCaps, TUid aSid, TUid aVid, TBool aExpectPass)
	: iCaps(aCaps), iSid(aSid), iVid(aVid), iExpectPass(aExpectPass)
	{
	}
