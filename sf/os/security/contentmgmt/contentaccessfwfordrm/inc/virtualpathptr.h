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


#ifndef __CAFVIRTUALPATHPTR_H__
#define __CAFVIRTUALPATHPTR_H__

#include <e32base.h>

namespace ContentAccess
	{
	class CVirtualPath;

	/** 
	The character used to separate the URI from the UniqueId 
	when the two are concatenated into a single descriptor 
	*/
	_LIT(KCafVirtualPathSeparator, "|");

	/**	Identifies the location of a file and a particular content object inside it.

	TVirtualPathPtr points to the two descriptors (the URI and UniqueId) used 
	to initialise it. These descriptors must not be modified or destroyed while 
	the TVirtualPathPtr object is in use. 
	
	The URI must be in the format specified in RFC2396, found at http://www.ietf.org/.

	The UniqueId will be created by the Agent. Content is only ever
	accessed by one agent so it is the agents responsibility to ensure the 
	UniqueId is truly unique within the file.

	It is also possible to flatten a virtual path into a single descriptor. The
	ContentAccess::CVirtualPath class concatenates the URI, a separator, the KCafVirtualPathSeparator
	character, and the UniqueId to form a single URI. TVirtualPathPtr can be used to 
	decode this virtual path into the URI and the content object UniqueId. The 
	concatenated URI and UniqueId take the format:
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
	class TVirtualPathPtr
		{
	public:
		/** Constructor used when the URI and UniqueId fields are separate 
		@param aUri The location of the file
		@param aUniqueId The location of the content within the file
		*/
		IMPORT_C TVirtualPathPtr(const TDesC& aUri, const TDesC& aUniqueId);

		/** Constructor used for a concatenated URI and UniqueId.
		
		Note that the descriptor here may be just a URI or it could be a URI
		concatenated with the file's UniqueId. If it is a concatenated URI and 
		UniqueId the URI and UniqueId will be seperated by the KCafVirtualPathSeparator character.
		For more information see above.
		@param aCombinedUriUniqueId The location of the content object
		*/
		IMPORT_C TVirtualPathPtr(const TDesC& aCombinedUriUniqueId);

		/** Copy constructor */
		IMPORT_C TVirtualPathPtr(const TVirtualPathPtr& aPtr);

		/** The location of the file containing the content object 
		@return The location of the file
		*/
		IMPORT_C const TDesC& URI() const;

		/** UniqueId supplied by a CAF Agent to identify the object within the 
		file.  
		@return The location of the content within the file
		*/
		IMPORT_C const TDesC& UniqueId() const;

		/** Assignment operator */
		IMPORT_C TVirtualPathPtr& operator = (const TVirtualPathPtr& aVirtualPath);
	
		/** Assignment operator converts a single descriptor to a TVirtualPathPtr.
		If the descriptor is just a URI, the TVirtualPathPtr will point to the KDefaultContentObject within that file.
		If it is a concatenated URI and UniqueId the URI and UniqueId will be seperated by the KCafVirtualPathSeparator. 
		character as detailed in the description above.   
		*/
		IMPORT_C TVirtualPathPtr& operator = (const TDesC& aCombinedUriUniqueId);


	private:
		// The Set() function is only used by CVirtualPath
		friend class CVirtualPath;

		/** Used to redirect the TVirtualPathPtr to point at a different set of descriptors */
		void Set(const TDesC& aUri, const TDesC& aUniqueId);

	private:
		TPtrC iUri;
		TPtrC iUniqueId;
		};
	}

#endif
