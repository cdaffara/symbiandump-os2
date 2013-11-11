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
// Keep it as prototype until the end of Diran to allow time for the Symbian Content Pipe OMX IL v1.x solution to be exercised,
// since we know this feature has recently been added to the standard and it is not yet in use.
//
//

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef OMXILSYMBIANCONTENTPIPEIF_H
#define OMXILSYMBIANCONTENTPIPEIF_H

#include <openmax/il/core/omxilcontentpipeif.h>

/**
Bridge to allow content pipes to be loaded as ECOM plugins.

@publishedPartner
*/
NONSHARABLE_CLASS(COmxILSymbianContentPipeIf) : private COmxILContentPipeIf
	{
public:
	static COmxILSymbianContentPipeIf* NewL();
	 ~COmxILSymbianContentPipeIf();
	TInt GetHandle(OMX_HANDLETYPE* aContentPipe);

private:
	COmxILSymbianContentPipeIf();

private:
	/** A handle to the CP_PIPETYPE structure */
	CP_PIPETYPE* iHandle;
	};

/**
	This Macro defines the ECOM plug-in entry point and must be used by the content pipe.

	In order to allow a content pipe to be loaded as an ECOM plugin you must:
	- Link the content pipe library against this component
	- Declare the OMXIL_CONTENTPIPE_ECOM_ENTRYPOINT macro with the content pipe UID
	- Implement the function "TInt ContentPipeInit(OMX_HANDLETYPE* aContentPipe)",
	the content pipe entry point.  This function initialises the CP_PIPETYPE structure
	and returns it in its first argument.  aContentPipe = Returns a pointer to the
	content pipe handle.  Must return KErrNone if successful, otherwise another
	system-wide error code.
	- Implement the function "TInt ContentPipeDeInit(OMX_HANDLETYPE aContentPipe)",
	the content pipe exit point.  This function deinitialises the CP_PIPETYPE structure
	passed as the first argument.  aContentPipe = A pointer to the content pipe handle
	that will be deinitialised.  Must return KErrNone if successful, otherwise another
	system-wide error code.

	@param aUid UID of the ECOM plug-in
	@see ContentPipeInit
	@see ContentPipeDeInit
*/
#define OMXIL_CONTENTPIPE_ECOM_ENTRYPOINT(aUid) \
\
const TImplementationProxy ImplementationTable[] = \
	{\
	IMPLEMENTATION_PROXY_ENTRY((aUid),	COmxILSymbianContentPipeIf::NewL),\
	};\
\
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)\
	{\
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);\
	return ImplementationTable;\
	}

/**
	Although this is tagged as InternalAll it is effectively PublishedPartner, however
	since it is not exported we are unable to tag it as such.
	Any change to this function signature will be a PublishedPartner break.

	Content pipe entry point
	This function initialises the CP_PIPETYPE structure and returns it in its first argument

	@return KErrNone if successful, otherwise another system-wide error code
	@param aContentPipe Returns a pointer to the content pipe handle

	@publishedPartner
*/
TInt ContentPipeInit(OMX_HANDLETYPE* aContentPipe);

/**
	Although this is tagged as InternalAll it is effectively PublishedPartner, however
	since it is not exported we are unable to tag it as such.
	Any change to this function signature will be a PublishedPartner break.

	Content pipe exit point
	This function deinitialises the CP_PIPETYPE structure passed as the first argument

	@return KErrNone if successful, otherwise another system-wide error code
	@param aContentPipe A pointer to the content pipe handle that will be deinitialised

	@publishedPartner
*/
TInt ContentPipeDeInit(OMX_HANDLETYPE aContentPipe);

#endif // OMXILSYMBIANCONTENTPIPEIF_H