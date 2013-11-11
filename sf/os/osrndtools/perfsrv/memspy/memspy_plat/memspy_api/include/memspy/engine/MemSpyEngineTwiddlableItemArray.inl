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

#ifndef MEMSPYENGINETWIDDLABLEITEMARRAY_INL
#define MEMSPYENGINETWIDDLABLEITEMARRAY_INL

//
// TMemSpyEngineVisibiltyFunction
//
template <class T>
inline TMemSpyEngineVisibiltyFunction<T>::TMemSpyEngineVisibiltyFunction( TBool (*aFunction)(const T*&, TAny* aRune), TAny* aRune )
:   iRune( aRune )
	{
    iFunction = (TMemSpyEngineGeneralVisibiltyFunction) aFunction;
    }


template <class T>
inline TMemSpyEngineVisibiltyFunction<T>::operator TMemSpyEngineGeneralVisibiltyFunction() const
	{
    return iFunction;
    }


template <class T>
inline TAny* TMemSpyEngineVisibiltyFunction<T>::Rune() const
    {
    return iRune;
    }







//
// CMemSpyEngineTwiddlableItemArray
//
template <class T>
inline CMemSpyEngineTwiddlableItemArray<T>::CMemSpyEngineTwiddlableItemArray( CMemSpyEngine& aEngine )
:   iEngine( aEngine )
    {
    }


template <class T>
inline CMemSpyEngineTwiddlableItemArray<T>::~CMemSpyEngineTwiddlableItemArray()
    {
    iAllEntries.ResetAndDestroy();
    iAllEntries.Close();

    // Doesn't own objects - just references those in the "iAllEntries" array
    iVisibleEntries.Close();
    }


template <class T>
inline TInt CMemSpyEngineTwiddlableItemArray<T>::Count() const
    {
    return iVisibleEntries.Count();
    }


template <class T>
inline void CMemSpyEngineTwiddlableItemArray<T>::ShowAllL()
    {
    TMemSpyEngineVisibiltyFunction<T> function( ShowAllItemsVisibilityFunction );
    ShowL( function );
    }


template <class T>
inline void CMemSpyEngineTwiddlableItemArray<T>::HideAllL()
    {
    iVisibleEntries.Reset();
    }


template <class T>
inline void CMemSpyEngineTwiddlableItemArray<T>::ShowL( TMemSpyEngineVisibiltyFunction<T> aFunction )
    {
    iVisibleEntries.Reset();
    //
    const TInt count = iAllEntries.Count();
    for( TInt i=0; i<count; i++ )
        {
        const T* entry = iAllEntries[ i ];
        const TAny* entryPtr = (TAny*) entry;
        //
        if  ( (*aFunction)( entryPtr, aFunction.Rune() ) )
            {
            iVisibleEntries.AppendL( entry );
            }
        }
    }


template <class T>
inline void CMemSpyEngineTwiddlableItemArray<T>::HideL( TMemSpyEngineVisibiltyFunction<T> aFunction )
    {
    iVisibleEntries.Reset();
    //
    const TInt count = iAllEntries.Count();
    for( TInt i=0; i<count; i++ )
        {
        const T* entry = iAllEntries[ i ];
        const TAny* entryPtr = (TAny*) entry;
        //
        if  ( (*aFunction)( entryPtr, aFunction.Rune() ) == EFalse )
            {
            iVisibleEntries.AppendL( entry );
            }
        }
    }


template <class T>
inline void CMemSpyEngineTwiddlableItemArray<T>::AddItemL( T* aItem )
    {
    TMemSpyEngineVisibiltyFunction<T> function( ShowAllItemsVisibilityFunction );
    AddItemL( aItem, function );
    }


template <class T>
inline void CMemSpyEngineTwiddlableItemArray<T>::AddItemL( T* aItem, TMemSpyEngineVisibiltyFunction<T> aFunction )
    {
    iAllEntries.AppendL( aItem );
    //
    const TAny* entryPtr = (TAny*) aItem;
    if  ( (*aFunction)( entryPtr, aFunction.Rune() ) )
        {
        iVisibleEntries.AppendL( aItem );
        }
    }


template <class T>
inline CMemSpyEngine& CMemSpyEngineTwiddlableItemArray<T>::Engine() const
    {
    return iEngine;
    }


template <class T>
inline T& CMemSpyEngineTwiddlableItemArray<T>::At( TInt aIndex )
    {
    T* entry = iVisibleEntries[ aIndex ];
    return *entry;
    }


template <class T>
inline const T& CMemSpyEngineTwiddlableItemArray<T>::At( TInt aIndex ) const
    {
    const T* entry = iVisibleEntries[ aIndex ];
    return *entry;
    }


template <class T>
inline TInt CMemSpyEngineTwiddlableItemArray<T>::ItemIndex( const T& aItem ) const
    {
    const TInt index = iVisibleEntries.Find( &aItem );
    return index;
    }


template <class T>
inline TBool CMemSpyEngineTwiddlableItemArray<T>::ShowAllItemsVisibilityFunction( const T*& /*aItem*/, TAny* /*aRune*/ )
    {
    return ETrue;
    }


template <class T>
inline void CMemSpyEngineTwiddlableItemArray<T>::Sort( TLinearOrder<T> aOrder )
    {
    iVisibleEntries.Sort( aOrder );
    }


#endif
