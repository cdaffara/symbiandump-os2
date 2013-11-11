/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

@publishedAll
@released
*/


#ifndef __DIRSTREAMABLE_H__
#define __DIRSTREAMABLE_H__

#include <f32file.h>
#include <s32strm.h>
#include <caf/caftypes.h>

namespace ContentAccess
	{
	/** Emulates the behavior of a CDir object with the addition of
	streaming. 
	
	Functions are not virtual and do not override the CDir
	functions so most of the time this class will be used as if it was 
	a CDir. It is only when used explicitly as a CDirStreamable that
	any additional functionality is invoked

	*/
	class CDirStreamable : public CDir
		{
	public:
		/** Constuct an empty CDirStreamable 
		@return a CDirStreamable with no entries
		*/
		IMPORT_C static CDirStreamable* NewL();

		/** Construct a CDirStreamable object from the result
		of a call to RFs::GetDir()
		
		@param aDir A CDir object which has been constructed by a previous call 
		to RFs::GetDir()
		@return A new CDirStreamable object
		*/
		IMPORT_C static CDirStreamable* NewL(CDir& aDir);
		
		/** Construct a CDirStreamable object from a stream
		
		@param aStream The stream to read the CDirStreamable object from
		@return A new CDirStreamable object
		*/
		IMPORT_C static CDirStreamable* NewL(RReadStream aStream);
		
	public:
        virtual ~CDirStreamable();

		/** Add an entry 
		@param aEntry The directory entry to add
		*/
		IMPORT_C void AddL(const TEntry &aEntry);

		/** The number of file system entries in the CDirStreamable
		
		@return The number of TEntry objects in the CDirStreamable
		*/
	    IMPORT_C TInt Count() const;
	    
	    /** Retrieve the file system entry at a particular index
		
		@see TEntry

	    @param aIndex The entry to retrieve
	    @return A reference to the TEntry stored by the CDirStreamable
		*/
		IMPORT_C const TEntry& operator[](TInt aIndex) const;
		
	    /** Sort the entries in the specified order 
	    @param aEntrySortKey A TEntryKey value used to sort the array.
	    @return KErrNone if sort was successful
		*/
        IMPORT_C TInt Sort(TUint aEntrySortKey);
        
    public:
		/** Write this CDirStreamable to a stream
	    @param aStream The stream to write to
		*/
    	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	private:
		CDirStreamable();
		void ConstructL();
		void ConstructL(CDir& aDir);
		
		void InternalizeL(RReadStream& aStream);
		};
	}
	
#endif

