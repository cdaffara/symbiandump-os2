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
* Description:  
*
*/


// INCLUDE FILES
#include    "SamplerPluginLoader.h"
//#include	<piprofiler/EngineUIDs.h>
#include    <utf.h> // CnvUtfConverter
#include  	<basched.h>

// CONSTANTS

// ----------------------------------------------------------------------------
// CSamplerPluginLoader::NewL
//
// EPOC two-phased constructor
// ----------------------------------------------------------------------------
//
CSamplerPluginLoader* CSamplerPluginLoader::NewL()
    {
    CSamplerPluginLoader* self = new( ELeave ) CSamplerPluginLoader;
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::CSamplerPluginLoader
//
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CSamplerPluginLoader::CSamplerPluginLoader() : CActive( EPriorityStandard )
    {
    LOGSTRING( "CSamplerPluginLoader()::CSamplerPluginLoader()" );
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::ConstructL
//
// EPOC default constructor can leave.
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::ConstructL( )
    {
    LOGSTRING( "CSamplerPluginLoader()::ConstructL()" );

    // get list of implementations
    CSamplerPluginInterface::ListAllImplementationsL( iImplInfoArray );

    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CSamplerPluginLoader::~CSamplerPluginLoader
//
// Destructor
// ----------------------------------------------------------------------------
//
CSamplerPluginLoader::~CSamplerPluginLoader()
    {
    LOGSTRING( "CSamplerPluginLoader()::~CSamplerPluginLoader()" );

    AbortAsyncLoad();

    Cancel();
    
    // reset ECOM implementation info array
    iImplInfoArray.ResetAndDestroy();   // This is needed
    iImplInfoArray.Close();
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::LoadAsync
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::LoadAsyncL(CArrayPtrFlat<CSamplerPluginInterface>* aPluginArray )
    {
    iPluginArray = aPluginArray;

    // Reset iterator
    iImplInfoArrayIterator = 0;

    LOGSTRING2( "CSamplerPluginLoader()::Implementation info count: %d",
        iImplInfoArray.Count() );

    NotifyProgress();

    //Begin CActive asynchronous loop.
    CompleteOwnRequest();
    }
    

// ----------------------------------------------------------------------------
// CSamplerPluginLoader::LoadSyncL
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::LoadSyncL(CArrayPtrFlat<CSamplerPluginInterface>* aPluginArray)
    {
    // cancel first active object from loading samplers dynamically
    Cancel();
    CSamplerPluginInterface* plugin = NULL;
    
    iPluginArray = aPluginArray;

    // Get a list of all implementations, even though we only want one specific
    // one. There appears to be no way to otherwise extract a specific implementation
    // info object :(
    // Search for the implementation that matches aImplementationUid
    const TInt impCount = iImplInfoArray.Count();
    for( TInt i=0; i<impCount; i++ )
        {
        const CImplementationInformation* info = iImplInfoArray[ i ];
            {
            TRAPD(ret, plugin = &CreatePluginInstanceL( *info ); );
            if( ret == KErrNone )
                {
                // Plugin ownership is transfered to iPluginArray
                InsertPluginInOrderL( plugin, iPluginArray );
                }
            else
                {
                // Error note is displayed even if plugin is not loaded
                LOGSTRING2("CSamplerPluginLoader::LoadSyncL() - plugin load failed, error %d", ret);
                }
            break;
            }
        }

    if  ( plugin == NULL )
        {
        User::Leave( KErrNotFound );
        }
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::AbortAsyncLoad
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::AbortAsyncLoad()
    {
    LOGSTRING( "CSamplerPluginLoader()::AbortAsyncLoad()" );
    Cancel();
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::RunL
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::RunL()
    {
    iRunLDebugCount++;
    LoadNextPluginL();

    // Check if there are still more plugins to be loaded:
    if ( iImplInfoArrayIterator < iImplInfoArray.Count() )
        {
        NotifyProgress();
        // Continue CActive asynchronous loop.
        CompleteOwnRequest();
        }
    else
        {
        // All plugins loaded:
        LOGSTRING( "CSamplerPluginLoader()::Loading plugins finished." );
        NotifyFinished();
        }
    }


// ---------------------------------------------------------------------------
// CScGenreItemConstructionConductor::CompleteOwnRequest
//
// Issue request complete notification.
// ---------------------------------------------------------------------------
void CSamplerPluginLoader::CompleteOwnRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::RunError
// ----------------------------------------------------------------------------
//
TInt CSamplerPluginLoader::RunError( TInt aError )
    {
    // This method is called when a plugin loading fails.
    // Always "fake" the return value so that ActiveSchedule
    // keeps running and later plugins are continued to be loaded.
    // Check if still plugins to be loaded:
    LOGTEXT(_L("CSamplerPluginLoader::RunError() - error in loading plugin"));
    if( iImplInfoArrayIterator < iImplInfoArray.Count() )
        {
        NotifyProgress();

        //Continue CActive asynchronous loop.
        CompleteOwnRequest();
        }
    else // All plugins loaded:
        {
        NotifyFinished();
        }

    if ( aError == KLeaveExit )
        {
        return KLeaveExit;
        }

    return KErrNone;
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::DoCancel
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::DoCancel()
    {

    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::NotifyProgress
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::NotifyProgress()
    {
    if( iObserver )
        {
        iObserver->HandlePluginLoaded( MSamplerPluginLoadObserver::ESamplerSuccess);
        }
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::NotifyFinished
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::NotifyFinished()
    {
    if( iObserver )
        {
        iObserver->HandlePluginLoaded( MSamplerPluginLoadObserver::ESamplerFinished );
        }
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::SetObserver
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::SetObserver(MSamplerPluginLoadObserver* aObserver)
    {
    LOGSTRING2("CSamplerPluginLoader()::Observer set:0x%X", aObserver);
    iObserver = aObserver;
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::ParseToUid
// Parses a UID from descriptor of form '0xNNNNNNNN' where N is hexadecimal.
//
// ----------------------------------------------------------------------------
//
TInt CSamplerPluginLoader::ParseToUid( const TDesC8& aSource, TUid& aTarget )
    {
    // Remove "0x" from the descriptor if it exists
    _LIT8(KHexPrefix, "0x");

    TPtrC8 pSource( aSource );
    const TInt prefixPosition = pSource.Find( KHexPrefix );
    if  ( prefixPosition != KErrNotFound )
        {
        pSource.Set( aSource.Mid( prefixPosition + KHexPrefix().Length() ) );
        }

    // Parse to integer
    TLex8 lex( pSource );
    TUint integer = 0;

    // Parse using TRadix::EHex as radix:
    const TInt err = lex.Val( integer, EHex );
    aTarget.iUid = integer;

    if( err != KErrNone )
        {
        // If parsing parent UID failed, do not load plugin:
        LOGSTRING2(
            "CSamplerPluginLoader()::Parsing parent UID failed. Error code:%d",
            err );
        }
    return err;
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::ParseOrderNumber
//
//
// ----------------------------------------------------------------------------
//
TInt CSamplerPluginLoader::ParseOrderNumber( const TDesC8& aSource, TInt& aOrderNumber )
    {
    // Parse plugin's order number from opaque_data:
    TLex8 lex( aSource );
    const TInt orderErr = lex.Val( aOrderNumber );
    return orderErr;
    }

// ----------------------------------------------------------------------------
// CSamplerPluginLoader::LoadNextPluginL
// Iterate through iImplInfoArray. Load the plugin if it is eligible for
// loading. Loaded plugin is added to iPluginArray. Each time a plugin is
// loaded, iObserver is notified.
//
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::LoadNextPluginL()
    {
    // Iterate through iImplInfoArray. This loop continues between function
    // calls. Therefore member variable iImplInfoArrayIterator is used as a
    // counter. Loop will break when match is found and continues on next RunL.
    for( ; iImplInfoArrayIterator < iImplInfoArray.Count();  )
        {
        const CImplementationInformation* info =
            iImplInfoArray[ iImplInfoArrayIterator ];

        iImplInfoArrayIterator++;

//        PrintInfoDebug( *info, iImplInfoArrayIterator, iImplInfoArray.Count() );
        LOGSTRING3("CSamplerPluginLoader() - iImplInfoArrayIterator %d, iImplInfoArray.Count() %d", 
                iImplInfoArrayIterator, 
                iImplInfoArray.Count() );
        
        // If this plugin is OK -> load it:
        LOGSTRING2( "CSamplerPluginLoader() %S eligible for parent",
                &info->DisplayName());
        CSamplerPluginInterface* plugin = NULL;
        TInt error(KErrNone);
        // Create plugin. Trap leave for debugging purposes.
        TRAP( error, plugin = &CreatePluginInstanceL( *info ); );

        if( error == KErrNone )
            {
            // Plugin ownership is transfered to iPluginArray
            InsertPluginInOrderL( plugin, iPluginArray );
            }
        else
            {
            // Error note is displayed even if plugin is not loaded
            LOGSTRING2("CSamplerPluginLoader::LoadNextPluginL() - plugin load failed, error %d", error);
            }
        // Wait for next round
        break;
        }
    }
 
// ----------------------------------------------------------------------------
// CSamplerPluginLoader::CreatePluginInstanceL
//
//
// ----------------------------------------------------------------------------
//

CSamplerPluginInterface& CSamplerPluginLoader::CreatePluginInstanceL(
    const CImplementationInformation& aImpInfo )
    {
    // Now we can load the plugin
    const TUid implUid = aImpInfo.ImplementationUid();

    CSamplerPluginInterface* plugin = CSamplerPluginInterface::NewL( implUid , (TAny*)&aImpInfo.DisplayName() );
    CleanupStack::PushL ( plugin );

    // Parse plugin's order number from opaque_data:
    TInt orderNumber(0);
    const TInt orderErr = ParseOrderNumber( aImpInfo.OpaqueData(), orderNumber );

    if  ( orderErr == KErrNone && orderNumber >= 0 )
        {
        	plugin->iOrder = orderNumber;
        }
    CleanupStack::Pop( plugin ); // CSamplerController is now responsible for this memory.

    return *plugin;
    }

// ----------------------------------------------------------------------------
// CSamplerPluginLoader::SortPluginsL
//
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::SortPluginsL(
        CArrayPtrFlat<CSamplerPluginInterface>* aPlugins )
    {
    RPointerArray<CSamplerPluginInterface> plugins;
    TLinearOrder<CSamplerPluginInterface> order( CSamplerPluginLoader::Compare );

    // Insertion will also order
    for( TInt i = 0; i < aPlugins->Count(); i++ )
        {
        plugins.InsertInOrderL( (*aPlugins)[i], order );
        }

    // Replace original array content with sorted items
    aPlugins->Reset();
    for( TInt i = 0; i < plugins.Count(); i++ )
        {
        aPlugins->AppendL( plugins[i] );
        }
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::Compare
//
// Compare two plugins.
// Precedence:
// [1. plugin provider category]
// 2. plugin order number
// 3. plugin caption
// Plugin provider gategory is currently disabled (not supported yet).
// ----------------------------------------------------------------------------
//
TInt CSamplerPluginLoader::Compare( const CSamplerPluginInterface& aFirst,
                               const CSamplerPluginInterface& aSecond )
    {
    // compare indexes and sort
    return CompareIndex( aFirst, aSecond );
    }


// ----------------------------------------------------------------------------
// CSamplerPluginLoader::InsertPluginInOrderL
//
// ----------------------------------------------------------------------------
//
void CSamplerPluginLoader::InsertPluginInOrderL(
    CSamplerPluginInterface* aPlugin,
    CArrayPtrFlat<CSamplerPluginInterface>* aPlugins )
    {
    CSamplerPluginInterface* comparedPlugin;
    TInt comparison = 0;
    TBool inserted = EFalse;

    for( TInt i = 0; i < aPlugins->Count(); i++ )
        {
        comparedPlugin = (*aPlugins)[i];
        // Optimization: do not call time consuming Compare() multiple times!
        comparison = Compare( *aPlugin, *comparedPlugin );
        if( comparison < 0 )
            {
            aPlugins->InsertL( i, aPlugin );
            inserted = ETrue;
            break;
            }
        else if( comparison == 0 )
            {
            aPlugins->InsertL( i+1, aPlugin );
            inserted = ETrue;
            break;
            }
        }
    // Plugin was not before any other plugin - make sure it's appended
    if( !inserted )
        {
        aPlugins->AppendL( aPlugin );
        }

    #ifdef _GS_PLUGINLOADER_SORTING_TRACES
        PrintOrderTraces( aPlugins );
    #endif // _GS_PLUGINLOADER_SORTING_TRACES

    }

// ----------------------------------------------------------------------------
// CSamplerPluginLoader::CompareIndex
// ----------------------------------------------------------------------------
//
TInt CSamplerPluginLoader::CompareIndex( const CSamplerPluginInterface& aFirst,
                                    const CSamplerPluginInterface& aSecond )
    {
    TInt comparison = KSamplerComparisonEqual;
    // The plugin having index is before the one not having one

    if( aFirst.iOrder  == KSamplerPluginNotIndexed &&
        aSecond.iOrder == KSamplerPluginNotIndexed )
        {
        // Neither have index -> equal
        comparison = KSamplerComparisonEqual;
        }
    else if( aFirst.iOrder == KSamplerPluginNotIndexed )
        {
        // The plugin having index is before the one not having one
        comparison = KSamplerComparisonAfter;
        }
    else if( aSecond.iOrder == KSamplerPluginNotIndexed )
        {
        // The plugin having index is before the one not having one
        comparison = KSamplerComparisonBefore;
        }
    else if( aFirst.iOrder < aSecond.iOrder )
        {
        // Compare actual index values
        comparison = KSamplerComparisonBefore;
        }
    else if( aFirst.iOrder > aSecond.iOrder )
        {
        // Compare actual index values
        comparison = KSamplerComparisonAfter;
        }
    return comparison;
    }

//  End of File
