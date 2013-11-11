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
class CPlatSecImpl3 : public CPlatSecInterface3
{
public:
	static CPlatSecImpl3* NewL();
	virtual ~CPlatSecImpl3();
	virtual TUid ImplId();

private:
	CPlatSecImpl3();
};

CPlatSecImpl3* CPlatSecImpl3::NewL()
	{
	CPlatSecImpl3* self= new (ELeave) CPlatSecImpl3;
	return self;
	}

CPlatSecImpl3::~CPlatSecImpl3()
	{
	}

CPlatSecImpl3::CPlatSecImpl3() :
	CPlatSecInterface3()
	{
	}


TUid CPlatSecImpl3::ImplId()
	{
	return KUidPlatSecECom3Impl;
	}

const TImplementationProxy ImplementationTable[] = 
	{
	//KUidPlatSecECom3Impl.iUid = 0x102026AE
	IMPLEMENTATION_PROXY_ENTRY(0x102026AE, CPlatSecImpl3::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

