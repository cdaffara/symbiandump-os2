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
// Destructor of this interface.
// 
//

inline COmxComponentLoaderIf::~COmxComponentLoaderIf()
 	{
 	REComSession::DestroyedImplementation(iDestructorKey);	
 	}

/**
Static method to load the particular ECOM plugin specified by the implementation UID passed as a parameter. 
As part of the creation of this interface the actual OMX_LOADERTYPE structure should be allocated and populated.	

@param 	aUid 
		UID of the ECOM plug-in
@return The pointer to the newly created COmxComponentLoaderIf object

@see KUidOmxILLoaderInterface
*/
inline COmxComponentLoaderIf* COmxComponentLoaderIf::CreateImplementationL(TUid aUid)
	{
	COmxComponentLoaderIf* self = reinterpret_cast<COmxComponentLoaderIf*>(REComSession::CreateImplementationL(aUid, _FOFF(COmxComponentLoaderIf, iDestructorKey)));
	return self;
	}
