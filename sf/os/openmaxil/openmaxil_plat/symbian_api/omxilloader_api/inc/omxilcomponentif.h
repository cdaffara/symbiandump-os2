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

#ifndef OMXILCOMPONENTIF_H
#define OMXILCOMPONENTIF_H

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

/**
The COmxILComponentIf class provides an interface to the OMX IL Component implementations.

Symbian OS provides a Symbian Component Loader that manages OMX IL Components that are
installed as ECOM plug-ins implementing the COmxILComponentIf. The Symbian Component Loader
creates a list of all the ECOM implementations of this component interface (identified by
the KUidOmxILSymbianComponentIf UID) installed in the system.

@see KUidOmxILSymbianComponentIf
*/
class COmxILComponentIf : public CBase
	{
public:
	~COmxILComponentIf();
	static COmxILComponentIf* CreateImplementationL(TUid aUid);

	/**
	Gets the valid handle of the already populated OMX_COMPONENTTYPE structure.

	@return The component's handle
	*/
	virtual OMX_HANDLETYPE Handle() = 0;

private:

	TUid iDestructorKey;
	};

/**
Static method to load the particular ECOM plugin specified by the implementation UID passed as a parameter.
As part of the creation of this interface the actual OMX_COMPONENTTYPE structure should be allocated and populated.

@param 	aUid
		UID of the ECOM plug-in
@return The pointer to the newly created COmxILComponentIf object

@see KUidOmxILSymbianComponentIf
*/
inline COmxILComponentIf* COmxILComponentIf::CreateImplementationL(TUid aImplementationUid)
	{
	COmxILComponentIf* self = reinterpret_cast<COmxILComponentIf*>( REComSession::CreateImplementationL( aImplementationUid, _FOFF(COmxILComponentIf, iDestructorKey)));
	return self;
	}

/**
Destructor of this interface.
*/
inline COmxILComponentIf::~COmxILComponentIf()
	{
	REComSession::DestroyedImplementation(iDestructorKey);
	}
#endif // OMXILCOMPONENTIF_H
