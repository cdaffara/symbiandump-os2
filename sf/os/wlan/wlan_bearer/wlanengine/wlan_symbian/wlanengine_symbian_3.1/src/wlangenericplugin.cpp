/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for instantiating Generic WLAN Plugins
*
*/

/*
* %version: 4 %
*/

#include <wlanpluginclient.h>
#include "wlangenericplugin.h"
#include "am_debug.h"


// -----------------------------------------------------------------------------
// CGenericWlanPlugin::CGenericWlanPlugin
// 
// -----------------------------------------------------------------------------
//
CGenericWlanPlugin::CGenericWlanPlugin()
    {
    DEBUG("CGenericWlanPlugin::CGenericWlanPlugin");
    iPluginArray.Reset();
    }

// -----------------------------------------------------------------------------
// CGenericWlanPlugin::~CGenericWlanPlugin
// 
// -----------------------------------------------------------------------------
//
CGenericWlanPlugin::~CGenericWlanPlugin()
    {
    DEBUG("CGenericWlanPlugin::~CGenericWlanPlugin");
    StopPlugins();
    iPluginArray.Close();
    }

// -----------------------------------------------------------------------------
// CGenericWlanPlugin::StopPlugins
// 
// -----------------------------------------------------------------------------
//
void CGenericWlanPlugin::StopPlugins()
    {
    DEBUG("CGenericWlanPlugin::StopPlugins");
    for (TInt i = 0; i < iPluginArray.Count(); ++i)
        {
        if ( iPluginArray[i].iActiveScheduler )
            {
            DEBUG3("CGenericWlanPlugin::StopPlugins - iPluginArray[%d] = UID: 0x%08X, ActiveScheduler 0x%08X", 
                    i, 
                    iPluginArray[i].iUid, 
                    iPluginArray[i].iActiveScheduler );
            iPluginArray[i].iActiveScheduler->Halt( KErrNone );
            }
        }
    iPluginArray.Reset();
    }

// -----------------------------------------------------------------------------
// CGenericWlanPlugin::StartPlugins
// 
// -----------------------------------------------------------------------------
//
void CGenericWlanPlugin::StartPlugins()
    {
    DEBUG("CGenericWlanPlugin::StartPlugins");

    // List implementations
    RImplInfoPtrArray implInfoArray;

    TRAPD( listImplementationsError, CGenericWlanPluginClient::ListImplementationsL( implInfoArray ) );
    if ( listImplementationsError )
        {
        DEBUG1("CGenericWlanPlugin::StartPlugins - CGenericWlanPluginClient::ListImplementationsL() failed with error %i", listImplementationsError );
        implInfoArray.ResetAndDestroy();
        return;
        }

    // Get number of implementations
    const TInt pluginCount( implInfoArray.Count() );

    // Loop implementation info and create instances
    for (TInt i = 0; i < pluginCount; i++)
        {
        // Get next implementation info
        CImplementationInformation* info = static_cast<CImplementationInformation*>(implInfoArray[i] );

#ifdef _DEBUG
        // Trace information about plugin
        DEBUG1( "CGenericWlanPlugin::StartPlugins - Generic WLAN Plugin # %i", i );
        DEBUG1( "CGenericWlanPlugin::StartPlugins   - ImplementationUid 0x%08X", info->ImplementationUid().iUid );
        TBuf8<KPrintLineLength> buf8;
        buf8.Copy( info->DisplayName() );
        DEBUG1S("CGenericWlanPlugin::StartPlugins   - DisplayName  ", buf8.Length(), buf8.Ptr() );
        DEBUG1( "CGenericWlanPlugin::StartPlugins   - Version      %i", info->Version() );
        DEBUG1S("CGenericWlanPlugin::StartPlugins   - DataType     ", info->DataType().Length(), info->DataType().Ptr() );
        DEBUG1S("CGenericWlanPlugin::StartPlugins   - OpaqueData   ", info->OpaqueData().Length(), info->OpaqueData().Ptr() );
        DEBUG1( "CGenericWlanPlugin::StartPlugins   - RomOnly      %i", info->RomOnly() );
        DEBUG1( "CGenericWlanPlugin::StartPlugins   - RomBased     %i", info->RomBased() );
        DEBUG1( "CGenericWlanPlugin::StartPlugins   - VendorId     0x%08X", info->VendorId().iId );
#endif
        CGenericWlanPlugin::TPluginArrayEntry entry;
        entry.iUid = info->ImplementationUid();
        // Active scheduler will be updated when thread is started and Active Scheduler is created.
        entry.iActiveScheduler = NULL;
        iPluginArray.Append( entry );
        
        RThread thread;
        TInt err = thread.Create( info->DisplayName(), GenericWlanPluginThreadMain, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, 
                reinterpret_cast<TAny*>( &iPluginArray[ iPluginArray.Count()-1 ] ) /*EOwnerProcess / EOwnerThread*/ );
        if (err != KErrNone)
            {
            DEBUG1("CGenericWlanPlugin::StartPlugins - thread.Create() failed with error %i", err );
            implInfoArray.ResetAndDestroy();
            return;
            }
        else
            {
            thread.Resume();
            }
        thread.Close();
        }

    // Reset and destroy implementations infos
    implInfoArray.ResetAndDestroy();

    DEBUG1("CGenericWlanPlugin::StartPlugins - Started plugin count %i", pluginCount );
    }

