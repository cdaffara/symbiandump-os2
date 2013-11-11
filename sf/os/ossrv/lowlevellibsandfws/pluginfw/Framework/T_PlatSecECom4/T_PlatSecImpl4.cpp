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
class CPlatSecImpl4 : public CPlatSecInterface4
{
public:
	static CPlatSecImpl4* NewL();
	virtual ~CPlatSecImpl4();
	virtual TUid ImplId();

private:
	CPlatSecImpl4();
};

CPlatSecImpl4* CPlatSecImpl4::NewL()
	{
	CPlatSecImpl4* self= new (ELeave) CPlatSecImpl4;
	return self;
	}

CPlatSecImpl4::~CPlatSecImpl4()
	{
	}

CPlatSecImpl4::CPlatSecImpl4() :
	CPlatSecInterface4()
	{
	}


TUid CPlatSecImpl4::ImplId()
	{
	return KUidPlatSecECom4Impl;
	}

const TImplementationProxy ImplementationTable[] = 
	{
	//KUidPlatSecECom4Impl.iUid = 0x102026B1
	IMPLEMENTATION_PROXY_ENTRY(0x102026B1, CPlatSecImpl4::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

