// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The implementation of the accessor class upon the CTransition
// 
//

// ______________________________________________________________________________
//
_LIT(KTTransition_DumpName,"C:\\System\\Data\\Logs\\TTransition_StateDump.bin");

inline TInt TTransition_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	if(aTestObject)
		{
		CTransition* theTransition = REINTERPRET_CAST(CTransition*, aTestObject);
		if(theTransition->iTransitionId.Length() == 0)
			return KErrArgument;
		}
	return KErrNone;
	}

inline TInt TTransition_StateAccessor::Internalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CTransition* theTransition = REINTERPRET_CAST(CTransition*, aTestObject);
		TRAP(error,InternalizeL(theTransition));
		}
	return error;
	}

inline TInt TTransition_StateAccessor::Externalize(TAny* aTestObject)
	{
	TInt error = KErrNone;
	if(aTestObject != NULL)
		{
		CTransition* theTransition = REINTERPRET_CAST(CTransition*, aTestObject);
		TRAP(error,ExternalizeL(theTransition));
		}
	return error;
	}

inline TBool TTransition_StateAccessor::IsRepeated(TAny* aTestObject) const
	{
	if(aTestObject != NULL)
		{
		CTransition* theTransition = REINTERPRET_CAST(CTransition*, aTestObject);
		return theTransition->iRepeatThis;
		}
	return EFalse;
	}

inline const TDesC* TTransition_StateAccessor::TransitionId(TAny* aTestObject) const
	{
	if(aTestObject != NULL)
		{
		CTransition* theTransition = REINTERPRET_CAST(CTransition*, aTestObject);
		return &(theTransition->iTransitionId);
		}
	return NULL;
	}

inline const TTransitionInfo* TTransition_StateAccessor::TransitionInfo(TAny* aTestObject) const
	{
	if(aTestObject != NULL)
		{
		CTransition* theTransition = REINTERPRET_CAST(CTransition*, aTestObject);
		return &(theTransition->iTransitionInfo);
		}
	return NULL;
	}

inline TBool TTransition_StateAccessor::BlockingFlag(TAny* aTestObject) const
	{
	if(aTestObject != NULL)
		{
		CTransition* theTransition = REINTERPRET_CAST(CTransition*, aTestObject);
		return theTransition->iBlockingTransition;
		}
	return EFalse;
	}

inline TInt TTransition_StateAccessor::ErrorCode(TAny* aTestObject) const
	{
	if(aTestObject != NULL)
		{
		CTransition* theTransition = REINTERPRET_CAST(CTransition*, aTestObject);
		return theTransition->iLeaveError;
		}
	return EFalse;
	}

inline TRequestStatus TTransition_StateAccessor::Status(const TAny* aTestObject) const
	{
	TRequestStatus status;
	Mem::FillZ(&status, sizeof(status));
	if(aTestObject != NULL)
		{
		status = (reinterpret_cast <const CTransition*> (aTestObject))->iStatus;
		}
	return status;
	}

inline void TTransition_StateAccessor::InternalizeL(RFileReadStream& aStream, CTransition* aTransition)
	{
	aTransition->iRepeatThis				= aStream.ReadInt32L();
	aTransition->iTransitionFinished		= aStream.ReadInt32L();
	aTransition->iLeaveError				= aStream.ReadInt32L();
	aTransition->iBlockingTransition		= aStream.ReadInt32L();
	TPckg<TTransitionInfo> info(aTransition->iTransitionInfo);
	aStream.ReadL(info);
	}

inline void TTransition_StateAccessor::ExternalizeL(RFileWriteStream& aStream, CTransition* aTransition)
	{
	aStream.WriteInt32L(aTransition->iRepeatThis);
	aStream.WriteInt32L(aTransition->iTransitionFinished);
	aStream.WriteInt32L(aTransition->iLeaveError);
	aStream.WriteInt32L(aTransition->iBlockingTransition);
	TPckg<TTransitionInfo> info(aTransition->iTransitionInfo);
	aStream.WriteL(info);
	}

// internal helpers
inline void TTransition_StateAccessor::InternalizeL(CTransition* aTransition)
	{
	// Read in the dumped data
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Read the object dump
	TParse filename;
	filename.Set(KTTransition_DumpName(),NULL,NULL);
	RFileReadStream stream;
	User::LeaveIfError(stream.Open(fs,filename.FullName(), EFileRead));
	CleanupClosePushL(stream);
	InternalizeL(stream, aTransition);
	CleanupStack::PopAndDestroy(2);		// stream & fs
	}

inline void TTransition_StateAccessor::ExternalizeL(CTransition* aTransition)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	// Write out the object dump
	TParse filename;
	filename.Set(KTTransition_DumpName(),NULL,NULL);
	RFileWriteStream stream;
	User::LeaveIfError(stream.Replace(fs,filename.FullName(), EFileWrite));
	CleanupClosePushL(stream);
	ExternalizeL(stream, aTransition);
	stream.CommitL();
	CleanupStack::PopAndDestroy(2);		// stream & fs
	}

