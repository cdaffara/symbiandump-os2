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
* X509 general name class
*
*/


/**
 @file 
 @publishedAll
 @released
*/

#ifndef __X509GN_H__
#define __X509GN_H__

#include <e32std.h>
#include <badesca.h>

/** A general name type.
 * 
 * @publishedAll
 * @released
 */
typedef TUint TGNType;

/**
 * @publishedAll
 * @released
 */
enum 
	{
	EX509RFC822Name = 1,
	EX509DNSName = 2,
	EX509DirectoryName = 4,
	EX509URI = 6,
	EX509IPAddress = 7
	};
//specific name forms we support...
//we should probably put these in a file of their own,
//but they can stay here for now

//superclass

class CX509DomainName : public CBase
/** An X.509 domain name.
* 
* Base class for email address and DNS names. 
* 
* @publishedAll
* @released
* @since v6.0 */
	{
public:
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509DomainName();
	
	/** Tests whether every sub domain in the specified domain name is the same as 
	* the corresponding sub domain in this object, starting at the end.
	* 
	* For example, 'john.doe@symbian.com' is within the subtree 'symbian.com' but 
	* is not within the subtree 'john' or 'symbian'.
	* 
	* @param aName	The domain name.
	* @return		ETrue, if every sub domain in the specified domain name is the same 
					as the corresponding sub domain in this object; EFalse, otherwise. */
	IMPORT_C TBool IsWithinSubtree(const CX509DomainName& aName) const;
	
	/** Gets the full domain name.
	* 
	* @return	A pointer descriptor representing the full domain name. */
	IMPORT_C TPtrC Name() const;
protected:
	/** Adds a domain by its subdomains to an array of pointer descriptors.
	* 
	* @param aPos	The position within the domain name.
	* @return		ETrue if the domain has been added; EFalse, otherwise. 
	* @internalAll
	*/
	TBool AddDomainL(TInt& aPos);
	
	/** Adds a subdomain to an array of pointer descriptors.
	* 
	* @param aPos	The position within the domain name.
	* @return		ETrue if the subdomain has been added; EFalse, otherwise. 
	* @internalAll
	*/
	TBool AddSubdomainL(TInt& aPos);
	
	/** Adds a subdomain separator after each subdomain entry in an array of pointer 
	* descriptors that represent the subdomains of the domain name.
	* 
	* @param aPos	The position within the domain name.
	* @return		ETrue if the subdomain separator has been added; EFalse, otherwise. 
	* @internalAll
	*/
	TBool AddSubdomainSeparatorL(TInt& aPos);
	
	/** Validates a string.
	* 
	* @param aStr	The string to be verified.
	* @return		ETrue if the string is valid; EFalse, otherwise.*/
	virtual TBool IsValidString(const TDesC& aStr) const;
	
	/** Validates a character.
	* 
	* @param aChar	The character to be validated.
	* @return		ETrue if the character is valid; EFalse, otherwise. */
	virtual TBool IsValidChar(const TChar& aChar) const;
	
	/** Tests whether a character is a letter or a numeral.
	* 
	* @param aChar	The character to be tested.
	* @return		ETrue if the character is a letter or a numeral; EFalse, otherwise. 
	* @internalAll
	*/
	TBool IsAlphaOrNum(const TChar& aChar) const;
	
	/** Tests whether the character is a letter.
	* 
	* @param aChar	The character to be tested.
	* @return		ETrue if the character is a letter; EFalse, otherwise. 
	* @internalAll
	*/
	TBool IsAlpha(const TChar& aChar) const;
	
	/** Default constructor. 
	 * @internalAll
	 */
	CX509DomainName();
protected:
	/** An array of pointer descriptors representing the subdomains of the DSN name. */
	RArray<TPtrC> iRep;
	/** A heap descriptor representing the full domain name. */
	HBufC* iName;
	};

//rfc822 email address has the form localpart@domain

