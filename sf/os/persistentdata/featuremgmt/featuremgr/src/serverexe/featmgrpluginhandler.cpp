// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include <ecom/ecom.h>
#include "featmgrpluginhandler.h"
#include "featmgrclientserver.h"
#include "featmgrdebug.h"
#include "featmgrserver.h"

// LOCAL CONSTANTS
_LIT(KPanicCategory, "FeatMgrPluginHandler");

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFeatMgrPluginHandler::CFeatMgrPluginHandler
// C++ constructor
// -----------------------------------------------------------------------------
//
CFeatMgrPluginHandler::CFeatMgrPluginHandler( CFeatMgrServer& aServer )
    : iPlugin( NULL ), 
      iServer( aServer ), 
      iTransId( 0 ) // Is always 0 because there is only one transaction between handler and plugin.
    {
    }

// -----------------------------------------------------------------------------
// CFeatMgrPluginHandler::ConstructL
// 2nd phase constructor gets plugin instance.
// -----------------------------------------------------------------------------
//
void CFeatMgrPluginHandler::ConstructL( TUid aImplementationUid )
    {
    FUNC_LOG

    // get plugin instance
    iPlugin = CFeatureInfoPlugin::NewL( aImplementationUid, *this );
    }

// -----------------------------------------------------------------------------
// CFeatMgrPluginHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
// 
CFeatMgrPluginHandler* CFeatMgrPluginHandler::NewL( TUid aImplementationUid, 
                                                    CFeatMgrServer& aServer )
    {
    CFeatMgrPluginHandler* self = new( ELeave ) CFeatMgrPluginHandler( aServer );
    
    CleanupStack::PushL( self );
    self->ConstructL( aImplementationUid );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CFeatMgrPluginHandler::~CFeatMgrPluginHandler()
    {
    FUNC_LOG
    delete iPlugin;    
    }

// -----------------------------------------------------------------------------
// CFeatMgrPluginHandler::SendCommandL
// Sends a command to plugin for processing.
// -----------------------------------------------------------------------------
// 
void CFeatMgrPluginHandler::SendCommandL( FeatureInfoCommand::TFeatureInfoCmd aCommandId )
    {
    // Load feature info.
    iCommandId = aCommandId;
    iTransId++;
    TPtrC8 ptr(KNullDesC8);
    iPlugin->ProcessCommandL( iCommandId, iTransId, ptr );
    }
        
// -----------------------------------------------------------------------------
// CFeatMgrPluginHandler::ProcessResponseL
// Routes response from plugin to server
// -----------------------------------------------------------------------------
//
void CFeatMgrPluginHandler::ProcessResponseL( FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                                              TUint8 aTransId,
                                              TDesC8& aData )
    {
    FUNC_LOG

    INFO_LOG2( "CFeatMgrPluginHandler::ProcessResponseL( aCommandId 0x%x, aTransId 0x%x )",
                            aCommandId, aTransId );

    if ( iTransId == aTransId &&
         ( aCommandId == FeatureInfoCommand::ELoadFeatureInfoCmdId ||
           aCommandId == FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId ) &&
         iCommandId == aCommandId )
        {
        // Copy response and ask server to process it
        if( aCommandId == FeatureInfoCommand::ELoadFeatureInfoCmdId )
            {
            RArray<FeatureInfoCommand::TFeature> featureList;
            CleanupClosePushL( featureList );
            FeatureInfoCommand::TFeatureInfoRespPckg response;
            response.Copy( aData );
            ProcessFeatureInfoL(featureList, response);
            CleanupStack::PopAndDestroy( &featureList );
            }
        else if( aCommandId == FeatureInfoCommand::ELoadEnhancedFeatureInfoCmdId )
            {
            RFeatureArray featureList;
            CleanupClosePushL( featureList );
            FeatureInfoCommand::TEnhancedFeatureInfoRespPckg response;
            response.Copy( aData );
            ProcessFeatureInfoL(featureList, response);
            CleanupStack::PopAndDestroy( &featureList );
            }
        }
    else
        {       
        ERROR_LOG2( "CFeatMgrPluginHandler::ProcessResponseL - Command ID , expected: 0x%x, got 0x%x",
                                      iCommandId, aCommandId );
        ERROR_LOG2( "CFeatMgrPluginHandler::ProcessResponseL - Transaction ID , expected: 0x%x, got 0x%x",
                                      iTransId, aTransId );
        User::Leave( KErrArgument );
        }
    }
        
// -----------------------------------------------------------------------------
// CFeatMgrPluginHandler::ProcessFeatureInfoL
// Asks server to process specified response from plugin 
// -----------------------------------------------------------------------------
//
void CFeatMgrPluginHandler::ProcessFeatureInfoL( 
        RArray<FeatureInfoCommand::TFeature>& aList, 
        FeatureInfoCommand::TFeatureInfoRespPckg aResponse )
    {
    FUNC_LOG

    // Go through dynamic feature list. Possible multiple same entries resolved by server.
    TInt err( KErrNone );
    TInt count = aResponse().iList.Count();
    
    if ( aResponse().iErrorCode == KErrNone )
        {
        for( TInt i = 0 ; i < count; i++)
            {    
            INFO_LOG2( "CFeatMgrPluginHandler::ProcessFeatureInfoL - featureID %d, value %d",
                aResponse().iList[i].iFeatureID, aResponse().iList[i].iValue );

            err = aList.Append( aResponse().iList[i] );
                
            if ( err != KErrNone )
                {
                ERROR_LOG1( "CFeatMgrPluginHandler::ProcessFeatureInfoL - Error %d when saving feature info",
                    err );
                break;                                
                }
            }

        if ( err == KErrNone )
            {   
            // Send feature info to the server
            iServer.FeatureInfoL( aList, this );
            }
        else
            {
            INFO_LOG1( "CFeatMgrPluginHandler::ProcessFeatureInfoL - FeatureInfoL not called %d", err );
            }
        }
    else
        {
        ERROR_LOG2( "CFeatMgrPluginHandler::ProcessFeatureInfoL - pluginhandler %x, error %d - going to panic",
                                     this, aResponse().iErrorCode );
        // This is considered fatal enough reason to panic
        ::FmgrFatalErrorL(aResponse().iErrorCode, KPanicCategory, EPanicInvalidFeatureInfo);                       
        }
    }

// -----------------------------------------------------------------------------
// CFeatMgrPluginHandler::ProcessFeatureInfoL
// Asks server to process specified response from plugin 
// -----------------------------------------------------------------------------
//
void CFeatMgrPluginHandler::ProcessFeatureInfoL( 
        RFeatureArray& aList, 
        FeatureInfoCommand::TEnhancedFeatureInfoRespPckg aResponse )
    {
    FUNC_LOG

    // Go through dynamic feature list. Possible multiple same entries resolved by server.
    TInt err( KErrNone );
    TInt count = aResponse().iList.Count();
    
    if ( aResponse().iErrorCode == KErrNone )
        {
        for( TInt i = 0 ; i < count; i++)
            {    
            INFO_LOG2( "CFeatMgrPluginHandler::ProcessFeatureInfoL - featureID %d, data %d",
                aResponse().iList[i].FeatureUid(), aResponse().iList[i].FeatureData() );

            err = aList.Append( aResponse().iList[i] );
                
            if ( err != KErrNone )
                {
                ERROR_LOG1( "CFeatMgrPluginHandler::ProcessFeatureInfoL - Error %d when saving feature info",
                    err );
                break;                                
                }
            }

        if ( err == KErrNone )
            {   
            // Send feature info to the server
            iServer.FeatureInfoL( aList, this );
            }
        else
            {
            INFO_LOG1( "CFeatMgrPluginHandler::ProcessFeatureInfoL - FeatureInfoL not called %d", err );
            }
        }
    else
        {
        ERROR_LOG2( "CFeatMgrPluginHandler::ProcessFeatureInfoL - pluginhandler %x, error %d - going to panic",
                                     this, aResponse().iErrorCode );
        // This is considered fatal enough reason to panic
        ::FmgrFatalErrorL(aResponse().iErrorCode, KPanicCategory, EPanicInvalidFeatureInfo);                       
        }
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
