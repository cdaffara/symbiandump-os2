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
* @file
* This is the test class for TEF (PREQ1521).
*
*/



#include "te_regblocktestwrapper.h"

/*@{*/
/// Enumerations
#define TESTA(a) \
if(a) {INFO_PRINTF1(_L("Test Passed!"));} \
else {SetBlockResult(EFail); ERR_PRINTF1(_L("Test Failed!"));} \

#define Macro(s) _LIT(K##s, #s)
Macro(WriteHexValue);
Macro(WriteIntValue);
Macro(WriteBoolValue);
Macro(WriteStringValue);
Macro(TestNullController);
Macro(WriteSharedData);
Macro(ReadSharedData);
Macro(SetDataObject);
//Macro(SerialWriter);

_LIT(KSharedName, "SharedName");
_LIT(KSharedValue, "Shard data value");

_LIT(KFail,										"EFail");
_LIT(KPass,										"EPass");

/// Commands
_LIT(KSetBlockResult,							"SetBlockResult");
_LIT(KSetError,									"SetError");
_LIT(KDummyFunction,							"DummyFunction");
_LIT(KGetDataObject,							"GetDataObject");
_LIT(KGetDataWrapper,							"GetDataWrapper");
_LIT(KPanickingFunction,						"PanickingFunction");
_LIT(KLeaveFunction,							"LeaveFunction");
_LIT(KModify,									"Modify");
_LIT(KModifyByName,								"ModifyByName");
_LIT(KNew,										"New");
_LIT(KStartTimer,								"StartTimer");
_LIT(KCheckTimer,								"CheckTimer");
_LIT(KIncreaseObjectValue,						"IncreaseObjectValue");
_LIT(KAddActiveObjectToScheduler,				"AddActiveObjectToScheduler");
_LIT(KActivateActiveObject,						"ActivateActiveObject");
_LIT(KCancelActiveObject,						"CancelActiveObject");
_LIT(KCheckActiveObjectWorkIsComplete,			"CheckActiveObjectWorkIsComplete");
_LIT(KCallMeFirstMkDir,							"CallMeFirstMkDir");
_LIT(KCallMeSecondRmDir,						"CallMeSecondRmDir");


/// command parameters
_LIT(KMaxIntervalSec,							"max_interval_sec");
_LIT(KMinIntervalSec,							"min_interval_sec");
_LIT(KPanicNumber,								"panic_number");
_LIT(KLeaveNumber,								"leave_number");
_LIT(KPanicString,								"panic_string");
_LIT(KNullExpected,								"null_expected");
_LIT(KObjectValue,								"object_value");
_LIT(KErrorCode,								"error_code");
_LIT(KBlockResult,								"block_result");
_LIT(KMicrosecondsOfWork,						"microseconds_of_work");
_LIT(KMustBeComplete,							"must_be_complete");
_LIT(KDirectoryPath,							"directory_path");

/*@}*/

CTe_RegBlockTestWrapper::CTe_RegBlockTestWrapper()
:	iActiveCallback(NULL)
,	iObject(NULL)
,	iActiveObject(NULL)
	{
	}

CTe_RegBlockTestWrapper::~CTe_RegBlockTestWrapper()
	{
	delete iActiveCallback;
	iActiveCallback = NULL;
	
	delete iActiveObject;
	iActiveObject = NULL;
	
	delete iObject;
	iObject = NULL;
	}

CTe_RegBlockTestWrapper* CTe_RegBlockTestWrapper::NewL()
	{
	CTe_RegBlockTestWrapper*	ret = new (ELeave) CTe_RegBlockTestWrapper();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;	
	}

void CTe_RegBlockTestWrapper::ConstructL()
	{
	iActiveCallback = CActiveCallback::NewL(*this);
	iObject			= new (ELeave) TInt;
	}
		
