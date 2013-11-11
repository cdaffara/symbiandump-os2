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

#ifndef MEMSPYENGINEHELPERWINDOWSERVER_H
#define MEMSPYENGINEHELPERWINDOWSERVER_H

// System includes
#include <e32base.h>
#include <badesca.h>

// Classes referenced


/** 
 * WindowGroup basic info
 */
class TMemSpyEngineWindowGroupBasicInfo
    {
public:
    TInt iId;
    TFullName iFullName;
    TThreadId iThreadId;
    TInt iOrdinalPosition;
    };


/** 
 * WindowGroup details
 */
class TMemSpyEngineWindowGroupDetails : public TMemSpyEngineWindowGroupBasicInfo
    {
public:
    TInt iPriority;
    TInt iWindowGroupHandle;
    TFullName iName;
    TUid iUID;
    TBool iIsBusy;
    TBool iIsSystem;
    TBool iIsHidden;
    TFullName iCaption;
    TFullName iDocName;
    TBool iIsFocused;
    };



class MMemSpyEngineWindowGroupList : public MDesCArray
    {
public:
    virtual ~MMemSpyEngineWindowGroupList() {};

public: // API
    virtual TInt Count() const = 0;
    virtual const TMemSpyEngineWindowGroupBasicInfo& At( TInt aIndex ) const = 0;
    virtual TInt ItemIndex( const TMemSpyEngineWindowGroupBasicInfo& aEntry ) const = 0;

public: // But not exported
    virtual void AddItemL( TMemSpyEngineWindowGroupBasicInfo& aItem ) = 0;

public: // From MDesCArray
    virtual TInt MdcaCount() const = 0;
    virtual TPtrC MdcaPoint( TInt aIndex ) const = 0;
    };




class MMemSpyEngineHelperWindowServer
    {
public:
    virtual ~MMemSpyEngineHelperWindowServer(){};

public: // API
    virtual MMemSpyEngineWindowGroupList* WindowGroupListL() = 0;
    virtual void GetWindowGroupListL( RArray<TMemSpyEngineWindowGroupBasicInfo>& aWindowGroups ) = 0;
    virtual void GetWindowGroupDetailsL( TInt aWindowGroupId, TMemSpyEngineWindowGroupDetails& aWindowGroupDetails ) = 0;
    virtual void SwitchToL( TInt aWindowGroupId ) = 0;
    };

#endif
