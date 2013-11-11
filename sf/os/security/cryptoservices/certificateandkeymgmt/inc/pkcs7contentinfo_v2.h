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

#ifndef __PKCS7_CONTENT_INFO_V2_H__
#define __PKCS7_CONTENT_INFO_V2_H__

#include <asn1dec.h>

/** pkcs7-1 data */
_LIT(KPkcs7DataOID, "1.2.840.113549.1.7.1");

/** pkcs7-2 SignedData */
_LIT(KPkcs7SignedDataOID, "1.2.840.113549.1.7.2"); 

/** pkcs7-3 EnvelopedData */
_LIT(KPkcs7EnvelopedDataOID, "1.2.840.113549.1.7.3"); 

/** pkcs7-4 SignedAndEnvelopedData */
_LIT(KPkcs7SignedAndEnvelopedDataOID, "1.2.840.113549.1.7.4"); 

/** pkcs7-5 DigestedData */
_LIT(KPkcs7DigestedDataOID, "1.2.840.113549.1.7.5");   

/** pkcs7-6 EncryptedData */
_LIT(KPkcs7EncryptedDataOID, "1.2.840.113549.1.7.6"); 

/** The numeric value of last element in the PKCS7 Data OID */
const TInt KPkcs7Data = 1;

/** The numeric value of last element in the PKCS7 Signed OID */
const TInt KPkcs7SignedData = 2;    

/**
 The RFC2315 ContentInfo entity
 This class provides details about the Content Type in the Content Info, 
 Provides access to the Content Data when the Content Type is Data and 
 provides access to the Entire ContentInfo Sequence.
 */
class CPKCS7ContentInfo : public CBase
	{
public:
	/**
	 Identifies the type of Content Type present in the PKCS7 ContentInfo Structure.
	 */
	enum TContentInfoType
		{
		/** The numeric value of last element in the PKCS7 ContentType Data OID */
		EContentTypeData = 1,
		/** The numeric value of last element in the PKCS7 ContentType SignedData OID */
		EContentTypeSignedData,
		/** The numeric value of last element in the PKCS7 ContentType EnvelopedData OID */
		EContentTypeEnvelopedData,
		/** The numeric value of last element in the PKCS7 ContentType SignedAndEnvelopedData OID */
		EContentTypeSignedAndEnvelopedData,
		/** The numeric value of last element in the PKCS7 ContentType DigestedData OID */
		EContentTypeDigestedData,
		/** The numeric value of last element in the PKCS7 ContentType EncryptedData OID */
		EContentTypeEncryptedData
		};
	/**
	 Creates a new PKCS#7 ContentInfo object.  
	 @param     aRawData contains a PKCS#7 ContentInfo Structure
	 @return    A pointer to the newly allocated object.
	 @leave     KErrNotSupported if the OID value is otherthan enum TContentInfoType.
	 @leave 	KErrArgument if the data is not PKCS7 contentInfo structure.
	 @see		TContentInfoType
	 */
	IMPORT_C static CPKCS7ContentInfo* NewL(const TDesC8& aRawData);
		
	/**
	 Destructor.
	 */
	virtual ~CPKCS7ContentInfo();
	
public:
    /**
     Provides the information about the ContentType in the 
     ContentInfo Sequence.
     It returns the last number in the ObjectIdentifier
     @return    The ContentType. This is
			      1 for Data
			      2 for SignedData
			      3 for EnvelopedData
			      4 for SignedAndEnvelopedData
			      5 for DigestedData
			      6 for EncryptedData
     */
	IMPORT_C TContentInfoType ContentType() const;
	
	/**
     Provides access to the Content Data present in PKCS#7 ContentInfo
     ContentData present in PKCS#7 ContentInfo is OPTIONAL
     The client has to check for data length 0 in case there is no ContentData.
     @return    The ContentData 
     */
	IMPORT_C const TPtrC8 ContentData() const; 
	
private:
	/**
	 Constructor.
	 */
	CPKCS7ContentInfo(void);
	
    /**
	 Copy Constructor.
	 @param aContentinfo A CPKCS7ContentInfo object.
	 */
	CPKCS7ContentInfo(const CPKCS7ContentInfo& aContentInfo);
	
	/**
	 Assignment operator.
	 @param aContentinfo A CPKCS7ContentInfo object.
	 @return A reference to CPKCS7ContentInfo class.
	 */
	CPKCS7ContentInfo& operator=(const CPKCS7ContentInfo& aContentInfo);
	    
	/**
	 Decodes the PKCS#7 ContentInfo object.  
	 @param     aRawData contains a PKCS#7 ContentInfo Structure.
	 @leave     KErrNotSupported if the OID val is otherthan 1 - 6
	 			i,e the last part of the OID.
	 @leave 	KErrArgument if the data is not PKCS7 contentInfo structure.
	 @ see		TASN1DecObjectIdentifier, 
	 */
	void ConstructL(const TDesC8& aRawData);
	
private:
	/** 
	 Indicates the type of content. Here it is the last Character
	 in the OID which identifies the Content Type. 
	 */
	TContentInfoType iContentType;

	/** The ContentData present in PKCS7 ContentInfo*/
	TPtrC8 iContentData;
	};

#endif // __PKCS7_CONTENT_INFO_V2_H__


