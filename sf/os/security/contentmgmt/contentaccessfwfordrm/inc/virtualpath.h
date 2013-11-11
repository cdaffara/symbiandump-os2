/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CAFVIRTUALPATH_H__
#define __CAFVIRTUALPATH_H__

#include <e32base.h>

#include <caf/virtualpathptr.h>

class RReadStream;
class RWriteStream;

namespace ContentAccess
	{
	class TVirtualPathPtr;

	/**  A virtual path describes the location of the file (URI) and the location of
	a content object within that file (Unique Id). 
	
	The URI must conform to the standard defined in RFC2396, found at http://www.ietf.org/.
	
	CVirtualPath makes a copy of the URI and UniqueId used to identify a particular content
	object within a file.

	The TVirtualPathPtr cast operator allows the CVirtualPath object to be used with 
	CAF functions requiring a TVirtualPathPtr.

	The GetCombinedUriUniqueId() function allows it to "flatten" a virtual 
	path into a single URI. The combined virtual path will be the URI concatenated with the UniqueId
	seperated by the KCafVirtualPathSeparator character taking the format. 
	@code
		<URI><KCafVirtualPathSeparator><UniqueID>
	@endcode
	
	An example of this format is shown below:		
	@code
	// Create a CVirtualPath object to point to OBJECT1 inside file.dcf
	CVirtualPath *path = CVirtualPath::NewL(_L("C:\\directory\file.dcf"), _L("OBJECT1"));

	// convert the URI and unique ID into a single URI.
	TVirtualPathPtr aPath = path->GetCombinedUriUniqueId();
	@endcode
	@note
	If a URI is supplied which contains multiple KCafVirtualPathSeparator characters 
	the rightmost KCafVirtualPathSeparator character will be taken as marking the end
	of the URI and the start of the UniqueId. When multiple KCafVirtualPathSeparator
	characters are present, under certain situations this will result in an invalid 
	URI and UniqueId being created for the virtual path and can lead to an undefined failure.

	*/
	class CVirtualPath : public CBase
		{
	public:
		/** Create a CVirtualPath object from a TVirtualPathPtr 
		@param aPtr The location of the content object
		@return a new CVirtualPath object
		*/
		IMPORT_C static CVirtualPath* NewL(const TVirtualPathPtr& aPtr);

		/** Create a virtual path object from a combined Uri and UniqueId 
		
		This constructs a CVirtual path from a single descriptor. 
		
		Note that the descriptor here may be just a URI or it could be a URI
		concatenated with the file's UniqueId. If it is a concatenated URI and 
		UniqueId the URI and UniqueId will be seperated by the KCasfVirtualPathSeparator character.
		For more information see above.
		@param aCombinedUriUniqueId The location of the content object
		@return a new CVirtualPath object
		*/
		IMPORT_C static CVirtualPath* NewL(const TDesC& aCombinedUriUniqueId);

		/** Create a virtual path object for a specific content object within a file 
		@param aURI The location of the file
		@param aUniqueId The location of the content within the file
		@return a new CVirtualPath object
		*/
		IMPORT_C static CVirtualPath* NewL(const TDesC& aURI, const TDesC& aUniqueId);
		
		/** Construct and read a CVirtualPath object from a stream 
		@param aStream The stream to read frin
		*/
		IMPORT_C static CVirtualPath* NewL(RReadStream& aStream);


		virtual ~CVirtualPath();

		/** The location of the file containing the content object 
		@return The location of the file
		*/
		IMPORT_C const TDesC& URI() const;

		/** UniqueId supplied by a CAF Agent to identify the object within the 
		file.  
		@return The uniqueId of the object within the file
		*/
		IMPORT_C const TDesC& UniqueId() const;
		
		/** Allows the class to write itself to a stream 
		@param aStream	The stream to write to
		*/
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
		
		/** Convert the CVirtualPath to a single URI representing a particular 
		content object. The URI and UniqueId will be separated by a KCafVirtualPathSeparator()
		@return A descriptor contaning the URI and UniqueId concatenated together
		*/
		IMPORT_C const TDesC& GetCombinedUriUniqueId();

		/** Cast operator allowing the CVirtualPath to be used as a TVirtualPathPtr 
		@return a TVirtualPathPtr with the same value as this object
		*/
		inline operator const TVirtualPathPtr& () const;

	private:
		CVirtualPath(const TVirtualPathPtr& aPtr);	

		/** Allow derived classes to call 2nd phase constructor
		*/
		void ConstructL();

		/** Used to continue initialisation and setup iCombinedPtr */
		void CreateCombinedUriUniqueIdL(const TDesC& aUri, const TDesC& aUniqueId);

		void InternalizeL(RReadStream& aStream);

	private:
				
		HBufC* iURI;
		HBufC* iUniqueId;
		HBufC* iCombinedUriUniqueId;
		TPtrC iCombinedUriUniqueIdPtr;
		TVirtualPathPtr iVirtualPathPtr;
		};
		

	// Cast operator for treating a CVirtualPath as a TVirtualPathPtr
	inline CVirtualPath::operator const TVirtualPathPtr& () const
			{
			return iVirtualPathPtr;
			}
	}

#endif