TBool CTe_RegBlockTestWrapper::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
	{
	TBool ret = ETrue;
	
	// Print out the parameters for debugging
	INFO_PRINTF2( _L("aCommand = %S"), &aCommand );
	INFO_PRINTF2( _L("aSection = %S"), &aSection );
	INFO_PRINTF2( _L("aAsyncErrorIndex = %D"), aAsyncErrorIndex );
	
	if(KModify() == aCommand)
		{
		DoCmdModify(aSection);
		}
	else if(KModifyByName() == aCommand)
		{
		DoCmdModifyByName(aSection);
		}
	else if(KNew() == aCommand)
		{
		DoCmdNewL(aSection);
		}
	else if(KSetBlockResult() == aCommand)
		{
		DoCmdSetBlockResult(aSection);
		}
	else if(KSetError() == aCommand)
		{
		DoCmdSetError(aSection);
		}
	else if(KDummyFunction() == aCommand)
		{
		DoCmdDummyFunction();
		}
	else if(KGetDataObject() == aCommand)
		{
		DoCmdGetDataObject(aSection);
		}
	else if(KGetDataWrapper() == aCommand)
		{
		DoCmdGetDataWrapper(aSection);
		}
	else if(KPanickingFunction() == aCommand)
		{
		DoCmdPanickingFunction(aSection);
		}
	else if(KLeaveFunction() == aCommand)
		{
		DoCmdLeaveFunction(aSection);
		}
	else if(KStartTimer() == aCommand)
		{
		DoCmdStartTimer();
		}
	else if(KCheckTimer() == aCommand)
		{
		DoCmdCheckTimer(aSection);
		}
	else if(KIncreaseObjectValue() == aCommand)
		{
		DoCmdIncreaseObjectValue();
		}
	else if(KAddActiveObjectToScheduler() == aCommand)
		{
		DoCmdAddActiveObjectToScheduler();
		}
	else if(KActivateActiveObject() == aCommand)
		{
		DoCmdActivateActiveObject(aSection, aAsyncErrorIndex);
		}
	else if(KCancelActiveObject() == aCommand)
		{
		DoCmdCancelActiveObject();
		}
	else if(KCheckActiveObjectWorkIsComplete() == aCommand)
		{
		DoCmdCheckActiveObjectWorkIsComplete(aSection);
		}
	else if(KCallMeFirstMkDir() == aCommand)
		{
		DoCmdCallMeFirstMkDir(aSection);
		}
	else if(KCallMeSecondRmDir() == aCommand)
		{
		DoCmdCallMeSecondRmDir(aSection);
		}
	else if (KWriteIntValue() == aCommand)
		{
		DoCmdWriteIntToConfig(aSection);
		}
	
	else if (KWriteBoolValue() == aCommand)
		{
		DoCmdWriteBoolToConfig(aSection);
		}
	
	else if (KWriteHexValue() == aCommand)
		{
		DoCmdWriteHexToConfig(aSection);
		}
	
	else if (KWriteStringValue() == aCommand)
		{
		DoCmdWriteStringToConfig(aSection);
		}
	else if (KTestNullController() == aCommand)
		{
		DoCmdTestNullController(aSection);
		}
	else if (aCommand == KWriteSharedData)
		{
		DoCmdWriteSharedData();
		}
	else if (aCommand == KReadSharedData)
		{
		DoCmdReadSharedData();
		}
	else if (aCommand == KSetDataObject)
		{
		DoCmdSetDataObject(aSection);
		}
	else
		{
		ret = EFalse;
		}
		
	return ret;
	}

