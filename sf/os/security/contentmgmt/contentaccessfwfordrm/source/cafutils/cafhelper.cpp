/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "cafhelper.h"
#include "cafhelperinterface.h"
#include <e32def.h>
#include <e32uid.h>

using namespace ContentAccess;

EXPORT_C CCAFHelper* CCAFHelper::NewL()
	{
	CCAFHelper* self = CCAFHelper::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CCAFHelper* CCAFHelper::NewLC()
	{
	CCAFHelper* self = new (ELeave) CCAFHelper;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CCAFHelper::CCAFHelper()
	{
	}

void CCAFHelper::ConstructL()
	{
	// Dynamically load the DLL.
	TUidType uidType(KDynamicLibraryUid, KCAFHelperInterfaceUID);
	
	User::LeaveIfError(iLibrary.Load(KCAFHelperLibraryName, uidType));
	
	// Function at ordinal 1 creates new CCAFAgentHelper.
	TLibraryFunction entryFunc = iLibrary.Lookup(1);
	if (entryFunc == NULL)
    	{
        iLibrary.Close();
        User::Leave(KErrBadLibraryEntryPoint);
    	}
		
	// Call the function to create new CCAFAgentHelper.
	iHelper = (MCAFHelperInterface*)entryFunc();
	}

EXPORT_C MCAFHelperInterface& CCAFHelper::operator()() const
	{
	return *iHelper;
	}
	
CCAFHelper::~CCAFHelper()
	{
	iHelper->Release();
	iLibrary.Close();
	}
	
