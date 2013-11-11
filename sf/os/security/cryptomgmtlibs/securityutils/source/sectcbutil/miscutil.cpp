/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Contains miscellaneous utility functions that can be used by any code in security component
*
*/


/**
 @file
*/

#include "miscutil.h"

namespace MiscUtil
{
/*
 * Function used to extract lines of text from a buffer. Note that this only works for non-unicode text. It skips
 * over blank lines and the characters '\n' or '\r' are treated as end-of-line markers.
 *
 * @param aBuffer		Input buffer
 * @param aPos		Starting index into aBuffer
 * @return			ETrue if a line was successfully extracted from the buffer, EFalse if end of buffer reached
 */

EXPORT_C TBool ReadNonEmptyLineL(const TDesC8& aBuffer, TInt& aPos, TPtrC8& aLine)
	{
	TBool lineIdentified = ETrue;
	aLine.Set(NULL, 0);

	TInt bufferLength = aBuffer.Length();
	__ASSERT_ALWAYS(aPos >=0 && aPos <= bufferLength, User::Leave(KErrArgument));

	// Skip blank lines
	while (aPos < bufferLength)
		{
		TChar  c = aBuffer[aPos];
		if (c != '\r' && c != '\n')
			{
			break;
			}
		aPos++;
		}

	// Find the position of the next delimter
	TInt endPos = aPos;
	while (endPos < bufferLength)
		{
		TChar c = aBuffer[endPos];
		if (c == '\n' || c == '\r') 	// Find end of line
			{
			break;
			}
		endPos++;
		}

	if (endPos != aPos)
		{
		TInt tokenLen = endPos - aPos;
		aLine.Set(&aBuffer[aPos], tokenLen);
		}
	else
		{
		lineIdentified = EFalse; // End of buffer
		}

	aPos = endPos;
	return lineIdentified;
	}

/*
 * Recursively deletes all folders in the path (as long as they are empty)
 *
 * @param aFs		Connected  filesystem session
 * @param aPath		Fully qualified path to start the recursive delete
 */
EXPORT_C void DeletePathIfEmpty(RFs& aFs, const TDesC& aPath)
	{
	TParse path;
	path.Set(aPath,NULL,NULL);

	if (path.PathPresent())
		{
		while ((aFs.RmDir(path.DriveAndPath()) == KErrNone) && (path.PopDir() == KErrNone))
			;
		}		
	}

}	// namespace MiscUtil
