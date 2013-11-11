/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* Factory class to create the appropriate token interface object according
* to the UID required
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CLIENTFACTORIES_H__
#define __CLIENTFACTORIES_H__

#include "clientsession.h"

class MCTTokenInterface;
class MCTToken;

class CClientInterfaceFactory
{
public:
	static MCTTokenInterface* ClientInterfaceL(TInt32 aUid, MCTToken& aToken, RFileStoreClientSession& aClient);
};

#endif	//	__CLIENTFACTORIES_H__
