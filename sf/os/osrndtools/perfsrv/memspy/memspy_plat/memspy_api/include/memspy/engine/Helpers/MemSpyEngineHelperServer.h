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

#ifndef MEMSPYENGINEHELPERSERVER_H
#define MEMSPYENGINEHELPERSERVER_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyengineobject.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineServerList;



NONSHARABLE_CLASS( CMemSpyEngineHelperServer ) : public CBase
    {
public:
    static CMemSpyEngineHelperServer* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperServer();

private:
    CMemSpyEngineHelperServer( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C CMemSpyEngineServerList* ServerListL();
    IMPORT_C void GetServerSessionsL( TAny* aServerHandle, RArray<TMemSpyDriverServerSessionInfo>& aSessions );
    IMPORT_C void GetServerSessionsL( const TMemSpyDriverHandleInfoGeneric& aServerDetails, RArray<TMemSpyDriverServerSessionInfo>& aSessions );
    IMPORT_C void GetServerListL( RArray<TMemSpyDriverHandleInfoGeneric>& aServers );

public: // But not exported
    CMemSpyEngine& Engine() const;

private:
    CMemSpyEngine& iEngine;
    };




NONSHARABLE_CLASS( CMemSpyEngineServerEntry ) : public CBase
    {
public:
    static CMemSpyEngineServerEntry* NewLC( const TMemSpyDriverHandleInfoGeneric& aInfo );
    IMPORT_C ~CMemSpyEngineServerEntry();

private:
    CMemSpyEngineServerEntry();
    void ConstructL( const TMemSpyDriverHandleInfoGeneric& aInfo );

public:
    inline const TDesC& Caption() const { return *iCaption; }
    inline const TDesC& Name() const { return *iName; }
    inline const TInt SessionCount() const { return iSessionCount; }
    inline TInt Id() const { return iId; }
    inline TAny* Handle() const { return iHandle; }
    IMPORT_C void OutputDataL( CMemSpyEngineHelperServer& aHelper, TBool aClientThreadColumns ) const;
    IMPORT_C void GetSessionsL( CMemSpyEngineHelperServer& aHelper, RArray<TMemSpyDriverServerSessionInfo>& aSessions ) const;

private: // Member data
    HBufC* iCaption;
    HBufC* iName;
    TInt iSessionCount;
    TInt iId;
    TAny* iHandle;
    };




NONSHARABLE_CLASS( CMemSpyEngineServerList ) : public CBase, public MDesCArray
    {
public:
    static CMemSpyEngineServerList* NewLC();
    IMPORT_C ~CMemSpyEngineServerList();

private:
    CMemSpyEngineServerList();
    void ConstructL();

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C const CMemSpyEngineServerEntry& At( TInt aIndex ) const;
    IMPORT_C void SortByNameL();
    IMPORT_C void SortBySessionCountL();
    IMPORT_C TInt ItemIndex( const CMemSpyEngineServerEntry& aEntry ) const;
    IMPORT_C static void OutputDataColumnsL( CMemSpyEngine& aEngine, TBool aClientThreadColumns = EFalse );

public: // But not exported
    void AddItemL( CMemSpyEngineServerEntry* aItem );

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // Internal methods
    static TInt CompareByName( const CMemSpyEngineServerEntry& aLeft, const CMemSpyEngineServerEntry& aRight );
    static TInt CompareBySessionCount( const CMemSpyEngineServerEntry& aLeft, const CMemSpyEngineServerEntry& aRight );

private: // Data members
    RPointerArray<CMemSpyEngineServerEntry> iItems;
    };



#endif