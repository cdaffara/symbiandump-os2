// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CompareTwoFiles.h"

//
// Return ETrue if the contents of the 2 files are identical.
TBool FilesAreIdenticalL(RFs& aFs,
						const TDesC& aFile1,
						const TDesC& aFile2)
	{
	TInt numPushed = 0;
	RFile file1, file2;
	TEntry entry;

	User::LeaveIfError(aFs.Entry(aFile1, entry));
	TInt len1 = entry.iSize;

	User::LeaveIfError(aFs.Entry(aFile2, entry));
	TInt len2 = entry.iSize;

	if (len1 != len2)
		{
		return EFalse;
		}

	User::LeaveIfError(file1.Open(aFs, aFile1,EFileRead|EFileShareReadersOnly));
	CleanupClosePushL(file1);
	numPushed++;

	User::LeaveIfError(file2.Open(aFs, aFile2,EFileRead|EFileShareReadersOnly));
	CleanupClosePushL(file2);
	numPushed++;

	const TInt KBufSize = 512;
	HBufC8* hbuf1 = HBufC8::NewLC(KBufSize);
	numPushed++;
	HBufC8* hbuf2 = HBufC8::NewLC(KBufSize);
	numPushed++;

	TPtr8 buf1 = hbuf1->Des();
	TPtr8 buf2 = hbuf2->Des();

	TInt ret = KErrNone;
	TInt totalRead = 0;
	do
		{
		User::LeaveIfError(file1.Read(buf1));
		User::LeaveIfError(file2.Read(buf2));
		if (!(buf1 == buf2))
			{
			ret = KErrGeneral;
			}
		totalRead += buf1.Length();
		} 
	while (totalRead < len1 && KErrNone == ret);

	CleanupStack::PopAndDestroy(numPushed);
	return (KErrNone == ret);
	}

