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



#ifndef __FEATMGRBCENHANCEDPLUGIN_H_
#define __FEATMGRBCENHANCEDPLUGIN_H_

#include "baseplugin.h"
#include <featmgr/featureinfoplugin.h>


class CFeatMgrBCEnhancedPlugin : public CFeatMgrBasePlugin
    {
    public:

        static CFeatMgrBCEnhancedPlugin* NewL();
        virtual ~CFeatMgrBCEnhancedPlugin();
        virtual void ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
				                const TUint8 aTransId,
				                TDesC8& aData );
		virtual void GenericTimerFiredL( MFeatureInfoPluginCallback& aService,
                                const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                const TUint8 aTransId,
                                TInt aRetVal );
                                        
                                
    private: // Construction

        CFeatMgrBCEnhancedPlugin();
        void ConstructL();       
 
    };

#endif // __FEATMGRBCENHANCEDPLUGIN_H_
