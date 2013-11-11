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
* Library to add s32strm support for IPC (ie. stream via multiple IPC read/writes instead of
* copying to a buffer and streaming to/from there.
*
*/


/**
 @file
 @internalTechnology
 @released
*/

#ifndef IPCSTREAM_INL
#define IPCSTREAM_INL

#include <scs/ipcstream.h>
#include <scs/streamingarray.h>


inline void WriteIntValueL(const RMessage2& aMessage, TInt aParam, TInt aValue)
	{
	TPckg<TInt> valuePak(aValue);
	aMessage.WriteL(aParam, valuePak);
	}

template <class T>
void WriteArraySizeL(const RMessage2& aMessage, TInt aParam, RPointerArray<T>& aArray)
	{
	TInt arraySize(0);
	// If the array is not empty, calculate and return the required buffer size to externalize it. 
	// Otherwise, return zero.
	if(aArray.Count())
		{
		arraySize = ExternalizedBufferSizeL(aArray);
		}
	WriteIntValueL(aMessage, aParam, arraySize);
	}

template <class T>
void WriteArraySizeL(const RMessage2& aMessage, TInt aParam, RArray<T>& aArray)
	{
	TInt arraySize(0); // if the array is empty, then zero length is returned
	if(aArray.Count())
		{ 
		arraySize = ExternalizedBufferSizeL(aArray);
		}
	WriteIntValueL(aMessage, aParam, arraySize);
	}

template <class T>
void WriteObjectSizeL(const RMessage2& aMessage, TInt aParam, const T* aObject)
	{
	// Calculate that how much buffer is needed to externalize the object
	TInt entrySize(0);
	if(aObject)
		{
		entrySize = GetObjectBufferSizeL(*aObject);
		}
	WriteIntValueL(aMessage, aParam, entrySize);
	}

template <class T>
void WriteArrayDataL(const RMessage2& aMessage, TInt aParam, RPointerArray<T>& aArray)
	{
	if(!aArray.Count())
		{
		User::Leave(KErrAbort);
		}
	RIpcWriteStream ipcstream;
	ipcstream.Open(aMessage, aParam);
	CleanupClosePushL(ipcstream);
	ExternalizePointersArrayL(aArray, ipcstream);
	CleanupStack::PopAndDestroy(&ipcstream); // Data is committed in Close method
	aArray.ResetAndDestroy(); // Reset the array to prevent it to be resent mistakenly.
	}

template <class T>
void WriteArrayDataL(const RMessage2& aMessage, TInt aParam, RArray<T>& aArray)
	{
	if(!aArray.Count())
		{
		User::Leave(KErrAbort);
		}
	RIpcWriteStream ipcstream;
	ipcstream.Open(aMessage, aParam);
	CleanupClosePushL(ipcstream);
	ExternalizeFixedLengthArrayL(aArray, ipcstream);
	CleanupStack::PopAndDestroy(&ipcstream); // Data is committed in Close method
	aArray.Reset(); // Reset the array to prevent it to be resent mistakenly.
	}

template <class T>
void WriteObjectDataL(const RMessage2& aMessage, TInt aParam, const T* aObject)
	{
	if(!aObject)
		{
		User::Leave(KErrAbort);
		}
	RIpcWriteStream ipcstream;
	ipcstream.Open(aMessage, aParam);
	CleanupClosePushL(ipcstream);
	ipcstream << *aObject;
	CleanupStack::PopAndDestroy(&ipcstream); // Data is committed in Close method
	}

template <class T>
T* ReadObjectFromMessageLC(const RMessage2& aMessage, TInt aSlot)
	{
	RIpcReadStream stream;
	CleanupClosePushL(stream);
	stream.Open(aMessage, aSlot);
	T *object = T::NewL(stream);
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PushL(object);
	return object;
	}

#endif /* IPCSTREAM_INL */
