/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file CTEFLogger.h
*/

#ifndef __TEF_LOGGER__
#define __TEF_LOGGER__

#include <e32base.h>
#include <test/testexecutelogger.h>

class CTEFLogger
/**
@internalComponent
@test
*/
	{
public:
	static void LogResult( const TDesC& aTestName, TInt aResult, CTestExecuteLogger& aLogger );
	static void LogTraverse( const TDesC& aSuiteName, CTestExecuteLogger& aLogger );
	};

#endif // __TEF_LOGGER__

