/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Example CTestStep derived implementation
*
*/

/**
 @file 
*/
#include <test/testexecutelog.h>
#include "CUsbComponentTest.h"


/**
 Compare personalities of USB from device with expected result in
 ini file. The compared fields are: personal IDs, Description
 and Class control IDs.
 */
TVerdict CUsbComponentTest::CompareUsbPersonalityL()

{
		
	//Get Personality IDs from usb server
	TInt checkStatus;
	RArray<TInt> idArray;
	checkStatus = iUsb.GetPersonalityIds(idArray);
	TEST(checkStatus==KErrNone);
	
	
	TInt expectedPersonalityID;
	TInt classID;
	HBufC* descriptor; 	
	TPtrC theString;
	RArray<TUid> uidArray;	
	TBool isClassSupported;
	TBuf<16> getStringName;
	TInt expectedPersonalityCount;
	TInt expectedClassCount;
	
	
	//get total personality counts
	if(!GetIntFromConfig(ConfigSection(), _L("personalityCount"),expectedPersonalityCount))
	{
		INFO_PRINTF1(_L("Can't get personality count from config file"));
		SetTestStepResult(EFail);
	}
	TEST(idArray.Count()==expectedPersonalityCount);
	
	for (TInt i = 0; i<idArray.Count(); i++)
	{
			_LIT(KIntFormat, "Id%d");
			getStringName.Format(KIntFormat,i+1);
			if(!GetIntFromConfig(ConfigSection(), getStringName,expectedPersonalityID))
			{
				INFO_PRINTF1(_L("Can't get personality id from config file"));
				SetTestStepResult(EFail);
			}
			
			_LIT(KStringFormat, "Description%d");
			getStringName.Format(KStringFormat,i+1);
			if(!GetStringFromConfig(ConfigSection(), getStringName,theString))
			{
				INFO_PRINTF1(_L("Can't get Description from config file"));
				SetTestStepResult(EFail);
			}
			
			
			//compare ID with expected result.	
			TEST(expectedPersonalityID == idArray[i]);
			
			//compare descriptions with expected result
			checkStatus = iUsb.GetDescription(expectedPersonalityID,descriptor);
			TEST(checkStatus==KErrNone);	
			TEST (descriptor->Compare(theString) == 0);	
			delete descriptor;
		
			//compare CC ids with expected result.check class supported
			uidArray.Reset();
			checkStatus = iUsb.GetSupportedClasses(expectedPersonalityID,uidArray);
			TEST(checkStatus==KErrNone);
			
			_LIT(KCountFormat, "classCount%d");
			getStringName.Format(KCountFormat,i+1);
			if(!GetIntFromConfig(ConfigSection(), getStringName,expectedClassCount))
			{
				INFO_PRINTF1(_L("Can't get class count from config file"));
				SetTestStepResult(EFail);	
			}
			TEST(expectedClassCount==uidArray.Count());	
			
			for(TInt j = 0; j<uidArray.Count();j++)
			{
				//get expected class ID from ini file
				_LIT(KClassFormat, "classID%d%d");
				getStringName.Format(KClassFormat,i+1, j+1);
				if(!GetHexFromConfig(ConfigSection(), getStringName, classID))
				{
					INFO_PRINTF1(_L("Can't get class id from config file"));
					SetTestStepResult(EFail);	
				}
				
				//check class supported and actual class id match expected Id
				TEST(uidArray[j] ==TUid::Uid(classID));									
				checkStatus = iUsb.ClassSupported(expectedPersonalityID,TUid::Uid(classID),isClassSupported);
				TEST(checkStatus==KErrNone);	
				TEST(isClassSupported);
				
			}
			
	}
	
	idArray.Close();
	uidArray.Close();

	return TestStepResult();
}

CUsbLoadPersonalityNormal::CUsbLoadPersonalityNormal()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbLoadPersonalityNormal);
	}


/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates USB device read personality list from resource file 
 and validate them when RUsb connect to USB server
 */
TVerdict CUsbLoadPersonalityNormal::doTestStepL()
	{
	return CompareUsbPersonalityL();
	}	

/**
 * Constructor
 */
CUsbLoadPersonalityAbNormal::CUsbLoadPersonalityAbNormal()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbLoadPersonalityAbNormal);
	//Skip connection to USB server during preamble
	iDoNotConnect = ETrue;
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates Loading ersonalities from CC missed
 when RUsb try to connect to USB server and should fail
 */
TVerdict CUsbLoadPersonalityAbNormal::doTestStepL()
	{
		
	TInt res = iUsb.Connect();
	TEST(res==KErrAbort);
	return TestStepResult();
	}	


