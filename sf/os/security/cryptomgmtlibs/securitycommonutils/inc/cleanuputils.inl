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
*
*/


#ifndef CLEANUPUTILS_INL
#define CLEANUPUTILS_INL

#include <scs/cleanuputils.h>

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

template <typename T>
inline void DeleteObjectZ(T*& aPtr)
	{
	delete aPtr;
	aPtr=NULL;
	}

inline HBufC8* ConvertBufferTo8bitL(const TDesC& aBuffer)
	{
	HBufC8* buf8 = HBufC8::NewLC(aBuffer.Length());
	TPtr8 bufPtr8(buf8->Des());
	bufPtr8.Copy(aBuffer);
	CleanupStack::Pop(buf8);
	return buf8;
	}

inline HBufC* ConvertBufferTo16bitL(const TDesC8& aBuffer)
	{
	HBufC* buf = HBufC::NewLC(aBuffer.Length());
	TPtr bufPtr(buf->Des());
	bufPtr.Copy(aBuffer);
	CleanupStack::Pop(buf);
	return buf;
	}

#endif /* CLEANUPUTILS_INL */
