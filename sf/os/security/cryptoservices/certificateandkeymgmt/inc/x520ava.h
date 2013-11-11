/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implementation of an attribute type and a value, as defined by the X.520 standard
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#if !defined(__X520AVA_H__)
#define __X520AVA_H__

#include <e32base.h>
#include <e32std.h>
#include <s32std.h>

//these are for internal use only
//these are the only attribute types we handle at present
_LIT(KX520CountryName,"2.5.4.6");
_LIT(KX520OrganizationName,"2.5.4.10");
_LIT(KX520OrganizationalUnitName,"2.5.4.11");
_LIT(KX520LocalityName,"2.5.4.7");
_LIT(KX520StateOrProvinceName,"2.5.4.8");
_LIT(KX520Title,"2.5.4.12");
_LIT(KX520CommonName,"2.5.4.3");
_LIT(KX520GivenName,"2.5.4.42");
_LIT(KX520Surname,"2.5.4.4");
_LIT(KX520Initials,"2.5.4.43");
_LIT(KX520GenerationQualifier,"2.5.4.44");
_LIT(KX520DNQualifier,"2.5.4.46");
_LIT(KX520SerialNumber,"2.5.4.5");
_LIT(KX520Description,"2.5.4.13");

//more attribute types we handle at present
_LIT(KX520PostalCode,"2.5.4.17");
_LIT(KRFC2247DomainComponent, "0.9.2342.19200300.100.1.25");
_LIT(KRFC2256Street,"2.5.4.9");
_LIT(KPKCS9UnstructuredName, "1.2.840.113549.1.9.2");

//email address is deprecated but we support it anyway...
_LIT(KPKCS9EmailAddress, "1.2.840.113549.1.9.1");

/** The maximum length allowed for a country name. */
const TInt KX520MaxCLength = 2;
/** The maximum length allowed for an organization name. */
const TInt KX520MaxOLength = 64;
/** The maximum length allowed for an organizational unit name. */
const TInt KX520MaxOULength = 64;
/** The maximum length allowed for a locality name. */
const TInt KX520MaxLLength = 128;
/** The maximum length allowed for a state or province name. */
const TInt KX520MaxSOPLength = 128;
/** The maximum length allowed for an title. */
const TInt KX520MaxTLength = 64;
/** The maximum length allowed for a common name. */
const TInt KX520MaxCNLength = 256;
/** The maximum length allowed for a given name. */
const TInt KX520MaxGNLength = 16;
/** The maximum length allowed for a surname. */
const TInt KX520MaxSLength = 40;
/** The maximum length allowed for initials. */
const TInt KX520MaxILength = 5;
/** The maximum length allowed for a generation qualifier. */
const TInt KX520MaxGQLength = 3;
/** The maximum length allowed for a serial number. */
const TInt KX520MaxSNLength = 64;
/** The maximum length allowed for a postal code. */
const TInt KX520MaxPostalCodeLength = 40;
/** The maximum length allowed for an email address. */
const TInt KPKCS9MaxEmailAddressLength = 256;
/** The maximum length allowed for an unstructured name. */
const TInt KPKCS9MaxUnstructuredNameLength = 256;
// No maximum was specified in the standard - 128 should be sufficient
/** The maximum length allowed for an RFC 2247 domain component. 
* 
* Each component of the domain name is a short string. */
const TInt KRFC2247MaxDomainComponentLength = 128;
/** The maximum length allowed a for street. */
const TInt KRFC2256StreetLength = 128;
/** The maximum length allowed for an Description field. */
const TInt KX520MaxDescriptionLength = 1024;


