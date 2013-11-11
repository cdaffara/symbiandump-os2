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
* This contains CTestBlockController
*
*/



/**
 @prototype
 @test
*/

#include "testblockcontroller.h"
#include "datawrapper.h"

_LIT(KName,				"name");


// Errors
_LIT(KErrNotExist,			"Object does not exist.");
_LIT(KErrNoCleanup,			"Object does not have a cleanup operation.");
_LIT(KErrNoName,			"No name provided.");
_LIT(KErrNoFunction,		"No function provided.");
_LIT(KErrSharedData,		"Test server needs to be loaded in -SharedData mode.");
_LIT(KErrCmdUnknown,		"Unknown command.");
_LIT(KErrWrapperCreate,		"Can't create an object wrapper of type %S");
_LIT(KErrBlockCorrupt,		"Corrupt block of commands.");
_LIT(KErrActiveSched,		"Active Scheduler, error = %d");
_LIT(KErrSharedMode,		"-SharedData mode required.");
_LIT(KErrSchedulerLoc,		"SHARED_ACTIVE_SCHEDULER command must be first in block.");
_LIT(KLocalScheduler,		"Creating a local active scheduler.");
_LIT(KSharedScheduler,		"Creating a shared active scheduler.");
_LIT(KSharedServer,			"Error accessing the test server.");
_LIT(KErrOutstanding,		"Outstanding failed with error %d.");

// Defaults
const TInt				KDefaultDelay		= 1000000;
const TInt				KDefaultInterval	= 1000000;

EXPORT_C CTestBlockController::CTestBlockController()
/**
 * Constructor
 */
:	iBlockArray(NULL)
,	iCommandProcessor(NULL)
,	iActiveScheduler(NULL)
,	iTimer(NULL)
,	iPeriodic(NULL)
,	iAsyncTimer(EFalse)
,	iTimerActive(EFalse)
,	iCommandIndex(0)
,	iAsyncCount(0)
,	iSharedData(NULL)
,	iDeleteSharedScheduler(ETrue)
	{
	}

EXPORT_C CTestBlockController::~CTestBlockController()
/**
 * Destructor
 */
	{
	if( iTimer )
		{
		delete iTimer;
		iTimer=NULL;
		}

	if( iPeriodic )
		{
		delete iPeriodic;
		iPeriodic=NULL;
		}

	if( iCommandProcessor )
		{
		delete iCommandProcessor;
		iCommandProcessor=NULL;
		}

	if( iActiveScheduler )
		{
		delete iActiveScheduler;
		iActiveScheduler=NULL;
		}

	if( IsSharedDataMode() && iDeleteSharedScheduler && iSharedData)
		{
		TRAP_IGNORE(iSharedData->DeleteActiveSchedulerL());
		}

	}

EXPORT_C TVerdict CTestBlockController::doTestStepPreambleL()
	{
	TVerdict	ret = CTestStep::doTestStepPreambleL();

	if( !iSharedData )
		{
		ERR_PRINTF1(KSharedServer);
		User::Leave(KErrBadHandle);
		}

	// Test for a persistent active scheduler command
	if( ETEFSharedActiveScheduler == iBlockArray->At(iCommandIndex).iItemType )
		{
		if( IsSharedDataMode() )
			{
			INFO_PRINTF1(KSharedScheduler);
			iSharedData->CreateActiveSchedulerL();
			}
		else
			{
			iBlockArray->At(iCommandIndex).iError = KErrNotSupported;
			ERR_PRINTF1(KErrSharedMode);

			// Create a new active scheduler
			INFO_PRINTF1(KLocalScheduler);
			iDeleteSharedScheduler = EFalse;
			iActiveScheduler=new (ELeave) CBlockActiveScheduler(*this);
			CActiveScheduler::Install(iActiveScheduler);
			}
		iBlockArray->At(iCommandIndex).iExecuted = ETrue;
		iCommandIndex++;
		}
	else
		{
		if( IsSharedDataMode() )
			{
			// Ensure there is no persistent active scheduler if in -SharedData mode
			iSharedData->DeleteActiveSchedulerL();
			}

		// Create a new active scheduler
		INFO_PRINTF1(KLocalScheduler);
		iDeleteSharedScheduler = EFalse;
		iActiveScheduler=new (ELeave) CBlockActiveScheduler(*this);
		CActiveScheduler::Install(iActiveScheduler);
		}

	iCommandProcessor=CCommandProcessor::NewL(*this);
	iTimer=CTEFTimer::NewL(*this);
	iPeriodic=CPeriodic::NewL(CActive::EPriorityStandard);

	return ret;
	}

