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
* Description:  Factory class for IPProxyEngine
*
*/



#ifndef IPPROXYENGINEFACTORY_H
#define IPPROXYENGINEFACTORY_H

// INCLUDE FILES
#include <e32def.h>

// FORWARD DECLARATIONS
class MIPProxyEngine;
class MAbstractConnection;

// CLASS DECLARATION

/**
*  Factory class for IPProxyEngine.
*/
class IPProxyEngineFactory
    {
    public: // New functions

        /**
        * IPProxyEngine factory method.
        * @param aConnection pointer to abstract connection
        * @return Instance of an object that implements IPProxyEngine
        * interface. Ownership transfers to the caller.
        */
        IMPORT_C static MIPProxyEngine* CreateProxyEngineL( MAbstractConnection* aConnection );

    private:

        /**
        * C++ default constructor.
        */
        IPProxyEngineFactory();
    };

#endif      // IPPROXYENGINEFACTORY_H

// End of File