/** The attribute type. 
* 
* @since v6.0 */
enum TAttributeType
	{
	/** A common name */
	ECommonName,
	/** A locality name */
	ELocalityName,
	/** A state or province name */
	EStateOrProvinceName,
	/** An organization name */
	EOrganizationName,
	/** An organizational unit name */
	EOrganizationalUnitName,
	/** A title */
	ETitle,
	/** A qualifier */
	EDNQualifier,
	/** The name of a country */
	ECountryName,
	/** A given name */
	EGivenName,
	/** A surname */
	ESurname,
	/** Initials */
	EInitials,
	/** A generation qualifier */
	EGenerationQualifier,
	/** An email address. This is deprecated. */
	EPKCS9EmailAddress,
	/** A postal code */
	EPostalCode,
	/** A serial number */
	ESerialNumber,
	/** An RFC 2247 domain component.
	* 
	* A domain name is made up of an ordered set of components.*/
	ERFC2247DomainComponent,
	/** RFC 2256 street component.
	* 
	* A street */
	ERFC2256Street,
	/** A name of the subject of a certificate as an unstructured ASCII string */
	EPKCS9UnstructuredName,
	/** A Description */
	EX520Description,
	};


class CASN1EncSequence;

class CX520AttributeTypeAndValue : public CBase
/** Contains an attribute type and a value, as defined by the X.520 standard.
* 
* A Distinguished Name object, as defined by the X.500 standard, consists of 
* a sequence of these objects. 
* 
* @since v6.0 
*/
// Attribute and value pair class. Attribute and value pairs 
//	are used in distinguished names and certificate attributes. 
//	Stores data in ASN.1 format internally.
	{
public:
	// Constructs a new object from ASN.1-encoded DER binary data
	/** Creates a new CX520AttributeTypeAndValue object from the specified buffer containing 
	* the encoded binary representation.
	* 
	* Initialises the object from its encoded binary form into an internal representation.	
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX520AttributeTypeAndValue object. */
	IMPORT_C static CX520AttributeTypeAndValue* NewL(const TDesC8& aBinaryData);

	// Constructs a new object from ASN.1-encoded DER binary data
	/** Creates a new CX520AttributeTypeAndValue object from the specified buffer containing 
	* the encoded binary representation, and puts a pointer to the new object onto 
	* the cleanup stack.
	* 
	* Initialises the object from its encoded binary form into an internal representation.	
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX520AttributeTypeAndValue object. */
	IMPORT_C static CX520AttributeTypeAndValue* NewLC(const TDesC8& aBinaryData);

	// Constructs a new object from ASN.1-encoded DER binary data
	/** Creates a new CX520AttributeTypeAndValue object from the specified buffer containing 
	* the encoded binary representation, starting at the specified offset.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX520AttributeTypeAndValue object. */
	IMPORT_C static CX520AttributeTypeAndValue* NewL(const TDesC8& aBinaryData, TInt& aPos);

	// Constructs a new object from ASN.1-encoded DER binary data
	/** Creates a new CX520AttributeTypeAndValue object from the specified buffer containing 
	* the encoded binary representation, starting at the specified offset, and puts 
	* a pointer to the new object onto the cleanup stack.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX520AttributeTypeAndValue object. */
	IMPORT_C static CX520AttributeTypeAndValue* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX520AttributeTypeAndValue object from an existing object.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aPair	The CX520AttributeTypeAndValue object.
	* @return		The new CX520AttributeTypeAndValue object. */
	IMPORT_C static CX520AttributeTypeAndValue* NewL(const CX520AttributeTypeAndValue& aPair);
	
	/** Creates a new CX520AttributeTypeAndValue object from an existing object, and 
	* puts a pointer to the new object onto the cleanup stack.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aPair	The CX520AttributeTypeAndValue object.
	* @return		The new CX520AttributeTypeAndValue object. */
	IMPORT_C static CX520AttributeTypeAndValue* NewLC(const CX520AttributeTypeAndValue& aPair);
	
	/** Creates a new CX520AttributeTypeAndValue object from the specified read stream.
	* 
	* @param aStream	Stream from which the object is to be internalised.
	* @return			The new CX520AttributeTypeAndValue object. */
	IMPORT_C static CX520AttributeTypeAndValue* NewL(RReadStream& aStream);
	
	/** Creates a new CX520AttributeTypeAndValue object from the specified read stream, 
	* and puts a pointer to the new object onto the cleanup stack.
	* 
	* @param aStream	Stream from which the object is to be internalised.
	* @return			The new CX520AttributeTypeAndValue object. */
	IMPORT_C static CX520AttributeTypeAndValue* NewLC(RReadStream& aStream);

	/** Creates a new CX520AttributeTypeAndValue object from an attribute type and value.
	* 
	* @param aType	Attribute type
	* @param aValue	Attribute value
	* @return		New CX520AttributeTypeAndValue object */
	IMPORT_C static CX520AttributeTypeAndValue* NewL(TAttributeType aType, const TDesC8& aValue);

	/** Creates a new CX520AttributeTypeAndValue object from an attribute type and value.
	* 
	* @param aType	Attribute type
	* @param aValue Attribute value
	* @return		New CX520AttributeTypeAndValue object on the cleanup stack */
	IMPORT_C static CX520AttributeTypeAndValue* NewLC(TAttributeType aType, const TDesC8& aValue);

	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX520AttributeTypeAndValue();

	/** Gets the encoded attribute type of the object.
	* 
	* @return	A pointer descriptor representing the encoded attribute type. */
	IMPORT_C const TPtrC Type() const;

	/** Gets the encoded attribute value of the object.
	* 
	* @return	The pointer descriptor representing the encoded attribute value. */
	IMPORT_C const TPtrC8 EncodedValue() const;

	// Creates a copy of object's ASN.1 DER encoding.
	// @return	New buffer containing object's ASN.1 DER encoding.
	/** Gets the decoded value.
	* 
	* @return	A heap descriptor containing the decoded value. */
	IMPORT_C virtual HBufC* ValueL() const;

	/** Encodes the contents into an ASN.1 sequence object.
	* 
	* This is useful to insert attribute type/value pairs into other ASN.1 
	* encoding trees.
	* 
	* @return	New ASN.1 sequence object containing attribute 
	* 			type/value pair in for of an OID and an octet string; 
	*			leaves the returned object on the cleanup stack. */
	IMPORT_C CASN1EncSequence* EncodeASN1LC() const;

	/** Encodes the contents into an ASN.1 sequence object.
	* 
	* Useful to insert attribute type/value pairs into other ASN.1 
	* encoding trees.
	* 
	* @return 	New ASN.1 sequence object containing attribute 
	* 			type/value pair in for of an OID and an octet string. */
	IMPORT_C CASN1EncSequence* EncodeASN1L() const;

	// This function compares the current element with aElement.
	/** Compares the attribute type and decoded value of the specified object.
	* 
	* @param aElement	The CX520AttributeTypeAndValue object to be compared.
	* @return 			ETrue, if the attribute types and the decoded values match; 
	* 					EFalse, otherwise. */
	IMPORT_C virtual TBool ExactMatchL(const CX520AttributeTypeAndValue& 
		aElement) const;
		
	/** Externalises an object of this class to a write stream.
	* 
	* The presence of this function means that the standard templated operator<<() 
	* can be used to externalise objects of this class.
	* 
	* @param aStream	Stream to which the object should be externalised. */
	virtual void ExternalizeL(RWriteStream& aStream) const;
private:
	virtual void InternalizeL(RReadStream& aStream);
	CX520AttributeTypeAndValue(const TAttributeType aType);
	CX520AttributeTypeAndValue(); ///< Protected default constructor
	void ConstructL(const CX520AttributeTypeAndValue& aPair);
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(TAttributeType aType, const TDesC8& aValue);
	/**
     * This method finds out if case-insensitive comparisons must be done or not.
	 * Email Address is the exceptional case of 'IA5String' value type for which comparisons must be
	 * done case-insensitivly.
	 * 
	 * @param aSource	contains the encoded value of the attribute type.
 	 * @return	    	ETrue, if case-insensitive comparisons are to be done. 
	 * 		        	This is only when value type is a Printable String OR if attribute type is an Email Address. 
	 * 		        	EFalse, otherwise.
 	 */ 
 	TBool IsCaseInSensitiveL(const TDesC8& aSource) const;
	HBufC* iType; ///< The encoded type
	HBufC8* iValue; ///< The encoded value
	};

#endif
