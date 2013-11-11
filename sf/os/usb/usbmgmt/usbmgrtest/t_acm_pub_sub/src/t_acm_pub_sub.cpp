/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include <hal.h>
#include <usbman.h>
#include <e32test.h>
#include <e32twin.h>
#include <e32def.h>
#include <c32comm.h>
#include <d32comm.h>
#include <f32file.h>
#include <e32cons.h>
#include <acmserver.h>
#include <AcmInterface.h>
#include <UsbClassUids.h>
#include <CUsbClassControllerPlugIn.h>
#include "CommonFramework.h"
#include <usb/acmconfig.h>

#include <e32std.h>
#include <test/testexecutelog.h>
#include <test/testexecutelogger.h>

#ifndef __WINS__
_LIT(KCommDriverName, "EUSBC");
#endif

RFs gFs;
RUsb usb;
CConsoleBase* tCon;
TPtrC16 KTestResult;

/**
 * Quick cleanup before exit
 */
static void Cleanup()
	{
	if(usb.Handle())
		{
		usb.Close();
		}
	gFs.Close();
	tCon = NULL;
	}

/**
 * Handy function for outputting the buffers length values whilst debugging.
 * @param CConsoleBase* a console for output of information and getting user input
 * @param RBuf& the buffer you want to check
 */
void PrintBuf(CConsoleBase* aCon, RBuf8& aBuf)
	{
	aCon->Printf(_L("aBuf.Length(): %d\t aBuf.MaxLength() %d.\n"), aBuf.Length(), aBuf.MaxLength());
	aCon->Getch();			
	}

/**
 * LoadDrivers
 * Attempts to load drivers for Usb. This only works on the device, the emulator will fail to load
 * correct drivers, so this is skipped for WINS builds
 */
static TInt LoadDrivers(CConsoleBase*  aCon)
	{
	TInt err = KErrNone;
	aCon->Printf(_L("loading LDD\n")); //leave this line outside of preprocessor if to avoid compiler warning.
#ifndef __WINS__
	err = User::LoadLogicalDevice(KCommDriverName);
	if(err != KErrNone && err != KErrAlreadyExists)
		return err;
#endif
	return err == KErrAlreadyExists ? KErrNone : err;
	}

/**
 * Attached a supplied RProperty to the AcmConfig published data
 * @param RProperty& aProp the RProperty to be attached to the published data
 * @return KErrNone on success or standard Symbian error code on failure
 */
void AttachPropertyL(RProperty& aProp)
	{
	User::LeaveIfError(aProp.Attach(KUidSystemCategory, KAcmKey));
	}

/**
 * Returns the number of acm functions currently available
 * @param RProperty aProp, a connected RProperty connected to the P&S data
 * @return TInt the number of functions currently available or a standard
 * Symbian error code.
 */
TInt GetCount(RProperty aProp)
	{
	TPckgBuf<TPublishedAcmConfigs> configBuf;
	TInt ret = aProp.Get(configBuf);
	if(ret == KErrNone)
		 {
		 ret = configBuf().iAcmCount;
		 }	 		 		 		 		 		 		 
	return ret;
	}

/**
 * Checks the function count against the expected value.
 * @param RProperty aProp, a connected RProperty connected to the P&S data
 * @param TInt8 aExpected the number of functions to test against.
 * @return KErrNone if count is expected. KErrGeneral if count differs
 * from the expected value. Otherwise standard Symbian error code  
 */
TInt CheckFunctionCount(RProperty aProp, TInt8 aExpected)
	{
	TInt ret = GetCount(aProp);
  	if(aExpected >=0) //check the expected count
  		{
  		if(ret != aExpected)
  			{
  			KTestResult.Set(_L("Check function count failed. "));
  			ret = KErrGeneral;
  			}
  		else
  			{
  			ret = KErrNone;	
  			}
  		}
  	else //we are checking for an error code 
  		{
  		if(ret == aExpected)
  			ret = KErrNone; //else return the error code
  		}
	return ret;
	}
	
	
//DisplayProperties is not strictly required by this test case but is 
//left in as it shows how to access all the AcmConfig properties in one
//convenient place.
/**
 * Displays a list of available acm fucntions with protocol numbers
 * @param RProperty aProp, a connected RProperty connected to the P&S data
 * @param CConsoleBase* aCon, console where output will be directed.
 */
void DisplayProperties(RProperty aProp, CConsoleBase* aCon)
	{
	TInt err;
	TPckgBuf<TPublishedAcmConfigs> configBuf;
	err = aProp.Get(configBuf);
	if(err != KErrNone)
		{
		aCon->Printf(_L("ERROR: Failed to get the published data as a buffer, code: %d\n\n"), err);
		}
	//purely to prove it can be done, create a TPublishedAcmConfig from the configBuf and use
	//it to display	the properties of the functions
	TPublishedAcmConfigs configs = configBuf();
	
	
	TInt version = configs.iAcmConfigVersion;
	TInt funcCount = configs.iAcmCount;
	
	aCon->Printf(_L("AcmConfig Version: %d\n"), version);
	for(int i=0;i<funcCount;i++)	
		{
		aCon->Printf(_L("Func Num: %d\tProtocol: %d\n"), i, configs.iAcmConfig[i].iProtocol);
		}
	aCon->Printf(_L("Done\n"));	
	}