/**
Purpose:

Ini file options:
	iniData - The data from the ini file at the section provided.

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdModify(const TDesC& aSection)
	{
	TInt objectValue = 0;
	if (!GetIntFromConfig(aSection, KObjectValue(), objectValue))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KObjectValue());
		SetBlockResult(EFail);
		}
	else
		{
		*iObject = objectValue;
		}
	}



/**
Purpose: calls SetBlockResult() to set block result

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdSetBlockResult(const TDesC& aSection)
	{
	TVerdict verdict = EFail;
	if (!GetVerdictFromConfig(aSection, KBlockResult(), verdict))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KBlockResult());
		SetBlockResult(EFail);
		}
	
	WARN_PRINTF2(_L("Setting test block result: %d"), verdict);
	SetBlockResult(verdict);
	TESTA(BlockResult() == verdict);
	}



/**
Purpose: remembers current time, so that it can be used for comparison in a subsequent CheckTime command call

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdStartTimer()
	{
	iTimerStart.HomeTime();
	}



/**
Purpose: remembers current time, so compares difference in time between the call and the previous call to Start Timer command

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdCheckTimer(const TDesC& aSection)
	{
	TInt maxIntervalSec = 0;
	if (!GetIntFromConfig(aSection, KMaxIntervalSec(), maxIntervalSec))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KMaxIntervalSec());
		SetBlockResult(EFail);
		return;
		}

	TInt minIntervalSec = 0;
	if (!GetIntFromConfig(aSection, KMinIntervalSec(), minIntervalSec))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KMinIntervalSec());
		SetBlockResult(EFail);
		return;
		}


	TTime now;
	TTimeIntervalSeconds secs;
	now.HomeTime();
	
	_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3  %-B %:0%J%:1%T%:2%S %+B");
	TBuf<50> dateString2;
	iTimerStart.FormatL(dateString2, KDateString);
	INFO_PRINTF2(_L("Timer start time: %S"), &dateString2);

	TBuf<50> dateString;
	now.FormatL(dateString, KDateString);
	INFO_PRINTF2(_L("Timer check time: %S"), &dateString);

	
	TInt err = now.SecondsFrom(iTimerStart, secs);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Call to TTime::SecondsFrom returned error: %d"), err);
		SetBlockResult(EFail);
		return;
		}
	
	
	if (secs > TTimeIntervalSeconds(maxIntervalSec))
		{
		ERR_PRINTF3(_L("The time interval is longer than expected. expected=%d, actual=%d"), maxIntervalSec, secs.Int());
		SetBlockResult(EFail);
		}

	if (secs < TTimeIntervalSeconds(minIntervalSec))
		{
		ERR_PRINTF3(_L("The time interval is shorter than expected. expected=%d, actual=%d"), minIntervalSec, secs.Int());
		SetBlockResult(EFail);
		}
	}



/**
Purpose: increases a counter value, that can be checked using GetDataObject command

@internalComponent
*/
void CTe_RegBlockTestWrapper::DoCmdIncreaseObjectValue()
	{
	(*iObject)++;
	INFO_PRINTF2(_L("Counter value: %d"), *iObject);
	}



/**
Purpose: A function that sets an error code (given in the ini-file).

@internalComponent
*/
void CTe_RegBlockTestWrapper::DoCmdSetError(const TDesC& aSection)
	{
	TInt errorCode = 0;
	if (!GetIntFromConfig(aSection, KErrorCode(), errorCode))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KErrorCode());
		SetBlockResult(EFail);
		}
	else
		{
		WARN_PRINTF2(_L("Setting an error: %d"), errorCode);
		SetError(errorCode);
		}
	}


/**
Purpose: An implementation of a dummy functions that doesn nothing

@internalComponent
*/
void CTe_RegBlockTestWrapper::DoCmdDummyFunction()
	{
	}



/**
Purpose: A function that throws panic

@internalComponent
*/
void CTe_RegBlockTestWrapper::DoCmdPanickingFunction(const TDesC& aSection)
	{
	TInt panicNumber = 0;
	if (!GetIntFromConfig(aSection, KPanicNumber(), panicNumber))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KPanicNumber());
		SetBlockResult(EFail);
		}
	else
		{
		TPtrC panicString;
		if (!GetStringFromConfig(aSection, KPanicString(), panicString))
			{
			ERR_PRINTF2(_L("No parameter %S"), &KPanicString());
			SetBlockResult(EFail);
			}
		WARN_PRINTF3(_L("Calling panic: %S %d"), &panicString, panicNumber);
		User::Panic(panicString, panicNumber);
		}
	}

/**
Purpose: A function that Leaves

@internalComponent
*/
void CTe_RegBlockTestWrapper::DoCmdLeaveFunction(const TDesC& aSection)
	{
	TInt LeaveNumber = 0;
	if (!GetIntFromConfig(aSection, KLeaveNumber(), LeaveNumber))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KLeaveNumber());
		SetBlockResult(EFail);
		}
	else
		{	
		WARN_PRINTF2(_L("Calling panic:%d"),LeaveNumber);
		User::Leave(LeaveNumber);
		}
	}

/**
Purpose: A function that creates a directory

@internalComponent
*/
void CTe_RegBlockTestWrapper::DoCmdCallMeFirstMkDir(const TDesC& aSection)
	{
	TPtrC dirPath;
	if (!GetStringFromConfig(aSection, KDirectoryPath(), dirPath))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KDirectoryPath());
		SetBlockResult(EFail);
		}
	INFO_PRINTF2(_L("MkDir %S"), &dirPath);
	RFs fs;
	fs.Connect();
	TInt err = fs.MkDir(dirPath);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to create a directory: %d"), err);
		SetBlockResult(EFail);
		}
	fs.Close();
	}
	

