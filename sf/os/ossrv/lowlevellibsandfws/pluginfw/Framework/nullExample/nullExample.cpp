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
//

// Includes
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <ecom/implementationproxy.h>
#include    "nullExample.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CInterfaceImpl::CInterfaceImpl():
	CExampleInterface()
    {
    
    }

// Two-phased constructor.
CInterfaceImpl* CInterfaceImpl::NewL(TAny* aConstructionParams)
    {
    __ASSERT_ALWAYS(aConstructionParams == NULL, User::Leave(KErrArgument));
    //return new (ELeave) CInterfaceImpl;
    CInterfaceImpl* self= new (ELeave) CInterfaceImpl();
	return self;
    }

// Destructor
CInterfaceImpl::~CInterfaceImpl()
    {
    }

TInt CInterfaceImpl::Version() const
    {
    return 2;
    }
    
void CInterfaceImpl::DoMethodL()
	{
	// Access TLS to ensure it has been set properly
	REINTERPRET_CAST(TInt*, Dll::Tls());
	}
	
TInt CInterfaceImpl::FireAndForget()
	{
	TRAPD(error,DoMethodL());
	return error;			// Always KErrNotSupported
	}
	
TUid CInterfaceImpl::ImplId()
	{
	TUid idVal = {0x01aa500c};
	return (idVal);
	}
	
// Provide the CActive overloads
void CInterfaceImpl::RunL()
	{
	// Do nothing : should never be called
	__ASSERT_DEBUG(EFalse,User::Invariant());
	}

void CInterfaceImpl::DoCancel()
	{
	// Do nothing
	}

TInt CInterfaceImpl::RunError(TInt aError)
	{
	return aError;
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy KImplTable[] = 
    {
    //{{0x01aa500c},	CInterfaceImpl::NewL}
    IMPLEMENTATION_PROXY_ENTRY(0x01aa500c, CInterfaceImpl::NewL)
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(KImplTable) / sizeof(TImplementationProxy);
    return KImplTable;
    }

//  End of File  
