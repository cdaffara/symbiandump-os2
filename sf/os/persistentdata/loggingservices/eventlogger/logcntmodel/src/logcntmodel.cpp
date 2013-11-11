// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <logcntmodel.h>

/** Signals the destruction of the plug-in to ECom.
*/
EXPORT_C CLogCntModel::~CLogCntModel()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

/** Attempts to load a plugin implementation of the contacts matching functionality.
	
@return A pointer to the implementation
	
@leave KErrNotFound	No implementation was found. 
@capability ProtServ WriteDeviceData ReadDeviceData ReadUserData WriteUserData 
*/
EXPORT_C CLogCntModel* CLogCntModel::NewL()
	{	
	// Use default resolver parameters.
	const TEComResolverParams noResolverParams;		
    TAny* ptr = REComSession::CreateImplementationL(KUidEComLogCntInterface,
													_FOFF(CLogCntModel,iDtor_ID_Key),
													noResolverParams);
	return reinterpret_cast<CLogCntModel*>(ptr); 
	}


