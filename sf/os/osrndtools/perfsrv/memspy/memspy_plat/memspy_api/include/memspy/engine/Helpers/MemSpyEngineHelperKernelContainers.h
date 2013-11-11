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

#ifndef MEMSPYENGINEHELPERKERNELCONTAINERS_H
#define MEMSPYENGINEHELPERKERNELCONTAINERS_H

// System includes
#include <e32base.h>
#include <u32std.h>
#include <badesca.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include <memspy/engine/memspyengineobject.h>

// Classes referenced
class CMemSpyEngine;
class CMemSpyEngineOutputSink;
class CMemSpyEngineGenericKernelObjectList;
class CMemSpyEngineGenericKernelObjectContainer;



NONSHARABLE_CLASS( CMemSpyEngineHelperKernelContainers ) : public CBase
    {
public:
    static CMemSpyEngineHelperKernelContainers* NewL( CMemSpyEngine& aEngine );
    ~CMemSpyEngineHelperKernelContainers();

private:
    CMemSpyEngineHelperKernelContainers( CMemSpyEngine& aEngine );
    void ConstructL();

public: // API
    IMPORT_C CMemSpyEngineGenericKernelObjectList* ObjectsForSpecificContainerL( TMemSpyDriverContainerType aForContainer );
    IMPORT_C CMemSpyEngineGenericKernelObjectContainer* ObjectsAllL();
    IMPORT_C void OutputL( const CMemSpyEngineGenericKernelObjectContainer& aContainer ) const;

public: // But not exported
    CMemSpyEngine& Engine() const;
    CMemSpyEngineGenericKernelObjectContainer* ObjectsAllLightweightL();
    static TMemSpyDriverContainerType MapToType( TObjectType aType ); 

private: // Internal methods
    void AddObjectsFromContainerL( CMemSpyEngineGenericKernelObjectList& aList, TMemSpyDriverContainerType aContainer, TBool aGetInfo = ETrue );

private:
    CMemSpyEngine& iEngine;
    };







NONSHARABLE_CLASS( CMemSpyEngineGenericKernelObjectList ) : public CBase
    {
public:
    static CMemSpyEngineGenericKernelObjectList* NewLC( TMemSpyDriverContainerType aType, CMemSpyEngine& aEngine );
    IMPORT_C ~CMemSpyEngineGenericKernelObjectList();

private:
    CMemSpyEngineGenericKernelObjectList( TMemSpyDriverContainerType aType );
    void ConstructL( CMemSpyEngine& aEngine );

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C const TMemSpyDriverHandleInfoGeneric& At( TInt aIndex ) const;
    IMPORT_C TMemSpyDriverContainerType Type() const;
    IMPORT_C static TPtrC TypeAsString( TMemSpyDriverContainerType aType );
    IMPORT_C TPtrC Name() const;
    IMPORT_C void OutputL( CMemSpyEngineOutputSink& aSink ) const;
    IMPORT_C TInt Size() const;
    IMPORT_C TInt ItemsCount() const;

public: // But not exported
    TAny* HandleAt( TInt aIndex ) const;
    void AddItemL( TAny* aHandle );
    void AddItemL( const TMemSpyDriverHandleInfoGeneric& aItem );

private: // Internal methods
    void UpdateNameL();

private: // Data members
    TMemSpyDriverContainerType iType;
    RArray<TMemSpyDriverHandleInfoGeneric> iItems;
    RPointerArray<TAny> iHandles;
    HBufC* iName;
    TInt iSize;
    };






NONSHARABLE_CLASS( CMemSpyEngineGenericKernelObjectContainer ) : public CBase, public MDesCArray
    {
public:
    static CMemSpyEngineGenericKernelObjectContainer* NewLC();
    IMPORT_C ~CMemSpyEngineGenericKernelObjectContainer();

private:
    CMemSpyEngineGenericKernelObjectContainer();
    void ConstructL();

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C const CMemSpyEngineGenericKernelObjectList& At( TInt aIndex ) const;
    IMPORT_C void OutputL( CMemSpyEngineOutputSink& aSink ) const;

public: // But not exported
    void AddListL( CMemSpyEngineGenericKernelObjectList* aList );

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // Data members
    RPointerArray<CMemSpyEngineGenericKernelObjectList> iItems;
    };



#endif

