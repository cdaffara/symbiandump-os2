/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __PKCS12ATTRIBUTE_H__
#define __PKCS12ATTRIBUTE_H__

#include <asn1dec.h>

namespace PKCS12
{
/**
 This class decodes the bagAttributes present in the SafeBag.
 It has methods to get the AttributeId and the AttributeValue.
 */
class CDecPkcs12Attribute : public CBase
	{
public:
    /**
	 Creates a new PKCS#12 attribute object.
	
	 PKCS#12AttrSet ATTRIBUTE ::= {
	 	friendlyName |
	 	localKeyId,
	 	... -- Other attributes are allowed
	 	}  
	 	
	 @param  aBagAttributes Contains a PKCS#12 attribute set.
	 @return A pointer to the newly allocated object.
	 @leave  if the bag attribute set is not a sequence or class 
	 		 tag name is not Universal.
	 */
	IMPORT_C static CDecPkcs12Attribute* NewL(const TDesC8& aBagAttributes);
	
	/**
	 Returns the ObjectIdentifier indicating the attribute type:
	 - OID ---- 1.2.840.113549.1.9.20 friendlyName
     - OID ---- 1.2.840.113549.1.9.21 localKeyId 
	 @return An object identifier indicating the attribute type.
	 */
	IMPORT_C const TDesC& AttributeId() const;
	
	/**
	 Returns the DER encoded attribute values present in the Safe Bag.
	 @return The ASN1Set of attribute values
	 */
	IMPORT_C const RPointerArray<TDesC8>& AttributeValue() const;
			
	/**
	 Destructor.
	 */
	virtual ~CDecPkcs12Attribute();
	
private:
	/**
     This decodes the entire BagAttributes structure
     @param aBagAttributes Contains a PKCS#12 attribute set.
     @leave if the bag attribute set is not a sequence or class 
	 		 tag name is not Universal.
     */
	void ConstructL(const TDesC8& aBagAttributes);
		  
	/**
	 Construtor.
	 */
	 CDecPkcs12Attribute();
		  
	/**
	 Copy Constructor.
	 @param aDecPkcs12Atrribute A CDecPkcs12Attribute object.
	 */
	 CDecPkcs12Attribute(const CDecPkcs12Attribute& aDecPkcs12Atrribute);
	 
	 /**
	  Assignment operator.
	  @param aDecPkcs12Atrribute A CDecPkcs12Attribute object.
	  @return A reference to CDecPkcs12Attribute class.
	  */
	 CDecPkcs12Attribute& operator=(const CDecPkcs12Attribute& aDecPkcs12Atrribute);
	 
private:
	/** Contains the object identifier indicating the attribute type */
	HBufC* iAttributeId;
	
	/** Contains the attribute value which is an DER encoded value */
	RPointerArray<TDesC8> iAttributeValue;
	};
} // namespace PKCS12
#endif // __PKCS12ATTRIBUTE_H__


