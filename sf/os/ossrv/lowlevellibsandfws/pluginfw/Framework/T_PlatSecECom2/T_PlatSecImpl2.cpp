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
CPlatSecInterface2 interface implementation.
@internalComponent
*/
class CPlatSecImpl2 : public CPlatSecInterface2
{
public:
	static CPlatSecImpl2* NewL();
	virtual ~CPlatSecImpl2();
	virtual TUid ImplId();

private:
	CPlatSecImpl2();
};

CPlatSecImpl2* CPlatSecImpl2::NewL()
	{
	CPlatSecImpl2* self= new (ELeave) CPlatSecImpl2;
	return self;
	}

CPlatSecImpl2::~CPlatSecImpl2()
	{
	}

CPlatSecImpl2::CPlatSecImpl2() :
	CPlatSecInterface2()
	{
	}


TUid CPlatSecImpl2::ImplId()
	{
	return KUidPlatSecECom2Impl;
	}

const TImplementationProxy ImplementationTable[] = 
	{
	//KUidPlatSecECom2Impl.iUid = 0x102026AC
	IMPLEMENTATION_PROXY_ENTRY(0x102026AC, CPlatSecImpl2::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

