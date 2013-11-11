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
* Description:  Class implementing a file-based storage for SSID lists.
*
*/


#include <f32file.h>
#include <s32file.h>
#include "wlanssidlistdb.h"

/** 
 * Path to SSID list database on disk.
 */ 
_LIT( KWlanSSidListDbFile, "c:\\private\\101f8ec5\\ssidlist.db" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanSsidListDb::CWlanSsidListDb()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanSsidListDb::ConstructL()
    {
    /**
     * Open a session to the file server.
     */ 
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    /**
     * Make sure the private directory exists.
     */
    fs.MkDirAll( KWlanSSidListDbFile );
    
    /**
     * Create a permanent file store for SSID lists.
     */
    CFileStore* store = CPermanentFileStore::ReplaceLC(
        fs,
        KWlanSSidListDbFile,
        EFileWrite );
    store->SetTypeL(KPermanentFileStoreLayoutUid);

    /**
     * Create the root stream. It's not really used for anything
     * since the file store is reset at every boot.
     */
    RStoreWriteStream stream;
    TStreamId streamId = stream.CreateLC( *store );
    stream.CommitL();
    store->SetRootL( streamId );
    store->CommitL();

    CleanupStack::PopAndDestroy(); // stream
    CleanupStack::PopAndDestroy(); // file store
    CleanupStack::PopAndDestroy( &fs );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanSsidListDb* CWlanSsidListDb::NewL()
    {
    CWlanSsidListDb* self = new (ELeave)CWlanSsidListDb();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CWlanSsidListDb::~CWlanSsidListDb()
    {
    iSsidListMap.Close();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanSsidListDb::WriteListL(
    TUint aIap,
    const CWlanSsidList& aSsidList )
    {
    TWlanSsidListMapEntry listEntry( aIap );

    /**
     * Locate the mapping entry.
     */
    TInt entryIdx( -1 );
    for( TInt idx( 0 ); entryIdx < 0 && idx < iSsidListMap.Count(); ++idx )
        {
        if( iSsidListMap[idx].iIap == aIap )
            {
            entryIdx = idx;
            }
        }

    /**
     * Handle an empty SSID list.
     */
    if( !aSsidList.Count() )
        {
        if( entryIdx >= 0 )
            {
            DeleteListL( aIap );
            }

        return;
        }

    /**
     * Open a session to the file server.
     */ 
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    /**
     * Open the file store for writing.
     */
    CFileStore* store = CPermanentFileStore::OpenLC(
        fs,
        KWlanSSidListDbFile,
        EFileWrite );
    
    /**
     * Create a stream for writing the SSID list data.
     */
    RStoreWriteStream stream;
    if( entryIdx < 0 )
        {
        listEntry.iStreamId = stream.CreateLC( *store );
        }
    else
        {
        stream.ReplaceLC( *store, iSsidListMap[entryIdx].iStreamId );
        }
    aSsidList.ExternalizeL( stream );
    stream.CommitL();
    store->CommitL();

    if( entryIdx < 0 )
        {
        /**
         * Store the mapping into the internal list if a new entry.
         */
        iSsidListMap.Append( listEntry );    
        }
    
    CleanupStack::PopAndDestroy(); // stream
    CleanupStack::PopAndDestroy(); // file store
    CleanupStack::PopAndDestroy( &fs );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanSsidListDb::ReadListL(
    TUint aIap,        
    CWlanSsidList& aSsidList )
    {
    /**
     * Locate the mapping entry.
     */
    TInt entryIdx( -1 );
    for( TInt idx( 0 ); entryIdx < 0 && idx < iSsidListMap.Count(); ++idx )
        {
        if( iSsidListMap[idx].iIap == aIap )
            {
            entryIdx = idx;
            }
        }
    if( entryIdx < 0 )
        {
        User::Leave( KErrNotFound );
        }

    /**
     * Open a session to the file server.
     */ 
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    /**
     * Open the file store for reading.
     */
    CFileStore* store = CPermanentFileStore::OpenLC(
        fs,
        KWlanSSidListDbFile,
        EFileRead );
    
    /**
     * Create a stream for writing the SSID list data.
     */
    RStoreReadStream stream;
    stream.OpenLC(
        *store,
        iSsidListMap[entryIdx].iStreamId  );
    aSsidList.InternalizeL( stream );

    CleanupStack::PopAndDestroy(); // stream
    CleanupStack::PopAndDestroy(); // file store
    CleanupStack::PopAndDestroy( &fs );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CWlanSsidListDb::DeleteListL(
    TUint aIap )
    {
    /**
     * Locate the mapping entry.
     */
    TInt entryIdx( -1 );
    for( TInt idx( 0 ); entryIdx < 0 && idx < iSsidListMap.Count(); ++idx )
        {
        if( iSsidListMap[idx].iIap == aIap )
            {
            entryIdx = idx;
            }
        }
    if( entryIdx < 0 )
        {
        User::Leave( KErrNotFound );
        }

    /**
     * Open a session to the file server.
     */ 
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    /**
     * Open the file store for writing.
     */
    CFileStore* store = CPermanentFileStore::OpenLC(
        fs,
        KWlanSSidListDbFile,
        EFileWrite );

    /**
     * Delete the stream.
     */
    store->DeleteL( iSsidListMap[entryIdx].iStreamId );
    store->CommitL();

    CleanupStack::PopAndDestroy(); // file store
    CleanupStack::PopAndDestroy( &fs );
    
    /**
     * Remove the mapping entry from the internal list.
     */
    iSsidListMap.Remove( entryIdx );
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CWlanSsidListDb::IsListAttached(
    TUint aIap )
    {
    for( TInt idx( 0 ); idx < iSsidListMap.Count(); ++idx )
        {
        if( iSsidListMap[idx].iIap == aIap )
            {
            return ETrue;
            }
        }

    return EFalse;
    }
