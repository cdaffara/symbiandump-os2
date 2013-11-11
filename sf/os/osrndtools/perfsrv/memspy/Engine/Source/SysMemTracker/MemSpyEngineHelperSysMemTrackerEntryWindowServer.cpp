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

#include "MemSpyEngineHelperSysMemTrackerEntryWindowServer.h"

// User includes
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyenginehelpersysmemtrackercycle.h>




CMemSpyEngineHelperSysMemTrackerEntryWindowServer::CMemSpyEngineHelperSysMemTrackerEntryWindowServer( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyEngineWindowGroupDetails& aWindowGroupDetails )
:   CMemSpyEngineHelperSysMemTrackerEntry( aTracker, EMemSpyEngineSysMemTrackerTypeFbserv ), 
    iCurrentWindowGroupDetails( aWindowGroupDetails ),
    iLastWindowGroupDetails( aWindowGroupDetails )
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryWindowServer::~CMemSpyEngineHelperSysMemTrackerEntryWindowServer()
    {
    }


void CMemSpyEngineHelperSysMemTrackerEntryWindowServer::ConstructL()
    {
    }


CMemSpyEngineHelperSysMemTrackerEntryWindowServer* CMemSpyEngineHelperSysMemTrackerEntryWindowServer::NewLC( CMemSpyEngineHelperSysMemTrackerImp& aTracker, const TMemSpyEngineWindowGroupDetails& aWindowGroupDetails )
    {
    CMemSpyEngineHelperSysMemTrackerEntryWindowServer* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerEntryWindowServer( aTracker, aWindowGroupDetails );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TUint64 CMemSpyEngineHelperSysMemTrackerEntryWindowServer::Key() const
    {
    return TUint64( iCurrentWindowGroupDetails.iId );
    }


void CMemSpyEngineHelperSysMemTrackerEntryWindowServer::UpdateFromL( const CMemSpyEngineHelperSysMemTrackerEntry& aEntry )
    {
    const CMemSpyEngineHelperSysMemTrackerEntryWindowServer& entry = static_cast< const CMemSpyEngineHelperSysMemTrackerEntryWindowServer& >( aEntry );
    iLastWindowGroupDetails = iCurrentWindowGroupDetails;
    iCurrentWindowGroupDetails = entry.iCurrentWindowGroupDetails;
    }


TBool CMemSpyEngineHelperSysMemTrackerEntryWindowServer::HasChangedL( const TMemSpyEngineHelperSysMemTrackerConfig& /*aConfig*/ ) const
    {
    const TBool hasChanged = ( iCurrentWindowGroupDetails.iIsFocused != iLastWindowGroupDetails.iIsFocused ) ||
                             ( iCurrentWindowGroupDetails.iCaption != iLastWindowGroupDetails.iCaption )
                             ;
    return hasChanged;
    }


void CMemSpyEngineHelperSysMemTrackerEntryWindowServer::CreateChangeDescriptorL( CMemSpyEngineHelperSysMemTrackerCycle& aCycle )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::TMemSpyWindowServerEvent event( CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::EMemSpyWindowServerEventNoEvent );
    if ( iCurrentWindowGroupDetails.iCaption != iLastWindowGroupDetails.iCaption )
        {
        event = CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::EMemSpyWindowServerEventNameChanged;
        CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::NewLC( Attributes(), iCurrentWindowGroupDetails, event );
        aCycle.AddAndPopL( changeDescriptor );
        }
    if ( iCurrentWindowGroupDetails.iIsFocused && !iLastWindowGroupDetails.iIsFocused )
        {
        event = CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::EMemSpyWindowServerEventFocusGained;
        CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::NewLC( Attributes(), iCurrentWindowGroupDetails, event );
        aCycle.AddAndPopL( changeDescriptor );
        }
    if ( !iCurrentWindowGroupDetails.iIsFocused && iLastWindowGroupDetails.iIsFocused )
        {
        event = CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::EMemSpyWindowServerEventFocusLost;
        CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::NewLC( Attributes(), iCurrentWindowGroupDetails, event );
        aCycle.AddAndPopL( changeDescriptor );
        }
    if ( event == CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::EMemSpyWindowServerEventNoEvent )
        {
        // If there were no events detected, it was a creation or deletion of a window group:
        CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup* changeDescriptor = CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::NewLC( Attributes(), iCurrentWindowGroupDetails );
        aCycle.AddAndPopL( changeDescriptor );        
        }
    }


void CMemSpyEngineHelperSysMemTrackerEntryWindowServer::UpdateCycleStatistics( CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    }























































CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup( TUint8 aAttribs, const TMemSpyEngineWindowGroupDetails aCurrent, TMemSpyWindowServerEvent aEvent )
:   CMemSpyEngineHelperSysMemTrackerCycleChange( aAttribs ),
    iCurrentWindowGroupDetails( aCurrent ),
    iEvent( aEvent )
    {
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::~CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup()
    {
    }


void CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::ConstructL()
    {
    BaseConstructL();
    }


CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup* CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::NewLC( TUint8 aAttribs, const TMemSpyEngineWindowGroupDetails aCurrent, TMemSpyWindowServerEvent aEvent )
    {
    CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup* self = new(ELeave) CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup( aAttribs, aCurrent, aEvent );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


TMemSpyEngineSysMemTrackerType CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::Type() const
    {
    return EMemSpyEngineSysMemTrackerTypeWindowServer;
    }
   

void CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::OutputHeaderL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KHeaderWindowServer, "Type, Id, Name, Order, Event, Attribs");
    aSink.OutputLineL( KHeaderWindowServer );
    }
 

void CMemSpyEngineHelperSysMemTrackerCycleChangeWindowGroup::OutputContentL( CMemSpyEngineOutputSink& aSink, CMemSpyEngineHelperSysMemTrackerCycle& /*aCycle*/ )
    {
    _LIT( KFormat, "%S,%d,%S,%d,%d,%S" );
    //
    TMemSpySWMTTypeName type;;
    FormatType( type );
    //
    TBuf<20> attribs;
    FormatAttributes( attribs );
    //
    TFullName name;
    if ( iCurrentWindowGroupDetails.iCaption.Length() )
        {
        name.Copy( iCurrentWindowGroupDetails.iCaption );
        }
    else
        {
        name.Copy( iCurrentWindowGroupDetails.iFullName );
        }

    HBufC* buf = HBufC::NewLC( 1024 );
    TPtr pBuf(buf->Des());
    
    // Now output line
    pBuf.Format( KFormat, 
                 &type,
                 //
                 iCurrentWindowGroupDetails.iId,
                 &name,
                 iCurrentWindowGroupDetails.iOrdinalPosition, 
                 iEvent,
                 &attribs
                 );

    aSink.OutputLineL( pBuf );
    CleanupStack::PopAndDestroy( buf );
    }