class CX509RFC822NameSubtree : public CX509DomainName
/** A full or partial RFC 822 email address.
* 
* The address may not contain the local host name as this is optional.
* 
* The object is initialised with 8-bit encoded binary data, which is parsed 
* into a series of sub domains and an optional local host. 
*
* @publishedAll
* @released */
	{
public:
	/** Creates an RFC 822 email address object from the specified buffer containing 
	* the binary coded representation.
	* 
	* The data is parsed into a series of sub domains and an optional local host.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new RFC 822 email address object. */
	IMPORT_C static CX509RFC822NameSubtree* NewL(const TDesC8& aBinaryData);
	
	/** Creates an RFC 822 email address object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* The data is parsed into a series of sub domains and an optional local host.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new RFC 822 email address object. */
	IMPORT_C static CX509RFC822NameSubtree* NewLC(const TDesC8& aBinaryData);
	
	/** Gets a reference to the array of pointer descriptors representing the subdomains 
	* of the RFC 822 email address.
	* 
	* @return	The array of pointer descriptors. */
	IMPORT_C const RArray<TPtrC>& Rep() const;

protected:
	/** Second-phase constructor.
	* 
	* @param aBinaryData	The encoded binary representation. */
	virtual void ConstructL(const TDesC8& aBinaryData);
	
	/** Adds a local host.
	* 
	* @param aPos	The position from which to start decoding.
	* @return		ETrue, if the host has been found and added; EFalse, otherwise. 
	* @internalAll
	*/
	TBool AddLocalHostL(TInt& aPos);
	
	/** Tests whether the character is valid.
	* 
	* @param aChar	The character to be tested.
	* @return		ETrue, if the character is valid; EFalse, otherwise. 
	* @internalAll
	*/
	TBool IsValidChar(const TChar& aChar) const;
	};

class CX509RFC822Name : public CX509RFC822NameSubtree
/** A full RFC 822 email address.
* 
* Exactly as subtree, but requires local host and full domain name. 
*
* @publishedAll
* @released */
	{
public:
	/** Creates a full RFC 822 email address object from the specified buffer containing 
	* the binary coded representation.
	* 
	* The data is parsed into a series of sub domains.
	* 
	* The data must represent a full RFC 822 email address, otherwise this function leaves.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new full RFC 822 email address object. */
	IMPORT_C static CX509RFC822Name* NewL(const TDesC8& aBinaryData);
	
	/** Creates a full RFC 822 email address object from the specified buffer containing 
	* the binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* The data is parsed into a series of sub domains.
	* 
	* The data must represent a full RFC 822 email address, otherwise this function leaves.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new full RFC 822 email address object. */
	IMPORT_C static CX509RFC822Name* NewLC(const TDesC8& aBinaryData);
private:
	virtual void ConstructL(const TDesC8& aBinaryData);
	};

class CX509DNSNameSubtree : public CX509DomainName
/** A Domain Name System (DNS) name subtree.
* 
* Initialised with 8-bit encoded binary data, which is parsed into a series 
* of sub domains.
* 
* Because it is a subtree it may start with a period. For example, '.symbian.com', 
* indicating that 'symbian.com' does not lie within the subtree. 
*
* @publishedAll
* @released */
	{
public:
	/** Creates a DNS name object from the specified buffer containing the binary coded 
	* representation.
	* 
	* The data is parsed into a series of sub domains.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new DNS name object. */
	IMPORT_C static CX509DNSNameSubtree* NewL(const TDesC8& aBinaryData);
	
	/** Creates a DNS name object from the specified buffer containing the binary coded 
	* representation, and puts a pointer to it onto the cleanup stack.
	* 
	* The data is parsed into a series of sub domains.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new DNS name object. */
	IMPORT_C static CX509DNSNameSubtree* NewLC(const TDesC8& aBinaryData);

//	IMPORT_C static CX509DNSNameSubtree* NewL(const CX509DNSNameSubtree& aName);
//	IMPORT_C static CX509DNSNameSubtree* NewLC(const CX509DNSNameSubtree& aName);
	
	/** Gets a reference to the array of pointer descriptors representing the subdomains 
	* of the DSN name.
	* 
	* @return	The array of pointer descriptors. */
	IMPORT_C const RArray<TPtrC>& Rep() const;
	
private:
	void ConstructL(const CX509DNSNameSubtree& aName);
	void ConstructL(const TDesC8& aBinaryData);
	TBool IsValidString(const TDesC& aStr) const;
	};

