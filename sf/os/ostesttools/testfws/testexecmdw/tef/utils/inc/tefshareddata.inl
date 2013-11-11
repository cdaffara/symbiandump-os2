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
* Template class implementation for creating RChunk object with memory for sharing data
* Construction of template object for sharing data requires
* Name for RChunk memory, size of data and null pointer of the object type to be shared
* Construction of template class for accessing the shared data requires
* Name for RChunk memory and null pointer of the object type already shared
*
*/



/**
 @file TEFSharedData.inl
 @prototype
 @test
*/

template <class T>
inline CTEFSharedData<T>::CTEFSharedData()
/**
 * Constructor
 */
	: iPtr(NULL)
	{
	// Intialising member data
	iSharedChunkName.Zero();
	iSharedMutexName.Zero();
	}

template <class T>
inline CTEFSharedData<T>* CTEFSharedData<T>::NewL(T*& aPtr, const TInt aLength, const TDesC& aName)
/**
 * Updates the null pointer with address of RChunk base for user to copy data
 * Two phase construction
 *
 * @param	aPtr - Address of null pointer to object that needs to be shared as a global chunk
 * @param	aLength - Size to be allocated in RChunk base for the object to be shared
 * @param	aName - Descriptor containing the name to be assigned to the global chunk
 *
 * @leave	system wide error
 *
 * @return	Instance of the TEFSharedData template class object
 */
	{
	CTEFSharedData<T>* self=CTEFSharedData<T>::NewLC(aPtr,aLength,aName);
	CleanupStack::Pop();
	return self;
	}

template <class T>
inline CTEFSharedData<T>* CTEFSharedData<T>::NewL(T*& aPtr, const TDesC& aName)
/**
 * Updates the null pointer with address of RChunk base for user to update data
 * Two phase construction
 *
 * @param	aPtr - Address of null pointer to object that is shared in a global chunk
 * @param	aName - Descriptor containing the name assigned to the global chunk
 *
 * @leave	system wide error
 *
 * @return	Instance of the TEFSharedData template class object
 */
	{
	CTEFSharedData<T>* self=CTEFSharedData<T>::NewLC(aPtr,aName);
	CleanupStack::Pop();
	return self;
	}

template <class T>
inline CTEFSharedData<T>* CTEFSharedData<T>::NewLC(T*& aPtr, const TInt aLength, const TDesC& aName)
/**
 * Updates the null pointer with address of RChunk base for user to copy data
 * Two phase construction
 *
 * @param	aPtr - Address of null pointer to object that needs to be shared as a global chunk
 * @param	aLength - Size to be allocated in RChunk base for the object to be shared
 * @param	aName - Descriptor containing the name to be assigned to the global chunk
 *
 * @leave	system wide error
 *
 * @return	Instance of the TEFSharedData template class object and pushes to CleanupStack
 */
	{
	CTEFSharedData<T>* self=new (ELeave) CTEFSharedData<T>();
	CleanupStack::PushL(self);
	self->ConstructL(aPtr,aLength,aName);
	return self;
	}

template <class T>
inline CTEFSharedData<T>* CTEFSharedData<T>::NewLC(T*& aPtr, const TDesC& aName)
/**
 * Updates the null pointer with address of RChunk base for user to update data
 * Two phase construction
 *
 * @param	aPtr - Address of null pointer to object that is shared in a global chunk
 * @param	aName - Descriptor containing the name assigned to the global chunk
 *
 * @leave	system wide error
 *
 * @return	Instance of the TEFSharedData template class object and pushes to CleanupStack
 */
	{
	CTEFSharedData<T>* self=new (ELeave) CTEFSharedData<T>();
	CleanupStack::PushL(self);
	self->ConstructL( aPtr, aName );
	return self;
	}