/**
 * Checks the properties of an Acm function are as expected
 * @param RProperty aProp the attached RProperty to be checked.
 * @param TInt aFuncNum the function to check
 * @param TInt aVersion the version of the config info being used. This is the same for all functions
 * @param TInt aProtocol the expected protocol of this function
 * @return TInt ret. Expected KErrNone or standard Symbian error code.
 */
TInt CheckProperties(RProperty aProp, TInt aFuncNum, TInt aVersion, TUint8 aProtocol)
	{
	TInt ret = KErrNone;
	TPckgBuf<TPublishedAcmConfigs> configBuf;
	TInt err = aProp.Get(configBuf);
	if(err != KErrNone)
		{
		KTestResult.Set(_L("Check properties failed to retrieve the P&S data"));
		ret = KErrGeneral;
		}
	else
		{
		//Check the version number
		if(configBuf().iAcmConfigVersion != aVersion)
			{
			KTestResult.Set(_L("Check properties version check failed."));
			ret = KErrGeneral;
			}
		else
			{
    		if(configBuf().iAcmConfig[aFuncNum].iProtocol != aProtocol)
				{
				KTestResult.Set(_L("Check properties protocol check failed."));
				ret = KErrGeneral;
				}
			}
		}
	return ret;
	}

/**
 * Build a new test ROM configuration where the ACM class controller has 
 * an .ini file which creates multiple ACMs, with different protocols. 
 * Again, check that the P&S key is appropriately set before TryStart(), 
 * while started, and after TryStop()
 */	
TInt TestCase1L()
	{
	RArray<TInt> availablePersonalities;
	TRequestStatus status;
	RProperty prop;
	TInt err = KErrNone;
		
	err = usb.Connect();
	if(err)
		{
		KTestResult.Set(_L("RUsb::Connect failed."));
		return err;
		}
		
	//check to see if the P&S data exists already
	//It should do as usbman should have been started during system startup, 
	//but no functions should exist until RUsb::Start | RUsb::TryStart calls
	//are made.
	AttachPropertyL(prop);
	err = CheckFunctionCount(prop, 0);
	if(err)
		{
		KTestResult.Set(_L("Expected 0 function to exist at this time."));
		return GetCount(prop); //Error return the number of functions found
		}
	
	err = usb.GetPersonalityIds(availablePersonalities);
	if(err) //couldn't retrieve the personalities. 
		{
		KTestResult.Set(_L("Failed to retrieve the supported personalities for this device."));
		return err;
		}
	if(availablePersonalities.Count() == 0) //no personalities 
		{
		KTestResult.Set(_L("GetPersonalities returned 0 supported personalities."));
		return KErrGeneral;
		}
	usb.TryStart(availablePersonalities[0], status); //personality 0 should be the default personality
	User::WaitForRequest(status);
	if(status.Int())
		{
		KTestResult.Set(_L("RUsb::TryStart failed."));
		return status.Int(); //USB failed to start the personality
		}
	
	//now that usb has started we should be able to check the P&S data.
	//Our NumberOfAcmFunctions.ini defines 3 functions so we should 
	//have three funtions now
	DisplayProperties(prop, tCon);
	err = CheckFunctionCount(prop, 3);
	if(err)
		{
		KTestResult.Set(_L("Incorrect number of functions found."));
		return GetCount(prop);//Error return the number of functions found
		}
	
	//We've gotten here so presumably all is going well, so check the version and 
	//protcol numbers on the functions created.
	//NumberOfAcmFunctions.ini defines functions with protocols 1, 255 & 127. 
	TInt version = 1; //simply for clarity
	
	if(err)
		{
		KTestResult.Set(_L("Failed to attach property for CheckProperties."));
		return err;
		}
	
	err = CheckProperties(prop, 0, version, 1);
	if(err)
		{
		KTestResult.Set(_L("Properties check failed for function 1."));
		return err;
		}
	
	err = CheckProperties(prop, 1, version, 255);
	if(err)
		{
		KTestResult.Set(_L("Properties check failed for function 2."));
		return err;
		}
	
	err = CheckProperties(prop, 2, version, 127);
	if(err)
		{
		KTestResult.Set(_L("Properties check failed for function 3."));
		return err;
		}
	
	//Assuming we're still here everything is correct, so shutdown Usbman and check 
	//the P&S data is destroyed 
	usb.TryStop(status);
	User::WaitForRequest(status);
	if(status.Int())
		{
		KTestResult.Set(_L("RUsb::TryStop failed."));
		return status.Int();
		}
	
	//Check to see if the P&S data was destroyed
	err = CheckFunctionCount(prop, 0);
	if(err)
		{
		KTestResult.Set(_L("P&S data was not destroyed by stopping usb."));
		return GetCount(prop); //Error return the number of functions found
		}
		
	//We got here so "All is well!"
	KTestResult.Set(_L("Test Passed."));
	return KErrNone;
	}

void doExampleL(CConsoleBase *aCon)
    {
 
	TInt err;
	tCon = aCon;  
	err = LoadDrivers(aCon);
    if(err != KErrNone)
    {
    	aCon->Printf(_L("Failed to load device drivers\n"));
    	Cleanup();
    }
    
    //Connect to the file server
    User::LeaveIfError(gFs.Connect());
    TInt result = KErrNone;
    TRAPD(error, result = TestCase1L());
    if(error)
    	{
    	KTestResult.Set(_L("TestCase1L Leave occurred."));
    	result = error;
    	}
	aCon->Printf(_L("TestCase 1 results: %S Result: %d\n"), &KTestResult, result);
	Cleanup();
	}


