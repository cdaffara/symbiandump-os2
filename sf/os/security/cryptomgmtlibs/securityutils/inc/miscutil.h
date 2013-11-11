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
 @publishedPartner
 @released
*/

#ifndef __MISCUTIL_H__
#define __MISCUTIL_H__

#include <e32base.h>
#include <f32file.h>

/**
 * This namespace contains common miscellaneous utilitarian functionality meant from use in security code
 */

namespace MiscUtil
	{
	/*
	 * Recursively deletes all folders in the path (as long as they are empty)
	 *
	 * @param aFs		Connected  filesystem session
	 * @param aPath		Fully qualified path to start the recursive delete
	 */
	IMPORT_C void DeletePathIfEmpty(RFs& aFs, const TDesC& aPath);
	/*
	 * Function used to extract lines of text from a buffer. Note that this only works for non-unicode text. It skips
	 * over blank lines and the characters '\n' or '\r' are treated as end-of-line markers.
	 *
	 * @param aBuffer		Input buffer
	 * @param aPos		Starting index into aBuffer (should be within the bounds of zero and length of buffer). 
	 *				It is updated after processing and should be passed in unchanged to correctly identify next line
	 * @param aLine		Set to point to the start of the line identified
	 * @return			ETrue if a line was successfully extracted from the buffer, EFalse if end of buffer reached
	 */
	IMPORT_C TBool ReadNonEmptyLineL(const TDesC8& aBuffer, TInt& aPos, TPtrC8& aLine);
	}

#endif // __MISCUTIL_H__
