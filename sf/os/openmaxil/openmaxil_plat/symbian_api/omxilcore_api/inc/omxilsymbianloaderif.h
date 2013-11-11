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

#ifndef OMXILSYMBIANLOADERIF_H
#define OMXILSYMBIANLOADERIF_H

#include <openmax/il/core/omxilloaderif.h>

/**
COmxSymbianLoaderIf is the Symbian implementation of the COmxComponentLoaderIf.
The COmxComponentLoaderIf class provides an interface to the Component Loader implementation.
Component Loaders shall be installed as ECOM plug-ins implementing the COmxComponentLoaderIf
to run under Symbian OMX IL Core.

@publishedPartner
*/
NONSHARABLE_CLASS( COmxSymbianLoaderIf ) : public COmxComponentLoaderIf
	{
public:
	static COmxSymbianLoaderIf* NewL();
	~COmxSymbianLoaderIf();

	// from COmxComponentLoaderIf
	OMX_HANDLETYPE Handle();

private:
	void ConstructL();
	COmxSymbianLoaderIf();

private:
	OMX_LOADERTYPE* iHandle;
	};


/**
Macro to facilitate the entry in the ECOM's ImplementationTable of any new Component Loader ECOM implementation UID.

To make easier the task of exposing Component Loaders as ECOM plug-ins, Symbian offers a static
library that facilitates the mapping from the ECOM COmxComponentLoaderIf interface to non-ECOM
implementations.
The steps to wrap up a Component Loader as an ECOM plugin are:
- Provide a .mmp project definition file specifying plugin as Targetype,
- Link the Component Loader implementation against the omxilsymbianloaderif.lib static library.
- Provide a .rss file with interface_uid = KUidOmxILLoaderInterface.
- Declare the OMX_LOADER_ECOM_ENTRYPOINT macro with the particular implementation UID of the Loader.
- Implement the function "OMX_ERRORTYPE OMX_LoaderSetup(OMX_HANDLETYPE aLoader)".  This function
is the entry point routine that Component Loaders have to implement to populate the actual structure
represented by the handle passed as argument.  aLoader = The handle to the OMX_LOADERTYPE structure
to be populated.  Must return an OpenMAX defined error, OMX_ErrorNone if successful.

@param aUid UID of the ECOM plug-in
@see KUidOmxILLoaderInterface
@see OMX_LoaderSetup()
*/
#define OMX_LOADER_ECOM_ENTRYPOINT(aUid) \
	\
	const TImplementationProxy ImplementationTable[] = \
		{\
		IMPLEMENTATION_PROXY_ENTRY((aUid),	COmxSymbianLoaderIf::NewL),\
		};\
	\
	EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)\
		{\
		aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);\
		return ImplementationTable;\
		}

/**
Although this is tagged as InternalAll it is effectively PublishedPartner, however since it is not exported
we are unable to tag it as such.
Any change to this function signature will be a PublishedPartner break.

Entry point routine that Component Loaders have to implement to populate the actual
structure represented by the handle passed as argument.
@param  aLoader
		The handle to the OMX_LOADERTYPE structure to be populated.
@return An OpenMAX defined error. OMX_ErrorNone if successful.

@publishedPartner
*/
OMX_ERRORTYPE OMX_LoaderSetup(OMX_HANDLETYPE aLoader);


#endif /*OMXILSYMBIANLOADERIF_H*/
