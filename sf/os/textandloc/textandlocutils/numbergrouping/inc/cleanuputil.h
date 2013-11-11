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

#ifndef CLEANUPUTIL_H_
#define CLEANUPUTIL_H_

#include <e32base.h>
    
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

#include "cleanuputil.inl"

#endif /* CLEANUPUTILS_H_ */
