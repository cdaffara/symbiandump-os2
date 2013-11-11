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

#include "SuicideStateAccessors.h"
#include "Interface.h"

TInt TSuicideInterface_StateAccessor::InvariantTest(TAny* aTestObject)
	{
	CExampleInterface* interfaceInstance = REINTERPRET_CAST(CExampleInterface*,aTestObject);
	if(interfaceInstance)
		{
		// Validate the internal state variables.
		}
	return KErrNone;
	}

TInt TSuicideInterface_StateAccessor::Internalize(TAny* aTestObject)
	{
	CExampleInterface* interfaceInstance = REINTERPRET_CAST(CExampleInterface*,aTestObject);
	if(interfaceInstance)
		{
		// Set the internal state variables.
		}
	return KErrNone;
	}

TInt TSuicideInterface_StateAccessor::Externalize(TAny* aTestObject)
	{
	CExampleInterface* interfaceInstance = REINTERPRET_CAST(CExampleInterface*,aTestObject);
	if(interfaceInstance)
		{
		// Get the internal state variables.
		}
	return KErrNone;
	}