/**
 * Constructor
 */
CUsbLoadPersonalityMissing::CUsbLoadPersonalityMissing()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbLoadPersonalityMissing);
	
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates Loading personalities from missing file
 when RUsb try to connect to USB server and startshould successful 
 */
TVerdict CUsbLoadPersonalityMissing::doTestStepL()
	{
		TRequestStatus status;
		TUsbServiceState currentState;	
		
		iUsb.Start(status );
		User::WaitForRequest(status);
		TEST(status==KErrNone);
		TEST(iUsb.GetServiceState(currentState)==KErrNone);
		TEST(EUsbServiceStarted==currentState);
		
		iUsb.Stop(status );
		User::WaitForRequest(status);
		TEST(status==KErrNone);
		TEST(iUsb.GetServiceState(currentState)==KErrNone);
		TEST(EUsbServiceIdle==currentState);
		
		TInt expectedPersonalityID;
		if(!GetIntFromConfig(ConfigSection(), _L("Id"), expectedPersonalityID))
		{
			INFO_PRINTF1(_L("Can't get personality id from config file"));
			SetTestStepResult(EFail);	
		}
		
		iUsb.TryStart(expectedPersonalityID,status);
		User::WaitForRequest(status);
		TEST(status==KErrNotSupported);
		HBufC* descriptor; 
		status = iUsb.GetDescription(expectedPersonalityID,descriptor);
		TEST(status==KErrNotSupported);
		delete descriptor;
	
		//Try to get suported class with bad personality ID, it will fail
		RArray<TUid> uidArray;
		status = iUsb.GetSupportedClasses(expectedPersonalityID,uidArray);
		TEST(status==KErrNotSupported);
		
		return TestStepResult();
	
	}		
	/**
 * Constructor
 */
CUsbTestOOM::CUsbTestOOM()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbTestOOM);
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates Loading personalities with OOM condition
 when RUsb connect to USB server
 */
TVerdict CUsbTestOOM::doTestStepL()
	{
	
 	//Frist time RUsb should connect to USB server successfully
	TVerdict result = CompareUsbPersonalityL();
	//set oom, RUsb whould fail connect to USB Server.
	RUsb aUsb;
	TInt count = 1;
	TInt res;
	for(;;)
	{
		iUsb.__DbgFailNext(count);
		res = aUsb.Connect();
		if (res!=KErrNoMemory) 
			break;
		count++;
	}
	
	TEST(res == KErrNone);
	iUsb.__DbgFailNext(0);
	aUsb.Close();
	iUsb.Close();
	// remove oom RUsb should connect to USB server successfully again.
	res = iUsb.Connect();
	TEST(res == KErrNone);
	result = CompareUsbPersonalityL();
	return result;

	}	

/**
 * Constructor
 */
CUsbSwitchPersonalityNormal::CUsbSwitchPersonalityNormal()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbSwitchPersonalityNormal);
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates switching different personality from current one
 when RUsb connect to USB server
 */
