// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "MultipleImageExampleInterface.h"
#include <ecom\ImplementationProxy.h>

/**
CMultipleImageInterface1 interface implementation.
@internalComponent
*/
class CMultipleImageImpl : public CMultipleImageInterface1
{
public:
	static CMultipleImageImpl* NewL();
	virtual ~CMultipleImageImpl();
	virtual TUid ImplId();

private:
	CMultipleImageImpl();
};

CMultipleImageImpl* CMultipleImageImpl::NewL()
	{
	CMultipleImageImpl* self= new (ELeave) CMultipleImageImpl;
	return self;
	}

CMultipleImageImpl::~CMultipleImageImpl()
	{
	}

CMultipleImageImpl::CMultipleImageImpl() :
	CMultipleImageInterface1()
	{
	}


TUid CMultipleImageImpl::ImplId()
	{
	return KUidMultipeImageImpl9;
	}

const TImplementationProxy ImplementationTable[] = 
	{
	//KUidMultipeImageImpl9.iUid = 0x10282318
	IMPLEMENTATION_PROXY_ENTRY(0x10282318, CMultipleImageImpl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

