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
#include    "WriterPluginLoader.h"
//#include	<piprofiler/EngineUIDs.h>
#include    <utf.h> // CnvUtfConverter
#include  	<basched.h>

// constants

// ----------------------------------------------------------------------------
// CWriterPluginLoader::NewL
//
// EPOC two-phased constructor
// ----------------------------------------------------------------------------
//
CWriterPluginLoader* CWriterPluginLoader::NewL()
    {
    CWriterPluginLoader* self = new( ELeave ) CWriterPluginLoader;
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::CWriterPluginLoader
//
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CWriterPluginLoader::CWriterPluginLoader() : CActive( EPriorityStandard )
    {
    LOGTEXT(_L("CWriterPluginLoader()::CWriterPluginLoader()" ));
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::ConstructL
//
// EPOC default constructor can leave.
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::ConstructL( )
    {
    LOGTEXT(_L("CWriterPluginLoader()::ConstructL()" ));
    
    // get list of implementations
    CWriterPluginInterface::ListAllImplementationsL( iImplInfoArray );
    
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CWriterPluginLoader::~CWriterPluginLoader
//
// Destructor
// ----------------------------------------------------------------------------
//
CWriterPluginLoader::~CWriterPluginLoader()
    {
    LOGTEXT(_L("CWriterPluginLoader()::~CWriterPluginLoader()") );

    AbortAsyncLoad();

    Cancel();

    // reset ECOM implementation info array
    iImplInfoArray.ResetAndDestroy(); // This is needed
    iImplInfoArray.Close();
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::LoadAsync
//
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::LoadAsyncL( CArrayPtrFlat<CWriterPluginInterface>* aPluginArray )
    {
    iPluginArray = aPluginArray;
    
    // Reset iterator:
    iImplInfoArrayIterator = 0;

    LOGSTRING2( "CWriterPluginLoader()::Implementation info count: %d",
        iImplInfoArray.Count() );

    NotifyProgress();

    //Begin CActive asynchronous loop.
    CompleteOwnRequest();
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::LoadSyncL
//
//
// ----------------------------------------------------------------------------
//
CWriterPluginInterface& CWriterPluginLoader::LoadSyncL( TUid aImplementationUid )
    {
    Cancel();
    CWriterPluginInterface* plugin = NULL;

    // Get a list of all implementations, even though we only want one specific
    // one. There appears to be no way to otherwise extract a specific implementation
    // info object :(
    // Search for the implementation that matches aImplementationUid
    const TInt impCount = iImplInfoArray.Count();
    for( TInt i=0; i<impCount; i++ )
        {
        const CImplementationInformation* info = iImplInfoArray[ i ];
        if  ( info->ImplementationUid() == aImplementationUid )
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
                LOGSTRING2("CWriterPluginLoader::LoadSyncL() - plugin load failed, error %d", ret);
                }
            break;
            }
        }

    if  ( plugin == NULL )
        {
        User::Leave( KErrNotFound );
        }
    return *plugin;
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::AbortAsyncLoad
//
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::AbortAsyncLoad()
    {
    LOGTEXT(_L("CWriterPluginLoader()::AbortAsyncLoad()" ));
    Cancel();
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::RunL
//
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::RunL()
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
        LOGTEXT(_L("CWriterPluginLoader()::Loading plugins finished." ));
        NotifyFinished();
        }
    }


// ---------------------------------------------------------------------------
// CScGenreItemConstructionConductor::CompleteOwnRequest
//
// Issue request complete notification.
// ---------------------------------------------------------------------------
void CWriterPluginLoader::CompleteOwnRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::RunError
//
//
// ----------------------------------------------------------------------------
//
TInt CWriterPluginLoader::RunError( TInt aError )
    {
    // This method is called when a plugin loading fails.
    // Always "fake" the return value so that ActiveSchedule
    // keeps running and later plugins are continued to be loaded.
    // Check if still plugins to be loaded:
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
// CWriterPluginLoader::DoCancel
//
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::DoCancel()
    {

    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::NotifyProgress
//
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::NotifyProgress()
    {
    if( iObserver )
        {
        iObserver->HandlePluginLoaded( MWriterPluginLoadObserver::EWriterSuccess);
        }
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::NotifyFinished
//
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::NotifyFinished()
    {
    if( iObserver )
        {
        iObserver->HandlePluginLoaded( MWriterPluginLoadObserver::EWriterFinished );
        }
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::SetObserver
//
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::SetObserver(MWriterPluginLoadObserver* aObserver)
    {
    LOGSTRING2("CWriterPluginLoader()::Observer set:0x%X", aObserver);
    iObserver = aObserver;
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::ParseToUid
// Parses a UID from descriptor of form '0xNNNNNNNN' where N is hexadecimal.
//
// ----------------------------------------------------------------------------
//
TInt CWriterPluginLoader::ParseToUid( const TDesC8& aSource, TUid& aTarget )
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
            "CWriterPluginLoader()::Parsing parent UID failed. Error code:%d",
            err );
        }
    return err;
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::ParseOrderNumber
//
//
// ----------------------------------------------------------------------------
//
TInt CWriterPluginLoader::ParseOrderNumber( const TDesC8& aSource, TInt& aOrderNumber )
    {
    // Parse plugin's order number from opaque_data:
    TLex8 lex( aSource );
    const TInt orderErr = lex.Val( aOrderNumber );
    return orderErr;
    }

// ----------------------------------------------------------------------------
// CWriterPluginLoader::LoadNextPluginL
// Iterate through iImplInfoArray. Load the plugin if it is eligible for
// loading. Loaded plugin is added to iPluginArray. Each time a plugin is
// loaded, iObserver is notified.
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::LoadNextPluginL()
    {
    // Iterate through iImplInfoArray. This loop continues between function
    // calls. Therefore member variable iImplInfoArrayIterator is used as a
    // counter. Loop will break when match is found and continues on next RunL.
    for( ; iImplInfoArrayIterator < iImplInfoArray.Count();  )
        {
        const CImplementationInformation* info =
            iImplInfoArray[ iImplInfoArrayIterator ];

        iImplInfoArrayIterator++;

        // If this plugin is OK -> load it:
        LOGSTRING2( "CWriterPluginLoader() %S eligible for parent",
                &info->DisplayName() );
        CWriterPluginInterface* plugin = NULL;
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
            LOGSTRING2("CWriterPluginLoader::LoadNextPluginL() - plugin load failed, error %d", error);
            }
        // Wait for next round
        break;
        }
    }

// ----------------------------------------------------------------------------
// CWriterPluginLoader::CreatePluginInstanceL
//
//
// ----------------------------------------------------------------------------
//

CWriterPluginInterface& CWriterPluginLoader::CreatePluginInstanceL(
    const CImplementationInformation& aImpInfo )
    {
    // Now we can load the plugin
    const TUid implUid = aImpInfo.ImplementationUid();

    CWriterPluginInterface* plugin = CWriterPluginInterface::NewL( implUid , (TAny*)&aImpInfo.DisplayName() );
    CleanupStack::PushL ( plugin );
   
    TInt orderNumber(0);
    const TInt orderErr = ParseOrderNumber( aImpInfo.OpaqueData(), orderNumber );
    
    if  ( orderErr == KErrNone && orderNumber >= 0 )
        {
        plugin->iOrder = orderNumber;
        }

    CleanupStack::Pop( plugin ); // CWriterController is now responsible for this memory.

    return *plugin;
    }

// ----------------------------------------------------------------------------
// CWriterPluginLoader::SortPluginsL
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::SortPluginsL(
        CArrayPtrFlat<CWriterPluginInterface>* aPlugins )
    {
    RPointerArray<CWriterPluginInterface> plugins;
    TLinearOrder<CWriterPluginInterface> order( CWriterPluginLoader::Compare );

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
// CWriterPluginLoader::Compare
//
// Compare two plugins.
// Precedence:
// [1. plugin provider category]
// 2. plugin order number
// 3. plugin caption
// Plugin provider gategory is currently disabled (not supported yet).
// ----------------------------------------------------------------------------
//
TInt CWriterPluginLoader::Compare( const CWriterPluginInterface& aFirst,
                               const CWriterPluginInterface& aSecond )
    {
    return CompareIndex( aFirst, aSecond );
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::InsertPluginInOrderL
//
// ----------------------------------------------------------------------------
//
void CWriterPluginLoader::InsertPluginInOrderL(
    CWriterPluginInterface* aPlugin,
    CArrayPtrFlat<CWriterPluginInterface>* aPlugins )
    {
    CWriterPluginInterface* comparedPlugin;
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
// CWriterPluginLoader::CompareIndex
//
//
// ----------------------------------------------------------------------------
//

TInt CWriterPluginLoader::CompareIndex( const CWriterPluginInterface& aFirst,
                                    const CWriterPluginInterface& aSecond )
    {
    TInt comparison = KWriterComparisonEqual;
    // The plugin having index is before the one not having one

    if( aFirst.iOrder  == KWriterPluginNotIndexed &&
        aSecond.iOrder == KWriterPluginNotIndexed )
        {
        // Neither have index -> equal
        comparison = KWriterComparisonEqual;
        }
    else if( aFirst.iOrder == KWriterPluginNotIndexed )
        {
        // The plugin having index is before the one not having one
        comparison = KWriterComparisonAfter;
        }
    else if( aSecond.iOrder == KWriterPluginNotIndexed )
        {
        // The plugin having index is before the one not having one
        comparison = KWriterComparisonBefore;
        }
    else if( aFirst.iOrder < aSecond.iOrder )
        {
        // Compare actual index values
        comparison = KWriterComparisonBefore;
        }
    else if( aFirst.iOrder > aSecond.iOrder )
        {
        // Compare actual index values
        comparison = KWriterComparisonAfter;
        }
    return comparison;
    }


// ----------------------------------------------------------------------------
// CWriterPluginLoader::GetDocument
//
//
// ----------------------------------------------------------------------------
//
/*
CWriterBaseDocument* CWriterPluginLoader::GetDocument()
    {
    CWriterBaseDocument* document = static_cast<CWriterBaseDocument*>( iAppUi->Document() );
    return document;
    }
*/

//  End of File