TVerdict CUsbSwitchPersonalityNormal::doTestStepL()
	{

	TRequestStatus status;	
	
	//When USB server is started, a defaul personality is set. 
	//Compare personal IDs
	TInt deviceCurrentID ,expectedPersonalityID;
	TInt checkStatus = iUsb.GetCurrentPersonalityId(deviceCurrentID);
	TEST(checkStatus==KErrNone);	
	
	if(!GetIntFromConfig(ConfigSection(), _L("Id2"), expectedPersonalityID))
	{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		SetTestStepResult(EFail);	
	}
	TEST(deviceCurrentID==expectedPersonalityID);
	
	//compare descriptions
	TPtrC theString;
	HBufC* descriptor; 
	checkStatus = iUsb.GetDescription(deviceCurrentID,descriptor);
	TEST(checkStatus==KErrNone);
	
	if(!GetStringFromConfig(ConfigSection(), _L("Description2"), theString))
	{
		INFO_PRINTF1(_L("Can't get Description from config file"));
		SetTestStepResult(EFail);
	}
	TEST(descriptor->Compare(theString) == 0);			
	delete descriptor;
			
	//compare CC ids
	RArray<TUid> uidArray;
	TInt classID;
	checkStatus = iUsb.GetSupportedClasses(deviceCurrentID,uidArray);
	TBuf<16> getStringName;
	
	TInt expectedClassCount;
	if(!GetIntFromConfig(ConfigSection(), _L("classCount2"),expectedClassCount))
	{
		INFO_PRINTF1(_L("Can't get class count from config file"));
		SetTestStepResult(EFail);
	}
	TEST(expectedClassCount==uidArray.Count());	
						
	for(TInt j = 0; j<uidArray.Count();j++)
	{
		//get expected class ID from ini file
		_LIT(KClassFormat, "classID2%d");
		getStringName.Format(KClassFormat,j+1);
		if(!GetHexFromConfig(ConfigSection(), getStringName, classID))
		{
			INFO_PRINTF1(_L("Can't get class id from config file"));
			SetTestStepResult(EFail);
		}
		TEST(uidArray[j] ==TUid::Uid(classID));	
	}
	
	
	//check whether usb service state is in idle before switch	
	TUsbServiceState currentState;	
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState);
	
	//Switch another personality
	if(!GetIntFromConfig(ConfigSection(),_L("Id1"),expectedPersonalityID))
	{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		SetTestStepResult(EFail);
	}
	iUsb.TryStart(expectedPersonalityID,status);
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	
	//Compare personal ID
	checkStatus = iUsb.GetCurrentPersonalityId(deviceCurrentID);
	TEST(checkStatus==KErrNone);	
	TEST(deviceCurrentID==expectedPersonalityID);
	
	//compare descriptions
	checkStatus = iUsb.GetDescription(deviceCurrentID,descriptor);
	TEST(checkStatus==KErrNone);
	if(!GetStringFromConfig(ConfigSection(), _L("Description1"), theString))
	{
		INFO_PRINTF1(_L("Can't get description from config file"));
		SetTestStepResult(EFail);
	}
	TEST(descriptor->Compare(theString) == 0);
	delete descriptor;			
			
	//compare CC ids
	uidArray.Reset();
	checkStatus = iUsb.GetSupportedClasses(deviceCurrentID,uidArray);

	if(!GetIntFromConfig(ConfigSection(), _L("classCount1"),expectedClassCount))
	{
		INFO_PRINTF1(_L("Can't get class count from config file"));
		SetTestStepResult(EFail);
	}
	TEST(expectedClassCount==uidArray.Count());	
	
	for(TInt k = 0; k<uidArray.Count();k++)
	{
		_LIT(KClassFormat2, "classID1%d");
		getStringName.Format(KClassFormat2,k+1);
		if(!GetHexFromConfig(ConfigSection(), getStringName, classID))
		{
			INFO_PRINTF1(_L("Can't get class id from config file"));
			SetTestStepResult(EFail);
		}
		TEST(uidArray[k] ==TUid::Uid(classID));
	}
	
	//try stop service
	iUsb.TryStop(status);
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState);
		
	// do clean job
	uidArray.Close();
	return TestStepResult();

	}	

/**
 * Constructor
 */
CUsbSwitchPersonalityAbNormal::CUsbSwitchPersonalityAbNormal()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbSwitchPersonalityAbNormal);
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates switching different personality from current one to abnormal one
 when RUsb connect to USB server
 */
TVerdict CUsbSwitchPersonalityAbNormal::doTestStepL()
	{
	
	TRequestStatus status;
	TInt expectedPersonalityID;
		
	//trystart personality
	if(!GetIntFromConfig(ConfigSection(),_L("goodId1"),expectedPersonalityID))
	{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		SetTestStepResult(EFail);
	}
	iUsb.TryStart(expectedPersonalityID,status);
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TUsbServiceState currentState;
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted==currentState);
	
	//trystart different personality when in started, it will fail
	if(!GetIntFromConfig(ConfigSection(),_L("goodId2"),expectedPersonalityID))
	{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		SetTestStepResult(EFail);
	}
	iUsb.TryStart(expectedPersonalityID,status);
	User::WaitForRequest(status);
	TEST(status==KErrAbort);
	
	//try to get description with bad personality, it will fail
	if(!GetIntFromConfig(ConfigSection(),_L("badId"),expectedPersonalityID))
	{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		SetTestStepResult(EFail);
	}
	HBufC* descriptor; 
	status = iUsb.GetDescription(expectedPersonalityID,descriptor);
	TEST(status==KErrNotSupported);
	delete descriptor;
	
	//Try to get suported class with bad personality ID, it will fail
	RArray<TUid> uidArray;
	status = iUsb.GetSupportedClasses(expectedPersonalityID,uidArray);
	TEST(status==KErrNotSupported);
	
	//trystart a bad personality ID, it will fail
	iUsb.TryStop(status);
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState);
	
	
	iUsb.TryStart(expectedPersonalityID,status);
	User::WaitForRequest(status);
	TEST(status==KErrNotFound);
	
	uidArray.Close();
	return TestStepResult();
		
	}
