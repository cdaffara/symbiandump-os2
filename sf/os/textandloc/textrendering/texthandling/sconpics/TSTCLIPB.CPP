/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TSTCLIPB.H"
#include <s32file.h>

const TUid KClipboardFileUid={268435515};


_LIT(KClipFilePath, "_:\\system\\clipbd\\main.sto");
typedef TBuf<28> TClipboardFileName;


/**
Construct full path and name of clipbard file.
@param aFileName On completion this will hold the result.
*/
static void GetClipboardFileName(TClipboardFileName& aFileName)
	{
	aFileName.Copy(KClipFilePath);
	aFileName[0] = 'A' + static_cast<TInt>(RFs::GetSystemDrive());
	}



EXPORT_C CClipboard* CClipboard::NewForReadingLC(RFs& aFsSession)
    {
    // get store filename
	TClipboardFileName clipboardFileName;
	GetClipboardFileName(clipboardFileName);

	TEntry entry;
	User::LeaveIfError(aFsSession.Entry(clipboardFileName,entry));
	if (entry[0]!=KDirectFileStoreLayoutUid || entry[1]!=KClipboardFileUid)
		User::Leave(KErrCorrupt); // !! get a better error
	CClipboard* self=new(ELeave) CClipboard(aFsSession);
    CleanupStack::PushL(self);
	self->ConstructForReadingL(clipboardFileName);
	return(self);
	}

/**
Construct a clipboard store for reading from.
*/
void CClipboard::ConstructForReadingL(const TDesC& aFileName)
    {
	iStore=CDirectFileStore::OpenL(iFsSession,aFileName,EFileRead);
	iStreamDictionary=CStreamDictionary::NewL();
	RStoreReadStream root;
	root.OpenLC(*iStore,iStore->Root());
	root>> *iStreamDictionary;
	CleanupStack::PopAndDestroy(); // root
	}


EXPORT_C CClipboard* CClipboard::NewForWritingLC(RFs& aFsSession)
    {
    // get store filename
	TClipboardFileName clipboardFileName;
	GetClipboardFileName(clipboardFileName);
    
	CClipboard* self=new(ELeave) CClipboard(aFsSession);
    CleanupStack::PushL(self);
	self->ConstructForWritingL(clipboardFileName);
	return(self);
	}


/**
Construct a clipboard store for writing to.
*/
void CClipboard::ConstructForWritingL(const TDesC& aFileName)
    {
	CreateDirectoryL(aFileName);
	iStore=CDirectFileStore::ReplaceL(iFsSession,aFileName,EFileRead|EFileWrite);
	iStore->SetTypeL(TUidType(KDirectFileStoreLayoutUid,KClipboardFileUid));
	iStreamDictionary=CStreamDictionary::NewL();
	}


CClipboard::CClipboard(RFs& aFsSession)
	: iFsSession(aFsSession)
	{
	}

EXPORT_C CClipboard::~CClipboard()
    {
   	delete(iStore);
	delete(iStreamDictionary);
    }



void CClipboard::CreateDirectoryL(const TDesC& aFileName)
	{
	TParse parser;
	parser.Set(aFileName,NULL,NULL);
	TInt err=iFsSession.MkDirAll(parser.DriveAndPath());
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	}

EXPORT_C void CClipboard::CommitL()
	{
	RStoreWriteStream stream;
	TStreamId streamId=stream.CreateLC(*iStore);
	stream<< *iStreamDictionary;
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // dictionary stream
	iStore->SetRootL(streamId);
	iStore->CommitL();
	}
