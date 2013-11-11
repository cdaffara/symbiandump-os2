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
* Description:  Contins ECom related implementaion of
*        CHTIServiceInterfaceinterface class
*
*/


inline CHTIServicePluginInterface* CHTIServicePluginInterface::NewL(
    const TUid aUID)
    {
    TAny* ptr = REComSession::CreateImplementationL(
        aUID,
        _FOFF(CHTIServicePluginInterface,iDtor_ID_Key) );

     return reinterpret_cast<CHTIServicePluginInterface*>( ptr );
    }

inline CHTIServicePluginInterface* CHTIServicePluginInterface::NewL(
    const TDesC8& aMatchString)
    {

    TEComResolverParams resolverParams;
    resolverParams.SetDataType(aMatchString);
    TAny* ptr = REComSession::CreateImplementationL(
        KHTIServiceInterfaceUid,
        _FOFF(CHTIServicePluginInterface,iDtor_ID_Key),
        resolverParams);

     return reinterpret_cast<CHTIServicePluginInterface*>( ptr );
    }

inline CHTIServicePluginInterface::~CHTIServicePluginInterface()
    {
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }


inline void CHTIServicePluginInterface::SetDispatcher(
                MHtiDispatcher* aDispatcher)
    {
    iDispatcher = aDispatcher;
    }

inline TBool CHTIServicePluginInterface::IsBusy()
    {
    return EFalse;
    }
