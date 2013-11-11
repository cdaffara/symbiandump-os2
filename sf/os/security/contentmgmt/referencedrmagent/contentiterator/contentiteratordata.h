/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent
 @released
*/


#ifndef __CAF_CONTENTITERATORDATA_H__
#define __CAF_CONTENTITERATORDATA_H__

#include <e32base.h>
#include <apmstd.h>
#include <caf/caftypes.h>

namespace ContentAccess
	{
	/** Operations that can be performed by the thread owned by CContentIterator
	
	@internalComponent
	@released
	*/
	enum TFileIteratorOperation
		{
		EIteratorShutdownThread			= 1,	///< The iterator thread must be shut down
		EIteratorFindNextContentObject	= 2		///< Find the next content object matching the requested mime type
		};

	/** Manages data shared between CContentIterator and the thread it uses to
	search for content.
	
	This class also includes a locking mechanism to ensure the data is thread-safe

	@internalComponent
	@released
	*/
	NONSHARABLE_CLASS(CContentIteratorData) : public CBase
		{
	public:
		/** Create a new CContentIteratorData object
		
		@param aSearchPath The path to search for content
		@param aRecursive ETrue to recursively search within directories
		@param aMimeType The mime type of content to search for, zero length indicates a wildcard
		@return a newCContentIteratorData object 
		*/
		static CContentIteratorData* NewL(const TDesC& aSearchPath, TBool aRecursive, const TDesC8& aMimeType);
		
		/** Destructor 
		*/
		virtual ~CContentIteratorData();
		
		/** Lock member data within this class. 
		
		Obtains a mutex lock representing the data within the class. If the data
		is already locked this function will wait until the lock is released
		before continuing.
		*/
		void Lock();

		/** Releases the mutex lock for the data within the class

		ie. enables another Lock() function to continue
		*/
		void Unlock();
		
		/** Complete a request in a client thread 
		
		@param aError The error code to complete the client
		*/
		void CompleteClientRequest(TInt aError);
		
		/** Set the thread Id and client request to complete when 
		content is found
		
		@param aClientThreadId The thread making the request
		@param aStatus The TRequestStatus to signal when content is found
		*/
		void SetClientRequest(TThreadId& aClientThreadId, TRequestStatus& aStatus);
		
		/** Allow the CContentIterator thread function to run
		
		@param aFunction The operation for the CContentIterator thread function to perform
		*/
		void RunThreadFunction(TFileIteratorOperation aFunction);		
		
		
		/** Wait for a call to RunThreadFunction()
		
		@return The function to execute in CContentIterators thread function 
		*/
		TFileIteratorOperation ThreadWait();		
		
		/** Set data relating to the content that was found 
		@param aFileName The name of the file where the content was found
		@param aUniqueId The uniqueId of the content within the file
		@param aName The name of the content object
		@param aMimeType The mime type of the content
		*/
		void SetData(const TDesC& aPath, const TDesC& aUniqueId, const TDesC& aName, const TDesC8& aMimeType);
		
		/** The path of the file containing content
		*/
		const TDesC& Path() const;
		
		/** The uniqueId of the content within the file found using Next()
		*/
		const TDesC& UniqueId() const;
		
		/** The name of the content object 
		*/
		const TDesC& Name() const;

		/** The mime type of the content
		*/
		const TDesC8& MimeType() const;
		
		/** Whether to perform a recursive search
		*/
		TBool IsRecursive() const;
		
	private:
	
		void ConstructL(const TDesC& aSearchPath, TBool aRecursive, const TDesC8& aMimeType);
		CContentIteratorData();
		
		TThreadId iClientThreadId;
		TRequestStatus* iClientRequest;
		
		RSemaphore iDataLockSemaphore;
		RSemaphore iThreadSemaphore;
		
		TFileIteratorOperation iFunction;
		TFileName iPath;
		TBuf8 <KMaxDataTypeLength> iMimeType;
		TBuf <KMaxCafUniqueId> iUniqueId;		
		TBuf <KMaxCafContentName> iName;		
		TBool iRecursive;
		};
	}
	
#endif
