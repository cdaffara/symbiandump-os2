/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "filecertstorenibbler.h"
#include <f32file.h>

_LIT(KFileCertstorePath, "\\system\\data\\cacerts.dat");

/*static*/ CFileCertstoreNibbler* CFileCertstoreNibbler::NewL(TTimeIntervalMicroSeconds32 aInterval)
{
	CFileCertstoreNibbler* me = new (ELeave) CFileCertstoreNibbler(aInterval);
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return (me);
}

CFileCertstoreNibbler::CFileCertstoreNibbler(TTimeIntervalMicroSeconds32 aInterval)
:	CTimer(EPriorityHigh), iNibbleInterval(aInterval)
{
	CActiveScheduler::Add(this);
}

CFileCertstoreNibbler::~CFileCertstoreNibbler()
{
	Cancel();
	iFs.Close();
}

void CFileCertstoreNibbler::StartTimer()
{
	After(iNibbleInterval);
}

void CFileCertstoreNibbler::ConstructL()
{
	User::LeaveIfError(iFs.Connect());
		
	RFile file;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> fileCertstorePath (sysDrive.Name());
	fileCertstorePath.Append(KFileCertstorePath);
	User::LeaveIfError(file.Open(iFs, fileCertstorePath, EFileWrite|EFileRead|EFileShareExclusive));
	CleanupClosePushL(file);
	CPermanentFileStore* store = CPermanentFileStore::FromLC(file);	
	
	RStoreWriteStream stream;
	iNibbleStreamId = stream.CreateLC(*store);	//	stream for cert
	
//	Write to stream
	_LIT(KNibble, "wool");
	stream << KNibble;
	
	stream.CommitL();
	CleanupStack::PopAndDestroy();	//	stream
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	CleanupStack::Pop();			//	file cleanup
		
	CTimer::ConstructL();
	StartTimer();
}

void CFileCertstoreNibbler::RunL()
{
	NibbleFileL();
	StartTimer();	
}

//	Writes to the filecertstore file (\system\data\cacerts.dat on system drive) to test
//	concurrency access. Writes to a separate stream so will have no effect
//	on certs data.  This doesn't test integrity of certstore in any way, is
//	simply a quick check that the file can be opened and written to whilst
//	other tests proceed.
void CFileCertstoreNibbler::NibbleFileL()
{	
	RFile file;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TBuf<128> fileCertstorePath (sysDrive.Name());
	fileCertstorePath.Append(KFileCertstorePath);
	User::LeaveIfError(file.Open(iFs, fileCertstorePath, EFileWrite|EFileRead|EFileShareExclusive));
	CleanupClosePushL(file);
	CPermanentFileStore* store = CPermanentFileStore::FromLC(file);	
		
	RStoreWriteStream stream;
	stream.ReplaceLC(*store, iNibbleStreamId);
	
//	Write to stream
	_LIT(KNibble, "clanger");
	stream << KNibble;
	
	stream.CommitL();
	CleanupStack::PopAndDestroy();	//	stream
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	CleanupStack::Pop();			//	file cleanup
}

TInt CFileCertstoreNibbler::RunError(TInt /*anError*/)
{
//	_LIT(KNibblerError, "CFileCertstoreNibbler::RunError");
//	User::Panic(KNibblerError, anError);
	return (KErrNone);
}

