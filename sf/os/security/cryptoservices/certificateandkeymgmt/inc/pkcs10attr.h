/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* PKCS#10 Certificate Request Attributes class.
*
*/


/**
 @file
 @publishedPartner
 @released 
*/

#if !defined (__PKCS10ATTR_H__)
#define __PKCS10ATTR_H__

class CASN1EncBase;
class CASN1EncSequence;
class CASN1EncSet;

//
// Class capturing common functionality of a PKCS attribute.
//
class CPKCSAttributeBase : public CBase
	{
public:

	/** Get the ASN.1 encoding of the attribute and relinquish ownership of it,
		leaving it on the cleanup stack.  This is called by CPKCS10Attribtues
		when AddAttribute() is called. 
		@panic E32USER-CBase 66 if a stack frame for the next PushL() cannot be allocated.
		@return ASN.1 encoding object
	*/
	virtual CASN1EncBase* GetEncodingLC() = 0;

	virtual ~CPKCSAttributeBase();

protected:
	/** @internalComponent */
	CPKCSAttributeBase();
	CASN1EncSequence* iRoot;
	CASN1EncSet* iValueSet;
	};

/** Class representing a generic certificate attribute.  Attributes have an
	object identifier and one or more values.
*/
class CPKCS10Attribute : public CPKCSAttributeBase
	{
public:

	/** Create an attribute with a specified OID and no values, leaving it on
		the cleanup stack.
		NOTE: This method is deprecated. Use the other forms of NewLC instead.
		@param aAttrOID	The specified OID in the form a.b.c. etc.
		@return New PKCS10 attribute object on the cleanup stack
		@deprecated
	*/
	IMPORT_C static CPKCS10Attribute* NewLC(const TDesC& aAttrOID);

	/** Create an attribute with a specified OID and one value, leaving it on
		the cleanup stack. The value forms part of a set. Additional values can
		be added by calling the AddValueL method.
		@param aAttrOID	The specified OID in the form a.b.c. etc.
		@param aASN1	ASN1 encoding object for attribute value
		 				-- this method takes ownership.
		@return New PKCS10 attribute object on the cleanup stack
	*/
	IMPORT_C static CPKCS10Attribute* NewLC(const TDesC& aAttrOID, CASN1EncBase* aASN1);

	/** Create an attribute with a specified OID and one value. The value forms 
		part of a set. Additional values can be added by calling the AddValueL method.
		@param aAttrOID	The specified OID in the form a.b.c. etc.
		@param aASN1	ASN1 encoding object for attribute value
		 				-- this method takes ownership.
		@return New PKCS10 attribute object
	*/
	IMPORT_C static CPKCS10Attribute* NewL(const TDesC& aAttrOID, CASN1EncBase* aASN1);

	/** Add a value to the attribute.
		@param aASN1	ASN1 encoding object for attribute value
		 				-- this method takes ownership.
	*/
	IMPORT_C void AddValueL(CASN1EncBase* aASN1);

	/** ResetL method to allow for re-use of the generic attribute object. 
		Additional values can be added by calling the AddValueL method.
		@param aAttrOID	The specified OID in the form a.b.c. etc.
		@param aASN1	ASN1 encoding object for attribute value
		 				-- this method takes ownership.
	*/
	IMPORT_C void ResetL(const TDesC& aAttrOID, CASN1EncBase* aASN1);

	virtual ~CPKCS10Attribute();

	/** 
	 * @internalComponent
	 *
	 * Get the ASN1 encoding of the attribute and relinquish ownership of it,
		leaving it on the cleanup stack. This is called by CPKCS10Attribtues
		when AddAttribute() is called. This method may leave with E32USER-CBase:66 
		if a stack frame for the next PushL() cannot be allocated.
		NOTE: This method is deprecated. Use GetEncodingLC() instead.
		@deprecated
	*/
	CASN1EncBase* TakeEncodingC();

	/** 
	 * @internalComponent
	 *
	 * Get the ASN1 encoding of the attribute and relinquish ownership of it,
		leaving it on the cleanup stack.  This is called by CPKCS10Attribtues
		when AddAttribute() is called. This method may leave with E32USER-CBase:66 
		if a stack frame for the next PushL() cannot be allocated.
		NOTE: This method is deprecated. Use GetEncodingLC() instead.
		@deprecated
	*/
	CASN1EncBase* TakeEncodingLC();

	/** Calls the base class implementation for getting the ASN1 encoding of the attribute.
		Note that this method cannot be reused unless ResetL() has been called. 
		@panic E32USER-CBase 66 if a stack frame for the next PushL() cannot be allocated.
		@return ASN.1 encoding object
	*/
	virtual CASN1EncBase* GetEncodingLC();

private:
	// Default constructor.
	CPKCS10Attribute();

	// Second phase construction function.
	void ConstructL(const TDesC& aAttrOID, CASN1EncBase* aASN1);
	};

/** Class for encoding a collection of certificate attributes and passing them
	to the CPKCS10Request class. Some possible attributes are described in
	PKCS#9.
  
	@publishedPartner
	@released
*/
class CPKCS10Attributes : public CBase
	{
public:
	/** Creates an instance of the class.
		@return New attributes object on the cleanup stack.
	*/
	IMPORT_C static CPKCS10Attributes* NewLC();

	/** Creates an instance of the class.
		@return New attributes object.
	*/
	IMPORT_C static CPKCS10Attributes* NewL();

	/** Adds a new attribute for the certificate request.
		@param aAttr The attribute to add - this method takes ownership.
		@deprecated
	*/
	IMPORT_C void AddAttributeL(CPKCS10Attribute* aAttr);

	/** Adds a new attribute for the certificate request.
		@param aAttr The attribute to add - this method takes ownership.
	*/
	IMPORT_C void AddPKCSAttributeL(CPKCSAttributeBase* aAttr);

	IMPORT_C ~CPKCS10Attributes(); // virtual from base

	/** 
	 * @internalComponent
	 *
	 * Get the ASN1 encoding of the attributes and relinquish ownership of it,
		leaving it on the cleanup stack. This is called by CPKCS10Request when
		SetAttributes() is called. This method may leave with E32USER-CBase:66 
		if a stack frame for the next PushL() cannot be allocated.
		@deprecated
	*/
	CASN1EncBase* TakeEncodingC();

	/** 
	 * @internalComponent
	 *
	 * Get the ASN.1 encoding of the attribute and relinquish ownership of it,
		leaving it on the cleanup stack. This is called by CPKCS10Request when
		SetAttributes() is called. 
		@panic E32USER-CBase 66 if a stack frame for the next PushL() cannot be allocated.
		@return ASN.1 encoding object
	*/
	CASN1EncBase* TakeEncodingLC();

private:
	/** Default constructor.
	*/
	CPKCS10Attributes();

	/** Second phase construction function. 
		@note The structure of the attribute node is as follows:
		@code
		  SET
		    SEQUENCE
			  OID of attribute
			  SET values
			...
		@endcode
	*/
	void ConstructL();
	
private:
	/** Pointer to sequence of user-supplied certificate attributes. 
		#AddAttributeL method is used to create and fill this member. 
		If it is initialized by the time #CalculateEncodingL method 
		is called, the latter uses attributes from this member variable 
		for the request, otherwise no attributes are inserted into 
		the request. 
		
		See #AddAttributeL for the explanation of ASN.1 encoding of
		attributes.
	*/
	CASN1EncSequence* iRoot;
	};

#endif // __PKCS10ATTR_H__
