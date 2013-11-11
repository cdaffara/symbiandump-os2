// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ULogger plug-in base class implementation
// 
//

/**
 @file
 @publishedPartner
 @prototype
*/


namespace Ulogger
{
const TUid KCPluginUid ={0x102836C9};
	

inline CPlugin* CPlugin::NewL(const TDesC8& aCue)
	{
	
	TEComResolverParams resolverParams;
	resolverParams.SetDataType(aCue);
	resolverParams.SetWildcardMatch(ETrue);
	TAny* pluginBase = REComSession::CreateImplementationL(KCPluginUid,_FOFF(CPlugin,iDtor_ID_Key),NULL,resolverParams);
	return (reinterpret_cast<CPlugin*>(pluginBase));
	}
	
	
inline CPlugin::~CPlugin()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

} // namespace
