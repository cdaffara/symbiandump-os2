/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* PKCS#9 attributes for use in PKCS#10 Certificate Request creation
*
*/


/**
 @file
 @publishedPartner
 @released 
*/

#if !defined (__PKCS9ATTR_H__)
#define __PKCS9ATTR_H__

#include <x509cert.h>
#include <pkcs10attr.h>

// OID of PKCS#9 Challenge Password attribute
_LIT(KPkcs9ChallengePasswordAttrOID, "1.2.840.113549.1.9.7");

// OID of PKCS#9 Extension Request attribute
_LIT(KPkcs9ExtensionRequestAttrOID, "1.2.840.113549.1.9.14");

// Lower bound of password length for PKCS#9 Challenge Password attribute
const TInt KPkcs9StringLB = 1;

// Upper bound of password length for PKCS#9 Challenge Password attribute
const TInt KPkcs9StringUB = 255;

/**
 Class representing a PKCS#9 Challenge Password attribute.
*/
class CPKCS9ChallengePasswordAttr : public CPKCSAttributeBase
	{
public:

	/** Create a PKCS#9 Challenge Password attribute with the given password, leaving it on
		the cleanup stack.
		@param aPassword The password - should be a Printable String with an upper bound 
		length of 255 characters and lower bound of 1 character.
		@return New Challenge Password attribute object
	*/
	IMPORT_C static CPKCS9ChallengePasswordAttr* NewLC(const TDesC8& aPassword);

	/** Create a PKCS#9 Challenge Password attribute with the given password.
		@param aPassword The password - should be a Printable String with an upper bound 
		length of 255 characters and lower bound of 1 character.
		@return New Challenge Password attribute object
	*/
	IMPORT_C static CPKCS9ChallengePasswordAttr* NewL(const TDesC8& aPassword);

	/** Reset method to allow for re-use of the attribute object.
		@param aPassword The password - should be a Printable String with an upper bound 
		length of 255 characters and lower bound of 1 character.
	*/
	IMPORT_C void ResetL(const TDesC8& aPassword);

	/** Get the ASN.1 encoding of the attribute and relinquish ownership of it,
		leaving it on the cleanup stack. This is called by CPKCS10Attribtues
		when AddAttribute() is called. Note that this method cannot be reused unless
		Reset() has been called. 
		@panic E32USER-CBase:66 if a stack frame for the next PushL() cannot be allocated.
		@return ASN.1 encoding object
	*/
	CASN1EncBase* GetEncodingLC();

	virtual ~CPKCS9ChallengePasswordAttr();

private:
	CPKCS9ChallengePasswordAttr();
	void ConstructL(const TDesC8& aPassword);
	};

/**
 Class representing a collection of one or more X.509 Certificate Extension attributes.
*/
class CPKCS9ExtensionRequestAttr : public CPKCSAttributeBase
	{
public:

	/** Create a PKCS#9 Extensions attribute with the a single extension, leaving it on
		the cleanup stack.
		@param aExtension An X.509 Certificate Extension object.
		@return New PKCS#9 Extensions attribute object
	*/
	IMPORT_C static CPKCS9ExtensionRequestAttr* NewLC(const CX509CertExtension& aExtension);

	/** Create a PKCS#9 Extensions attribute with the a single extension.
		@param aExtension An X.509 Certificate Extension object.
		@return New PKCS#9 Extensions attribute object
	*/
	IMPORT_C static CPKCS9ExtensionRequestAttr* NewL(const CX509CertExtension& aExtension);

	/** Adds an extension to the extensions attribute.
		@param aExtension An X.509 Certificate Extension object.
	*/
	IMPORT_C void AddExtensionL(const CX509CertExtension& aExtension);

	/** Reset method to allow for re-use of the attribute object. Already existing 
		extensions are removed.
		@param aExtension An X.509 Certificate Extension object.
	*/
	IMPORT_C void ResetL(const CX509CertExtension& aExtension);

	virtual ~CPKCS9ExtensionRequestAttr();

	/** Get the ASN.1 encoding of the attribute and relinquish ownership of it,
		leaving it on the cleanup stack. This is called by CPKCS10Attribtues
		when AddAttribute() is called. Note that this method cannot be reused unless
		Reset() has been called. 
		@panic E32USER-CBase:66 if a stack frame for the next PushL() cannot be allocated.
		@return ASN.1 encoding object
	*/
	CASN1EncBase* GetEncodingLC();

private:
	CPKCS9ExtensionRequestAttr();
	void ConstructL(const CX509CertExtension& aExtension);

private:
	CASN1EncSequence* iExtSeq;
	};

#endif // __PKCS9ATTR_H__
