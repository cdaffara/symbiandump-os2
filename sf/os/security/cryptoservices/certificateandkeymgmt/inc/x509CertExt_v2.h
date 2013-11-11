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
* X509CERTEXT.H
* X509 extensions v2
* Common specific extensions:
* 1)	basic constraints
* 2)	alt name
* 3)	key usage
* 4)	name constraints
* 5)	policy constraints
* 6)	policies
* 7)	policy mapping
* 8)	authority key ID
* 9)	subject key ID
* 10)	extended key usage
* 11)	authority information access
* critical extension
* 12)   inhibit-any policy 
*
*/


/**
 @file 
 @publishedAll
 @released
*/

#if !defined (__X509CERTEXT_H__)
#define __X509CERTEXT_H__

#include <e32base.h>
#include <e32std.h>
#include <x509gn.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <x509bitstring.h>
#endif

class RReadStream;
class RWriteStream;
class CX509BitString;
/** A list of values that defines what an X.509 key can be used for.
* These values can be ANDed together if a key has several usages. 
* 
* @since v7.0 */
enum TX509KeyUsage
	{
	/** A digital signature. */
	EX509DigitalSignature,
	/** Non repudiation. */
	EX509NonRepudiation,
	/** Key encipherment. */
	EX509KeyEncipherment,
	/** Data encipherment. */
	EX509DataEncipherment,
	/** Key agreement. */
	EX509KeyAgreement,
	/** Key certificate sign. */
	EX509KeyCertSign,
	/** CRL sign. */
	EX509CRLSign,
	/** Encipher only. */
	EX509EncipherOnly,
	/** Decipher only. */
	EX509DecipherOnly
	};

/** X509 Extension OIDs
* 
* @since v9.5 */

//OIDS for the extensions we define here...
_LIT(KBasicConstraints,"2.5.29.19");
_LIT(KSubjectAltName,"2.5.29.17");
_LIT(KIssuerAltName,"2.5.29.18");
_LIT(KKeyUsage,"2.5.29.15");
_LIT(KNameConstraints,"2.5.29.30");
_LIT(KPolicyConstraints,"2.5.29.36");
_LIT(KCertPolicies,"2.5.29.32");
_LIT(KPolicyMapping,"2.5.29.33");
_LIT(KAuthorityKeyId,"2.5.29.35");
_LIT(KSubjectKeyId,"2.5.29.14");
_LIT(KExtendedKeyUsage,"2.5.29.37");
_LIT(KAuthorityInfoAccess, "1.3.6.1.5.5.7.1.1");
_LIT(KInhibitAnyPolicy, "2.5.29.54");

// OIDs for access methods
_LIT(KAccessMethodOCSP, "1.3.6.1.5.5.7.48.1");

//deprecated OIDs we might still encounter
_LIT(KOldBasicConstraints,"2.5.29.10");
_LIT(KOldBasicConstraints2,"2.5.29.13");
_LIT(KOldSubjectAltName,"2.5.29.7");
_LIT(KOldIssuerAltName,"2.5.29.8");
_LIT(KOldNameConstraints,"2.5.29.11");
_LIT(KOldPolicyConstraints,"2.5.29.2");
_LIT(KOldPolicyConstraints2,"2.5.29.34");
_LIT(KOldCertPolicies,"2.5.29.3");
_LIT(KOldPolicyMapping,"2.5.29.5");
_LIT(KOldAuthorityKeyId,"2.5.29.1");

// Define OIDs for Symbian certificate constraints.
_LIT(KDeviceIdListConstraint,"1.2.826.0.1.1796587.1.1.1.1");
_LIT(KSidListConstraint,"1.2.826.0.1.1796587.1.1.1.4");
_LIT(KVidListConstraint,"1.2.826.0.1.1796587.1.1.1.5");
_LIT(KCapabilitiesConstraint,"1.2.826.0.1.1796587.1.1.1.6");

//1) basic constraints...

class CX509ExtensionBase : public CBase
/** A certificate extension base class. 
* 
* @since v6.0 */
	{
protected:
	/** Second-phase constructor.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The position from which to start decoding. */
	virtual void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Implementation for second-phase construction.
	* 
	* This is called by ConstructL().
	* 
	* @param aBinaryData	The encoded binary representation. This is the same as
	* 						passed to ConstructL().
	* @param aPos			The position from which to start decoding. Note that the value
	* 						passed points, in effect, to the content, by passing the header data. */
	virtual void DoConstructL(const TDesC8& aBinaryData, TInt& aPos) = 0;
	};

