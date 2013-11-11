// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <openmax/il/loader/omxilsymbiancomponentif.h>

/**
Constructor of this interface. 
*/
COmxILSymbianComponentIf::COmxILSymbianComponentIf()
	{
	}

/**
Static method to create this interface. 
The actual OMX_COMPONENTTYPE structure should be allocated and populated as part 
of the construction of this interface.	

@return    A pointer to the newly constructed COmxILSymbianComponentIf.

@internalTechnology
*/
COmxILSymbianComponentIf* COmxILSymbianComponentIf::NewL()
    {
    COmxILSymbianComponentIf* self = new (ELeave) COmxILSymbianComponentIf;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
    return self;
    }

/**
Constructs and populates the OMX_COMPONENTTYPE structure encapsulated within this interface. 
*/
void COmxILSymbianComponentIf::ConstructL()
	{
	ipHandle = new(ELeave) OMX_COMPONENTTYPE;
	OMX_ERRORTYPE error = OMX_ErrorInsufficientResources;

	error = ::OMX_ComponentInit(ipHandle);
	if ( OMX_ErrorNone != error )
		{
		if( error == OMX_ErrorInsufficientResources )
			{
			User::Leave(KErrNoMemory);
			}
		else
			{
			User::Leave(KErrGeneral);
			}
		}
	}

/**
Destructor of this interface. 
Deletes the component's structure.  

@internalTechnology
*/
COmxILSymbianComponentIf::~COmxILSymbianComponentIf()
	{
	delete ipHandle;
	}


/**
Gets the valid handle of the already populated OMX_HANDLETYPE structure.

@return The component's handle
*/
OMX_HANDLETYPE COmxILSymbianComponentIf::Handle()
	{
	return ipHandle;
	}