EXPORT_C TVerdict CTestBlockController::doTestStepL()
	{
	//	Execute oommands
	StartCommands();
	CActiveScheduler::Start();

	return TestStepResult();
	}

EXPORT_C TVerdict CTestBlockController::doTestStepPostambleL()
	{
	iDataDictionary.Empty();
	return TestStepResult();
	}

EXPORT_C void CTestBlockController::SetBlockArray( TTEFItemArray* aBlockArray )
	{
	iBlockArray = aBlockArray;
	}

EXPORT_C void CTestBlockController::SetSharedData( MSharedData* aSharedData )
	{
	iSharedData = aSharedData;
	}

EXPORT_C void CTestBlockController::SetError(const TInt aError)
	{
	iBlockArray->At(iCommandIndex).iError = aError;
	}

EXPORT_C void CTestBlockController::SetAsyncError(const TInt aIndex, const TInt aError)
	{
	iBlockArray->At(aIndex).iAsyncError = aError;
	}

EXPORT_C void CTestBlockController::SetBlockResult(const TVerdict aResult)
	{
	SetTestStepResult(aResult);
	}

EXPORT_C TVerdict CTestBlockController::BlockResult()
	{
	return TestStepResult();
	}

EXPORT_C TBool CTestBlockController::DoCommandL(TTEFBlockItem& aCommand, const TInt aAsyncErrorIndex)
	{
	TBool			synchronous	= ETrue;

	if( ETEFCreateObject==aCommand.iItemType )
		{
		CreateObjectL(aCommand);
		}
	else if( ETEFRestoreObject==aCommand.iItemType )
		{
		RestoreObjectL(aCommand);
		}
	else if( ETEFStore==aCommand.iItemType )
		{
		StoreL(aCommand);
		}
	else if( ETEFDelay==aCommand.iItemType )
		{
		User::After(aCommand.iTime?aCommand.iTime:KDefaultDelay );
		}
	else if( ETEFAsyncDelay==aCommand.iItemType )
		{
		synchronous=EFalse;
		StartTimer(aCommand);
		}
	else if( ETEFOutstanding==aCommand.iItemType )
		{
		synchronous=EFalse;
		iPeriodic->Start(	0,
							aCommand.iTime?aCommand.iTime:KDefaultInterval,
							TCallBack(OutstandingCallback,this));
		}
	else if( ETEFSharedActiveScheduler==aCommand.iItemType )
		{
		aCommand.iError = KErrNotSupported;
		ERR_PRINTF1(KErrSchedulerLoc);
		}
	else if( ETEFStoreActiveScheduler==aCommand.iItemType )
		{
		StoreActiveScheduler(aCommand);
		}
	else if( ETEFCommand==aCommand.iItemType )
		{
		CommandL(aCommand, aAsyncErrorIndex);
		}
	else
		{
		ERR_PRINTF1(KErrCmdUnknown);
		aCommand.iError = KErrNotSupported;
		}

	// Update the iExecuted flag
	aCommand.iExecuted = ETrue;

	return synchronous;
	}


void CTestBlockController::CreateObjectL(TTEFBlockItem& aCommand)
	{
	// Retrieve the object name from the ini file
	TPtrC	name;
	if( GetStringFromConfig(aCommand.iSection, KName, name) &&
		0 != aCommand.iSection.Compare(KTEFNull) )
		{
		// Create the wrapper
		CDataWrapper*	data = CreateDataL(aCommand.iObjectType);
		if( NULL != data)
			{
			CleanupStack::PushL(data);
			data->SetTestBlockController(this);
			data->SetDataDictionary(&iDataDictionary);
			data->InitialiseL();
			// Add it to the dictionary with the lookup name provided
			iDataDictionary.AddDataL(name, data);
			CleanupStack::Pop(data);
			}
		else
			{
			aCommand.iError = KErrNotFound;
			ERR_PRINTF2( KErrWrapperCreate, &name );
			}
		}
	else
		{
		ERR_PRINTF1(KErrNoName);
		aCommand.iError = KErrNotFound;
		}
	}

