/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Handle declaration
*
*/

/*
* %version: 3 %
*/

#ifndef WLANHANDLE_H
#define WLANHANDLE_H

#include "wlanobject.h"

/**
 *  Just another handle in the handle body idiom  
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
template<class T> 
class Handle : public DBase, public WlanObject
    {
	typedef Handle<T> Self;

    friend TBool operator== ( const Self& aLhs, const Self& aRhs);

public:

    /**
     * ctor
     * NOTE: call IsValid() member from super class 
     * to validate object after ctor
     *
     * @since S60 v3.2
     * @param aElem data to be handled
     */
    explicit inline Handle( T& aElem );

    /**
     * delete data to be handled if this is the last owner
     *
     * @since S60 v3.2
     */
    virtual ~Handle();

    /**
     * copy ctor (one more owner for the data)
     *
     * @since S60 v3.2
     * @param aElem me, myself and I
     */
    inline Handle( const Self& aElem );

    /**
     * assignment operator (unshare old and share the new value)
     *
     * @since S60 v3.2
     * @param aElem me, myself and I
     * @return me, myself and I
     */
	inline Self& operator=( const Self& aElem );

    /**
     * operator ->
     *
     * @since S60 v3.2
     * @return representation
     */
	inline T* operator->();

    /**
     * extract data that is handled
     *
     * @since S60 v3.2
     * @return data that is handled
     */
    inline T* Data();

    /**
     * extract data that is handled
     *
     * @since S60 v3.2
     * @return data that is handled
     */
    inline const T* Data() const;

    /**
     * rebind handle to a new representation
     *
     * @since S60 v3.2
     * @param aElem data to be handled
     * @return ETrue for success any other failure
     */
    inline TBool Bind( T& aElem );

private:

    /**
     * removes the reference to the representation
     *
     * @since S60 v3.2
     */
    inline void Dispose();

private: // data

    /**
     * data to be handled
     */
    T*      iData;

    /**
     * reference count for the data to be handled
     */
    TInt*   iRefCount;   
    };

template<class T>
inline TBool operator== ( const Handle<T>& aLhs, const Handle<T>& aRhs)
    {
    return ( *(aLhs.iData) == *(aRhs.iData) );
    }

#include "osahandle.inl"

#endif // WLANHANDLE_H