/**
Purpose: A function that removes a directory

@internalComponent
*/
void CTe_RegBlockTestWrapper::DoCmdCallMeSecondRmDir(const TDesC& aSection)
	{
	TPtrC dirPath;
	if (!GetStringFromConfig(aSection, KDirectoryPath(), dirPath))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KDirectoryPath());
		SetBlockResult(EFail);
		}
	INFO_PRINTF2(_L("RmDir %S"), &dirPath);
	RFs fs;
	fs.Connect();
	TInt err = fs.RmDir(dirPath);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Failed to remove a directory: %d. The function was called before CallMeFirstMkDir?"), err);
		SetBlockResult(EFail);
		}
	fs.Close();
	}
	
/**
Purpose:

Ini file options:
	iniData - The data from the ini file at the section provided.

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdModifyByName(const TDesC& aSection)
	{
	TInt objectValue = 0;
	if (!GetIntFromConfig(aSection, KObjectValue(), objectValue))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KObjectValue());
		SetBlockResult(EFail);
		}
	else
		{
		// call GetObject() and check that the wrapped object is found
		TInt* wrappedObject = NULL;
		TRAPD(err, wrappedObject = static_cast<TInt*>(GetDataObjectL(aSection)));
		if (err != KErrNone)
			{
			ERR_PRINTF2(_L("GetDataObjectL() error: %d"), err);
			SetError(err);
			return;
			}
			
		if (wrappedObject == NULL)
			{
			ERR_PRINTF2(_L("Wrapped object is not found using GetDataObject(). section=%S"), &aSection);
			SetBlockResult(EFail);
			}
		else
			{
			*wrappedObject = objectValue;
			}
		}
	}


/**
Purpose: To create a new object of the CTEFTest type through the API.

Ini file options:
	iniData - The data from the ini file at the section provided.

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdNewL(const TDesC& aSection)
	{
	TInt objectValue = 0;
	if (!GetIntFromConfig(aSection, KObjectValue(), objectValue))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KObjectValue());
		SetBlockResult(EFail);
		}
	else
		{
		delete iObject;
		iObject = new (ELeave) TInt(objectValue);
		}
	}


/**
Purpose: To test that an data wrapper object exists in the dictionary.

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdGetDataWrapper(const TDesC& aSection)
	{
	// check if null is expected
	TBool nullExpected = EFalse;
	GetBoolFromConfig(aSection, KNullExpected(), nullExpected);

	// call GetDataWrapper() and check that the wrapper is found
	CDataWrapper* wrapper = NULL;
	TRAPD(err, wrapper = GetDataWrapperL(aSection));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GetDataWrapperL() error: %d"), err);
		SetError(err);
		}
	
	// check if is null if is expetced to be null and vice versa
	if (wrapper == NULL && !nullExpected)
		{
		ERR_PRINTF1(_L("Wrapper is not expected to be null!"));
		SetBlockResult(EFail);
		}
	else if (wrapper != NULL && nullExpected)
		{
		ERR_PRINTF1(_L("Wrapper is expected to be null!"));
		SetBlockResult(EFail);
		}
	}

/**
Purpose: To test that set object value is correct and also check the object's value.

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdSetDataObject(const TTEFSectionName& aSection)
	{
	TInt* object = new TInt(10);
	TRAPD(err, SetDataObjectL(aSection, object));
	TESTA(err==KErrNone);
	object = NULL;
	TRAP(err, object=(TInt*)GetDataObjectL(aSection));
	TESTA(*object==10);
	}


/**
Purpose: To test that an wrapped object exists in the dictionary. And optionally to check the object's value.

@internalComponent
@param  aSection Current ini file command section
*/
void CTe_RegBlockTestWrapper::DoCmdGetDataObject(const TDesC& aSection)
	{
	// check if null is expected
	TBool nullExpected = EFalse;
	GetBoolFromConfig(aSection, KNullExpected(), nullExpected);

	// call GetDataObject() and check that the wrapped object is found
	TInt* wrappedObject = NULL;
	TRAPD(err, wrappedObject = static_cast<TInt*>(GetDataObjectL(aSection)));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GetDataObjectL() error: %d"), err);
		SetError(err);
		}
	
	// check if is null if is expetced to be null and vice versa
	if (wrappedObject == NULL && !nullExpected)
		{
		ERR_PRINTF1(_L("Wrapped object is not expected to be null!"));
		SetBlockResult(EFail);
		}
	else if (wrappedObject != NULL)
		{
		if (nullExpected)
			{
			ERR_PRINTF1(_L("Wrapped object is expected to be null!"));
			SetBlockResult(EFail);
			}
		else
			{
			// check expected value
			TInt objectValue = 0;
			if (GetIntFromConfig(aSection, KObjectValue(), objectValue))
				{
				if (*wrappedObject != objectValue)
					{
					ERR_PRINTF3(_L("Wrapped object's value is not as expected. expected=%d, actual=%d"), objectValue, *wrappedObject);
					SetBlockResult(EFail);
					}
				}
			}
		}
	}


