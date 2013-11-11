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



#ifndef __SIMPLE_FEATURE_INFO_PLUGIN_H__
#define __SIMPLE_FEATURE_INFO_PLUGIN_H__

#include <featmgr/featureinfoplugin.h>
#include "plugintimer.h"


class CSimpleFeatureInfoPlugin: public CFeatureInfoPlugin,
								public MPluginTimerCallback
    {
    public:
    	static CSimpleFeatureInfoPlugin* NewL();	
        
        /**
        * Method to invoke a particular command in the plugin
        * @param aCommandId Command ID
        * @param aTransId   Transaction ID
        * @param aData      Data associated with command
        */
        virtual void ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                      const TUint8 aTransId,
                                      TDesC8& aData );
        
        // From CActive
        virtual ~CSimpleFeatureInfoPlugin();
        // From MPluginTimerCallback
        void GenericTimerFiredL();                        
                                
    protected: 
        CSimpleFeatureInfoPlugin();
        void ConstructL();
   
    protected:    
        CPluginTimer* iTimer;
        FeatureInfoCommand::TFeatureInfoCmd iCommandId;
        TUint8 iTransId;
    };

#endif // __SIMPLE_FEATURE_INFO_PLUGIN_H__
