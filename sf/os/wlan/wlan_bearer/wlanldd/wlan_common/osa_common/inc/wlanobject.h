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
* Description:   WlanObject declaration
*
*/

/*
* %version: 3 %
*/

#ifndef WLANOBJECT_H
#define WLANOBJECT_H

/**
 *  base object for every object instance that needs its validy
 *  checked after constructor call
 *
 *
 *  @lib wlanosa.lib
 *  @since S60 v3.2
 */
class WlanObject 
    {

public: 

    /**
     * checks validy
     *
     * @since S60 v3.2
     * @return ETrue for valid object any other for invalid object
     */
    inline TBool IsValid() const;

protected:

    /**
     * Constructor.
     *
     * @since S60 v3.2
     */
    WlanObject() : iValid( ETrue ) {};

    /**
     * validates object
     *
     * @since S60 v3.2
     */
    inline void Validate();

    /**
     * invalidates the object
     *
     * @since S60 v3.2
     */
    inline void InValidate();

private: // data

    /**
     * marker to mark is the object valid or not
     */
    TBool iValid;

    };

#include "wlanobject.inl"

#endif // WLANOBJECT_H
