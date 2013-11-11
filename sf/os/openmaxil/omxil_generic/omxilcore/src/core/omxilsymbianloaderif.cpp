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

#include <openmax/il/core/omxilsymbianloaderif.h>

/**
Entry point routine that Component Loaders have to implement to populate the actual
structure represented by the handle passed as argument.
@param  aLoader
		The handle to the OMX_LOADERTYPE structure to be populated.
@return An OpenMAX defined error. OMX_ErrorNone if successful.

@publishedPartner
@prototype
*/
OMX_ERRORTYPE OMX_LoaderSetup(OMX_HANDLETYPE aLoader);

/**
Constructor of this interface. 
*/
COmxSymbianLoaderIf::COmxSymbianLoaderIf()
	{
	}

/**
Static method to create this interface. 
The actual OMX_LOADERTYPE structure should be allocated and populated as part 
of the construction of this interface.	

@return    A pointer to the newly constructed COmxSymbianLoaderIf.

@internalTechnology
*/
COmxSymbianLoaderIf* COmxSymbianLoaderIf::NewL()
	{
	COmxSymbianLoaderIf* self = new(ELeave)COmxSymbianLoaderIf;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructs and populates the OMX_LOADERTYPE structure encapsulated within this interface.  
*/
void COmxSymbianLoaderIf::ConstructL()
	{
	iHandle = new(ELeave)OMX_LOADERTYPE;
	
	OMX_ERRORTYPE error = OMX_ErrorInsufficientResources;
	error = ::OMX_LoaderSetup(iHandle);
	
	if ( error != OMX_ErrorNone )
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
Deletes this loader's OMX_LOADERTYPE structure. 

@internalTechnology
*/
COmxSymbianLoaderIf::~COmxSymbianLoaderIf()
	{
	delete iHandle;
	}

/**
Gets the valid handle of the already populated OMX_LOADERTYPE struct.

@return The loader's handle
*/
OMX_HANDLETYPE COmxSymbianLoaderIf::Handle()
	{
	return iHandle;
	}

