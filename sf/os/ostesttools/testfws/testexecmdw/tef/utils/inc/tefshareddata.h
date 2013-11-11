/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



/**
 @file TEFSharedData.h
*/
#if !(defined __TEF_SHARED_DATA_H__)
#define __TEF_SHARED_DATA_H__
#include <e32std.h>

const TInt KMaxSharedObjectNameSize=30;

template <class T>
class CTEFSharedData : public CBase
/**
* @internalComponent
* @test
*/
	{
public:
	inline CTEFSharedData();
	inline ~CTEFSharedData();
	inline static CTEFSharedData<T>* NewL(T*& aPtr, const TInt aLength, const TDesC& aName);
	inline static CTEFSharedData<T>* NewLC(T*& aPtr, const TInt aLength, const TDesC& aName);
	inline static CTEFSharedData<T>* NewL(T*& aPtr, const TDesC& aName);
	inline static CTEFSharedData<T>* NewLC(T*& aPtr, const TDesC& aName);

	inline T* Ptr();
	inline void EnterCriticalSection();
	inline void ExitCriticalSection();

private:
	inline void ConstructL(T*& aPtr, const TInt aLength, const TDesC& aName);
	inline void ConstructL(T*& aPtr, const TDesC& aName);
	inline void SetNameL( const TDesC& aName );

private:
	T* iPtr;
	RChunk iSharedDataChunk;
	RMutex iMutex;
	TBuf<KMaxSharedObjectNameSize> iSharedChunkName;
	TBuf<KMaxSharedObjectNameSize> iSharedMutexName;
	};

#include <test/tefshareddata.inl>
#endif
