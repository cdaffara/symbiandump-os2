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



#ifndef __MIXED_FEATURE_INFO_PLUGIN_H__
#define __MIXED_FEATURE_INFO_PLUGIN_H__

#include <featmgr/featureinfoplugin.h>
#include "plugintimer.h"


class CMixedFeatureInfoPlugin: public CFeatureInfoPlugin,
								public MPluginTimerCallback
    {
    public:
    		
    	static CMixedFeatureInfoPlugin* NewL();	

        /**
        * Method to invoke a particular command in the plugin
        * @param aCommandId Command ID
        * @param aTransId   Transaction ID
        * @param aData      Data associated with command
        */
        virtual void ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                      const TUint8 aTransId,
                                      TDesC8& aData );
        
        virtual ~CMixedFeatureInfoPlugin();
        // From MPluginTimerCallback
        void GenericTimerFiredL();                        
                                
    protected:
        CMixedFeatureInfoPlugin();
        void ConstructL();
        
    protected:
        CPluginTimer* iTimer;
        FeatureInfoCommand::TFeatureInfoCmd iCommandId;
        TUint8 iTransId;
    };

#endif // __MIXED_FEATURE_INFO_PLUGIN_H__
