/*
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
* This contains CTestPlatSecSetCapabilities class functions implementation.
* This class sets the capabilities of the executables and libraries.
*
*/


#include "testplatsecsetcapabilities.h"

//TEF include
#include <test/testexecutelog.h>

/*@{*/
_LIT(KExecutableName,		"executable");
_LIT(KCapabalities,			"capabilities");
_LIT(KDestinationPath,		"destinationPath");
/*@}*/

// Test step name
_LIT(KSetCapabilitiesStep,	"SetCapabilities");

/**
 Destructor
*/
CTestPlatSecSetCapabilities::~CTestPlatSecSetCapabilities()
	{
	}

/** 
 Constructor
*/
CTestPlatSecSetCapabilities::CTestPlatSecSetCapabilities()
	{
	SetTestStepName(KSetCapabilitiesStep);
	}

/**
 The implementation of the CTestStepPlatTest::doTestStepL() pure virutal
 function. Sets the defined capabilities to the executables. It reads 
 the executable name, destination executable name and its capabilities 
 from the ini file.
 @return TVerdict - result of the test step
 @leave - system wide error codes
*/
TVerdict CTestPlatSecSetCapabilities::doTestStepL()
	{
	INFO_PRINTF1(_L("Setting the capabilities of the executable."));

	TPtrC ServerName(GetServerName());
	INFO_PRINTF1(_L("Server Name"));
	INFO_PRINTF2(_L("Server Name %S"), &ServerName);
	
	//read the executable name from the ini file
	TPtrC	executableName;
	if(!GetStringFromConfig(ConfigSection(), KExecutableName, executableName))
		{
		ERR_PRINTF1(_L("Executable/Server name not provided in INI file"));
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF2(_L("Executable name read from the ini file is  %S"),&executableName); 
		//read capabilities to be set
		TPtrC capabilities;
		if(!GetStringFromConfig(ConfigSection(), KCapabalities, capabilities))
			{
			ERR_PRINTF1(_L("Capabilities to be set are not specified in the INI file")); 
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("New capabilities for the executable :  %S"),&capabilities); 
			//read the destination executable name including path
			TPtrC destination;
			if(!GetStringFromConfig(ConfigSection(), KDestinationPath, destination))
				{
				ERR_PRINTF1(_L("Destination  not specified in the INI file")); 
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF2(_L("Destination path specified is  :  %S"),&destination); 
				//set the capabilities
				SetCapabilitiesL(executableName, capabilities,destination);
				}
			}
		}
	return TestStepResult();
	}

/**
 Sets the capabilites for a given executable. Uses SetCap.exe
 @param aExecutable - name of the executable
 @param aCapabilities - capabilities to be set for the executable
 @param aDestination - name of the output executable
 @leave - system wide error codes
 @return None
*/
void CTestPlatSecSetCapabilities::SetCapabilitiesL(const TDesC& aExecutable, const TDesC& aCapabilities,const TDesC& aDestination)
	{
	//create the commandline arguments
	TBuf<KMaxTestExecuteCommandLength> buf(aCapabilities);
	TUint hexCapability = CreateHexCapabilities(buf);

	INFO_PRINTF2( _L("Capality Hex value is %X"),hexCapability);
	TBuf<KMaxTestExecuteCommandLength> commandLine(aExecutable);
	commandLine.Append(_L(" "));
	commandLine.AppendNumUC(hexCapability,EHex);
	commandLine.Append(_L(" "));
	commandLine.Append(aDestination);

	INFO_PRINTF2( _L("command line arguments for setcap  is  %S"),&commandLine);
	
	//instantiate Setcap.exe as a new process with the executable and capabilities as command line arguments
	RProcess process;
	CleanupClosePushL(process);
	TInt err=KErrNone;
	User::LeaveIfError(err = process.Create(_L("setcap.exe"),commandLine));
	if(err!=KErrNone)
		{
		ERR_PRINTF2(_L("Creating the process failed with error : %D"),err);
		SetTestStepResult(EFail);
		}
	else
		{
		TRAPD(error,process.Resume();)
		if(error!=KErrNone)
			{
			ERR_PRINTF2(_L("Creating the process failed with error : %D"),err);
			SetTestStepResult(EFail);
			}
		}
	CleanupStack::PopAndDestroy(1);
	}

