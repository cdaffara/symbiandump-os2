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
* Implementation of the class that holds the Distinguished Name of a subject
*
*/


/**
 @file 
 @publishedAll
 @released
*/
#if !defined (__X500DN_H__)
#define __X500DN_H__

#include <e32base.h>
#include <e32std.h>
#include <s32file.h>
#include <x520ava.h>

class CX500DistinguishedName : public CBase
/** Holds the Distinguished Name of a subject.
*
* A collection of 'attribute type and value' objects, as defined by the X.520 
* standard.
*
* The name used in X.509 certificates is the X.500 Distinguished Name, which 
* describes a path through an X.500 Directory Information Tree.
*
* A Distinguished Name is a series of name-value pairs that uniquely identify 
* an entity, i.e. the certificate subject. 
*
* @publishedAll
* @released
*
* <!-- 
* This is necessary when working with certificates, certificate requests, directories, etc. 
* -->
*/
	{
public:
	/** Creates a new Distinguished Name object from the specified buffer containing 
	* the encoded binary representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The position from which to start decoding.
	* @return				The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new Distinguished Name object from the specified buffer containing 
	* the encoded binary representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The position from which to start decoding.
	* @return				The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new Distinguished Name object from the specified buffer containing 
	* the encoded binary representation.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new Distinguished Name object from the specified buffer containing 
	* the encoded binary representation, and puts a pointer to it onto the cleanup stack.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new Distinguished Name object from the specified read stream.
	* 
	* Construction is from a stream.
	* 
	* @param aStream	Stream from which the contents should be internalised.
	* @return			The newDistinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewL(RReadStream& aStream);
	
	/** Creates a new Distinguished Name object from the specified read stream, and 
	* puts a pointer to it onto the cleanup stack.
	* 
	* @param aStream	Stream from which the contents should be internalised.
	* @return			The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewLC(RReadStream& aStream);	
	
	/** Creates a new Distinguished Name object from the specified array of 'attribute 
	* type and value' objects.
	* 
	* The elements of the specified array are copied into this object.
	* 
	* @param aElements	The array of 'attribute type and value' objects to be copied.
	* @return			The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewL(const CArrayPtr<CX520AttributeTypeAndValue>& aElements);
	
	/** Creates a new Distinguished Name object from the specified array of 'attribute 
	* type and value' objects, and puts a pointer to it onto the cleanup stack.
	* 
	* The elements of the specified array are copied into this object.
	* 
	* @param aElements	The array of 'attribute type and value' objects to be copied.
	* @return			The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewLC(const CArrayPtr<CX520AttributeTypeAndValue>& aElements);	
	
	/** Creates a new Distinguished Name object from an existing object.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aName	The Distinguished Name object to be copied.
	* @return		The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewL(const CX500DistinguishedName& aName);
	
	/** Creates a new Distinguished Name object from an existing object, and puts a 
	* pointer to it onto the cleanup stack.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aName	The Distinguished Name object to be copied.
	* @return		The new Distinguished Name object. */
	IMPORT_C static CX500DistinguishedName* NewLC(const CX500DistinguishedName& aName);
	
	/** Externalises an object of this class to a write stream.
	* 
	* The presence of this function means that the standard templated operator<<() 
	* can be used to externalise objects of this class.
	* 
	* @param aStream	Stream to which the object should be externalised. */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
	/** Gets the number of 'attribute type and value' objects contained by this Distinguished 
	* Name object.
	* 
	* @return	The number of 'attribute type and value' objects. */
	IMPORT_C TInt Count() const;
	
	/** Gets a reference to specific 'attribute type and value' object as identified 
	* by the specified index.
	* 
	* @param aIndex	The index number identifying the specific 'attribute type and 
	* 				value' object. This number is relative to zero. This value must be non-negative 
	* 				and less than the number of objects currently contained by this Distinguished 
	* 				Name object.
	* @return		The referenced 'attribute type and value' object. */
	IMPORT_C const CX520AttributeTypeAndValue& Element(TInt aIndex) const;
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX500DistinguishedName(); 

	/** Compares the specified Distinguished Name object with this Distinguished Name 
	* object.
	* 
	* @param aName	The Distinguished Name object to be compared.
	* @return		ETrue, if the Distinguished Name objects contain exactly the same 'attribute 
	* 				type and value' objects; EFalse, otherwise.
	* @see CX520AttributeTypeAndValue::ExactMatchL() 
	*
	* <!--	This function checks if all the fields of aName and 
	*		iElements are the same but not necessarily in the same order. -->
	*/
	IMPORT_C TBool ExactMatchL(const CX500DistinguishedName& aName) const;

	/** <!--
	 // This function checks if the elements of iElements are a superset of
	 // the fields in aName.
	 // @param aName We want to check that the fields in aName are present in iElements.
	 // @return 
	 // <UL>
	 // <LI>ETrue if all the fields in aName have a corresponding field in iElements.</LI>
	 // <LI>EFalse otherwise</LI>
	 // </UL>
	 -->
	* Tests whether all 'attribute type and value' objects contained in the specified 
	* Distinguished Name object are also contained within this Distinguished Name object.
	* 
	* This function checks if the elements of iElements are a superset of the fields in aName.
	* 
	* @param aName	The Distinguished Name object to be compared.
	* @return		ETrue, if all 'attribute type and value' objects contained in the specified 
	* 				Distinguished Name object are also contained within this Distinguished Name 
	* 				object; EFalse, otherwise. */
	IMPORT_C TBool IsWithinSubtreeL(const CX500DistinguishedName& aName) const;

	/** <!--
	 // This function compares each of the elements in iElements with aElement. The comparison
	 // is made by the CX520AttributeTypeAndValue::ExactMatchL function.
	 // @param aElement The element which we want to compare the elements of iElements with.
	 // @return 
	 // <UL>
	 // <LI>ETrue if one of the elements in iElements is equal to aElement.</LI>
	 // <LI>EFalse otherwise</LI>
	 // </UL>
	 -->
	* Tests whether this Distinguished Name object contains the specified 'attribute 
	* type and value' object.
	* 
	* This function compares each of the elements in iElements with aElement. The comparison
	* is made by the CX520AttributeTypeAndValue::ExactMatchL() function.
	* 
	* @param aElement	The 'attribute type and value' object to be checked.
	* @return			ETrue, if the specified 'attribute type and value' object is contained 
	* 					in this Distinguished Name object; EFalse otherwise. */
	IMPORT_C TBool MatchElementL(const CX520AttributeTypeAndValue& aElement) const;
	
	/** Gets the decoded value associated with the specified field/attribute name, 
	* from the Distinguished Name.
	* 
	* @param aFieldName	The field name.
	* @return			A heap descriptor containing the decoded value associated with the 
	* 					specified field name. */
	IMPORT_C HBufC* ExtractFieldL(const TDesC& aFieldName) const;
	
	/** Gets the decoded value for the common or organisation name.
	* 
	* @return A heap descriptor containing the decoded value of the common or organisation name. */
	IMPORT_C HBufC* DisplayNameL() const;

	/** Encodes a DN into an ASN.1 object tree.
	* 
	* Note that the tree has the following format:
	* @code
    *         SEQUENCE-OF
    *             SET-OF
    *                 SEQUENCE-OF
    *                     OID
    *                     value (usually OCTET STRING)
    *             ...
    * @endcode
	* 
	* A SEQUENCE-OF object with a changed tag is used instead of 
	* a SET-OF object. This should be all right as long as it 
	* contains only one child, because otherwise child order in 
	* a SET-OF becomes important.
	* 
	* This function does not introduce an additional dependency 
	* on ASN1 library because X500 library already depends on 
	* it -- the attribute type/value class stores and manipulated 
	* ASN.1 encodings as its values.
	* 	
	* @return	ASN.1 sequence object containing the DN, 
	* 			pushed on the cleanup stack.
	*/
	IMPORT_C CASN1EncSequence* EncodeASN1LC() const;

	/** Encodes a DN into an ASN.1 object tree. 
	* 
	* See note in the description of #EncodeASN1LC for the explanation of 
	* the resulting encoding tree format.
	* 	
	* @return	ASN.1 sequence object containing the DN. */
	IMPORT_C CASN1EncSequence* EncodeASN1L() const;

private:
	CX500DistinguishedName();
	void ConstructL(const CArrayPtr<CX520AttributeTypeAndValue>& aElements);
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(RReadStream& aStream);
	void InternalizeL(RReadStream& aStream);
	CArrayPtrFlat<CX520AttributeTypeAndValue>* iElements;
	};

#endif