class CX509BasicConstraintsExt : public CX509ExtensionBase
/** An X.509 certificate extension that defines basic constraints.
* 
* It indicates whether the certificate belongs to a Certificate Authority or 
* an end Entity. 
* 
* @since v6.0 */
	{
public:
	/** Creates a new CX509BasicConstraintsExt object from the specified 
	* buffer containing the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509BasicConstraintsExt object. */
	IMPORT_C static CX509BasicConstraintsExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509BasicConstraintsExt object from the specified 
	* buffer containing the binary coded representation, and puts a pointer to it 
	* onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509BasicConstraintsExt object. */
	IMPORT_C static CX509BasicConstraintsExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509BasicConstraintsExt object from the specified 
	* buffer containing the binary coded representation, starting at the specified 
	* offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509BasicConstraintsExt object. */
	IMPORT_C static CX509BasicConstraintsExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509BasicConstraintsExt object from the specified 
	* buffer containing the binary coded representation, starting at the specified 
	* offset, and puts a pointer to it onto the cleanup stack.
	*
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509BasicConstraintsExt object. */
	IMPORT_C static CX509BasicConstraintsExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	virtual ~CX509BasicConstraintsExt();
	
	/** Tests whether the certificate belongs to a Certificate Authority.
	* 
	* @return	ETrue, if the certificate belongs to a Certificate Authority; 
	* 			EFalse, otherwise. */
	IMPORT_C TBool IsCA() const;
	
	/** Gets the maximum number of certificates that can follow this one in the chain.
	* 
	* @return	The number of certificates. */
	IMPORT_C TInt MaxChainLength() const;//0 if not set
private:
	CX509BasicConstraintsExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	TBool iIsCA;
	TInt iMaxChainLen;
	};

//2) alt name

class CX509AltNameExt : public CX509ExtensionBase
/** An X.509 certificate extension that defines an alternate name.
* 
* It appears as an Issuer Alt Name extension or a Subject Alt Name extension 
* and is used to contain extra identifying information that will not fit into 
* a Distinguished Name.
* 
* It consists of an array of X.509 General Names.
*
* @since v6.0 */
	{
public:
	/** Creates a new CX509AltNameExt object from the specified 
	* buffer containing the binary coded representation.
	*
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509AltNameExt object. */
	IMPORT_C static CX509AltNameExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509AltNameExt object from the specified 
	* buffer containing the binary coded representation, and puts a pointer to it 
	* onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509AltNameExt object. */
	IMPORT_C static CX509AltNameExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509AltNameExt object from the specified 
	* buffer containing the binary coded representation, starting at the specified 
	* offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509AltNameExt object. */
	IMPORT_C static CX509AltNameExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509AltNameExt object from the specified 
	* buffer containing the binary coded representation, starting at the specified 
	* offset, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509AltNameExt object. */
	IMPORT_C static CX509AltNameExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	 virtual ~CX509AltNameExt();
	
	/** Gets a reference to the array of general names that forms the alternate name 
	* extension.
	* 
	* @return	The array of general names. */
	IMPORT_C const CArrayPtrFlat<CX509GeneralName>& AltName() const;
	
	/** Checks whether the corressponding elements of two equally sized arrays of X.509 general names
	* match.
	*
	* @param aExt	An X.509 certificate extension object that defines an alternate name.
	* @return		TRUE if all the elements in the arrays match.
	*/
	IMPORT_C TBool Match(const CX509AltNameExt& aExt) const;
private:
	CX509AltNameExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CArrayPtrFlat<CX509GeneralName>* iAuthorityName;
	};

class CX509KeyUsageExt : public CX509ExtensionBase
/** An X.509 certificate extension that defines the uses to which a key may be put.
* 
* @since v6.0 */
	{
public:
	/** Creates a new CX509KeyUsageExt object from the specified buffer 
	* containing the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509KeyUsageExt object. */
	IMPORT_C static CX509KeyUsageExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509KeyUsageExt object from the specified buffer 
	* containing the binary coded representation, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509KeyUsageExt object. */
	IMPORT_C static CX509KeyUsageExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509KeyUsageExt object from the specified buffer 
	* containing the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509KeyUsageExt object. */
	IMPORT_C static CX509KeyUsageExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509KeyUsageExt object from the specified buffer 
	* containing the binary coded representation, starting at the specified offset, 
	* and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509KeyUsageExt object. */
	IMPORT_C static CX509KeyUsageExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	virtual ~CX509KeyUsageExt();
	
	/** Tests whether a particular usage is set in the extension.
	* 
	* 
	* @param aUsage	The usage.
	* @return		ETrue, if the specific usage is set in the extension; EFalse, otherwise. */
	IMPORT_C TBool IsSet(TX509KeyUsage aUsage) const;
private:
	CX509KeyUsageExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CX509BitString* iData;
	};

//4) name constraints

