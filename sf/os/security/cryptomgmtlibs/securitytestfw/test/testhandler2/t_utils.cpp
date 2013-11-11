/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32file.h>
#include "t_utils.h"


const TInt KBufferSize = 1024;

EXPORT_C TBool Utils::CompareFileL(const TDesC& aFileName1, const TDesC& aFileName2, TInt &aError)
	{
	RFile file1, file2;
	RFs fs;
	TInt err1, err2, size1, size2;
	TBool success = EFalse;

	// checks files are different
	if(aFileName1 == aFileName2)
		{
		aError = KErrNotFound;
		return(EFalse);
		};

	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	// opens first file
	err1 = file1.Open(fs, aFileName1, EFileRead);
	if (err1 != KErrNone)
		{
		CleanupStack::PopAndDestroy(); // fs
		aError = KErrNotFound;
		return(EFalse);
		}
	CleanupClosePushL(file1);

	//opens second file
	err2 = file2.Open(fs, aFileName2, EFileRead);
	if (err2 != KErrNone)
		{
		CleanupStack::PopAndDestroy(2); // fs, file1
		aError = KErrNotFound;
		return(EFalse);
		}
	CleanupClosePushL(file2);

	TBool finished = EFalse;
	HBufC8 *fileBuf1 = HBufC8::NewLC(KBufferSize);
	HBufC8 *fileBuf2 = HBufC8::NewLC(KBufferSize);
	TPtr8 filePtr1(fileBuf1->Des());
	TPtr8 filePtr2(fileBuf2->Des());

	file1.Size(size1);
	file2.Size(size2);

	// compares size, no need to do any more checking if they are different
	if(size1 == size2)
		{
		success = ETrue;
		do
			{
			// reads in buffer from each file
			// cannot rely on max length of descriptor so pass in size of 
			// buffer to read
			err1 = file1.Read(filePtr1,KBufferSize);
			err2 = file2.Read(filePtr2,KBufferSize);
			// checks if an error has occured
			if(err1 != KErrNone && err2 != KErrNone)
				{
				finished = ETrue;
				success = EFalse;
				};
			// executes a binary compare
			if(fileBuf1->Compare(*fileBuf2)!=0)
				{
				// binary compare failed, file are different
				finished = ETrue;
				success = EFalse;
				};
			// checks for EOF
			if(fileBuf1->Length() != KBufferSize)
				finished = ETrue;
			}
		while(!finished);
		};

	
	CleanupStack::PopAndDestroy(5); // fs, file1, file2, fileBuf1, fileBuf2
	return(success);
	};

