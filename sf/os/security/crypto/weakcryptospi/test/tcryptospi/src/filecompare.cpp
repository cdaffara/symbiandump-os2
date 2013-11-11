/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/
#include "filecompare.h"

#include "cryptospi/cryptospidef.h"
#include <e32std.h>
#include <e32def.h>
#include <f32file.h>

const TInt KCompareBlockSize = 0xff;

//const TInt KCompareEqual = 0;
const TInt KCompareGreaterThan = 1;
const TInt KCompareLessThan = -1;

// a simple utility class to compare 2 files, returns
TInt TFileCompare::CompareL(TPtrC aFile1Path, TPtrC aFile2Path)
	{
	// assume is all well, compare always 
	TInt retVal = 0;

	RFs 	rFs;
	RFile	file1;
	RFile	file2;

	User::LeaveIfError(rFs.Connect());
	CleanupClosePushL(rFs);
	User::LeaveIfError(file1.Open(rFs, aFile1Path, EFileRead));
	CleanupClosePushL(file1);
	User::LeaveIfError(file2.Open(rFs, aFile2Path, EFileRead));
	CleanupClosePushL(file2);
	
	TInt file1Size;
	file1.Size(file1Size); 
	
	TInt file2Size;
	file2.Size(file2Size);

	// bail out if the sizes of the 2 files are different
	if(file1Size == file2Size)
		{
				
		TBuf8<KCompareBlockSize> tempBuf1;
		TBuf8<KCompareBlockSize> tempBuf2;
	
		TInt compareBlockSize;
		TInt loopCount;
		
		if(file1Size > KCompareBlockSize)
		{
			compareBlockSize = KCompareBlockSize;
			loopCount = file1Size / compareBlockSize;
		}
		else
		{
			loopCount = 1;
			compareBlockSize = file1Size;
		}
			
	
		// read through the files comparing each block
		for(TInt i = 0; i < loopCount; i++)
			{
			
			file1.Read(tempBuf1, compareBlockSize);
			file2.Read(tempBuf2, compareBlockSize);
			
			// remember, 0 is return value for a successful comparison
			
			retVal = tempBuf1.Compare(tempBuf2);
			if( retVal != 0 )
				{
				//if files fail to match, break out of the loop
				break;
				}		
			}
		}
	else
		{
		if(file1Size > file2Size)
			retVal = KCompareGreaterThan;
		else
			retVal = KCompareLessThan;
		}

	CleanupStack::PopAndDestroy(&file2);
	CleanupStack::PopAndDestroy(&file1);
	
	CleanupStack::PopAndDestroy(&rFs);
	
	return retVal;		
	}