class CX509GeneralSubtree : public CBase
/** Provides access to the general name and the min/max lengths of the subtree. 
* 
* @since v6.0 */
	{
public:
	/**
	 * @internalComponent
	 *  
	 * Creates a new CX509GeneralSubtree object from the specified buffer containing 
	 * the encoded binary representation.
	 * 
	 * @param aBinaryData	The encoded binary representation. 
	 * @return 		The new CX509GeneralSubtree object. 
	 **/
	static CX509GeneralSubtree* NewL(const TDesC8& aBinaryData);
	
	/** 
	 * @internalComponent
	 * 
	 * Creates a new CX509GeneralSubtree object from the specified buffer containing 
	 * the encoded binary representation, and puts a pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation. 
	 * @return 				The new CX509GeneralSubtree object. 
	 **/
	static CX509GeneralSubtree* NewLC(const TDesC8& aBinaryData);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509GeneralSubtree object from the specified buffer containing 
	 * the encoded binary representation.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The position from which to start decoding. 
	 * @return 				The new CX509GeneralSubtree object. 
	 **/
	static CX509GeneralSubtree* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509GeneralSubtree object from the specified buffer containing 
	 * the encoded binary representation, and puts a pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The position from which to start decoding.
	 * @return 				The new CX509GeneralSubtree object. 
	 **/
	static CX509GeneralSubtree* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object. */
	virtual ~CX509GeneralSubtree();
	
	/** Gets the general name.
	* 
	* @return	The general name. */
	IMPORT_C const CX509GeneralName& Name() const;
	
	/** Gets the minimum distance of the CX509GeneralSubtree object.
	* 
	* @return	The minimum distance of the CX509GeneralSubtree object. */
	IMPORT_C TInt MinDistance() const;
	
	/** Gets the maximum distance of the CX509GeneralSubtree object.
	* 
	* @return	The maximum distance of the CX509GeneralSubtree object. */
	IMPORT_C TInt MaxDistance() const;
private:
	CX509GeneralSubtree();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CX509GeneralName* iName;
	TInt iMaxDist;
	TInt iMinDist;
	};

class CX509NameConstraintsExt : public CX509ExtensionBase
/** An X.509 certificate extension that defines constraints on an entity's name.
* 
* This extension allows Certification Authorities to restrict or prevent the issuing 
* of certificates to entities whose names lie within a defined name space. 
*
*/
	{
public:
	/** Creates a new CX509NameConstraintsExt object from the specified 
	* buffer containing the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509NameConstraintsExt object. */
	IMPORT_C static CX509NameConstraintsExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509NameConstraintsExt object from the specified 
	* buffer containing the binary coded representation, and puts a pointer to it 
	* onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509NameConstraintsExt object. */
	IMPORT_C static CX509NameConstraintsExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509NameConstraintsExt object from the specified 
	* buffer containing the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509NameConstraintsExt object. */
	IMPORT_C static CX509NameConstraintsExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509NameConstraintsExt object from the specified 
	* buffer containing the binary coded representation, starting at the specified 
	* offset, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509NameConstraintsExt object. */
	IMPORT_C static CX509NameConstraintsExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509NameConstraintsExt();
	
	/** Gets a reference to the array of exclusion subtree names.
	* 
	* No certificates after this one may contain names within the subtrees specified.
	* 
	* @return	The excluded subtrees. */
	IMPORT_C const CArrayPtrFlat<CX509GeneralSubtree>& ExcludedSubtrees() const;
	
	/** Gets a reference to the array of permitted subtree names.
	* 
	* All certificates after this one must contain names within the subtrees specified.
	* 
	* @return	The permitted subtrees. */
	IMPORT_C const CArrayPtrFlat<CX509GeneralSubtree>& PermittedSubtrees() const;
private:
	CX509NameConstraintsExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void AddSubtreesL(CArrayPtrFlat<CX509GeneralSubtree>& aSubtrees, const TDesC8& aBinaryData);
	CArrayPtrFlat<CX509GeneralSubtree>* iExcludedSubtrees;
	CArrayPtrFlat<CX509GeneralSubtree>* iPermittedSubtrees;
	};

//5) policy constraints

class TX509PolicyConstraint
/** Defines whether a policy constraint applies.
* 
* Objects of this type are used by the X.509 certificate extension that constrains 
* the use of policies.
* 
* An object of this type encapsulates two pieces of information:
* @li	whether a specific policy applies to subsequent certificates
* @li	if a specific policy applies, the number of certificates that can follow the 
* 		current one before the policy constraint applies.
* 
* Enables a CA to constrain the use of policies in two ways: they can enforce 
* the appearance of explicit certificate policies in subsequent certificates, 
* and prevent policy mapping from being performed. 
*
*/
	{
public:
	/** 
	 * @internalComponent
	 *
	 * Constructs a TX509PolicyConstraint definition object with the specified requirement 
 	 * and countdown values.
	 * 
	 * @param aRequired	The policy requirement. Set to ETrue, if the policy constraint 
	 * 					applies; set to EFalse, otherwise.
	 * @param aCountdown	The countdown value. This is the number of certificates that 
	 * 					can follow the current one before the constraint applies. 
	 *					This only has meaning if the policy constraint applies. 
	 **/
	TX509PolicyConstraint(TBool aRequired, TInt aCountdown);
	
	/** 
	 * @internalComponent
	 *
	 * Default constructor.
	 * 
	 * Sets policy requirement to EFalse and iRequired to EFalse and the iCountdown 
	 * to zero. 
	 **/
	TX509PolicyConstraint();
	
	/** The policy requirement.
	* 
	* This has the value ETrue, if the policy constraint applies; EFalse otherwise. */
	TBool iRequired;
	
	/** The count down value.
	* 
	* Defines the number of certificates that can follow the current one before 
	* the policy constraint applies. This has no meaning if the policy constraint 
	* does not apply. */
	TInt iCountdown;
	};

