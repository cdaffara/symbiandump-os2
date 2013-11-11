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

#include <e32base.h>
#include <openmax/il/core/omxilsymbiancontentpipeif.h>
   
/**
	Content pipe entry point
	This function initialises the CP_PIPETYPE structure and return it in its first argument

	@return KErrNone if successful, otherwise another system-wide error codes
	@param aContentPipe Returns a pointer to the content pipe handle

	@publishedPartner
	@prototype
*/
TInt ContentPipeInit(OMX_HANDLETYPE* aContentPipe);

/**
	Content pipe exit point
	This function deinitialises the CP_PIPETYPE structure passed as first argument

	@return KErrNone if successful, otherwise another system-wide error codes
	@param aContentPipe A pointer to the content pipe handle that will be deinitialised

	@publishedPartner
	@prototype
*/
TInt ContentPipeDeInit(OMX_HANDLETYPE aContentPipe);

/**
	Constructor
*/
COmxILSymbianContentPipeIf::COmxILSymbianContentPipeIf()
 	{
 	}

/** 
	Constructs a new instance of COmxILSymbianContentPipeIf.
	@return    COmxILSymbianContentPipeIf*	A pointer to the newly constructed COmxILSymbianContentPipeIf.
 
	@internalTechnology
*/
COmxILSymbianContentPipeIf* COmxILSymbianContentPipeIf::NewL()
    {
    return new(ELeave)COmxILSymbianContentPipeIf;
    }

/**
	Destructor

	@internalTechnology
*/
COmxILSymbianContentPipeIf::~COmxILSymbianContentPipeIf()
 	{
 	ContentPipeDeInit(iHandle);
 	}

TInt COmxILSymbianContentPipeIf::GetHandle(OMX_HANDLETYPE* aContentPipe)
	{
	if(iHandle)
		{	// Content Pipe already created
		*aContentPipe = iHandle;
		return KErrNone;
		}
	
	TInt err = ContentPipeInit(aContentPipe);
	if(err == KErrNone)
		{
		iHandle = reinterpret_cast<CP_PIPETYPE*>(*aContentPipe);
		}
	return err;
	}
