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

#ifndef MEMSPYENGINETWIDDLABLEITEMARRAY_H
#define MEMSPYENGINETWIDDLABLEITEMARRAY_H

// System includes
#include <e32base.h>

// Classes referenced
class CMemSpyEngine;

// Type definitions
typedef TBool (*TMemSpyEngineGeneralVisibiltyFunction)( const TAny*&, TAny* aRune );



template <class T>
NONSHARABLE_CLASS( TMemSpyEngineVisibiltyFunction )
	{
public:
    inline TMemSpyEngineVisibiltyFunction( TBool (*aFunction)( const T*&, TAny* aRune ), TAny* aRune = NULL );

public: // API
    inline operator TMemSpyEngineGeneralVisibiltyFunction() const;
    inline TAny* Rune() const;

private:
	TMemSpyEngineGeneralVisibiltyFunction iFunction;
    TAny* iRune;
	};




template <class T>
NONSHARABLE_CLASS( CMemSpyEngineTwiddlableItemArray ) : public CBase
    {
public:
    inline ~CMemSpyEngineTwiddlableItemArray();

protected:
    inline CMemSpyEngineTwiddlableItemArray( CMemSpyEngine& aEngine );

public: // API
    inline TInt Count() const;
    inline void ShowAllL();
    inline void HideAllL();
    inline void ShowL( TMemSpyEngineVisibiltyFunction<T> aFunction );
    inline void HideL( TMemSpyEngineVisibiltyFunction<T> aFunction );
    inline void AddItemL( T* aItem );
    inline void AddItemL( T* aItem, TMemSpyEngineVisibiltyFunction<T> aFunction );
    //
    inline CMemSpyEngine& Engine() const;
    inline T& At( TInt aIndex );
    inline const T& At( TInt aIndex ) const;
    inline TInt ItemIndex( const T& aItem ) const;

protected: // Framework methods
    inline void Sort( TLinearOrder<T> aOrder );

private: // Internal methods
    static inline TBool ShowAllItemsVisibilityFunction( const T*& aItem, TAny* aRune );

private:
    CMemSpyEngine& iEngine;
    RPointerArray<T> iAllEntries;
    RPointerArray<T> iVisibleEntries;
    };




#include <memspy/engine/memspyenginetwiddlableitemarray.inl>

#endif
