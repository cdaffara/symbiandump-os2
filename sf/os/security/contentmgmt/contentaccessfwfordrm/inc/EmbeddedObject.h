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
 @publishedAll
 @released
*/


#ifndef __EMBEDDEDOBJECT_H__
#define __EMBEDDEDOBJECT_H__

#include <apmstd.h>
#include <caf/virtualpath.h>
#include <caf/caftypes.h>

namespace ContentAccess
	{
	/** Represents a content object embedded within a file
	The agent defined uniqueId field describes the location of the object within 
	the file.

	*/
	class CEmbeddedObject : public CBase
		{
	public:
		/** Create a new CEmbeddedObject with a different name and uniqueId

		@param aUniqueId The UniqueId of the embedded object
		@param aName A user friendly name for the object
		@param aMimeType The mime type of the object (if it not a container object)
		@param aType The type of object (Content, container, agentspecific etc)
		@return a CEmbeddedObject
		*/
		IMPORT_C static CEmbeddedObject* NewL(const TDesC& aUniqueId, const TDesC& aName, const TDesC8& aMimeType, TEmbeddedType aType);

		/** Create a new CEmbeddedObject where the Name field is the same as the uniqueId

		@param aUniqueId The UniqueId of the embedded object
		@param aMimeType The mime type of the object (if it not a container object)
		@param aType The type of object (Content, container, agentspecific etc)
		@return a CEmbeddedObject
		*/
		IMPORT_C static CEmbeddedObject* NewL(const TDesC& aUniqueId, const TDesC8& aMimeType, TEmbeddedType aType);

		/** Create a new CEmbeddedObject from a stream 

		@param aStream The stream to read the CEmbeddedObject from
		@return a CEmbeddedObject
		*/
		IMPORT_C static CEmbeddedObject* NewL(RReadStream& aStream);

		/** destructor */
		virtual ~CEmbeddedObject();

		/** The agent defined UniqueId 
		@return The uniqueId
		*/
		IMPORT_C const TDesC& UniqueId() const;
		
		/** User friendly name of the object 
		@return The name of the object
		*/
		IMPORT_C const TDesC& Name() const;
		
		/** The type of object 
		@return The type
		*/
		IMPORT_C TEmbeddedType Type() const;	

		/** Mime type of the object
		@return The mime type of the object or KNullDesC8() if this is a container object
		*/
		IMPORT_C const TDesC8& MimeType() const;

		/** Allow the class write itself out to a stream 

		@param aStream The stream to write to
		*/ 
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	private:
		CEmbeddedObject(TEmbeddedType aType);
		void ConstructL(const TDesC& aUniqueId, const TDesC& aName, const TDesC8& aMimeType);

		void InternalizeL(RReadStream& aStream);

	private:
		TEmbeddedType iType;
		HBufC* iUniqueId;
		HBufC* iName;
		TBuf8 <KMaxDataTypeLength> iMimeType;
		};

	} // namespace ContentAccess

#endif