class CX509PolicyConstraintsExt : public CX509ExtensionBase
/** Enables a Certification Authority to constrain the use of policies in two ways:
* @li to enforce the appearance of explicit certificate policies in subsequent certificates
* @li to prevent policy mapping from being performed.
* 
* @since v6.0 */
	{
public:
	/** Creates a new CX509PolicyConstraintsExt object from the specified buffer 
	* containing the encoded binary representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509PolicyConstraintsExt object. */
	IMPORT_C static CX509PolicyConstraintsExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new Policy constraints extension object from the specified buffer 
	* containing the encoded binary representation, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509PolicyConstraintsExt object. */
	IMPORT_C static CX509PolicyConstraintsExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509PolicyConstraintsExt object from the specified buffer 
	* containing the encoded binary representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509PolicyConstraintsExt object. */
	IMPORT_C static CX509PolicyConstraintsExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509PolicyConstraintsExt object from the specified buffer 
	* containing the encoded binary representation, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509PolicyConstraintsExt object. */
	IMPORT_C static CX509PolicyConstraintsExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509PolicyConstraintsExt();
	
	/** Gets the explicit policy constraint.
	* 
	* @return	The explicit policy constraint. */
	IMPORT_C TX509PolicyConstraint ExplicitPolicyRequired() const;
	
	/** Gets the inhibiting policy mapping.
	* 
	* @return	The inhibiting policy mapping. */
	IMPORT_C TX509PolicyConstraint InhibitPolicyMapping() const;
private:
	CX509PolicyConstraintsExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	TX509PolicyConstraint iRequirePolicy;
	TX509PolicyConstraint iInhibitPolicyMapping;
	};

//6) policies

class CX509PolicyQualifierInfo : public CBase
/** Gets X.509 policy qualifier information. 
* 
* @since v6.0 */
	{
public:
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyQualifierInfo object from the specified 
	 * buffer containing the encoded binary representation.
	 * 
	 * @param aBinaryData	The encoded binary representation. 
	 * @return				The new CX509PolicyQualifierInfo object. 
	 **/
	static CX509PolicyQualifierInfo* NewL(const TDesC8& aBinaryData);
	
	/**
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyQualifierInfo object from the specified 
	 * buffer containing the encoded binary representation, and puts a pointer to 
	 * it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation. 
	 * @return				The new CX509PolicyQualifierInfo object. 
	 **/
	static CX509PolicyQualifierInfo* NewLC(const TDesC8& aBinaryData);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyQualifierInfo object from the specified 
	 * buffer containing the encoded binary representation.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The position from which to start decoding. 
	 * @return				The new CX509PolicyQualifierInfo object. 
	 **/
	static CX509PolicyQualifierInfo* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyQualifierInfo object from the specified 
	 * buffer containing the encoded binary representation, and puts a pointer to 
	 * it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The position from which to start decoding. 
	 * @return				The new CX509PolicyQualifierInfo object. 
	 **/
	static CX509PolicyQualifierInfo* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyQualifierInfo object from an existing X.509 
	 * Policy Qualifier Information object.
	 * 
	 * @param aQualifierInfo	The CX509PolicyQualifierInfo object to be copied. 
	 * @return				The new CX509PolicyQualifierInfo object. 
	 **/
	static CX509PolicyQualifierInfo* NewL(const CX509PolicyQualifierInfo& aQualifierInfo);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyQualifierInfo object from an existing X.509 
	 * Policy Qualifier Information object, and puts a pointer to it onto the cleanup stack.
	 * 
	 * @param aQualifierInfo	The CX509PolicyQualifierInfo object to be copied. 
	 * @return				The new CX509PolicyQualifierInfo object. 
	 **/
	static CX509PolicyQualifierInfo* NewLC(const CX509PolicyQualifierInfo& aQualifierInfo);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyQualifierInfo object from a stream.
	 * 
	 * @return	The new CX509PolicyQualifierInfo object. 
	 **/
	static CX509PolicyQualifierInfo* NewL(RReadStream& aStream);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyQualifierInfo object from a stream, and puts a pointer to it onto 
	 * the cleanup stack.
	 * 
	 * @return	The new CX509PolicyQualifierInfo object. 
	 **/
	static CX509PolicyQualifierInfo* NewLC(RReadStream& aStream);
	
	/** The destructor.
	* 
	* Frees all resources owned by the object. */
	virtual ~CX509PolicyQualifierInfo();
	
	/** Gets the policy qualifier Id.
	* 
	* @return	A non-modifiable pointer descriptor representing the policy qualifier Id. */
	IMPORT_C TPtrC Id() const;
	
	/** Gets the policy qualifier data.
	* 
	* @return	A non-modifiable pointer descriptor representing the policy qualifier data. */
	IMPORT_C TPtrC8 Data() const;

	// Internalization/Externalization
  // Externalize. Writes the data out to a stream
	/** 
	 * @internalComponent
	 * 
	 * Externalises an object of this class to a write stream.
	 *  
	 * The presence of this function means that the standard templated operator<<() 
	 * can be used to externalise objects of this class.
	 * 
	 * @param aStream	Stream to which the object should be externalised. 
	 **/
	void ExternalizeL(RWriteStream& aStream) const;

	// Internalize. Reads the data from a stream 
	/** 
	 * @internalComponent
	 *
	 * Internalises an object of this class from a read stream.
	 * 
	 * The presence of this function means that the standard templated operator>>() 
	 * can be used to internalise objects of this class.
	 * 
	 * Note that this function has assignment semantics: it replaces the old value 
	 * of the object with a new value read from the read stream.	
	 * 
	 * @param aStream	Stream from which the object should be internalised. 
	 **/
	void InternalizeL(RReadStream& aStream);
	
private:
	CX509PolicyQualifierInfo();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const CX509PolicyQualifierInfo& aQualifier);
	void ConstructL(RReadStream& aStream);
	HBufC* iPolicyQualifierId;
	HBufC8* iData; //opaque (this is another any-defined-by, so could be anything)
	};

