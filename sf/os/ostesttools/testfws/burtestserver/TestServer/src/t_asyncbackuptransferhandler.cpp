// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @released
*/


#include "t_asyncbackuptransferhandler.h"

using namespace conn;

const TInt KZero = 0;

namespace bur_ts
	{
	CAsyncBackupTransferHandler* CAsyncBackupTransferHandler::NewL(CBURTestStepBase* aTestStep)
		/**
		Symbian OS Constructor
		
		@internalComponent
		@released
		
		@param aTestStep - A pointer to a CBURTestStepBackup or CBURTestStepRestore that owns this object
		
		@return Pointer to a newly created CAsyncBackupTransferHandler object.
		*/
		{
		CAsyncBackupTransferHandler* self = new (ELeave) CAsyncBackupTransferHandler(aTestStep);
		CleanupStack::PushL(self);
		self->ConstructL();
		CleanupStack::Pop();
		return self;
		}
	
	CAsyncBackupTransferHandler::CAsyncBackupTransferHandler(CBURTestStepBase* aTestStep)
		/**
		C++ Constructor
		
		@internalComponent
		@released
		
		@param aTestStep - A pointer to a CBURTestStepBackup or CBURTestStepRestore that owns this object
		*/
	: 	CActive(EPriorityStandard),
		iActiveScheduler(NULL),
	  	iCurrentIndex(KZero),
	  	iSuccess(ETrue),
	  	iTestStep(aTestStep),
	  	iID(NULL)
		{}
	
	void CAsyncBackupTransferHandler::ConstructL()
		/**
		Symbian OS 2nd phase Constructor
		
		@internalComponent
		@released
		*/
		{
		iID = HBufC::NewL(KZero);
		iActiveScheduler = new (ELeave) CActiveSchedulerWait;
		CActiveScheduler::Add(this);
		}
	
	CAsyncBackupTransferHandler::~CAsyncBackupTransferHandler()
		/**
		C++ Destructor
		
		@internalComponent
		@released
		*/
		{
		// Cancel the active object
		if (IsActive())
			{
			Cancel();
			}
		delete iID; 
		iID = 0;
		delete iActiveScheduler;
		iActiveScheduler = 0;
		}
	
	void CAsyncBackupTransferHandler::StartL()
		/**
		@internalComponent
		@released
		*/
		{
		iCurrentIndex = KZero;
		iSuccess = ETrue;
		ContinueL();
		iActiveScheduler->Start();
		}
	
	void CAsyncBackupTransferHandler::ContinueL()
		/**
		@internalComponent
		@released
		*/
		{
		// If all transfer types have been dealt with, there are no more requests 
		// to be made and hence we can stop the active scheduler:
		if (iCurrentIndex == iTestStep->TransferTypes().Count())
			{
			iActiveScheduler->AsyncStop();
			}
		else if (!IsActive())
			{
			CurrentIDL();
			TUint expectedSize = 0;
			
			_LIT(KCurrentSid, "Current ID: ");
			iTestStep->LogWithText(LOG_LEVEL3, KCurrentSid, *iID);
			
			TRAPD(err, expectedSize = iTestStep->BackupClient()->ExpectedDataSizeL(*iTestStep->TransferTypes()[iCurrentIndex]));
			if(err != KErrNone)
				{
				_LIT(KLog1e, "Request expected data size error: ");
				iTestStep->LogWithNum(LOG_LEVEL5, KLog1e, err);
				}
							
			_LIT(KExpSize, "Expected Data Size: ");
			iTestStep->LogWithNum(LOG_LEVEL4, KExpSize, expectedSize);		

			MakeRequestDataCallL();
			}
		else
			{
			// Being called during an active state
			User::Leave(KErrInUse);
			}
		}
		
	void CAsyncBackupTransferHandler::MakeRequestDataCallL()
		{
		TRAPD(err1,iTestStep->BackupClient()->RequestDataL(*iTestStep->TransferTypes()[iCurrentIndex], iStatus));
		if (err1 != KErrNone)
			{
			_LIT(KLog1, "Failed to request data for ID: ");
			iTestStep->LogWithText(LOG_LEVEL5, KLog1, *iID);
			_LIT(KLog2, "Error code: ");
			iTestStep->LogWithNum(LOG_LEVEL5, KLog2, err1);
			}						
		iStatus = KRequestPending;
		SetActive();	
		}
	
	void CAsyncBackupTransferHandler::RunL()
		/**
		Upon completion of the request, gets backup data from the backup server, and 
		saves it to the backup archive.
		
		@internalComponent
		@released
		*/
		{
		if (iStatus == KErrNone)
			{
			TBool isFinished;
			
			// Initialise following to NULL, or else SBEClient panics in debug builds:		
			CSBGenericTransferType* receivedType = NULL;
			TPtrC8 pData = iTestStep->BackupClient()->TransferDataInfoL(receivedType, isFinished);
			CleanupStack::PushL(receivedType);
			
			// no need to push it onto CleanupStack as there is no ownership transfer
			CSBGenericTransferType* currentTransfer = iTestStep->TransferTypes()[iCurrentIndex];
			
			if (KErrNone != (currentTransfer->Externalise()).CompareF(receivedType->Externalise()))
				{
				_LIT(KLogWrong, "The type of data requested doesn't much the received, please contact Connectivity team !");
				iTestStep->LogWithText(LOG_LEVEL3, KLogWrong, *iID);
				}
			else if (pData.Size() > 0)
				{
				TRAPD(err, iTestStep->StorageManager()->ArchiveDataL(currentTransfer, pData, isFinished));
				if (err != KErrNone)
					{
					iTestStep->StorageManager()->Reset();
					_LIT(KLog1, "Error Saving data to archive for ID: ");
					iTestStep->LogWithText(LOG_LEVEL3, KLog1, *iID);
					_LIT(KLog2, "Error code: ");
					iTestStep->LogWithNum(LOG_LEVEL3, KLog2, err);
					}
				else
					{
					_LIT(KLog3, "Number of bytes saved: ");
					iTestStep->LogWithNum(LOG_LEVEL3, KLog3, pData.Size());
					}
				}
				
			CleanupStack::PopAndDestroy(receivedType);
			if (isFinished)  // if not then there are more data to come, so run again on the same type
				{
				++iCurrentIndex;
				// Restart the active object to request more data:
				ContinueL();
				}
			else // continue to request data
				{
				MakeRequestDataCallL();
				}
			
			}
		else
			{
			User::Leave(iStatus.Int());
			}
		}
	
	
	void CAsyncBackupTransferHandler::DoCancel()
		/**
		@internalComponent
		@released
		*/
		{}
	
	TInt CAsyncBackupTransferHandler::RunError(TInt aError)
		/**
		This overridden method prints an error message to the TestExecute log
		and then returns with the error passed to it by the Active Scheduler.
		
		@internalComponent
		@released
		
		@param aError - Error code passed down by the Active Scheduler.
		@return The error code returned by the Active Scheduler.
		*/
		{
		++iCurrentIndex;
		_LIT(KErrorText, "Error while async call : ");
		iTestStep->LogWithNum(LOG_LEVEL4, KErrorText, aError);
		iSuccess = EFalse;
		TRAPD(err, ContinueL());
		if (err != KErrNone)
			{
			_LIT(KErrorText1, "Error trying to issue another requst : ");
			iTestStep->LogWithNum(LOG_LEVEL4, KErrorText1, err);
			}
		return KErrNone;
		}
	
	void CAsyncBackupTransferHandler::CurrentIDL()
		/**
		@internalComponent
		@released
		
		@return SecureId of the data owner currently being processed.
		*/
		{
		delete iID;
		iID = NULL;
		CSBGenericTransferType* transfer = iTestStep->TransferTypes()[iCurrentIndex];
		TSBDerivedType type = transfer->DerivedTypeL();
		TInt numberOfDigits = EHex;
		
		if (type == ESIDTransferDerivedType)
			{
			CSBSIDTransferType* sidType = CSBSIDTransferType::NewL(transfer);
			CleanupStack::PushL(sidType); 
			iID = HBufC::NewL(numberOfDigits);
			TPtr pID = iID->Des();
			
			pID.AppendNumUC(sidType->SecureIdL(), EHex);
			CleanupStack::PopAndDestroy(sidType);
			}
		else if (type == EPackageTransferDerivedType)
			{
			CSBPackageTransferType* pidType = CSBPackageTransferType::NewL(transfer);
			CleanupStack::PushL(pidType);
			iID = HBufC::NewL(numberOfDigits);
			TPtr pID = iID->Des();
			pID.AppendNum(TSecureId(pidType->PackageIdL()), EHex);
			CleanupStack::PopAndDestroy(pidType);
			}
		else if (type == EJavaTransferDerivedType)
			{
			CSBJavaTransferType* javaType = CSBJavaTransferType::NewL(transfer);
			CleanupStack::PushL(javaType);
			iID = HBufC::NewL(javaType->SuiteHashL().Length());
			TPtr pID = iID->Des();
			pID.Append(javaType->SuiteHashL());
			CleanupStack::PopAndDestroy(javaType);
			}
		}
		
	
	
	TBool CAsyncBackupTransferHandler::Success()
		/**
		@internalComponent
		@released
		
		@return Whether or not all data transfers completed successfully and resets the success.
		*/
		{
		TBool res = iSuccess;
		iSuccess = ETrue;
		return res;
		}
	}	// end namespace
