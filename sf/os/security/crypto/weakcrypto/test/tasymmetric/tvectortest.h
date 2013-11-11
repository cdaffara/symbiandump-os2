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
* Vector tests for asymmetric ciphers
*
*/


#ifndef __TVECTOR_H__
#define __TVECTOR_H__

#include "t_testaction.h"
#include <e32std.h>
#include <asymmetric.h>
#include <asymmetrickeys.h>
#include <random.h>

/**
 * Base class for vector tests that implements the virtual methods they all have
 * in common.
 */

class CVectorTest : public CTestAction
{
public:
	virtual void PerformAction(TRequestStatus& aStatus);
protected:
    /** Constructor for derived classes. */
    CVectorTest(CConsoleBase& aConsole, Output& aOut);

//	xxx should this be protected?
    virtual ~CVectorTest();
	virtual void ConstructL(const TTestActionSpec& aTestActionSpec);
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformPostrequisite(TRequestStatus& aStatus);
	virtual void DoReportAction(void);
	virtual void DoCheckResult(TInt);
protected:
	virtual void DoPerformActionL() = 0;
	virtual void DoPerformanceTestActionL() = 0;
protected:
	TInt iPerfTestIterations;
private:
	TInt iActionResult;
	TInt iKeyBits;
};

#endif
