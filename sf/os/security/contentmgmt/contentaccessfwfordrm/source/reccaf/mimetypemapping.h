/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__MIMETYPEMAPPING_H__)
#define __MIMETYPEMAPPING_H__


/**
The Content Access Framework abstracts access to the file
and hence the content it contains. This class provides a mechanism
for mapping the mime type of the content within the file to be a 
prefixed version of that same mime type. The mime type is related to 
the content independent of the actual type of file containing the content.

Only used by the sample recognizer CCafApaRecognizer

@internalComponent
@released
*/

class CMimeTypeMapping : public CBase
	{
public:
	/** Creates a new CAF mime mapping for the given content type. 
	* For example a content type of <code>image/jpeg</code> will be automatically mapped to a 
	* CAF mime type of <code>x-caf-image/jpeg</code>.
	* 
	* @param aContentMimeType	The mime type of the content to be mapped to a new CAF mime type 
	*/
	static CMimeTypeMapping *NewL(const TDesC8& aContentMimeType);

	/** Explicitly creates a mapping from the content type to a CAF mime type.
	* 
	* @param aContentMimeType	The content mime type to be mapped to a new CAF mime type 
	* @param aCafDrmMimeType	The new CAF mime type.
	*/
	static CMimeTypeMapping *NewL(const TDesC8& aContentMimeType, const TDesC8& aCafDrmMimeType);
		
	~CMimeTypeMapping();

	/** Accessor for the Content Mime type. */
	const TDesC8& ContentMimeType() const;
	
	/** Accessor for the CAF mime mapped to the content type. */
	const TDesC8& CafMimeType() const;

	/** Compare iContentMimeType and return ETrue if the content is identical */
	static TBool ContentMimeTypeMatch(const CMimeTypeMapping& aFirst, const CMimeTypeMapping& aSecond);

private:
	CMimeTypeMapping();
	void ConstructL(const TDesC8& aContentMimeType);
	void ConstructL(const TDesC8& aContentMimeType, const TDesC8& aCafMimeType);
	
	HBufC8* iCafMimeType;
	HBufC8* iContentMimeType;
	};

#endif

