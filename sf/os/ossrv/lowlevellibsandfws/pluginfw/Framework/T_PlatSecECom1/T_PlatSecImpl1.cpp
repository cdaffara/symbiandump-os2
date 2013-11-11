// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../T_PlatSecECom/T_PlatSecInterface.h"
#include <ecom/implementationproxy.h> 

/**
CPlatSecInterface1 interface implementation.
@internalComponent
*/
class CPlatSecImpl1 : public CPlatSecInterface1
{
public:
	static CPlatSecImpl1* NewL();
	virtual ~CPlatSecImpl1();
	virtual TUid ImplId();

private:
	CPlatSecImpl1();
};

CPlatSecImpl1* CPlatSecImpl1::NewL()
	{
	CPlatSecImpl1* self= new (ELeave) CPlatSecImpl1;
	return self;
	}

CPlatSecImpl1::~CPlatSecImpl1()
	{
	}

CPlatSecImpl1::CPlatSecImpl1() :
	CPlatSecInterface1()
	{
	}


TUid CPlatSecImpl1::ImplId()
	{
	return KUidPlatSecECom1Impl;
	}

const TImplementationProxy ImplementationTable[] = 
	{
	//KUidPlatSecECom1Impl.iUid = 0x102026AA
	IMPLEMENTATION_PROXY_ENTRY(0x102026AA, CPlatSecImpl1::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

