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
* Description:  IPProxyEngine panic codes
*
*/



#ifndef IPPROXYENGINE_PAN
#define IPPROXYENGINE_PAN

enum TIPProxyEnginePanics
    {
    IPProxyEngineGeneral = 1,
    IPProxyEngineNullSocket = 2,
    };


_LIT( KPanicText, "IPProxyEngine" );


inline void Panic( TIPProxyEnginePanics aReason )
    {
    User::Panic( KPanicText, aReason );
    }


#endif // IPPROXYENGINE_PAN
