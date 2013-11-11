/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Defines WLAN meta connection provider factory
*
*/

/*
* %version: 5 %
*/

#ifndef __WLANMCPRFACTORY_H__
#define __WLANMCPRFACTORY_H__

#include <comms-infras/ss_metaconnprov.h>

class CWlanMetaConnectionProviderFactory : public ESock::CMetaConnectionProviderFactoryBase
	{
public:
    enum { iUid = 0x2001B2D2 };
	static CWlanMetaConnectionProviderFactory* NewL(TAny* aParentContainer);

protected:
	CWlanMetaConnectionProviderFactory(TUid aFactoryId, ESock::CMetaConnectionFactoryContainer& aParentContainer);
	
	ESock::ACommsFactoryNodeId* CWlanMetaConnectionProviderFactory::DoCreateObjectL(ESock::TFactoryQueryBase& aQuery);
	};

#endif //__WLANMCPRFACTORY_H__

