/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* FORM Cursor Navigation header file holding policy interface class.
*
*/
#ifndef CLEANUPUTIL_INL
#define CLEANUPUTIL_INL

#include "cleanuputil.h"

template <class T>
class CleanupResetAndDestroy
    {
public:
    inline static void PushL(T& aRef);
private:
    static void ResetAndDestroy(TAny *aPtr);
    };

template <class T>
inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
    {
    CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));
    }

template <class T>
void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
    {
    static_cast<T*>(aPtr)->ResetAndDestroy();
    }

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
    {
    CleanupResetAndDestroy<T>::PushL(aRef);
    }
	
#endif