// -----------------------------------------------------------------------------
// CGenericWlanPlugin::GenericWlanPluginThreadMain
// 
// -----------------------------------------------------------------------------
//
TInt CGenericWlanPlugin::GenericWlanPluginThreadMain( TAny* aPluginArrayEntryPtr )
    {
    DEBUG("CGenericWlanPlugin::GenericWlanPluginThreadMain()");
    
    // Create cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if ( !cleanup )
        {
        DEBUG("CGenericWlanPlugin::GenericWlanPluginThreadMain() - CTrapCleanup::New() failed. Stopping thread.");
        User::Exit( KErrNoMemory );
        }
    
    __UHEAP_MARK;
    
    TRAPD(createError, LoadGenericWlanPluginL( reinterpret_cast<TPluginArrayEntry *>( aPluginArrayEntryPtr ) ) );
    if ( createError )
        {
        return createError;
        }

    __UHEAP_MARKEND;
    
    delete cleanup;
    REComSession::FinalClose();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CGenericWlanPlugin::LoadGenericWlanPluginL
// 
// -----------------------------------------------------------------------------
//
void CGenericWlanPlugin::LoadGenericWlanPluginL( TPluginArrayEntry * aPluginArrayEntryPtr )
    {
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
    
    aPluginArrayEntryPtr->iActiveScheduler = scheduler;
    
    // Initialize WLAN plugin
    CGenericWlanPluginClient* wlanPluginClient( NULL );
    DEBUG1( "Instantiating CGenericWlanPluginClient(UID 0x%08X)", aPluginArrayEntryPtr->iUid );
    TRAPD( ret, wlanPluginClient = CGenericWlanPluginClient::NewL( aPluginArrayEntryPtr->iUid ) );
    // Create new instance of the plugin
    if( ret == KErrNone )
        {
        DEBUG1("CGenericWlanPlugin::LoadGenericWlanPluginL() - wlanPluginClient instance 0x%08X", wlanPluginClient);
        /*
         * This thread will run in CActiveScheduler::Start() until CActiveScheduler is stopped.
         */
        DEBUG("CGenericWlanPlugin::LoadGenericWlanPluginL() - Starting CActiveScheduler");
        CActiveScheduler::Start();

        DEBUG("CGenericWlanPlugin::LoadGenericWlanPluginL() - deleting wlanPluginClient");
        delete wlanPluginClient;
        }
    else
        {
        DEBUG1( "CGenericWlanPlugin::LoadGenericWlanPluginL - CGenericWlanPluginClient::NewL leaved with %d, let's ignore it.", ret );
        }
    
    // Delete active scheduler
    CleanupStack::PopAndDestroy(scheduler);
    
    DEBUG("CGenericWlanPlugin::LoadGenericWlanPluginL() - exit");
    }
