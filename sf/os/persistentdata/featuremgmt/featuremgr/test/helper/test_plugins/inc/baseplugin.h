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
* Description:
*
*/



#ifndef __FEATMGRBASEPLUGIN_H_
#define __FEATMGRBASEPLUGIN_H_

#include <featmgr/featureinfoplugin.h>
#include "plugintimer.h"


class CFeatMgrBasePlugin : public CFeatureInfoPlugin,
                       public MPluginTimerCallback
    {
    public:
    		
        virtual ~CFeatMgrBasePlugin ();

        /**
        * Method to invoke a particular command in the plugin
        * @param aCommandId Command ID
        * @param aTransId   Transaction ID
        * @param aData      Data associated with command
        */
        virtual void ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                      const TUint8 aTransId,
                                      TDesC8& aData );
        
        // From MPluginTimerCallback
        virtual void GenericTimerFiredL( MFeatureInfoPluginCallback& aService,
                                const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                const TUint8 aTransId,
                                TInt aRetVal );
        
                                
                                
    protected: // Construction

        CFeatMgrBasePlugin();
        void ConstructL();
        
       CPluginTimer* iTimer;
 
    };

#endif // __FEATMGRBASEPLUGIN_H_