class CX509DNSName : public CX509DNSNameSubtree
/** A Domain Name System (DNS) name.
* 
* The name must begin with a valid sub domain and not a period. 
*
* @publishedAll
* @released */
	{
public:
	/** Creates a DNS name object from the specified buffer containing the binary coded 
	* representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new DNS name object. */
	IMPORT_C static CX509DNSName* NewL(const TDesC8& aBinaryData);
	
	/** Creates a DNS name object from the specified buffer containing the binary coded 
	* representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new DSN name object. */
	IMPORT_C static CX509DNSName* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new DSN name object from an existing object.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aName	The DSN name object to be copied.
	* @return		The DSN name object. */
	IMPORT_C static CX509DNSName* NewL(const CX509DNSName& aName);
	
	/** Creates a new DSN name object from an existing object, and puts a pointer to 
	* it onto the clean up stack.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aName	The DNS name object to be copied.
	* @return		The new DSN name object. */
	IMPORT_C static CX509DNSName* NewLC(const CX509DNSName& aName);
	
	/** Constructs a new DSN name object from a name string.
	* 
	* @param aNameString	The name string.
	* @return				The new DSN name object. */
	IMPORT_C static CX509DNSName* NewL(const TDesC& aNameString);
	
	/** Creates a DSN name object from a name string, and puts a pointer to it onto 
	* the cleanup stack.
	* 
	* @param aNameString	The name string.
	* @return				The new DSN name object. */
	IMPORT_C static CX509DNSName* NewLC(const TDesC& aNameString);
private:
	void ConstructL(const TDesC8& aBinaryData);
	void ConstructL(const TDesC& aNameString);
	void ConstructL(const CX509DNSName& aName);
	void ParseNameL();
	};

class CX509IPBasedURI : public CBase
/** A URI.
* 
* The class extracts a DNS name, i.e. the host part of the URI.
* 
* Note that it must be of 'ip-based' form (see RFC 1738 section 3.1) and contain 
* a domain name (not an IP address). 
*
* @publishedAll
* @released */
	{
public:
	/** Creates an IP based URI name object from the specified buffer containing the 
	* binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new URI name object. */
	IMPORT_C static CX509IPBasedURI* NewL(const TDesC8& aBinaryData);
	
	/** Creates a URI name object from the specified buffer containing the binary coded 
	* representation, and puts a pointer to it onto the clean up stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new URI name object. */
	IMPORT_C static CX509IPBasedURI* NewLC(const TDesC8& aBinaryData);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509IPBasedURI();
	
	/** Gets the host part.
	* 
	* @return	The host part. */
	IMPORT_C const CX509DNSName& Host() const;
	
	/** Gets the whole name.
	* 
	* @return	A pointer descriptor representing the whole name. */
	IMPORT_C TPtrC Name() const;
	
	/** Default constructor.
	 * @internalAll
	 */
	CX509IPBasedURI();
private:
	void ConstructL(const TDesC8& aBinaryData);
	TPtrC ExtractHostNameL() const;
	CX509DNSName* iHost;
	HBufC* iName;
	};

class CX509IPSubnetMask : public CBase
/** An IP subnet mask.
* 
* Input data is parsed into an 8-bit base address and an 8-bit mask. 
*
* @publishedAll
* @released */
	{
	friend class CX509IPAddress;
public:
	/** Creates an IP subnet mask object from the specified buffer containing the binary 
	* coded representation.
	* 
	* The data is parsed into an 8-bit base address and an 8-bit mask.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new IP subnet mask object. */
	IMPORT_C static CX509IPSubnetMask* NewL(const TDesC8& aBinaryData);
	
	/** Creates an IP subnet mask object from the specified buffer containing the binary 
	* coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* The data is parsed into an 8-bit base address and an 8-bit mask.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new IP subnet mask object. */
	IMPORT_C static CX509IPSubnetMask* NewLC(const TDesC8& aBinaryData);
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509IPSubnetMask();
	
	/** Gets the base IP address.
	* 
	* @return	A pointer descriptor representing the base IP address. */
	IMPORT_C TPtrC8 BaseAddress() const;
	
	/** Gets the subnet mask.
	* 
	* @return	A pointer descriptor representing the subnet mask. */
	IMPORT_C TPtrC8 Mask() const;
	
	/** Constructor. 
	 * @internalAll
	 */
	CX509IPSubnetMask();
private:
	void ConstructL(const TDesC8& aBinaryData);
	HBufC8* iName;
	};