/**
 Creates the hex values to be passed to SetCap.exe.
 @param aCapabilities - capabilites for which hex values to be created
 @return TInt - integer value after manipulating all capabilities
 @leave - none
*/
TInt  CTestPlatSecSetCapabilities::CreateHexCapabilities(TDesC& aCapabilities )
	{
	TInt capability=0;
	TInt capCountCheck=0;
	TLex parser(aCapabilities);
	
	//create the capability hex value
	while(!parser.Eos())
		{
		TPtrC token(parser.NextToken());
		INFO_PRINTF2( _L("Adding capability %S"),&token);

		TCapability capabilityValue;
		
		TInt err = GetCapability(token, capabilityValue);
		if (err != KErrNone)
			{
			WARN_PRINTF1(_L("Unrecognised capability, rejected"));
			}
		else
			{
			capCountCheck++;
			if(capabilityValue == ECapability_None)
				{
				capability = ECapability_None;
				break;
				}
			else
				{
				capability = capability | (1<<capabilityValue);
				}
			}
		if(capCountCheck == 0)
			{
			WARN_PRINTF1(_L("No capability given is valid, capability set to None"));
			capability = ECapability_None;
			}
		}
	return capability;
	}
	
/**
 Get the capability id (enum value) for the capability name 
 given as string.
 @param aCapability - Capability name 
 @param aCapabilityValue - value for the capability name
 @return TInt - error codes
 @leave - None
*/
TInt CTestPlatSecSetCapabilities::GetCapability(TPtrC aCapability, TCapability& aCapabilityValue)
	{
	INFO_PRINTF2(_L("The capability get is %S"), &aCapability);
	TInt	ret=KErrNone;

	TBuf<KMaxTestExecuteCommandLength>	capCaseValue(aCapability);
	capCaseValue.LowerCase();

	if(!capCaseValue.Compare(_L("tcb" )))
		{
		aCapabilityValue=ECapabilityTCB;
		}
	else if(!capCaseValue.Compare(_L("commdd" )))
		{
		aCapabilityValue=ECapabilityCommDD;
		}
	else if(!capCaseValue.Compare(_L("powermgmt" )))
		{
		aCapabilityValue=ECapabilityPowerMgmt;
		}
	else if(!capCaseValue.Compare(_L("multimediadd" )))
		{
		aCapabilityValue=ECapabilityMultimediaDD;
		}
	else if(!capCaseValue.Compare(_L("readdevicedata" )))
		{
		aCapabilityValue=ECapabilityReadDeviceData;
		}
	else if(!capCaseValue.Compare(_L("writedevicedata" )))
		{
		aCapabilityValue=ECapabilityWriteDeviceData;
		}
	else if(!capCaseValue.Compare(_L("drm" )))
		{
		aCapabilityValue=ECapabilityDRM;
		}
	else if(!capCaseValue.Compare(_L("trustedui" )))
		{
		aCapabilityValue=ECapabilityTrustedUI;
		}
	else if(!capCaseValue.Compare(_L("protserv" )))
		{
		aCapabilityValue=ECapabilityProtServ;
		}
	else if(!capCaseValue.Compare(_L("diskadmin" )))
		{
		aCapabilityValue=ECapabilityDiskAdmin;
		}
	else if(!capCaseValue.Compare(_L("networkcontrol" )))
		{
		aCapabilityValue=ECapabilityNetworkControl;
		}
	else if(!capCaseValue.Compare(_L("allfiles" )))
		{
		aCapabilityValue=ECapabilityAllFiles;
		}
	else if(!capCaseValue.Compare(_L("swevent" )))
		{
		aCapabilityValue=ECapabilitySwEvent;
		}
	else if(!capCaseValue.Compare(_L("networkservices" )))
		{
		aCapabilityValue=ECapabilityNetworkServices;
		}
	else if(!capCaseValue.Compare(_L("localservices" )))
		{
		aCapabilityValue=ECapabilityLocalServices;
		}
	else if(!capCaseValue.Compare(_L("readuserdata" )))
		{
		aCapabilityValue=ECapabilityReadUserData;
		}
	else if(!capCaseValue.Compare(_L("writeuserdata")))
		{
		aCapabilityValue=ECapabilityWriteUserData;
		}
	else if(!capCaseValue.Compare(_L("location")))
		{
		aCapabilityValue=ECapabilityLocation;
		}
	else if(!capCaseValue.Compare(_L("surroundingsdd")))
		{
		aCapabilityValue=ECapabilitySurroundingsDD;
		}
	else if(!capCaseValue.Compare(_L("userenvironment")))
		{
		aCapabilityValue=ECapabilityUserEnvironment;
		} 		
	else if(!capCaseValue.Compare(_L("none")))
		{
		aCapabilityValue=ECapability_None;
		}
	else
		{
		INFO_PRINTF2(_L("Unrecognised capability %S, will be rejected"), &capCaseValue);
		ret=KErrNotFound;
		}

	return ret;
	}
