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

#ifndef OMXILLOADERIF_H
#define OMXILLOADERIF_H

#include <openmax/il/loader/OMX_Loader.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>

/**
The COmxComponentLoaderIf class provides an interface to the Component Loader implementation.
Component Loaders shall be installed as ECOM plug-ins implementing the COmxComponentLoaderIf
to run under Symbian OMX IL Core.
Symbian OMX IL Core creates a list of all the ECOM implementations of this Component Loader
interface installed in the system.

@see KUidOmxILLoaderInterface
*/
class COmxComponentLoaderIf : public CBase
	{
public:
	inline virtual ~COmxComponentLoaderIf();
	inline static COmxComponentLoaderIf* CreateImplementationL(TUid aUid);

	/**
	Gets the valid handle of the already populated OMX_LOADERTYPE structure.

	@return The loader's handle
	*/
	virtual OMX_HANDLETYPE Handle() = 0;

private:
	TUid iDestructorKey;
	};

#include <openmax/il/core/omxilloaderif.inl>

#endif /*OMXILLOADERIF_H*/
