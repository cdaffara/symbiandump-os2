/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TE_PERFORMANCETESTS_H__
#define __TE_PERFORMANCETESTS_H__

#include <hal.h>
#include "te_ezlibeziptests.h"

/* Performance tests */
_LIT(KTestDeflatePerformanceL, "TestDeflatePerformanceL");
_LIT(KTestInflatePerformanceL, "TestInflatePerformanceL");
_LIT(KTestGZipDeflatePerformanceL, "TestGZipDeflatePerformanceL");
_LIT(KTestGZipInflatePerformanceL, "TestGZipInflatePerformanceL");
_LIT(KTestZipArchiveDecompressPerformanceL, "TestZipArchiveDecompressPerformanceL");

/* Information Messages */
_LIT(KEZlib2Failed, "EZlib2 has failed to perform!");
_LIT(KEZlibTimeMilliseconds, "EZLib took %f milliseconds.");
_LIT(KEZlib2TimeMilliseconds, "EZLib2 took %f milliseconds.");
_LIT(KEZlib2FasterMilliseconds, "EZlib2 was %fms (%f%%) faster than EZlib.");
_LIT(KEZlib2SlowerMilliseconds, "EZlib2 was %fms (%f%%) slower than EZlib.");

class CPerformanceTests : public CEZlibEZipTests
	{
public:
	CPerformanceTests(const TDesC &aStepName);
	virtual TVerdict doTestStepL();
private:
	static const TInt percentPerformanceDiff = 10;
private:
	TReal FastCountToMilliseconds(const TUint32 aFastCount);
	TBool IsEZlib2InPerformanceRange(const TUint32 aEZlibTestTime, const TUint32 aEZlib2TestTime, const TInt aPercentPerformanceDiff = percentPerformanceDiff);
	void PrintTestTimeInfo(const TUint32 aEZlibTestTime, const TUint32 aEZlib2TestTime);
	TVerdict TestDeflatePerformanceL();
	TVerdict TestInflatePerformanceL();
	TVerdict TestGZipDeflatePerformanceL();
	TVerdict TestGZipInflatePerformanceL();
	TVerdict TestZipArchiveDecompressPerformanceL();
	};

#endif /*__TE_PERFORMANCETESTS_H__*/
