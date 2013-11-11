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



#ifndef __ENHANCED_FEATURE_INFO_PLUGIN_H__
#define __ENHANCED_FEATURE_INFO_PLUGIN_H__

#include <featmgr/featureinfoplugin.h>
#include "plugintimer.h"


class CEnhancedFeatureInfoPlugin: public CFeatureInfoPlugin,
								public MPluginTimerCallback
    {
    public:
    		
    	static CEnhancedFeatureInfoPlugin* NewL();	

        /**
        * Method to invoke a particular command in the plugin
        * @param aCommandId Command ID
        * @param aTransId   Transaction ID
        * @param aData      Data associated with command
        */
        virtual void ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                      const TUint8 aTransId,
                                      TDesC8& aData );
        
        virtual ~CEnhancedFeatureInfoPlugin();
        // From MPluginTimerCallback
        void GenericTimerFiredL();                        
                                
    protected: 
        CEnhancedFeatureInfoPlugin();
        void ConstructL();
   
    protected:
        CPluginTimer* iTimer;
        FeatureInfoCommand::TFeatureInfoCmd iCommandId;
        TUint8 iTransId;
    };

#endif // __ENHANCED_FEATURE_INFO_PLUGIN_H__