class CX509IPAddress : public CBase
/** A single IP address. 
*
* @publishedAll
* @released */
	{
public:
	/** Creates a single IP address object from the specified buffer containing the 
	* binary coded representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new single IP addres. */
	IMPORT_C static CX509IPAddress* NewL(const TDesC8& aBinaryData);
	
	/** Creates a single IP address object from the specified buffer containing the 
	* binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new single IP addres object. */
	IMPORT_C static CX509IPAddress* NewLC(const TDesC8& aBinaryData);
	
	/** Tests whether the IP address lies within the specified subnet address.
	* 
	* @param aName	The subnet mask name.
	* @return		ETrue, if the IP address lies within the specified subnet address; 
	* EFalse, otherwise. */
	IMPORT_C TBool IsWithinSubtree(const CX509IPSubnetMask& aName) const;
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509IPAddress();
	
	/** Gets the IP address.
	* 
	* @return	A pointer descriptor representing the single IP address. */
	IMPORT_C TPtrC8 Address() const;
	
	/** Constructor. 
	 * @internalAll
	 */
	CX509IPAddress();
private:
	void ConstructL(const TDesC8& aBinaryData);
	HBufC8* iName;//4 octets for v4, 16 for v6
	};

//general name...

class CX509GeneralName : public CBase
/** Defines a general name.
* 
* The class contains a tag and a name whose form depends on the value of that tag.
* 
* When a General Name is included as part of a Name Constraints Extension, the 
* values need not be complete names but may be simply subtrees.
* 
* For example, 'symbian.com' instead of john.doe@symbian.com'. Both names are 
* valid subtrees but only the second is a valid RFC822 name. 
*
* @publishedAll
* @released */
	{
public:
	/** Creates a new general name object from the specified buffer containing the 
	* binary coded representation.	
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new general name object. */
	IMPORT_C static CX509GeneralName* NewL(const TDesC8& aBinaryData);
	
	/** Creates a new general name object from the specified buffer containing the 
	* binary coded representation, and puts a pointer to it onto the cleanup stack.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @return				The new general name object. */
	IMPORT_C static CX509GeneralName* NewLC(const TDesC8& aBinaryData);
	
	/** Creates a new general name object from the specified buffer containing the 
	* binary coded representation,starting at the specified offset.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new general name object. */
	IMPORT_C static CX509GeneralName* NewL(const TDesC8& aBinaryData, TInt& aPos);
	
	/** Creates a new general name object from the specified buffer containing the 
	* binary coded representation, starting at the specified offset, and puts a 
	* pointer to it onto the cleanup stack.
	* 
	* Initialises the object from its encoded binary form into an internal representation.
	* 
	* @param aBinaryData	The encoded binary representation.
	* @param aPos			The offset position from which to start decoding.
	* @return				The new general name object. */
	IMPORT_C static CX509GeneralName* NewLC(const TDesC8& aBinaryData, TInt& aPos);

	/** Creates a new general name object from an existing object.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aName			The general name object to be copied.
	* @return				The new general name object. */
	IMPORT_C static CX509GeneralName* NewL(const CX509GeneralName& aName);
	
	/** Creates a new general name object from an existing object, and puts a pointer 
	* to it onto the cleanup stack.
	* 
	* This is equivalent to a copy constructor.
	* 
	* @param aName			The general name object to be copied.
	* @return				The new general name object. */
	IMPORT_C static CX509GeneralName* NewLC(const CX509GeneralName& aName);

	/** Gets the type tag.
	* 
	* @return	The type tag. */
	IMPORT_C TGNType Tag() const;
	
	/** Gets the name.
	* 
	* @return	A pointer descriptor representing the name. */
	IMPORT_C TPtrC8 Data() const;
	
	/** Destructor.
	* 
	* Frees all resources owned by the object, prior to its destruction. */
	IMPORT_C ~CX509GeneralName(); 
	
	/** Dummy Function which always returns EFalse.
	* @param aName	The general name object to be copied.
	* @return		EFalse.
	*/
	IMPORT_C TBool ExactMatch(const CX509GeneralName& aName) const;
private:
	CX509GeneralName();
	CX509GeneralName(TGNType aType);
	void ConstructL();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const TDesC8& aData);
	TGNType iTag;
	HBufC8* iData;
	};

#endif