/**
Create an active object and add it to the current active scheduler
*/
void CTe_RegBlockTestWrapper::DoCmdAddActiveObjectToScheduler()
	{
	if (iActiveObject != NULL)
		{
		delete iActiveObject;
		iActiveObject = NULL;
		}

	TRAPD(err, iActiveObject = new (ELeave) RTimer);
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("RTimer creation failed with error: %d"), err);
		SetBlockResult(EFail);
		return;
		}
		
	iActiveObject->CreateLocal();
	}

/**
Check that the active object is already added to the active scheduler
*/
void CTe_RegBlockTestWrapper::DoCmdActivateActiveObject(const TDesC& aSection, const TInt aAsyncErrorIndex)
	{
	TInt microsecondsOfWork = 0;
	if (!GetIntFromConfig(aSection, KMicrosecondsOfWork(), microsecondsOfWork))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KMicrosecondsOfWork());
		SetBlockResult(EFail);
		}

	iActiveObject->After(iActiveCallback->iStatus, microsecondsOfWork);
	iActiveCallback->Activate(aAsyncErrorIndex);
	IncOutstanding();
	}


/**
Cancel active object
*/
void CTe_RegBlockTestWrapper::DoCmdCancelActiveObject()
	{
	iActiveObject->Cancel();
	}


/**
Check that the active object work is finished (or not finished depending on the expectation)
*/
void CTe_RegBlockTestWrapper::DoCmdCheckActiveObjectWorkIsComplete(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("CTe_RegBlockTestWrapper::DoCmdCheckActiveObjectWorkIsComplete"));
	TBool mustBeComplete = EFalse;
	if (!GetBoolFromConfig(aSection, KMustBeComplete(), mustBeComplete))
		{
		ERR_PRINTF2(_L("No parameter %S"), &KMustBeComplete());
		SetBlockResult(EFail);
		}

	if (Outstanding() == mustBeComplete)
		{
		ERR_PRINTF3(_L("Outstanding status is not as expected. expected=%d, actual=%d"), mustBeComplete, !Outstanding());
		SetBlockResult(EFail);
		}
	}


void CTe_RegBlockTestWrapper::DoCmdWriteIntToConfig(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("Test WriteIntToConfig..."));
	TInt intValue = 36;
	TInt getIntValue = 0;
	GetIntFromConfig(aSection, KWriteIntValue, getIntValue);
	TESTA(intValue!=getIntValue);
	WriteIntToConfig(aSection, KWriteIntValue, intValue);
	intValue = 0;
	GetIntFromConfig(aSection, KWriteIntValue, intValue);
	TESTA(intValue==36);
	WriteIntToConfig(aSection, KWriteIntValue, getIntValue);
	}

void CTe_RegBlockTestWrapper::DoCmdWriteBoolToConfig(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("Test WriteBoolToConfig..."));
	TBool boolValue = EFalse;
	TBool getBoolValue = EFalse;
	GetBoolFromConfig(aSection, KWriteBoolValue, getBoolValue);
	TESTA(boolValue!=getBoolValue);
	WriteBoolToConfig(aSection, KWriteBoolValue, boolValue);
	boolValue = ETrue;
	GetBoolFromConfig(aSection, KWriteBoolValue, boolValue);
	TESTA(!boolValue);
	WriteBoolToConfig(aSection, KWriteBoolValue, getBoolValue);
	}

void CTe_RegBlockTestWrapper::DoCmdWriteHexToConfig(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("Test WriteHexToConfig..."));
	TInt hexValue = 0x00AB;
	TInt getHexValue = 0;
	GetHexFromConfig(aSection, KWriteHexValue, getHexValue);
	TESTA(hexValue!=getHexValue);
	WriteHexToConfig(aSection, KWriteHexValue, hexValue);
	hexValue = 0;
	GetHexFromConfig(aSection, KWriteHexValue, hexValue);
	TESTA(hexValue==0xAB);
	WriteHexToConfig(aSection, KWriteHexValue, getHexValue);
	}

