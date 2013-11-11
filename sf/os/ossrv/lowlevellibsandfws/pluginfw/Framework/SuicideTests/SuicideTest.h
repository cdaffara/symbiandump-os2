// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the definition of the class CSuicideTest
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

#ifndef __SUICIDETEST_H__
#define __SUICIDETEST_H__

#include <e32base.h>

#include "ComponentTester.h"

class CSuicideTest : public CComponentTester
	{
public:
	static CSuicideTest* NewLC(CDataLogger& aDataLogger, 
									MComponentTestObserver& aObserver);

private:
	CSuicideTest(CDataLogger& aDataLogger, 
						MComponentTestObserver& aObserver);
	void ConstructL();
	};

#endif