class CX509CertPolicyInfo : public CBase
/** Defines a specific policy followed by a Certification Authority.
* 
* The policy under which a certificate has been issued may contain a number 
* of these specific policies.
* 
* @see CX509CertPoliciesExt 
* @since v6.0 */
	{
public:
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509CertPolicyInfo object from the specified buffer containing the 
	 * binary coded representation.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CX509CertPolicyInfo object. 
	 **/
	static CX509CertPolicyInfo* NewL(const TDesC8& aBinaryData);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509CertPolicyInfo object from the specified buffer containing the 
	 * binary coded representation, and puts a pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CX509CertPolicyInfo object. 
	 **/
	static CX509CertPolicyInfo* NewLC(const TDesC8& aBinaryData);
	
	/** 
	 * @internalComponent
	 * Creates a new CX509CertPolicyInfo object from the specified buffer containing the 
	 * binary coded representation, starting at the specified offset.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding.
	 * @return				The new CX509CertPolicyInfo object. 
	 **/
	static CX509CertPolicyInfo* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** 
	 * @internalComponent
	 * 
	 * Creates a new CX509CertPolicyInfo object from the specified buffer containing the 
	 * binary coded representation, starting at the specified offset, and puts a 
	 * pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding.
	 * @return				The new CX509CertPolicyInfo object. 
	 **/
	static CX509CertPolicyInfo* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509CertPolicyInfo object from an existing object.
	* 
	* This is equivalent to a copy constructor.	
	* 
	* @param aPolicy		The CX509CertPolicyInfo object to be copied.
	* @return				The new CX509CertPolicyInfo object. */
	IMPORT_C static CX509CertPolicyInfo* NewL(const CX509CertPolicyInfo& aPolicy);
	
	/** Creates a new CX509CertPolicyInfo object from an existing object, and puts a pointer 
	* to it onto the cleanup stack.	
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aPolicy	The CX509CertPolicyInfo object to be copied.
	* @return			The new CX509CertPolicyInfo object. */
	IMPORT_C static CX509CertPolicyInfo* NewLC(const CX509CertPolicyInfo& aPolicy);
	
	/** Creates a new CX509CertPolicyInfo object from a stream. 
	* 
	* @return	The new CX509CertPolicyInfo object. */
	IMPORT_C static CX509CertPolicyInfo* NewL(RReadStream& aStream);
	
	/** Creates a new CX509CertPolicyInfo object from a stream, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @return	The new CX509CertPolicyInfo object. */
	IMPORT_C static CX509CertPolicyInfo* NewLC(RReadStream& aStream);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	virtual ~CX509CertPolicyInfo();
	
	/** Gets a reference to the array of policy qualifers.
	* 
	* @return	The array of policy qualifiers. */
	IMPORT_C const CArrayPtrFlat<CX509PolicyQualifierInfo>& Qualifiers() const;
	
	/** Gets the specific policy's Object Identifier (OID).
	* 
	* @return	A pointer descriptor representing the specific policy's OID. */
	IMPORT_C TPtrC Id() const;


	// Internalization/Externalization
	// Externalize. Writes the data out to a stream
	/** Externalises an object of this class to a write stream.
	* 
	* The presence of this function means that the standard templated operator<<() 
	* can be used to externalise objects of this class.
	* 
	* @param aStream	Stream to which the object should be externalised. */
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

	// Internalize. Reads the data from a stream 
	/** Internalises an object of this class from a read stream.
	* 
	* The presence of this function means that the standard templated operator>>() 
	* can be used to internalise objects of this class.
	* 
	* Note that this function has assignment semantics: it replaces the old value 
	* of the object with a new value read from the read stream.	
	* 
	* @param aStream	Stream from which the object should be internalised. */
	IMPORT_C void InternalizeL(RReadStream& aStream);
	

private:
	CX509CertPolicyInfo();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const CX509CertPolicyInfo& aInfo);
	void ConstructL(RReadStream& aStream);
	HBufC* iCertPolicyId;//OID for this policy
	CArrayPtrFlat<CX509PolicyQualifierInfo>* iQualifiers;
	};

