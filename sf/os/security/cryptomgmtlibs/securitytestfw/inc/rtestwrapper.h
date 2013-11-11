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
* Wrapper for RTest which logs to a file, counts failures (without panicing) and
* generates a final result line the ONB will parse.
*
*/




/**
 @file
 @internalTechnology
 @test
*/
#ifndef RTESTWRAPPER_H
#define RTESTWRAPPER_H
#include <e32test.h>

class RTestWrapper : public RTest
	{
public:
	IMPORT_C RTestWrapper(const TDesC &aTitle,TInt aThrowaway,const TText* anOtherThrowaway);
	IMPORT_C void operator()(TInt aResult,TInt aLineNum,const TText* aFileName);

	IMPORT_C void Title(const TDesC &aFileName);
	IMPORT_C void Start(const TDesC &aHeading);
	IMPORT_C void Next(const TDesC &aHeading);
	IMPORT_C void End();

private:
	/// Test group nest level. Incremented by Start, decremented by End
	TUint iNestLevel;

	/// Incremented whenever Start or Next is called
	TUint iTestCount;

	/// Set if current test fails, and iFailedCount incremented.
	/// Cleared whenever Start or Next is Called.
	TBool iThisTestFailed;

	/// Count of failed tests
	TUint iFailedCount;

	TBuf<0x40> iTitle;
	};


// End of file
#endif
