/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




/**
 @file
 @publishedPartner
 @released
*/

#include <ecom/ecom.h>

// -----------------------------------------------------------------------------
// CFeatureInfoPlugin::~CFeatureInfoPlugin
// Destructor informs ECom that implementation has been destroyed.
// -----------------------------------------------------------------------------
//
inline CFeatureInfoPlugin::~CFeatureInfoPlugin()
    {
    REComSession::DestroyedImplementation( iDestructorIDKey );
    }

// -----------------------------------------------------------------------------
// CFeatureInfoPlugin::NewL
// Factory method to create implementation. Implementation is found via ECom.
// -----------------------------------------------------------------------------
//
inline CFeatureInfoPlugin* CFeatureInfoPlugin::NewL(
    TUid aImplementationUid,
    MFeatureInfoPluginCallback& aResponseCallback )
    {
    TAny* interface = REComSession::CreateImplementationL(
        aImplementationUid,
        _FOFF( CFeatureInfoPlugin, iDestructorIDKey ) );

    CFeatureInfoPlugin* service = reinterpret_cast<CFeatureInfoPlugin*>( interface );

    // Set response callback pointer
    service->iResponseCallback = &aResponseCallback;

    return service;
    }

// End of File