class CX509CertPoliciesExt : public CX509ExtensionBase
/**  The policy under which this certificate has been issued.
*
* Contains further information on a client's signature.
*
* @since v6.0 */
	{
public:
	/** Creates a new CX509CertPoliciesExt object from the specified buffer 
	* containing the encoded binary representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The CX509CertPoliciesExt object. */
	IMPORT_C static CX509CertPoliciesExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509CertPoliciesExt object from the specified buffer 
	* containing the encoded binary representation, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The CX509CertPoliciesExt object. */
	IMPORT_C static CX509CertPoliciesExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509CertPoliciesExt object from the specified buffer 
	* containing the encoded binary representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The CX509CertPoliciesExt object. */
	IMPORT_C static CX509CertPoliciesExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509CertPoliciesExt object from the specified buffer 
	* containing the encoded binary representation, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The CX509CertPoliciesExt object. */
	IMPORT_C static CX509CertPoliciesExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object. */
	virtual ~CX509CertPoliciesExt();
	
	/** Gets the certificate policy extension information.
	* 
	* @return	The certificate policy extension information. */
	IMPORT_C const CArrayPtrFlat<CX509CertPolicyInfo>& Policies() const;
private:
	CX509CertPoliciesExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CArrayPtrFlat<CX509CertPolicyInfo>* iPolicies;
	};

//7) policy mapping

class CX509PolicyMapping : public CBase
/** A set of policy mappings. 
* 
* @since v6.0 */
	{
public:
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyMapping object from the specified buffer containing 
	 * the encoded binary representation, and puts a pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation. 
	 **/
	static CX509PolicyMapping* NewL(const TDesC8& aBinaryData);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyMapping object from the specified buffer containing 
	 * the encoded binary representation, and puts a pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation. 
	 **/
	static CX509PolicyMapping* NewLC(const TDesC8& aBinaryData);
	
	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509PolicyMapping object from the specified buffer containing 
	 * the encoded binary representation, starting at the specified offset, and puts 
	 * a pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The position from which to start decoding. 
	 **/
	static CX509PolicyMapping* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** 
	 * @internalComponent
	 * 
	 * Creates a new CX509PolicyMapping object from the specified buffer containing 
	 * the encoded binary representation, starting at the specified offset, and puts 
	 * a pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The position from which to start decoding. 
	 **/
	static CX509PolicyMapping* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509PolicyMapping object copied from an existing one.
	* 
	* @param aMapping	The CX509PolicyMapping object to be copied. */
	IMPORT_C static CX509PolicyMapping* NewL(const CX509PolicyMapping& aMapping);
	
	/** Creates a new CX509PolicyMapping object copied from an existing one, and 
	* puts a pointer to the object onto the cleanup stack.
	* 
	* @param aMapping	The CX509PolicyMapping object to be copied. */
	IMPORT_C static CX509PolicyMapping* NewLC(const CX509PolicyMapping& aMapping);
	
	/** The destructor.
	* 
	* Frees all resources owned by the object. */
	virtual ~CX509PolicyMapping();
	
	/** Gets the issuer policy.
	* 
	* @return	The issuer policy. */
	IMPORT_C TPtrC IssuerPolicy() const;
	
	/** Gets the subject policy.
	* 
	* @return	The subject policy. */
	IMPORT_C TPtrC SubjectPolicy() const;
private:
	CX509PolicyMapping();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const CX509PolicyMapping& aMapping);
	HBufC* iIssuerPolicy;
	HBufC* iSubjectPolicy;
	};

class CX509PolicyMappingExt : public CX509ExtensionBase
/** An X.509 certificate extension that contains a set of policy mappings.
*
* A policy mapping allows a Certification Authority to declare that two certificate 
* policies are equivalent.
*
* @since v6.0 */
	{
public:
	/** Creates a new CX509PolicyMappingExt object from the specified buffer containing 
	* the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509PolicyMappingExt object. */
	IMPORT_C static CX509PolicyMappingExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509PolicyMappingExt object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509PolicyMappingExt object. */
	IMPORT_C static CX509PolicyMappingExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509PolicyMappingExt object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509PolicyMappingExt object. */
	IMPORT_C static CX509PolicyMappingExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509PolicyMappingExt object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509PolicyMappingExt object. */
	IMPORT_C static CX509PolicyMappingExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	virtual ~CX509PolicyMappingExt();
	
	/** Gets a reference to the array of policy mappings.
	* 
	* @return	The array of policy mappings. */
	IMPORT_C const CArrayPtrFlat<CX509PolicyMapping>& Mappings() const;
private:
	CX509PolicyMappingExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CArrayPtrFlat<CX509PolicyMapping>* iPolicies;
	};

