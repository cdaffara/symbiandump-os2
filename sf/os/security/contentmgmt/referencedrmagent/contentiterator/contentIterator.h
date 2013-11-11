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
 @publishedPartner
 @released
*/


#ifndef __CAF_CONTENTITERATOR_H__
#define __CAF_CONTENTITERATOR_H__

#include <e32base.h>
#include <apmstd.h>
#include <caf/caftypes.h>

namespace ContentAccess 
	{
	class CContentIteratorData;
	class TVirtualPathPtr;
	
	/** This class can be used to asynchronously search through directories
	on the files system to find content with a particular mime type
	
	This class creates a thread that is used to search recursively. It uses
	a considerable amount of memory and should be destroyed as soon as it
	is no longer needed.
	 
	It must work in a thread rather than as a server so the CAF agents can
	perform capability checking against the client process. It uses a thread
	because it is recursive and could lead to a stack overflow if the recursion
	occurred in the client thread
	
	Since CContentIterator is an active object clients should not 
	use the User::WaitForRequest() API since it will not give 
	CContentIterator::RunL() a chance to run.
	
	@publishedPartner
	@released
	*/
	class CContentIterator : public CActive
		{
	public:
	
		/** Create a CContentIterator
		
		@param aPath The path to search for content
		@param aRecursive ETrue to recursively search within directories
		@param aMimeType The mime type to search for, a zero length descriptor can be used as a wildcard to find all content
		@return a new CContentIterator
		*/
		IMPORT_C static CContentIterator* NewL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType);

		/** Destructor */
		virtual ~CContentIterator();

		/** Find the next content object
		
		Clients should not use the User::WaitForRequest() API when calling 
		the Next() function since it will not give CContentIterator's RunL() 
		a chance to run.
		
		@param aStatus Request to complete when the next content item is found or KErrNotFound if no further content was found
		*/
		IMPORT_C void Next(TRequestStatus &aStatus);

		/** The name of the file containing the content object found in the most recent call to Next()
	
		@return The name of the file
		*/
		IMPORT_C TVirtualPathPtr VirtualPath();

		/** The mime type of the content object found in the most recent call to Next()
	
		@return The name of the file
		*/
		IMPORT_C const TDesC8& MimeType();

		/** The name of the content object found in the most recent call to Next()
	
		@return The name of the content object
		*/
		IMPORT_C const TDesC& Name();


	protected:
		virtual void DoCancel();
		virtual void RunL();
			
	private:
		CContentIterator();
		void ConstructL(const TDesC& aPath, TBool aRecursive, const TDesC8& aMimeType);
		
		static TInt ThreadEntry(TAny* aAny);
	
	private:
		TBuf8 <KMaxDataTypeLength> iMimeType;
		TFileName iFileName;
		TBuf <KMaxCafContentName> iName;
		TBuf <KMaxCafUniqueId> iUniqueId;
	
		RThread iWorkerThread;	
		CContentIteratorData* info;
		};
	}

#endif
