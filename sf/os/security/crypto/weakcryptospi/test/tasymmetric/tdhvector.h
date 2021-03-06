/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __TDHVECTOR_H__
#define __TDHVECTOR_H__

#include "tvectortest.h"

/**
 * Runs DH vectors.
 */
class CDHVector : public CVectorTest
    {
public:
	static CTestAction* NewL(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	static CTestAction* NewLC(RFs& aFs, CConsoleBase& aConsole,
		Output& aOut, const TTestActionSpec& aTestActionSpec);
	virtual ~CDHVector();
protected:
	virtual void DoPerformActionL();
	virtual void DoPerformPrerequisite(TRequestStatus& aStatus);
	virtual void DoPerformanceTestActionL();
private:
	CDHVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut);
	void ConstructL(const TTestActionSpec& aTestActionSpec);

	TInt iActionMode;
	HBufC8 *iN, *iG, *iX, *iNd, *iGd;
	RInteger iN1, iN2, iN3, iG1, iG2, iG3, iX1, iX2;
    };

#endif