//8) authority key ID

class CX509AuthorityKeyIdExt : public CX509ExtensionBase
/** An X.509 certificate extension that provides a way to find the public key corresponding 
* to the private key used to sign the certificate.
* 
* This is referred to as the authority key ID extension.
* 
* The key may be identified by the issuer's name and the issuing certificate's 
* serial number, or by a key identifier value either derived from the public 
* key or by some method of generating unique IDs. 
* 
* @since v6.0 */
	{
public:
	/** Creates a new CX509AuthorityKeyIdExt object from the specified buffer containing 
	* the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509AuthorityKeyIdExt object. */
	IMPORT_C static CX509AuthorityKeyIdExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509AuthorityKeyIdExt object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509AuthorityKeyIdExt object. */
	IMPORT_C static CX509AuthorityKeyIdExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509AuthorityKeyIdExt object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509AuthorityKeyIdExt object. */
	IMPORT_C static CX509AuthorityKeyIdExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509AuthorityKeyIdExt object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509AuthorityKeyIdExt object. */
	IMPORT_C static CX509AuthorityKeyIdExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	virtual ~CX509AuthorityKeyIdExt();
	
	/** Gets the authority name.
	* 
	* @return	The authority name. */
	IMPORT_C const CArrayPtrFlat<CX509GeneralName>& AuthorityName() const;
	
	/** Gets the authority certificate serial number.
	* 
	* @return	A pointer descriptor representing the authority certificate serial number. */
	IMPORT_C TPtrC8 AuthorityCertSerialNumber() const;
	
	/** Gets the key identifier value.
	* 
	* @return	A pointer descriptor representing the key identifier value. */
	IMPORT_C TPtrC8 KeyId() const;
private:
	CX509AuthorityKeyIdExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void DecodeNameL(const TDesC8& aBinaryData);
	void DecodeSerialNoL(const TDesC8& aBinaryData);
	void DecodeKeyIdL(const TDesC8& aBinaryData);
	CArrayPtrFlat<CX509GeneralName>* iAuthorityName;
	HBufC8* iAuthorityCertSerialNumber;
	HBufC8* iKeyIdentifier;
	};

//9) subject key ID

class CX509SubjectKeyIdExt : public CX509ExtensionBase
/** An X.509 certificate extension that provides a way of identifying certificates
* that contain a specific public key.
* 
* This is referred to as the subject key ID extension.
* 
* It consists of a key identifier value either derived from the public key or
* by some method of generating unique IDs.
* 
* @since v6.0 */
	{
public:
	/** Creates a new CX509SubjectKeyIdExt object from the specified buffer containing 
	* the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509SubjectKeyIdExt object. */
	IMPORT_C static CX509SubjectKeyIdExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509SubjectKeyIdExt object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509SubjectKeyIdExt object. */
	IMPORT_C static CX509SubjectKeyIdExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509SubjectKeyIdExt object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509SubjectKeyIdExt object. */
	IMPORT_C static CX509SubjectKeyIdExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509SubjectKeyIdExt object from the specified buffer containing 
	* the binary coded representation, starting at the specified offset, and puts 
	* a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509SubjectKeyIdExt object. */
	IMPORT_C static CX509SubjectKeyIdExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509SubjectKeyIdExt();
	
	/** Gets the key ID.
	* 
	* @return	A pointer descriptor representing the key ID. */
	IMPORT_C TPtrC8 KeyId() const;
private:
	CX509SubjectKeyIdExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	HBufC8* iKeyIdentifier;
	};

//10) extended key usage

class CX509ExtendedKeyUsageExt : public CX509ExtensionBase
/** An X.509 certificate extension that defines the extra uses to which a key may be put.
* 
* This is referred to as the extended key usage extension. 
* 
* @since v6.0 */
	{
public:
	/** Creates a new CX509ExtendedKeyUsageExt object from the specified buffer 
	* containing the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary data.
	* @return				The new CX509ExtendedKeyUsageExt object. */
	IMPORT_C static CX509ExtendedKeyUsageExt* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new CX509ExtendedKeyUsageExt object from the specified buffer 
	* containing the binary coded representation, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509ExtendedKeyUsageExt object. */
	IMPORT_C static CX509ExtendedKeyUsageExt* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new CX509ExtendedKeyUsageExt object from the specified buffer 
	* containing the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509ExtendedKeyUsageExt object. */
	IMPORT_C static CX509ExtendedKeyUsageExt* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new CX509ExtendedKeyUsageExt object from the specified buffer 
	* containing the binary coded representation, starting at the specified offset, 
	* and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509ExtendedKeyUsageExt object. */
	IMPORT_C static CX509ExtendedKeyUsageExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509ExtendedKeyUsageExt();
	
	/** Gets the sequence of Object Ids (OID) that identify the key usages.
	* 
	* The sequence is held in a set of heap descriptors.
	* 
	* @return	A reference to the array of pointers to the heap descriptors that 
	* 	contain the OIDs. */
	IMPORT_C const CArrayPtrFlat<HBufC>& KeyUsages() const;
private:
	CX509ExtendedKeyUsageExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CArrayPtrFlat<HBufC>* iKeyUsages;
	};

