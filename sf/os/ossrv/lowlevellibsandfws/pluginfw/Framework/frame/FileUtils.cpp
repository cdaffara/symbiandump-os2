// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Utils.cpp
// Implementation for classes in Utils.h
// 
//

/**
 @file
*/

#include <f32file.h>

#include "FileUtils.h"



/**
Set the given filename and path to be relative to the given drive.

It is assumed that the input filepath is of the form:

	?:*   (? = any single character, * = any number of characters)

The ? will be replaced by the letter corresponding to the given drive number.

@param aFileName The input filepath. On completion this will contain the 
					resulting path.
@param aDrive	The drive number to set to.
*/
void TEComFileUtils::SetToDrive(TDes& aFileName, TInt aDrive)
	{
	aFileName[0] = 'A' + static_cast<TInt>(aDrive);
	}

	
	

