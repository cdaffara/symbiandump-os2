/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contins ECom related implementaion of CHTICommInterface
*        definition
*
*/


inline CHTICommPluginInterface* CHTICommPluginInterface::NewL()
    {
    return NewL(KCommDefaultImplementation);
    }

inline CHTICommPluginInterface* CHTICommPluginInterface::NewL(
    const TDesC8& aMatchString)
    {

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatchString);
    TAny* ptr = REComSession::CreateImplementationL(
        KHTICommInterfaceUid,
        _FOFF(CHTICommPluginInterface,iDtor_ID_Key),
        resolverParams);

     return reinterpret_cast<CHTICommPluginInterface*>( ptr );
    }

inline CHTICommPluginInterface::~CHTICommPluginInterface()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }
