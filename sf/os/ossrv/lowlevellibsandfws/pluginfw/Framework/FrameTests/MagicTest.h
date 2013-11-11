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
// This file contains the definition of the class CExampleInterface_ComponentTester
// This file comment is for DOxygen only and is ignored by EDoc.
// 
//

#ifndef __MAGICTEST_H__
#define __MAGICTEST_H__

#include <e32base.h>

#include "ComponentTester.h"

class CExampleInterface_ComponentTester : public CComponentTester
	{
public:
	static CExampleInterface_ComponentTester* NewLC(CDataLogger& aDataLogger, 
									MComponentTestObserver& aObserver);

private:
	CExampleInterface_ComponentTester(CDataLogger& aDataLogger, 
						MComponentTestObserver& aObserver);
	void ConstructL();
	};

#endif
