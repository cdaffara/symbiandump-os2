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

#ifndef MEMSPYENGINETHREADINFOCONTAINER_H
#define MEMSPYENGINETHREADINFOCONTAINER_H

// System includes
#include <e32base.h>
#include <badesca.h>

// User includes
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/engine/memspyengineobject.h>

// Classes referenced
class CMemSpyThread;
class CMemSpyEngine;

class MMemSpyThreadInfoContainerObserver
    {
public: // Enumerations
    enum TEvent
        {
        EInfoItemChanged = 0,
        EInfoItemDestroyed
        };

public: // From MMemSpyThreadInfoContainerObserver
    virtual void HandleMemSpyEngineInfoContainerEventL( TEvent aEvent, TMemSpyThreadInfoItemType aType ) = 0;
    };

NONSHARABLE_CLASS( CMemSpyThreadInfoContainer ) : public CMemSpyEngineObject, public MDesCArray
    {
public:
    static CMemSpyThreadInfoContainer* NewL( CMemSpyThread& aThread, TBool aAsync = ETrue );
    static CMemSpyThreadInfoContainer* NewLC( CMemSpyThread& aThread, TBool aAsync = ETrue );
    static CMemSpyThreadInfoContainer* NewLC( CMemSpyThread& aThread, TMemSpyThreadInfoItemType aSpecificType );

private:
    CMemSpyThreadInfoContainer( CMemSpyThread& aThread );
    ~CMemSpyThreadInfoContainer();
    void ConstructL( TBool aAsync );
    void ConstructItemByTypeL( TBool aAsync, TMemSpyThreadInfoItemType aType );

public: // From CMemSpyEngineObject
    IMPORT_C void Open();
    IMPORT_C void Close();

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint(TInt aIndex) const;

public: // API
    inline CMemSpyThread& Thread() { return *iThread; }
    inline const CMemSpyThread& Thread() const { return *iThread; }
    IMPORT_C CMemSpyEngine& Engine() const;
    IMPORT_C CMemSpyThreadInfoItemBase& Item( TInt aIndex );
    IMPORT_C CMemSpyThreadInfoItemBase& Item( TMemSpyThreadInfoItemType aType );
    IMPORT_C TInt InfoItemIndexByType( TMemSpyThreadInfoItemType aType );
    IMPORT_C void ObserverAddL( MMemSpyThreadInfoContainerObserver& aObserver );
    IMPORT_C void ObserverRemove( MMemSpyThreadInfoContainerObserver& aObserver );
    IMPORT_C void PrintL();
    void AddItemL( TMemSpyThreadInfoItemType aType );

public: // But not exported
    void NotifyObserverL( MMemSpyThreadInfoContainerObserver::TEvent aEvent, TMemSpyThreadInfoItemType aType );

private: // Internal methods
    void OpenAllInfoItems();
    void CloseAllInfoItems();

private:
    CPeriodic* iIdleResetListboxTimer;
    CMemSpyThread* iThread;
    RArray< CMemSpyThreadInfoItemBase* > iItems;
    RArray< MMemSpyThreadInfoContainerObserver* > iObservers;
    };





#endif