void CTe_RegBlockTestWrapper::DoCmdWriteStringToConfig(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("Test WriteStringToConfig..."));
	_LIT(KChangedString, "Here is the changed string");
	TPtrC string;
	TPtrC getString;
	TBuf<64> stringBuf;
	GetStringFromConfig(aSection, KWriteStringValue, getString);
	stringBuf.Copy(getString);
	TESTA(getString!=KChangedString());
	WriteStringToConfig(aSection, KWriteStringValue, KChangedString());
	GetStringFromConfig(aSection, KWriteStringValue, string);
	TESTA(string==KChangedString());
	WriteStringToConfig(aSection, KWriteStringValue, stringBuf);
	}

void CTe_RegBlockTestWrapper::DoCmdWriteSharedData()
	{
	INFO_PRINTF1(_L("Test WriteSharedDataL..."));
	TPtrC sharedValue(KSharedValue);
	CTestSharedData* sharedObject = NULL;
	CTEFSharedData<CTestSharedData>::NewL(sharedObject, 64, KSharedName);
	TRAPD(err, WriteSharedDataL(KSharedName, sharedValue, ESetText));
	TESTA(err==KErrNone);
	}

void CTe_RegBlockTestWrapper::DoCmdReadSharedData()
	{
	INFO_PRINTF1(_L("Test ReadSharedDataL..."));
	TBuf<64> sharedData;
	TRAP_IGNORE(ReadSharedDataL(KSharedName, sharedData));
	TESTA(sharedData==KSharedValue);
	}

void CTe_RegBlockTestWrapper::DoCmdTestNullController(const TTEFSectionName& aSection)
	{
	INFO_PRINTF1(_L("Test with null controller..."));
	CTe_RegBlockTestWrapper* testWrapper = new (ELeave) CTe_RegBlockTestWrapper();
	TBool testRet = ETrue;
	
	TInt getIntValue = 0;
	testRet = testWrapper->WriteIntToConfig(aSection, KWriteIntValue, getIntValue);
	getIntValue = 10;
	testRet |= testWrapper->GetIntFromConfig(aSection, KWriteIntValue, getIntValue);
	TESTA(!testRet && 10==getIntValue);
	
	testRet = ETrue;
	TBool getBoolValue = EFalse;
	testRet = testWrapper->WriteBoolToConfig(aSection, KWriteBoolValue, getBoolValue);
	getBoolValue = ETrue;
	testRet |= testWrapper->GetBoolFromConfig(aSection, KWriteBoolValue, getBoolValue);
	TESTA(!testRet && getBoolValue);
	
	testRet = ETrue;
	TInt getHexValue = 0;
	testRet = testWrapper->WriteHexToConfig(aSection, KWriteHexValue, getHexValue);
	getHexValue = 0xFFFF;
	testRet |= testWrapper->GetHexFromConfig(aSection, KWriteHexValue, getHexValue);
	TESTA(!testRet && 0xFFFF==getHexValue);
	
	testRet = ETrue;
	TPtrC getString = KNullDesC();
	testRet = testWrapper->WriteStringToConfig(aSection, KWriteStringValue, getString);
	_LIT(KString, "String");
	getString.Set(KString());
	testRet |= testWrapper->GetStringFromConfig(aSection, KWriteStringValue, getString);
	TESTA(!testRet && getString==KString());
	
	delete testWrapper;
	}
void CTe_RegBlockTestWrapper::RunL(CActive* aActive, TInt aIndex)
	{
	INFO_PRINTF1(_L("CTe_RegBlockTestWrapper::RunL"));
	TInt err = aActive->iStatus.Int();
	SetAsyncError( aIndex, err );
	DecOutstanding();
	}


/** Reads drive mapping operation name from INI-file */
TBool CTe_RegBlockTestWrapper::GetVerdictFromConfig(const TDesC& aSection, const TDesC& aParameterName, TVerdict& aVerdict)
	{
	// Read drives mapping operation name from INI file
	TPtrC verdictStr;
	TBool ret = GetStringFromConfig(aSection, aParameterName, verdictStr);
	if (ret)
		{
		if (verdictStr == KPass())
			{
			aVerdict = EPass;
			}
		else if (verdictStr == KFail())
			{
			aVerdict = EFail;
			}
		else
			{
			TInt verdict = 0;
			ret = GetIntFromConfig(aSection, aParameterName, verdict);
			if (ret)
				{
				aVerdict = (TVerdict) verdict;
				}
			}
		}

	return ret;
	}