/**
 Constructor
 */
CUsbStartStopPersonality1::CUsbStartStopPersonality1()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbStartStopPersonality1);
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates switching personality with CC fail to start from current one
 when RUsb connect to USB server
 */
TVerdict CUsbStartStopPersonality1::doTestStepL()
	{
	
	TInt deviceCurrentID ;
	TRequestStatus status;
	TUsbServiceState currentState;	
	
	//First time set personalty successfully
	if(!GetIntFromConfig(ConfigSection(),_L("IdGood"),deviceCurrentID))
	{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		SetTestStepResult(EFail);
	}
	iUsb.TryStart(deviceCurrentID,status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);

	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted==currentState); 
	
	// Stop service, service state should be idle
	iUsb.TryStop(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);	
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState); 
	
	//Second time set personalty with a CC fail to start, switch will fail
	if(!GetIntFromConfig(ConfigSection(),_L("IdFail"),deviceCurrentID))
	{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		SetTestStepResult(EFail);
	}
	iUsb.TryStart(deviceCurrentID,status );
	User::WaitForRequest(status);
	TEST(status==KErrGeneral);
	
	return TestStepResult();
	}	

/**
 * Constructor
 */
CUsbStartStopPersonality2::CUsbStartStopPersonality2()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbStartStopPersonality2);
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates TryStart/TryStop personality at different USB server's state
 when RUsb connect to USB server
 */
TVerdict CUsbStartStopPersonality2::doTestStepL()
	{
	
	TInt deviceCurrentID ;
	TRequestStatus status;
	TRequestStatus status2;
	TUsbServiceState currentState;
	
	
	// call tryStart when server's state is in idle.
	// the server state is started
	if(!GetIntFromConfig(ConfigSection(),_L("IdGood"),deviceCurrentID))
	{
		INFO_PRINTF1(_L("Can't get personality id from config file"));
		SetTestStepResult(EFail);
	}
	iUsb.TryStart(deviceCurrentID,status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted==currentState); 
	
	// call tryStart when server's state is in started.
	// the server state is started
	iUsb.TryStart(deviceCurrentID,status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted==currentState); 
	
	//call trystart when server's state is in stoping,
	//it has no effect, server state finally is in idle.
	TBool startCompleteFirst = EFalse;	
	iUsb.TryStop(status );
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStopping ==currentState);
	iUsb.TryStart(deviceCurrentID,status2 );	

	
	//The Usb service should stop.
	User::WaitForRequest(status, status2);
	if(status == KRequestPending)
	{
		TEST(status2==KErrServerBusy);
		User::WaitForRequest(status);
		startCompleteFirst = ETrue;	
	}
	else 
	{
		User::WaitForRequest(status2);
		SetTestStepResult(EFail);
	}
	if (startCompleteFirst)
	{
		TEST(iUsb.GetServiceState(currentState)==KErrNone);
		TEST(EUsbServiceIdle ==currentState);
	}
	
	//call trystart when server's state is in starting,
	//it has no effect, server state finally is in started.
	TBool secondStartCompleteFirst = EFalse;	
	iUsb.TryStart(deviceCurrentID,status );
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarting ==currentState);
	iUsb.TryStart(deviceCurrentID,status2 );	
	
	//The Usb service should start.
	User::WaitForRequest(status, status2);
	if(status == KRequestPending)
	{
		TEST(status2==KErrServerBusy);
		User::WaitForRequest(status);
		secondStartCompleteFirst = ETrue;	
	}
	else 
	{
		User::WaitForRequest(status2);
		SetTestStepResult(EFail);
	}
	if (secondStartCompleteFirst)
	{
		TEST(iUsb.GetServiceState(currentState)==KErrNone);
		TEST(EUsbServiceStarted ==currentState);
	}
	
	
	
	//call trystop when server's state is started
	//server state finally is in idle.
	iUsb.TryStop(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState); 
	
	//call trystop when server's state is idle
	//server state finally is in idle.
	iUsb.TryStop(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState); 
	
	//call trystop when server's state is in staring,
	//it has no effect, server's state finally is in started	
	
	iUsb.TryStart(deviceCurrentID,status );
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarting ==currentState);	
	iUsb.TryStop(status2 );

	TBool stopCompleteFirst = EFalse;
	User::WaitForRequest(status, status2);
	
	if(status == KRequestPending)
	{
		TEST(status2==KErrServerBusy);
		User::WaitForRequest(status);
		stopCompleteFirst = ETrue;	
	}
	else 
	{
		User::WaitForRequest(status2);
		SetTestStepResult(EFail);
	}
	if (stopCompleteFirst)
	{
		TEST(iUsb.GetServiceState(currentState)==KErrNone);
		TEST(EUsbServiceStarted ==currentState);
	}
	
	//call trystop when server's state is in stoping,
	//it has no effect, server's state finally is in idle	
	
	iUsb.TryStop(status );
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStopping ==currentState);	
	iUsb.TryStop(status2 );

	TBool secondStopCompleteFirst = EFalse;
	User::WaitForRequest(status, status2);
	
	if(status == KRequestPending)
	{
		TEST(status2==KErrServerBusy);
		User::WaitForRequest(status);
		secondStopCompleteFirst = ETrue;	
	}
	else 
	{
		User::WaitForRequest(status2);
		SetTestStepResult(EFail);
	}
	if (secondStopCompleteFirst)
	{
		TEST(iUsb.GetServiceState(currentState)==KErrNone);
		TEST(EUsbServiceIdle ==currentState);
	}
	
	return TestStepResult();
	}	


