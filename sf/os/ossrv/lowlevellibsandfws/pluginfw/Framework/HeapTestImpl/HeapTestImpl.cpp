// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// HeapTestImpl.cpp
// 
//


#include "HeapTestInterface.h"
#include "ImplementationProxy.h"

class CHeapTest : public CHeapTestInterface
{
public:
	static CHeapTest* NewL();
	virtual ~CHeapTest();
	TUid ImplId();
	void DisplayMessage();


private:
	CHeapTest();

private:
	enum EEE {KSize = 100};
	TUint8 iNooo[KSize];

};

// __________________________________________________________________________
// Implementation

CHeapTest* CHeapTest::NewL()
	{
	CHeapTest* self = new (ELeave) CHeapTest();
	return self;
	}

CHeapTest::~CHeapTest()
	{
	}

CHeapTest::CHeapTest() :
	CHeapTestInterface()
	{
	for(TInt8 i=0;i<KSize;i++)
		{
		iNooo[i] = i;
		}
	}

TUid CHeapTest::ImplId()
	{
	TUid idVal = {0x101FE393};
	return idVal;
	}

void CHeapTest::DisplayMessage()
{
	RDebug::Printf("Implementation of DLL with version 1");
}


// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x101FE393,CHeapTest::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