void CTestBlockController::RestoreObjectL(TTEFBlockItem& aCommand)
	{
	if( IsSharedDataMode() )
		{
		// Retrieve the object name from the ini file
		TPtrC	name;
		if( 0 != aCommand.iSection.Compare(KTEFNull) &&
			GetStringFromConfig(aCommand.iSection, KName, name) )
			{
			// Create the wrapper
			CDataWrapper*	data = CreateDataL(aCommand.iObjectType);

			if( NULL != data)
				{
				CleanupStack::PushL(data);
				data->SetTestBlockController(this);
				data->SetDataDictionary(&iDataDictionary);
				data->InitialiseL();

				// Add it to the dictionary with the lookup name provided
				iDataDictionary.AddDataL(name, data);

				// Retrieve the object from the persistent store
				TAny*	object = NULL;
				TRAPD( err, object = iSharedData->GetObjectAndOwnL(name) );
				if( KErrNone == err )
					{
					CleanupStack::PushL(object);
					// Update the object contained within the wrapper
					iDataDictionary.SetObjectL(name, object);
					CleanupStack::Pop(object);
					}
				else
					{
					ERR_PRINTF1(KErrNotExist);
					iDataDictionary.DeleteDataL(name);
					aCommand.iError = KErrNotFound;
					}
				CleanupStack::Pop(data);
				}
			else
				{
				aCommand.iError = KErrNotFound;
				ERR_PRINTF2( KErrWrapperCreate, &name );
				}
			}
		else
			{
			ERR_PRINTF1(KErrNoName);
			aCommand.iError = KErrNotFound;
			}
		}
	else
		{
		ERR_PRINTF1(KErrSharedData);
		aCommand.iError = KErrNotSupported;
		}
	}