/**
 * Constructor
 */

CUsbStartStopPersonality3::CUsbStartStopPersonality3()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KUsbStartStopPersonality3);
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Demonstrates start/stop personality at different USB server's state
 when RUsb connect to USB server
 */
TVerdict CUsbStartStopPersonality3::doTestStepL()
	{

	TRequestStatus status;
	TRequestStatus status2;
	TUsbServiceState currentState;	
	
	
	//call start, when server's state is idle
	//State is started
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState); 
	iUsb.Start(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted==currentState); 
	
	//call started when server's state is started, 
	//State is started
	
	iUsb.Start(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted==currentState); 
	
	//call started when server's state is starting, 
	//State is started
	iUsb.Stop(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	
	iUsb.Start(status );
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarting ==currentState);
	iUsb.Start(status2 );	
	User::WaitForRequest(status);
	User::WaitForRequest(status2);	
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted ==currentState);
	
	//call start when server's state is in stopping,
	//it will starting sevice and finally state is in started
	iUsb.Stop(status );
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStopping ==currentState);
	iUsb.Start(status2 );

	User::WaitForRequest(status);
	User::WaitForRequest(status2);
	
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarted ==currentState);
	
	//call stop when server's state is started, 
	//the state is idle
	iUsb.Stop(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState); 
	
	//call stop when server's state is idle, 
	//the state is idle
	iUsb.Stop(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle==currentState);
	
	//call stop when server's state is in staring,
	//finally state is in idle
	
	iUsb.Start(status );
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStarting ==currentState);
	iUsb.Stop(status2 );	

	User::WaitForRequest(status);
	User::WaitForRequest(status2);	
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle ==currentState);
	
	//call stop when server's state is in stoping,
	//finally state is in idle
	
	iUsb.Start(status );
	User::WaitForRequest(status);
	TEST(status==KErrNone);
	iUsb.Stop(status );
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceStopping ==currentState);
	iUsb.Stop(status2 );	

	User::WaitForRequest(status);
	User::WaitForRequest(status2);	
	TEST(iUsb.GetServiceState(currentState)==KErrNone);
	TEST(EUsbServiceIdle ==currentState);
	
	
	return TestStepResult();
	}	

/**
 * Constructor
 */
CMemAllocationFailure::CMemAllocationFailure()

	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KMemAllocationFailure);
	}

/**
 @return - TVerdict code
 Override of base class pure virtual
 Test memory allocation failure scenario
 */
TVerdict CMemAllocationFailure::doTestStepL()
	{
#ifdef _DEBUG
	TInt personalityId = 2;	// dummy, not used

	__UHEAP_FAILNEXT(1);
	HBufC* hbuf;		
	TEST(iUsb.GetDescription(personalityId, hbuf)==KErrNoMemory);
	__UHEAP_RESET;
	
	__UHEAP_FAILNEXT(1);
	RArray<TInt> personalityIds;		
	TEST(iUsb.GetPersonalityIds(personalityIds)==KErrNoMemory);
	personalityIds.Close();
	__UHEAP_RESET;

	__UHEAP_FAILNEXT(1);
	RArray<TUid>uids;		
	TEST(iUsb.GetSupportedClasses(personalityId, uids)==KErrNoMemory);
	uids.Close();
	
	__UHEAP_RESET;
#else
	SetTestStepResult(EPass);
#endif

	return TestStepResult();
	}	

