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



#ifndef __FEATMGRSLOWSTARTPLUGIN_H_
#define __FEATMGRSLOWSTARTPLUGIN_H_

#include "baseplugin.h"
#include <featmgr/featureinfoplugin.h>


class CFeatMgrSlowStartPlugin : public CFeatMgrBasePlugin
    {
	public:
		static CFeatMgrSlowStartPlugin* NewL();
		virtual ~CFeatMgrSlowStartPlugin();
		virtual void ProcessCommandL( const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
									  const TUint8 aTransId,
									  TDesC8& aData );
		virtual void GenericTimerFiredL( MFeatureInfoPluginCallback&,
								const FeatureInfoCommand::TFeatureInfoCmd,
								const TUint8,
								TInt );

	private: // Construction
		CFeatMgrSlowStartPlugin();
		void ConstructL();       
	};

#endif // __FEATMGRSLOWSTARTPLUGIN_H_
