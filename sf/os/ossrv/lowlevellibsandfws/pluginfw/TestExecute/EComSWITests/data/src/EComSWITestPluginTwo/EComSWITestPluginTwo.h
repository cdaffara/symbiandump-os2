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


#include <ecom/ecom.h>
#include "CEComSwiPluginInterfaceDefiniton.h"
#include "ImplementationProxy.h"

// ____________________________________________________________________________
// 

class CEComSwiPluginInterfaceImplementationTwo : public CEComSwiPluginInterfaceDefiniton
{
public:
	static CEComSwiPluginInterfaceImplementationTwo* NewL();
	
	virtual void ReturnInt(TInt& aInt);
	virtual ~CEComSwiPluginInterfaceImplementationTwo();

private:
	CEComSwiPluginInterfaceImplementationTwo();
	void ConstructL();

private:
};  // End of CEComSwiPluginInterfaceImplementationTwo Implemt
