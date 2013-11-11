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


#ifndef __T_INPUTEXTRA_H__
#define __T_INPUTEXTRA_H__

#include <e32cons.h>
#include <f32file.h>
#include <e32std.h>
#include <badesca.h> 
#include "t_input.h"

/**
 * This class provides useful functions to read and parse the input file.
 */
class InputExtra : public Input
	{
public: 
	/*
	Reads multilpe files into a 8 bit buffer array
	CDesCArray& aFilenames	[IN] - Array of filename to read
	const TDesC& aPath		[IN] - (Optional) Path of files, if missing path must be included in aFilenames
	RFs& aFs				[IN] - Reference to the file system handle class

	Returns
	HBufC8*		-  Pointer to the 8 bit file data
	*/
	IMPORT_C static HBufC8* ReadFilesLC(CDesCArray& aFilenames, RFs& aFs);
	IMPORT_C static HBufC8* ReadFilesLC(CDesCArray& aFilenames, const TDesC& aPath, RFs& aFs);
	/*
	Reads a ascii hex file into a binary HBufC i.e. a file containing "FFAA11" will be converted into a 3 byte buffer

	Parameters
	const TDesC& aFilename 	[IN] - Name of file including path to read
	RFs &aFs			[IN] - Reference to the file system handle class

	Returns
	HBufC8*		-  Pointer to the 8 bit file data
	*/
	IMPORT_C static HBufC8* ReadHexFileLC(const TDesC& aFilename, RFs& aFs);
	/**
	 * This function reads an entire Unicode file and returns it as an HBufC.
	 * This function only works for Unicode builds of EPOC.
	 */
	IMPORT_C static HBufC* ReadFile16LC(const TDesC& aFilename, RFs& aFS);
	};

#endif
