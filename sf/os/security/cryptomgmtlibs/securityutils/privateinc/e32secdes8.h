/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




/**
 @file
 @internalTechnology
*/

#ifndef __TSECBUF8_H__
#define __TSECBUF8_H__

#include <e32des8.h>

template <TInt S> 
class TSecBuf8 : public TBuf8<S>
	{
public:
	inline TSecBuf8(void);
	inline TSecBuf8(TInt aLength);
	inline TSecBuf8(const TUint8* aString);
	inline TSecBuf8(const TDesC8& aDes);
	inline ~TSecBuf8(void);
	inline TSecBuf8<S>& operator=(const TUint8* aString);
	inline TSecBuf8<S>& operator=(const TDesC8& aDes);
	inline TSecBuf8<S>& operator=(const TSecBuf8<S>& aBuf);
	};

template <TInt S> 
inline TSecBuf8<S>::TSecBuf8(void) 
	: TBuf8<S>() 
	{}
template <TInt S> 
inline TSecBuf8<S>::TSecBuf8(TInt aLength) 
	: TBuf8<S>(aLength) 
	{}
template <TInt S> 
inline TSecBuf8<S>::TSecBuf8(const TUint8* aString)
	: TBuf8<S>(aString) 
	{}
template <TInt S> 
inline TSecBuf8<S>::TSecBuf8(const TDesC8& aDes)
	: TBuf8<S>(aDes) 
	{}
template <TInt S> 
inline TSecBuf8<S>::~TSecBuf8(void)
	{
	FillZ(MaxLength());
	SetLength(0);
	}
template <TInt S>
inline TSecBuf8<S>& TSecBuf8<S>::operator=(const TUint8* aString)
	{
	Copy(aString); return(*this);
	}
template <TInt S>
inline TSecBuf8<S>& TSecBuf8<S>::operator=(const TDesC8& aDes)
	{
	Copy(aDes); return(*this);
	}
template <TInt S>
inline TSecBuf8<S>& TSecBuf8<S>::operator=(const TSecBuf8<S>& aBuf)
	{
	Copy(aBuf); return(*this);
	}

#endif
