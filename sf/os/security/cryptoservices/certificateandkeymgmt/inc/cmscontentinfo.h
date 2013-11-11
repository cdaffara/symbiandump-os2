/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CMSCONTENTINFO_H
#define CMSCONTENTINFO_H

#include "cmsdefs.h"

class CASN1EncSequence;

/**
The RFC2630 EncapsulatedContentInfo entity.
This class provides details about the Content Type in the Content Info,
provides access to the Content Data when the Content Type is Data and
provides access to the entire ContentInfo sequence.
*/
class CEncapsulatedContentInfo : public CBase
	{
public:

	/**
	 * @internalComponent
	 *
	 * Creates an EncapsulatedContentInfo as defined in RFC2630.
	 * @param aRawData the encoded encapsulated data.
	 * @return	The fully constructed object
	 **/
	static CEncapsulatedContentInfo* NewL(const TDesC8& aRawData);
	
	/**
	 * @internalComponent
	 * 
	 * Creates an EncapsulatedContentInfo as defined in RFC2630, and leaves the instance on the cleanup stack.
	 * @param aRawData The encoded encapsulated data.
	 * @return	The fully constructed object.
	 **/
	static CEncapsulatedContentInfo* NewLC(const TDesC8& aRawData);
	
	/**
	 * @internalComponent
	 *
	 * Creates an EncapsulatedContentInfo as defined in RFC2630.
	 * @param aContentInfoType The type of the encapsulated content.
	 * @param aIsEContentDataPresent Indicates whether the encapsulated data is present.
	 * @param aContentData The encapsulated data.
	 * @return	The fully constructed object.
	 **/
	static CEncapsulatedContentInfo* NewL(TCmsContentInfoType aContentInfoType, TBool aIsEContentDataPresent, const TDesC8& aContentData);

	/**
	 * @internalComponent
	 * 
	 * Creates an EncapsulatedContentInfo as defined in RFC2630, and leaves the instance on the cleanup stack.
	 * @param aContentInfoType The type of the encapsulated content.
	 * @param aIsEContentDataPresent Indicates whether the encapsulated data is present.
	 * @param aContentData The encapsulated data.
	 * @return	The fully constructed object
	 **/	
	static CEncapsulatedContentInfo* NewLC(TCmsContentInfoType aContentInfoType, TBool aIsEContentDataPresent, const TDesC8& aContentData);

	/**
	 * @internalComponent
	 *
	 * Creates the DER sequence of the ContentInfo and leaves the DER sequence on the cleanup stack.
	 * @return The ASN.1 sequence of the ContentInfo.
	 **/
	CASN1EncSequence* EncodeASN1DERLC() const;

	/**
	Provides information about the ContentType in the ContentInfo sequence.
	It returns the last number in the ObjectIdentifier.
	@return    The ContentType. This is:

	- 1 for Data

	- 2 for SignedData

	- 3 for EnvelopedData

	- 5 for DigestedData

	- 6 for EncryptedData
	*/
    IMPORT_C TCmsContentInfoType ContentType() const;
	
	/**
	Returns whether the ContentInfo is present or not.
	ContentData present in EncapsulatedContentInfo is OPTIONAL.
	@return  A boolean value that indicates whether or not the ContentInfo is present.
	*/
	IMPORT_C TBool IsContentDataPresent() const;

	/**
	Provides access to the Content Data present in CMS. ContentData present
	in EncapsulatedContentInfo is OPTIONAL. The client has to check for
	data length 0 or if the data content is present in case there is no ContentData.
	@return  The ContentData.
	*/    
	IMPORT_C const TPtrC8 ContentData() const;

private:
	/*
	Constuctor and second phase constructor.
	*/
	CEncapsulatedContentInfo();	
	CEncapsulatedContentInfo(TCmsContentInfoType aContentInfoType, TBool aIsEContentDataPresent, const TDesC8& aContentData);
	void ConstructL(const TDesC8& aRawData);
	
private:
	/**
	The Type of the ContentInfo
	*/
	TCmsContentInfoType iContentType;

	/**
	Represents whether the Content is present or not.
	*/
	TBool iIsContentDataPresent;

	/**
	The Content Data.
	*/
	TPtrC8 iContentData;
	};


/**
The RFC2630 ContentInfo entity.
This class provides details about the content type in the ContentInfo,
provides access to the content data when the content type is Data and
provides access to the entire ContentInfo sequence.
At present, only data content type and signed data type are supported.
*/
class CCmsContentInfo : public CBase
	{
public:
	/**
	Creates a ContentInfo as defined in RFC2630.
	@param aRawData The encoded encapsulated data.
	@return	The fully constructed object.
	*/
	IMPORT_C static CCmsContentInfo* NewL(const TDesC8& aRawData);
	/**
	Creates a ContentInfo as defined in RFC2630,
	and leaves the instance on the cleanup stack.
	@param aRawData The encoded encapsulated data.
	@return	The fully constructed object.
	*/
	IMPORT_C static CCmsContentInfo* NewLC(const TDesC8& aRawData);

	/**
	Creates a ContentInfo as defined in RFC2630.
	@param aContentInfoType The type of the encapsulated content.
	@param aContentData The encapsulated data.
	@return	The fully constructed object.
	*/
	IMPORT_C static CCmsContentInfo* NewL(TCmsContentInfoType aContentInfoType, const TDesC8& aContentData);

	/**
	Creates a ContentInfo as defined in RFC2630 and leaves the object on the cleanup stack.
	@param aContentInfoType The type of the encapsulated content.
	@param aContentData The encapsulated data.
	@return	The fully constructed object.
	*/
	IMPORT_C static CCmsContentInfo* NewLC(TCmsContentInfoType aContentInfoType, const TDesC8& aContentData);

	/**
	Creates the DER sequence of the ContentInfo and leaves the DER sequence on the cleanup stack.
	@return The ASN.1 sequence of the ContentInfo.
	*/
	IMPORT_C CASN1EncSequence* EncodeASN1DERLC() const;
	
	/**
	Destructor
	*/
	IMPORT_C ~CCmsContentInfo();
	
	/**
	Provides the information about the ContentType in the ContentInfo sequence.
	It returns the last number in the Object Identifier.
	@return The ContentType. This is:

	- 1 for Data

	- 2 for SignedData

	- 3 for EnvelopedData

	- 5 for DigestedData

	- 6 for EncryptedData
	*/
	IMPORT_C TCmsContentInfoType ContentType() const;

	/**
	Provides access to the content data.
	@return  The content data.
	*/
	IMPORT_C const TPtrC8 ContentData() const;


private:
	/**
	default constructor
	*/
	CCmsContentInfo();
	
	/**
	constructor to build the CMS content info from the given parameters
	@param aContentInfoType the type of data contained in the CMS content info
	@param aContentData the descriptor which contains the encoded CMS oject
	*/	
	CCmsContentInfo(TCmsContentInfoType aContentInfoType, const TDesC8& aContentData);
	
	/**
	constructor to build the CMS content info from the raw data.
	@param aContentInfoType the type of data contained in the CMS content info
	@param aContentData the descriptor which contains the encoded CMS oject
	*/		
	void ConstructL(const TDesC8& aRawData);
	
private:
	/**
	The Type of the ContentInfo
	*/
	TCmsContentInfoType iContentType;

	/**
	The Content Data
	*/	
	TPtrC8 iContentData;
	};

#endif // CMSCONTENTINFO_H

