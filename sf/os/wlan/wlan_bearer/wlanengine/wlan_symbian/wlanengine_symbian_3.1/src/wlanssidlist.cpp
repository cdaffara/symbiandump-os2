/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class implementing storage for an SSID list.
*
*/


#include "wlanssidlist.h"
#include "am_debug.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanSsidList::CWlanSsidList(
    TUint aGranularity ) :
    iSsidList( aGranularity ),
    iLinearOrder( &CWlanSsidList::LinearOrder )
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanSsidList::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanSsidList* CWlanSsidList::NewL(
    TUint aGranularity )
    {
    CWlanSsidList* self = new (ELeave)CWlanSsidList( aGranularity );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanSsidList::~CWlanSsidList()
    {
    iSsidList.Close();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanSsidList::AddSsid(
    const TWlanSsid& aSsid )
    {
    if( !aSsid.Length() )
        {
        return KErrArgument;
        }

    /**
     * Silently ignore KErrAlreadyExists since the list may contain
     * duplicate entries.
     */
    TInt ret = iSsidList.InsertInOrder(
        aSsid,
        iLinearOrder );
    if( ret == KErrAlreadyExists )
        {
        return KErrNone;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CWlanSsidList::IsInList(
    const TWlanSsid& aSsid ) const
    {
    return (iSsidList.FindInOrder(
        aSsid,
        iLinearOrder ) >= 0 );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TUint CWlanSsidList::Count() const
    {
    return iSsidList.Count();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanSsidList::ExternalizeL(
    RWriteStream& aStream) const
    {
    aStream.WriteInt32L( iSsidList.Count() );
    for( TInt idx( 0 ); idx < iSsidList.Count(); ++idx )
        {
        aStream.WriteInt8L(
            iSsidList[idx].Length() );
        aStream.WriteL(
            iSsidList[idx] );        
        }        
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanSsidList::InternalizeL(
    RReadStream& aStream )
    {    
    TInt count( 
        aStream.ReadInt32L() );
    TWlanSsid ssid;

    for( TInt idx( 0 ); idx < count; ++idx )
        {
        aStream.ReadL(
            ssid, aStream.ReadInt8L() );
        iSsidList.Append(
            ssid );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CWlanSsidList::LinearOrder(
    const TWlanSsid& aLeft,
    const TWlanSsid& aRight )
    {
#if 0
    TBuf<KWlanMaxSsidLength> leftBuf;
    TBuf<KWlanMaxSsidLength> rightBuf;
    leftBuf.Copy( aLeft );
    rightBuf.Copy( aRight );
    DEBUG2( "LinearOrder() - aLeft: %S, aRight: %S",
        &leftBuf, &rightBuf );
#endif // 0

    return aLeft.Compare( aRight );
    }

