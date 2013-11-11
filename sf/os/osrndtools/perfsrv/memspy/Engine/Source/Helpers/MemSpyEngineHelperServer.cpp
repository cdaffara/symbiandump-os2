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

#include <memspy/engine/memspyenginehelperserver.h>

// Driver includes
#include <memspy/driver/memspydriverclient.h>
#include <memspy/driver/memspydriverconstants.h>

// User includes
#include <memspy/engine/memspyengine.h>
#include <memspy/engine/memspyengineoutputsink.h>
#include <memspy/engine/memspyengineobjectthread.h>
#include <memspy/engine/memspyengineobjectprocess.h>
#include <memspy/engine/memspyengineobjectcontainer.h>
#include <memspy/engine/memspyenginehelperchunk.h>

// Literal constants
_LIT( KMemSpyEngineServListOutputComma, ", " );



CMemSpyEngineHelperServer::CMemSpyEngineHelperServer( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }

    
CMemSpyEngineHelperServer::~CMemSpyEngineHelperServer()
    {
    }


void CMemSpyEngineHelperServer::ConstructL()
    {
    }


CMemSpyEngineHelperServer* CMemSpyEngineHelperServer::NewL( CMemSpyEngine& aEngine )
    {
    CMemSpyEngineHelperServer* self = new(ELeave) CMemSpyEngineHelperServer( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


EXPORT_C void CMemSpyEngineHelperServer::GetServerSessionsL( TAny* aServerHandle, RArray<TMemSpyDriverServerSessionInfo>& aSessions )
    {
	const TInt KMaxCount = 256;
	TAny* handles[KMaxCount];
	TInt c = KMaxCount;

	TInt r = iEngine.Driver().GetServerSessionHandles( aServerHandle, handles, c );
	if  ( r == KErrNone )
    	{
        if  ( c > 0 )
            {
        	if (c > KMaxCount)
        		{
        		c = KMaxCount;
        		}

        	TMemSpyDriverServerSessionInfo info;
        	for (TInt i=0; i<c; i++)
        		{
        		r = iEngine.Driver().GetServerSessionInfo( handles[i], info );
        		if (r == KErrNone)
        			{
        			aSessions.AppendL( info );
        			}
        		}
            }
        }
    }


EXPORT_C void CMemSpyEngineHelperServer::GetServerSessionsL( const TMemSpyDriverHandleInfoGeneric& aServerDetails, RArray<TMemSpyDriverServerSessionInfo>& aSessions )
    {
    GetServerSessionsL( aServerDetails.iHandle, aSessions );
    }


EXPORT_C void CMemSpyEngineHelperServer::GetServerListL( RArray<TMemSpyDriverHandleInfoGeneric>& aServers )
    {
	const TInt KMaxCount = 256;
	TAny* handles[KMaxCount];
	TInt c = KMaxCount;

	TInt r = iEngine.Driver().GetContainerHandles( EMemSpyDriverContainerTypeServer, handles, c );
	if  ( r == KErrNone )
    	{
        if  ( c > 0 )
            {
        	if (c > KMaxCount)
        		{
        		c = KMaxCount;
        		}

        	TMemSpyDriverHandleInfoGeneric info;
        	for (TInt i=0; i<c; i++)
        		{
        		r = iEngine.Driver().GetGenericHandleInfo( KMemSpyDriverEnumerateContainerHandles, EMemSpyDriverContainerTypeServer, handles[i], info );
        		if (r == KErrNone)
        			{
        			aServers.AppendL( info );
        			}
        		}
            }
        }
    }


EXPORT_C CMemSpyEngineServerList* CMemSpyEngineHelperServer::ServerListL()
    {
    CMemSpyEngineServerList* list = CMemSpyEngineServerList::NewLC();
    //
    RArray<TMemSpyDriverHandleInfoGeneric> servers;
    CleanupClosePushL( servers );
    //
    GetServerListL( servers );
    //
    const TInt count = servers.Count();
    for( TInt i=0; i<count; i++ )
        {
        const TMemSpyDriverHandleInfoGeneric& details = servers[ i ];
        //
        CMemSpyEngineServerEntry* serverEntry = CMemSpyEngineServerEntry::NewLC( details );
        list->AddItemL( serverEntry );
        CleanupStack::Pop( serverEntry );
        }
    //
    CleanupStack::PopAndDestroy( &servers );

    // Sort the list by session count
    list->SortBySessionCountL();

    // Done
    CleanupStack::Pop( list );
    return list;
    }


CMemSpyEngine& CMemSpyEngineHelperServer::Engine() const
    {
    return iEngine;
    }



































CMemSpyEngineServerList::CMemSpyEngineServerList()
    {
    }


EXPORT_C CMemSpyEngineServerList::~CMemSpyEngineServerList()
    {
    iItems.ResetAndDestroy();
    iItems.Close();
    }


void CMemSpyEngineServerList::ConstructL()
    {
    }


CMemSpyEngineServerList* CMemSpyEngineServerList::NewLC()
    {
    CMemSpyEngineServerList* self = new(ELeave) CMemSpyEngineServerList();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


EXPORT_C TInt CMemSpyEngineServerList::Count() const
    {
    return iItems.Count();
    }


EXPORT_C const CMemSpyEngineServerEntry& CMemSpyEngineServerList::At( TInt aIndex ) const
    {
    return *iItems[ aIndex ];
    }


EXPORT_C void CMemSpyEngineServerList::SortByNameL()
    {
    TLinearOrder< CMemSpyEngineServerEntry > comparer( CompareByName );
    iItems.Sort( comparer );
    }


EXPORT_C void CMemSpyEngineServerList::SortBySessionCountL()
    {
    TLinearOrder< CMemSpyEngineServerEntry > comparer( CompareBySessionCount );
    iItems.Sort( comparer );
    }


EXPORT_C TInt CMemSpyEngineServerList::ItemIndex( const CMemSpyEngineServerEntry& aEntry ) const
    {
    TInt ret = KErrNotFound;
    //
    const TInt count = Count();
    for( TInt i=0; i<count; i++ )
        {
        const CMemSpyEngineServerEntry* item = iItems[ i ];
        //
        if  ( item == &aEntry )
            {
            ret = i;
            break;
            }
        }
    //
    return ret;
    }


EXPORT_C void CMemSpyEngineServerList::OutputDataColumnsL( CMemSpyEngine& aEngine, TBool aClientThreadColumns )
    {
    HBufC* columns = HBufC::NewLC( 1024 );
    TPtr pColumns( columns->Des() );
    //
    _LIT(KCol1, "Thread/Process Id");
    pColumns.Append( KCol1 );
    pColumns.Append( KMemSpyEngineServListOutputComma );
    //
    _LIT(KCol2, "Name");
    pColumns.Append( KCol2 );
    pColumns.Append( KMemSpyEngineServListOutputComma );
    //
    _LIT(KCol3, "Connected Session Count");
    pColumns.Append( KCol3 );
    //
    if  ( aClientThreadColumns )
        {
        _LIT(KCol4, "Connected Client");
        pColumns.Append( KMemSpyEngineServListOutputComma );
        pColumns.Append( KCol4 );
        }
    //
    aEngine.Sink().OutputLineL( pColumns );
    CleanupStack::PopAndDestroy( columns );
    }


void CMemSpyEngineServerList::AddItemL( CMemSpyEngineServerEntry* aItem )
    {
    iItems.AppendL( aItem );
    }


EXPORT_C TInt CMemSpyEngineServerList::MdcaCount() const
    {
    return Count();
    }


EXPORT_C TPtrC CMemSpyEngineServerList::MdcaPoint( TInt aIndex ) const
    {
    const CMemSpyEngineServerEntry& item = At( aIndex );
    return TPtrC( item.Caption() );
    }


TInt CMemSpyEngineServerList::CompareByName( const CMemSpyEngineServerEntry& aLeft, const CMemSpyEngineServerEntry& aRight )
    {
    const TInt ret = aLeft.Name().CompareF( aRight.Name() );
    return ret;
    }


TInt CMemSpyEngineServerList::CompareBySessionCount( const CMemSpyEngineServerEntry& aLeft, const CMemSpyEngineServerEntry& aRight )
    {
    TInt ret = -1;
    //
    if  ( aLeft.SessionCount() < aRight.SessionCount() )
        {
        ret = 1;
        }
    else if ( aLeft.SessionCount() == aRight.SessionCount() )
        {
        ret = 0;
        }
    //
    return ret;
    }



















CMemSpyEngineServerEntry::CMemSpyEngineServerEntry()
    {
    }


EXPORT_C CMemSpyEngineServerEntry::~CMemSpyEngineServerEntry()
    {
    delete iCaption;
    delete iName;
    }


void CMemSpyEngineServerEntry::ConstructL( const TMemSpyDriverHandleInfoGeneric& aInfo )
    {
    iName = HBufC::NewL( aInfo.iName.Length() );
    iName->Des().Copy( aInfo.iName );
    iSessionCount = aInfo.iCount;
    iId = aInfo.iId;
    iHandle = aInfo.iHandle;

    // Make caption
    _LIT(KServerListFormat, "\t%S\t\t%d session");
    TBuf<KMaxFullName + 128> item;
    //
    item.Format( KServerListFormat, iName, iSessionCount );
    //
    if  ( iSessionCount != 1 )
        {
        // Add missing 's' 
        item.Append( TChar('s') );
        }
    //
    iCaption = item.AllocL();
    }


CMemSpyEngineServerEntry* CMemSpyEngineServerEntry::NewLC( const TMemSpyDriverHandleInfoGeneric& aInfo )
    {
    CMemSpyEngineServerEntry* self = new(ELeave) CMemSpyEngineServerEntry();
    CleanupStack::PushL( self );
    self->ConstructL( aInfo );
    return self;
    }


EXPORT_C void CMemSpyEngineServerEntry::OutputDataL( CMemSpyEngineHelperServer& aHelper, TBool aClientThreadColumns ) const
    {
    _LIT(KMemSpyEngineServListOutputDecimal, "%d");
    _LIT(KMemSpyEngineServListOutputString, "%S");
    //
    HBufC* columns = HBufC::NewLC( 1024 );
    TPtr pColumns( columns->Des() );
    //
    pColumns.AppendFormat( KMemSpyEngineServListOutputDecimal, Id() );
    pColumns.Append( KMemSpyEngineServListOutputComma );
    //
    pColumns.AppendFormat( KMemSpyEngineServListOutputString, &Name() );
    pColumns.Append( KMemSpyEngineServListOutputComma );
    //
    pColumns.AppendFormat( KMemSpyEngineServListOutputDecimal, SessionCount() );
    //
    aHelper.Engine().Sink().OutputLineL( pColumns );

    if  ( aClientThreadColumns )
        {
        RArray<TMemSpyDriverServerSessionInfo> sessions;
        CleanupClosePushL( sessions );

        // Get the sessions
        GetSessionsL( aHelper, sessions );

        // Output an additional line per entry
        const TInt count = sessions.Count();
        for( TInt i=0; i<count; i++ )
            {
            const TMemSpyDriverServerSessionInfo& session = sessions[ i ];
            //
            pColumns.Copy( session.iName );
            pColumns.Insert( 0, KMemSpyEngineServListOutputComma );
            pColumns.Insert( 0, KMemSpyEngineServListOutputComma );
            pColumns.Insert( 0, KMemSpyEngineServListOutputComma );
            //
            aHelper.Engine().Sink().OutputLineL( pColumns );
            }
        //
        CleanupStack::PopAndDestroy( &sessions );
        }

    CleanupStack::PopAndDestroy( columns );
    }


EXPORT_C void CMemSpyEngineServerEntry::GetSessionsL( CMemSpyEngineHelperServer& aHelper, RArray<TMemSpyDriverServerSessionInfo>& aSessions ) const
    {
    aHelper.GetServerSessionsL( Handle(), aSessions );
    }


