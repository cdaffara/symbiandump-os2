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
* T_INPUT.H
*
*/


#ifndef __T_UTILS_H__
#define __T_UTILS_H__

#include <e32cons.h>
#include <f32file.h>
#include <e32std.h>
#include <badesca.h>

/**
 * This class provides useful functions to read and parse the input file.
 */
class Utils
	{
	public:
	IMPORT_C static TBool CompareFileL(const TDesC& aFile1, const TDesC& aFile2, TInt &aError);
	};

#endif