template <class T>
inline void CTEFSharedData<T>::ConstructL(T*& aPtr, const TInt aLength, const TDesC& aName)
/**
 * Opens up a handle and retrieves base address of the created chunk memory to iPtr
 * Updates the null pointer with address of RChunk base for user to copy data
 *
 * @param	aPtr - Address of null pointer to object that needs to be shared in a global chunk
 * @param	aLength - Size to be allocated in RChunk base for the object to be shared
 * @param	aName - Descriptor containing the name to be assigned to the global chunk
 *
 * @leave	system wide error
 */
	{
	iSharedChunkName.Zero();
	iSharedChunkName.Copy(_L("TEF"));
	iSharedChunkName.Append(aName);

	iSharedMutexName.Zero();
	iSharedMutexName.Copy(_L("TEFMutex"));
	iSharedMutexName.Append(aName);

	TInt error = KErrNone;
	error = iSharedDataChunk.CreateGlobal(iSharedChunkName, aLength, aLength + 1);
	if( error == KErrAlreadyExists )
		{
		// Chunk memory already created by another process
		// Open a handle to already created global chunk
		error = iSharedDataChunk.OpenGlobal(iSharedChunkName,EFalse);
		if( error != KErrNone )
			{
			User::Leave(error);
			}
		// Open a mutex handle to existing global chunk
		error = iMutex.OpenGlobal(iSharedMutexName);
		if( error != KErrNone )
			{
			User::Leave(error);
			}

		// If we have looked up a shared RChunk we will need to update it
		//  with the object passed into the constructor
		T* baseAddress = (T*)iSharedDataChunk.Base();
		iPtr = baseAddress;
		aPtr = iPtr;
		}
	else if( error == KErrNone )
		{
		// Global chunk created by current process
		// Copy the object to be shared to Chunks base address
		T* baseAddress = (T*)iSharedDataChunk.Base();
		iPtr = baseAddress;
		aPtr = iPtr;
		// Create a mutex object for the newly created global chunk
		error = iMutex.CreateGlobal(iSharedMutexName);
		if( error != KErrNone )
			{
			User::Leave(error);
			}
		}
	else
		{
		User::Leave(error);
		}
    }

template <class T>
inline void CTEFSharedData<T>::ConstructL(T*& aPtr, const TDesC& aName)
/**
 * Opens up a handle and retrieves base address of the created chunk memory to iPtr
 * Updates the null pointer with address of RChunk base for user to update data
 *
 * @param	aPtr - Address of null pointer to object type shared in a global chunk
 * @param	aName - Descriptor containing the name assigned to the global chunk
 *
 * @leave	system wide error
 */
	{
	SetNameL( aName );
	aPtr = Ptr();
    }


template <class T>
inline T* CTEFSharedData<T>::Ptr()
/**
 * @return - Pointer to the base of the RChunk object memory in heap
 */
	{
	// Since we are using RChunk::Close() the pointer will always be valid
	// The shared RChunk can not have been deleted as it is a reference counting object
	return iPtr;
	}

template <class T>
inline void CTEFSharedData<T>::SetNameL( const TDesC& aName )
/**
 * Opens up a handle to the existing chunk & retrieves the base address to iPtr
 *
 * @param aName - Name of the global chunk memory for a shared object
 */
	{
	// Check to see if we already have a handle to a shared RChunk
	if( iPtr != NULL )
		{
		// Close this before searching for the new one
		TInt handle = 0;
		handle = iSharedDataChunk.Handle();
		if(handle != 0)
			{
			iPtr = NULL;
			iMutex.Close();
			iSharedDataChunk.Close();
			}
		}

	// Now search for and open the new shared RChunk
	iSharedChunkName.Zero();
	iSharedChunkName.Copy(_L("TEF"));
	iSharedChunkName.Append(aName);

	iSharedMutexName.Zero();
	iSharedMutexName.Copy(_L("TEFMutex"));
	iSharedMutexName.Append(aName);

	TInt error = KErrNone;
	// Open a handle to a existing chunk memory
	error = iSharedDataChunk.OpenGlobal(iSharedChunkName,EFalse);
	if( error == KErrNone )
		{
		// Open a mutex handle for the chunk memory
		error = iMutex.OpenGlobal(iSharedMutexName);
		if( error == KErrNone )
			{
			// Retrieve base address of chunk memory & assign it to iPtr
			T* baseAddress = reinterpret_cast<T*>(iSharedDataChunk.Base());
			iPtr = baseAddress;
			}
		}

	if( error != KErrNone )
		{
		User::Leave(error);
		}
	}

template <class T>
inline void CTEFSharedData<T>::EnterCriticalSection()
/**
 * Establishes a lock to global chunk access
 * Makes other proces to wait for a signal
 */
	{
	iMutex.Wait();
	}

template <class T>
inline void CTEFSharedData<T>::ExitCriticalSection()
/**
 * Releases the lock to global chunk access
 * Signals for a process to establish a lock
 */
	{
	iMutex.Signal();
	}

template <class T>
inline CTEFSharedData<T>::~CTEFSharedData()
/**
 * Destructor
 */
	{
	// Cleanup everything.
	TInt handle = 0;
	// Checks for an existing handle to the global chunk
	// Ensures global objects are not closed already by another process
	handle = iMutex.Handle();
	if (handle != 0)
		{
		iMutex.Close();
		}

	handle = iSharedDataChunk.Handle();
	if (handle != 0)
		{
		iSharedDataChunk.Close();
		}
	iPtr = NULL;
	}