//11) authority information access

class CX509AccessDescription : public CBase
/** This class provides the access method OID and access location as used by X.509 private internet extensions
 * (authority information access).
 *
 */
	{
public:

	/** 
	 * @internalComponent
	 * 
	 * Creates a new CX509AccessDescription object from the specified buffer 
	 * containing the binary coded representation.
	 * 
	 * @param aBinaryData	The encoded binary data.
	 * @return				The new CX509AccessDescription object. 
	 **/
	static CX509AccessDescription* NewL(const TDesC8& aBinaryData);

	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509AccessDescription object from the specified buffer 
	 * containing the binary coded representation, and puts a pointer to it onto 
	 * the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CX509AccessDescription object. 
	 **/
	static CX509AccessDescription* NewLC(const TDesC8& aBinaryData);

	/** 
	 * @internalComponent
	 *
	 * Creates a new CX509AuthInfoAccessExt object from the specified buffer 
	 * containing the binary coded representation, starting at the specified offset.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding.
	 * @return				The new CX509AuthInfoAccessExt object. 
	 **/
	static CX509AccessDescription* NewL(const TDesC8& aBinaryData, TInt& aPos);

	/**
	 * @internalComponent
	 *
	 *  Creates a new CX509AccessDescription object from the specified buffer 
	 * containing the binary coded representation, starting at the specified offset, 
	 * and puts a pointer to it onto the cleanup stack.
	 * 
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding.
	 * @return				The new CX509AccessDescription object. 
	 **/
	static CX509AccessDescription* NewLC(const TDesC8& aBinaryData, TInt& aPos);

	/** Destructor. Frees all resources owned by the object, prior to its destruction. 
	*/
	virtual ~CX509AccessDescription();

	/** Gets the Access Method OID.
	*
	* @return	A non modifiable pointer descriptor to point to the access method OID.
	*/
	IMPORT_C TPtrC Method() const; 

	/** Gets the Access Location field which specifies the location where the additional 
	* information of the CA can be obtained.
	*
	* @return	A reference to access location .
	*/
	IMPORT_C const CX509GeneralName& Location() const;
private:
	CX509AccessDescription();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	HBufC* iMethodId;
	CX509GeneralName* iLocation;
	};

class CX509AuthInfoAccessExt : public CX509ExtensionBase
/** An X.509 certificate extension that defines the authority information access.
 * 
 * 
 */

	{
public:

	/** Creates a new CX509AuthInfoAccessExt object from the specified buffer 
	* containing the binary coded representation.
	* 
	* @param aBinaryData	The encoded binary data.
	* @return				The new CX509AuthInfoAccessExt object. 
	*/
	IMPORT_C static CX509AuthInfoAccessExt* NewL(const TDesC8& aBinaryData);

	/** Creates a new CX509AuthInfoAccessExt object from the specified buffer 
	* containing the binary coded representation, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new CX509AuthInfoAccessExt object. 
	*/
	IMPORT_C static CX509AuthInfoAccessExt* NewLC(const TDesC8& aBinaryData);

	/** Creates a new CX509AuthInfoAccessExt object from the specified buffer 
	* containing the binary coded representation, starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509AuthInfoAccessExt object. 
	*/
	IMPORT_C static CX509AuthInfoAccessExt* NewL(const TDesC8& aBinaryData, TInt& aPos);

	/** Creates a new CX509AuthInfoAccessExt object from the specified buffer 
	* containing the binary coded representation, starting at the specified offset, 
	* and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new CX509AuthInfoAccessExt object. 
	*/
	IMPORT_C static CX509AuthInfoAccessExt* NewLC(const TDesC8& aBinaryData, TInt& aPos);

	/** Destructor. Frees all resources owned by the object, prior to its destruction. 
	*/
	IMPORT_C ~CX509AuthInfoAccessExt();

	/** Gets the authority access description containing the method and location of additional information of CA ,
	* who issued the certificate in which this extension appears.
	*
	* @return	A reference to the array of pointers to the authority access descriptions.
	*/
	IMPORT_C const CArrayPtrFlat<CX509AccessDescription>& AccessDescriptions() const;
private:
	CX509AuthInfoAccessExt();
	void DoConstructL(const TDesC8& aBinaryData, TInt& aPos);
	CArrayPtrFlat<CX509AccessDescription>* iAccessDescs;
	};

#endif
