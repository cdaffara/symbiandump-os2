/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "fstokenutil.h"
#include <f32file.h>
#include <s32file.h>

// RMessage::Panic() also completes the message. This is:
// (a) important for efficient cleanup within the kernel
// (b) a problem if the message is completed a second time
void PanicClient(const RMessage2& aMessage, ETokenTypeServerPanic aPanic)
	{
	_LIT(KPanicClient,"TTSERVERCLIENT");
	aMessage.Panic(KPanicClient,aPanic);
	}

void PanicServer(ETokenTypeServerPanic aPanicCode)
	{
	_LIT(KPanicServer,"TOKENTYPESERVER");
	User::Panic(KPanicServer, aPanicCode);
	}

/// Rom drive where the initial store data is
_LIT(KFileStoreROMDrive, "Z:");

TBool FileUtils::ExistsL(RFs& aFs, const TDesC& aFile)
	{
	TBool result = EFalse;
	TBool open;
	TInt err = aFs.IsFileOpen(aFile, open);
	
	if (err == KErrNone)
		{
		result = ETrue;
		}
	else if (err != KErrNotFound && err != KErrPathNotFound)
		{
		User::Leave(err);
		}

	return result;
	}

void FileUtils::EnsurePathL(RFs& aFs, const TDesC& aFile)
	{
	TInt err = aFs.MkDirAll(aFile);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	}

void FileUtils::CopyL(RFs& aFs, const TDesC& aSouce, const TDesC& aDest)
	{
	RFileReadStream in;
	User::LeaveIfError(in.Open(aFs, aSouce, EFileRead | EFileShareReadersOnly));
	CleanupClosePushL(in);

	RFileWriteStream out;
	User::LeaveIfError(out.Replace(aFs, aDest, EFileWrite | EFileShareExclusive));
	CleanupClosePushL(out);

	in.ReadL(out);	
	CleanupStack::PopAndDestroy(2, &in);
	}

void FileUtils::MakePrivateFilenameL(RFs& aFs, const TDesC& aLeafName, TDes& aNameOut)
	{
	aNameOut.SetLength(0);	
	aNameOut.Append(RFs::GetSystemDriveChar());

	aNameOut.Append(':');

	// Get private path
	TBuf<20> privatePath;
	aFs.PrivatePath(privatePath);
	aNameOut.Append(privatePath);
	
	aNameOut.Append(aLeafName);
	}

void FileUtils::MakePrivateROMFilenameL(RFs& aFs, const TDesC& aLeafName, TDes& aNameOut)
	{
	aNameOut.Copy(KFileStoreROMDrive);

	// Get private path
	TBuf<20> privatePath;
	aFs.PrivatePath(privatePath);
	aNameOut.Append(privatePath);
	
	aNameOut.Append(aLeafName);
	}
