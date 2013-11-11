// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TlsData.h"
#include "LoadManager.h"

/**
Standard phase-one construction method for creation of a single instance of 
CGlobalData class.
If the call succeeds, a pointer to the created singleton will be stored in TLS.
@leave KErrNoMemory Out of memory
@return	A pointer to the created CGlobalData object.
@internalComponent
*/
CGlobalData* CGlobalData::NewL()
	{
	CGlobalData* self = CGlobalData::Instance();
	if(!self)
		{
		self = new (ELeave) CGlobalData;
		CleanupStack::PushL(self);
		self->ConstructL();
		User::LeaveIfError(Dll::SetTls(self));
		CleanupStack::Pop(self);
		}
	return self;
	}

/**
The function returns a pointer to the single CGlobalData instance or NULL if it
is not created yet.
@return	A pointer to the single CGlobalData instance. It might be NULL.
@internalComponent
*/
CGlobalData* CGlobalData::Instance()
	{
	return static_cast <CGlobalData*> (Dll::Tls());
	}

CGlobalData::CGlobalData()
	{
	}

/**
Removes from memory the controlled CLoadManager instance.
Closes the session connection with the ECOM server.
Sets TLS to NULL.
*/
CGlobalData::~CGlobalData()
	{
	delete iLoadManager;
	iEComSession.ReallyClose();
	(void)Dll::SetTls(NULL);
	}
		
/**
Standard phase-two construction method for creation of CGlobalData objects.
It will create single REComSession instance and single CLoadManager instance.
@leave KErrNoMemory Out of memory
*/
void CGlobalData::ConstructL()
	{
	iEComSession.ConstructL();
	iLoadManager = CLoadManager::NewL();
	}
