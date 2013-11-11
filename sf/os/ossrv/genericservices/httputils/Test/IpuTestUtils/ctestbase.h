// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
 


#ifndef		__CTESTBASE_H__
#define		__CTESTBASE_H__

#include <e32base.h>
#include "IpuTestUtils.h"

class CTestBase : public CBase
	{
public:
	virtual void DoTestsL() = 0;
	IMPORT_C static void MemoryLeakTestL(CTestBase& aTest, CIpuTestHarness& aTestHarness);
	};

#endif //	__CTESTBASE_H__