void CTestBlockController::StoreL(TTEFBlockItem& aCommand)
	{
	if( IsSharedDataMode() )
		{
		// Retrieve the object name from the ini file
		TPtrC	name;
		if( 0 != aCommand.iSection.Compare(KTEFNull) &&
			GetStringFromConfig(aCommand.iSection, KName, name) )
			{
			// Lookup the wrapper
			CDataWrapper*	data = iDataDictionary.GetDataL( name );

			// Retrieve the object being wrapped
			TAny*			object = data->GetObject();
			if ( object!=NULL )
				{
				// Add it to the persistent store
				TCleanupOperation	operation=data->CleanupOperation();
				if ( operation!=NULL )
					{
					iSharedData->PutAndDisownL(name, object, operation);
					data->DisownObjectL();
					}
				else
					{
					ERR_PRINTF1(KErrNoCleanup);
					aCommand.iError = KErrNotFound;
					}
				}
			else
				{
				ERR_PRINTF1(KErrNotExist);
				aCommand.iError = KErrNotFound;
				}
			}
		else
			{
			ERR_PRINTF1(KErrNoName);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		ERR_PRINTF1(KErrSharedData);
		aCommand.iError = KErrNotSupported;
		}
	}

void CTestBlockController::StoreActiveScheduler(TTEFBlockItem& aCommand)
	{
	if( IsSharedDataMode() )
		{
		// Don't delete the persistent active scheduler
		iDeleteSharedScheduler = EFalse;
		}
	else
		{
		aCommand.iError = KErrNotSupported;
		ERR_PRINTF1(KErrSharedMode);
		}
	}

void CTestBlockController::CommandL(TTEFBlockItem& aCommand, const TInt aAsyncErrorIndex)
	{
	// Retrieve the object name from the ini file
	TPtrC	name;
	if( 0 != aCommand.iCommand.iObject.Compare(KTEFNull) &&
		GetStringFromConfig(aCommand.iCommand.iObject, KName, name) )
		{
		CDataWrapper*	data = iDataDictionary.GetDataL(name);
		if( data!=NULL )
			{
			TBool cmdExists = EFalse;
			TRAPD(err, cmdExists = data->DoCommandL(aCommand.iCommand.iFunction, aCommand.iSection, aAsyncErrorIndex));
			if (KErrNone != err)
				{
				ERR_PRINTF4(_L("Command \"%S\" of the object \"%S\" leaves with error code %d"), &aCommand.iCommand.iFunction, &aCommand.iCommand.iObject, err);
				aCommand.iError = err;
				SetBlockResult(EFail);
				}
			else if( !cmdExists )
				{
				ERR_PRINTF1(KErrNoFunction);
				aCommand.iError = KErrNotFound;
				}
			}
		else
			{
			ERR_PRINTF1(KErrNotExist);
			aCommand.iError = KErrNotFound;
			}
		}
	else
		{
		ERR_PRINTF1(KErrNoName);
		aCommand.iError = KErrNotFound;
		}
	}

EXPORT_C void CTestBlockController::StartTimer(const TTEFBlockItem& aCommand)
	{
	iTimer->After(aCommand.iTime?aCommand.iTime:KDefaultDelay);
	}

EXPORT_C void CTestBlockController::CancelTimer()
	{
	if ( iTimerActive )
		{
		iTimer->Cancel();
		}
	}

EXPORT_C void CTestBlockController::TimerCompleted()
	{
	if( iAsyncTimer )
		{
		DecAsyncCount();
		}
	iTimerActive=EFalse;
	StartCommands();
	}

EXPORT_C TBool CTestBlockController::OutstandingCallback( TAny* aAny )
	{
	CTestBlockController* self = STATIC_CAST(CTestBlockController*,aAny);
	if( self )
		{
		self->OutstandingCallback();
		}
	return EFalse;
	}

EXPORT_C void CTestBlockController::OutstandingCallback()
	{
	TBool			ok=ETrue;
	TTEFBlockItem&	block=iBlockArray->At(iCommandIndex-1);

	TPtrC			name(KTEFNull);
	if( 0 != block.iSection.Compare(KTEFNull)  )
		{
		if ( !GetStringFromConfig(block.iSection, KName, name) )
			{
			block.iError = KErrNotFound;
			ERR_PRINTF1(KErrNoName);
			ERR_PRINTF1(block.iSection);
			ok=EFalse;
			}
		}

	if ( ok )
		{
		TBool	moreToDo;
		TInt	err=iDataDictionary.Outstanding(name, moreToDo);
		if( err!=KErrNone )
			{
			block.iError = err;
			ERR_PRINTF2(KErrOutstanding, err);
			}
		if ( !moreToDo )
			{
			iPeriodic->Cancel();
			DecAsyncCount();
			StartCommands();
			}
		}
	}

EXPORT_C CTestBlockController::CBlockActiveScheduler::CBlockActiveScheduler(CTestBlockController& aTestStep)
:	CActiveScheduler()
,	iTestStep(aTestStep)
	{
	}

EXPORT_C CTestBlockController::CBlockActiveScheduler::~CBlockActiveScheduler()
	{
	}

EXPORT_C void CTestBlockController::CBlockActiveScheduler::Error(TInt aError) const
	{
	if ( KErrNone != aError )
		{
		iTestStep.ERR_PRINTF2(KErrActiveSched, aError);
		iTestStep.SetTestStepResult(EFail);
		}
	CActiveScheduler::Error(aError);
	}

EXPORT_C void CTestBlockController::StartCommands()
	{
	iCommandProcessor->KickState();
	}

EXPORT_C void CTestBlockController::NextCommandL()
	{
	if( iBlockArray )
		{
		if( iCommandIndex < iBlockArray->Count() )
			{
			TInt synchronous = EFalse;
			synchronous = DoCommandL(iBlockArray->At(iCommandIndex), iCommandIndex);
			if( synchronous )
				{
				StartCommands();
				}
			else
				{
				IncAsyncCount();
				}
			iCommandIndex++;
			}
		else
			{
			CActiveScheduler::Stop();
			}
		}
	else
		{
		ERR_PRINTF1(KErrBlockCorrupt);
		SetTestStepResult(EFail);
		CActiveScheduler::Stop();
		}
	}
