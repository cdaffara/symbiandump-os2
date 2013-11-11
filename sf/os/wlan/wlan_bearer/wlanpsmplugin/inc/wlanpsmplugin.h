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
* Description:   hdr file for WLAN PSM plug-in
*
*/

/*
* %version: 4 %
*/

#ifndef WLANPSMPLUGIN_H
#define WLANPSMPLUGIN_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

#include <psmpluginbase.h>

#include "rwlmserver.h"

class MPsmSettingsProvider;

/**
 *  WLAN PSM plugin
 *  This class implements WLAN PSM plugin logic.
 *  
 *  @since S60 S60 v5.1
 */
class CWlanPsmPlugin : public CPsmPluginBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.
    * @return The created object.
    */
    static CWlanPsmPlugin* NewL( TPsmPluginCTorParams& initParams );
    
    /**
    * Destructor.
    */
    virtual ~CWlanPsmPlugin();

public: 
    
    // From CPsmPluginBase
    void NotifyModeChange( const TInt aMode );
    
private:
    CWlanPsmPlugin( TPsmPluginCTorParams& aInitParams );
    void ConstructL();
    void NotifyModeChangeL( const TInt aMode );

private: // data
    
    /**
     * Interface to WLAN Engine. 
     */
    RWLMServer iWlmServer;

    };

#endif // WLANPSMPLUGIN_H
