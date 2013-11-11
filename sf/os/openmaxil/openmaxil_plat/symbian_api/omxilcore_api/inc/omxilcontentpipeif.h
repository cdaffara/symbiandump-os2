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

#ifndef OMXILCONTENTPIPEIF_H
#define OMXILCONTENTPIPEIF_H

#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <openmax/il/khronos/v1_x/OMX_ContentPipe.h>

/**
	Bridge to allow content pipes to be loaded as an ECOM plugin.
	Inheritance from this class allows it
*/
class COmxILContentPipeIf : public CBase
	{
public:
	~COmxILContentPipeIf();
	static COmxILContentPipeIf* CreateImplementationL(TUid aUid);

	/**
		Gets a valid content pipe handle

		@return KErrNone if successful, otherwise another system-wide error codes
		@param aContentPipe Returns a pointer to the content pipe handle
	*/
	virtual TInt GetHandle(OMX_HANDLETYPE* aContentPipe) = 0;

private:
	/** A 32-bit instance key returned by the ECom framework */
	TUid iDestructorKey;
	};

/**
	Destructor
*/
inline COmxILContentPipeIf::~COmxILContentPipeIf()
 	{
 	REComSession::DestroyedImplementation(iDestructorKey);
 	}

/**
	This function loads the ECOM plug-in specified by the UID passed as argument and
	return a newly created COmxContentPipeIf object

	@return Newly created COmxContentPipeIf object
	@param aImplementationUid UID of the ECOM plug-in
*/
inline COmxILContentPipeIf* COmxILContentPipeIf::CreateImplementationL(TUid aImplementationUid)
	{
	COmxILContentPipeIf* self = reinterpret_cast<COmxILContentPipeIf*>(REComSession::CreateImplementationL(aImplementationUid, _FOFF(COmxILContentPipeIf, iDestructorKey)));
	return self;
	}

#endif // OMXILCONTENTPIPEIF_H
