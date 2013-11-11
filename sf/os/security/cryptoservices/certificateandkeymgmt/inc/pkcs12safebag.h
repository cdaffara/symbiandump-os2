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

#ifndef __PKCS12_SAFE_BAG_H__
#define __PKCS12_SAFE_BAG_H__

#include <asn1dec.h>
#include <pkcs12attribute.h>

namespace PKCS12
{
class CDecPkcs12Attribute;

/**
 This class is the base class for all the bags and decodes the SafeBag.
 SafeBag has BagId, BagValue, and BagAttributes.
 
 BagId's are in the form of an Object Identifiers
 OID ----  1.2.840.113549.1.12.10.1.1 keybag
 OID ----  1.2.840.113549.1.12.10.1.2 shroudedKeyBag
 OID ----  1.2.840.113549.1.12.10.1.3 certBag
 OID ----  1.2.840.113549.1.12.10.1.4 crlBag
 OID ----  1.2.840.113549.1.12.10.1.5 SecretBag
 OID ----  1.2.840.113549.1.12.10.1.6 SafeContentsBag
 **/	
class CDecPkcs12SafeBag	: public CBase
	{
public:
	enum TBagId
	/**
	 Identifies the type of Bag present in the PKCS#12 safeBag structure.
	 */
		{
		/** The numeric value of last element in the PKCS#12 KeyBag OID */
		EKeyBag = 1,
		
		/** The numeric value of last element in the PKCS#12 ShroudedKeyBag OID */
		EShroudedKeyBag,
		
		/** The numeric value of last element in the PKCS#12 CertBag OID */
		ECertBag,
		
		/** The numeric value of last element in the PKCS#12 CRLBag OID */
		ECrlBag,
		
		/** The numeric value of last element in the PKCS#12 SecretBag OID */
		ESecretBag,
		
		/** The numeric value of last element in the PKCS#12 SafeContentsBag OID */
		ESafeContentsBag
		};
	/**
	 Creates a new PKCS#12SafeBag object.
	  
	 @param aSafeBagData contains a PKCS#12 SafeBag Structure
	 @return A pointer to the newly allocated object.
	 @leave  KErrArgument if the data is not Pkcs12 safeBag structure.
	 */
    IMPORT_C static CDecPkcs12SafeBag* NewL(const TDesC8& aSafeBagData);
    
	/**
	 The BagId present in the SafeBag.
	 BagID is an ObjectIdentifier which identifies the Type of bag present
	 inside the SafeBag.
	 @return BagID present within the SafeBag
	 */
     IMPORT_C TBagId BagID() const ; 
    
	/**
	 This method returns the BagValue present in the Safe Bag.
	 @return BagValue present within the SafeBag
	 */
     IMPORT_C const TDesC8& BagValue() const ;
    
	/**
	 This method returns an array of BagAttributes.
	 BagAttributes is an Optional field. It allows users to assign nicknames
	 and identifiers to keys, etc., and permits visual tools to display 
	 meaningful strings of some sort to the user.
	 @return an array of BagAttributes present within the SafeBag.
	 @see CDecPkcs12Attribute
	 */
     IMPORT_C const RPointerArray<CDecPkcs12Attribute>& BagAttributes() const ;
        
    /**
	 Destructor.
	 */
	IMPORT_C ~CDecPkcs12SafeBag();
    
protected:
	/**
	 * @internalComponent
	 *
	 * This decodes the entire safeBag structure.
	 * @param aRawData contains a PKCS#12 safeBag Structure.
	 * @leave  KErrArgument if the data is not Pkcs12 safeBag structure.
	 * @see CDecPkcs12Attribute
	 **/
	void ConstructL(const TDesC8& aRawData);	
	
	/**
	 * @internalComponent
	 *
	 * Constructor.
	 **/
	CDecPkcs12SafeBag();
	
	/**
	 * @internalComponent
	 *
	 * Copy Constructor.
	 * @param aDecPkcs12safeBag A CDecPkcs12SafeBag object.
	 **/
	CDecPkcs12SafeBag(const CDecPkcs12SafeBag& aDecPkcs12safeBag);
	
	/**
	 * @internalComponent
	 *
	 * Assignment operator.
	 * @param aDecPkcs12safeBag A CDecPkcs12SafeBag object.
	 * @return A reference to CDecPkcs12SafeBag class.
	 **/
	CDecPkcs12SafeBag& operator=(const CDecPkcs12SafeBag& aDecPkcs12safeBag);
	
protected:
 	/** Contains the BagId. That is ObjectIdentifier for SafeContents Bag */
	TBagId iBagId;
	
	/** Contains the BagValue */
	TPtrC8 iBagValue;
	
	/* contains the BagAttributes */
	RPointerArray <CDecPkcs12Attribute> iBagAttributes;
	};
} // namespace PKCS12
#endif // __PKCS12_SAFE_BAG_H__
