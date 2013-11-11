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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef OMXILSYMBIANCOMPONENTIF_H
#define OMXILSYMBIANCOMPONENTIF_H

#include <openmax/il/loader/omxilcomponentif.h>

/**
COmxILSymbianComponentIf is the Symbian implementation of the COmxILComponentIf.
The COmxILComponentIf class provides an interface that the Symbian Component Loader uses to access
Symbian OMX IL Components .


@publishedPartner


*/
NONSHARABLE_CLASS(COmxILSymbianComponentIf) : private COmxILComponentIf
	{
public:
	static COmxILSymbianComponentIf* NewL();
	~COmxILSymbianComponentIf();

	// from COmxILComponentIf
	OMX_HANDLETYPE Handle();

private:
	COmxILSymbianComponentIf();
	void ConstructL();

private:
	OMX_COMPONENTTYPE* ipHandle;
	};

/**
Macro to facilitate the entry in the ECOM's ImplementationTable of any new Component ECOM implementation UID.

To make easier the task of exposing OMX IL Components as ECOM plug-ins, Symbian offers a static
library that facilitates the mapping from the ECOM COmxILComponentIf interface to non-ECOM
implementations.
The steps to wrap up a Component as an ECOM plugin are:
- Provide a .mmp project definition file specifying plugin as Targetype,
- Link the Component implementation against the omxilcomponentif.lib static library.
- Provide a .rss file with interface_uid = KUidOmxILSymbianComponentIf
- Declare the OMXIL_COMPONENT_ECOM_ENTRYPOINT macro with the particular implementation UID of the Component.
- Implement the function "OMX_ERRORTYPE OMX_ComponentInit(OMX_HANDLETYPE aComponent)".  This function
is the entry point routine that OMX IL Components have to implement to populate the actual structure
represented by the handle passed as argument.  aComponent = The handle to the OMX_COMPONENTTYPE
structure to be populated.  Must return an OpenMAX defined error, OMX_ErrorNone if successful.

@param aUid UID of the ECOM plug-in
@see KUidOmxILSymbianComponentIf
@see OMX_ComponentInit()
@see COmxComponentLoaderIf
*/
#define OMXIL_COMPONENT_ECOM_ENTRYPOINT(aUid) \
\
const TImplementationProxy ImplementationTable[] = \
	{\
	IMPLEMENTATION_PROXY_ENTRY((aUid),	COmxILSymbianComponentIf::NewL),\
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

Entry point routine that OMX IL Components have to implement to populate the actual
structure represented by the handle passed as argument.
@param  aComponent
		The handle to the OMX_COMPONENTTYPE structure to be populated.
@return An OpenMAX defined error. OMX_ErrorNone if successful.

@publishedPartner
*/
OMX_ERRORTYPE OMX_ComponentInit(OMX_HANDLETYPE aComponent);


#endif //OMXILSYMBIANCOMPONENTIF_H
