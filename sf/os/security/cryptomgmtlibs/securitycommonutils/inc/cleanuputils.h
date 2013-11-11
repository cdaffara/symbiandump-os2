/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* Template class CleanupResetAndDestroy to clean up the array
* of implementation information from the cleanup stack.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef CLEANUPUTILS_H
#define CLEANUPUTILS_H

#include <e32base.h>
	
template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef);

template <typename T>
inline void DeleteObjectZ(T*& aPtr);

inline HBufC8* ConvertBufferTo8bitL(const TDesC& aBuffer);
inline HBufC* ConvertBufferTo16bitL(const TDesC8& aBuffer);

#include <scs/cleanuputils.inl>

#endif /* CLEANUPUTILS_H*/
