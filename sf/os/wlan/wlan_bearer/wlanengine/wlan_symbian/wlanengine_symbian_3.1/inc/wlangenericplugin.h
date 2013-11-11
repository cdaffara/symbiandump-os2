/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Interface for instantiating Generic WLAN Plugins
*
*/


#ifndef WLANGENERICPLUGIN_H_
#define WLANGENERICPLUGIN_H_

#include <e32def.h>
#include <e32cmn.h> // TUid
#include <e32base.h>

NONSHARABLE_CLASS( CGenericWlanPlugin ): public CBase
    {
public:
    
    /**
     * Maximum size of threads heap.
     */
    static const TInt KMaxHeapSize = 0x10000;
    
    /**
     * Information structure of plugin.
     */
    struct TPluginArrayEntry
        {
        TUid iUid;
        CActiveScheduler * iActiveScheduler;
        };

    /**
     * Constructor.
     */
    CGenericWlanPlugin();
    
    /**
     * Destructor.
     */
    ~CGenericWlanPlugin();
    
    /**
     * Start all plugins having Generic WLAN Plugin interface UID.
     */
    void StartPlugins();

    /**
     * Stop all started plugins.
     */
    void StopPlugins();

private:

    /**
     * Main function for thread.
     * 
     * Create cleanup stack for thread.
     * 
     * @param aPluginArrayEntryPtr Pointer to plugin information.
     * @return Symbian specific error code.
     */
    static TInt GenericWlanPluginThreadMain( TAny* aPluginArrayEntryPtr );
    
    /**
     * Load ECom plugin.
     * 
     * Create active scheduler for thread and load ECom plugin which 
     * implementation UID is aPluginArrayEntryPtr->iUid.
     * 
     * @param aPluginArrayEntryPtr Pointer to plugin information.
     */
    static void LoadGenericWlanPluginL( TPluginArrayEntry * aPluginArrayEntryPtr );
    
private: // Data
    /**
     * UID and Active Scheduler for each thread.
     */
    RArray<TPluginArrayEntry> iPluginArray;
    };

#endif  // WLANGENERICPLUGIN_H_
            
// End of File
