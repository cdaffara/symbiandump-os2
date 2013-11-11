/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#if !defined (__WTLSNAMES_H__)
#define __WTLSNAMES_H__

#include <e32base.h>
#include <e32std.h>
#include <x500dn.h>

/**
@file
This file contains the definition for class CWTLSName.

@publishedAll
@released

enum { null(0), text(1), binary(2), key_hash_sha(254), x509_name(255)}
	IdentifierType;

We only support text and x509_name as these are the only meaningful identifiers..
x509_name is X.500 Distinguished Name, and should use our existing X.500 DN implementation.  

struct {
		IdentifierType identifier_type;
		select (identifier_type) {
			case null: struct {};
			case text:
				CharacterSet character_set;
				opaque name<1.. 2^8-1>;
			case binary: opaque identifier<1..2^8-1>;
			case key_hash_sha: opaque key_hash[20];
			case x509_name: opaque distinguished_name<1..2^8-1>;
		} 
		Identifier;

uint16 CharacterSet;

This maps on to one of the IANA defined character sets. There are rather a lot
of these.  We just support the text type, with either Latin1 or UTF8 encoding.
*/

/**
 * Enumerates the types of WTLS certificate name forms/identifiers.
 * 
 * Only text strings and X.500 Distinguished Names are currently supported.
 * 
 */
enum 
	{
	/* Null */
	EWTLSNull =			0x00,
	/* Text string (Latin-1 or Unicode). 
	 *
	 * A text identifier consists of a 16-bit character set identifier; 
	 * this represents the IANA-assigned character set number. */
	EWTLSText =			0x01,
	/* Binary identifier.
	 *
	 * Certificates of this type will be rejected.*/
	EWTLSBinary =		0x02,
	/* Key Hash SHA-1. 
	 *
	 * Certificates of this type will be rejected.*/
	EWTLSKeyHashSha =	0xfe,
	/* X.500 Distinguished Name. */
	EWTLSX500DN =		0xff
	};

typedef TUint8 TWTLSNameType;

typedef TInt TWTLSCharSet;

// MIBenum constants from the IANA list of character sets.
// See http://www.iana.org/assignments/character-sets for more info.

/** MIBenum constant for the Latin1 IANA character set */
const TInt KWTLSLatin1CharSet = 4;

/** MIBenum constant for the UTF-8 IANA character set */
const TInt KWTLSUTF8CharSet = 106;

class CWTLSName : public CBase
/**
 * Stores the type of a WTLS name and the underlying encoding of the type.
 *
 */
	{
public:
	/**
	 * Creates a new CWTLSName object from the specified buffer containing the binary coded representation.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CWTLSName object.
	 */
	IMPORT_C static CWTLSName* NewL(const TDesC8& aBinaryData);

	/**
	 * Creates a new CWTLSName object from the specified buffer containing the binary coded representation,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CWTLSName object.
	 */
	IMPORT_C static CWTLSName* NewLC(const TDesC8& aBinaryData);
	
	/**
	 * Creates a new CWTLSName object from the specified buffer containing the binary coded representation, 
	 * starting at the specified offset.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding. It specifies an offset into the descriptor, 
	 *						and is updated to the position at the end of the object.
	 * @return				The new CWTLSName object.
	 */
	IMPORT_C static CWTLSName* NewL(const TDesC8& aBinaryData, TInt& aPos);

	/**
	 * Creates a new CWTLSName object from the specified buffer containing the binary coded representation, 
	 * starting at the specified offset, and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding. It specifies an offset into the descriptor, 
	 *						and is updated to the position at the end of the object.
	 * @return				The new CWTLSName object.
	 */
	IMPORT_C static CWTLSName* NewLC(const TDesC8& aBinaryData, TInt& aPos);

	/**
	 * Creates a new CWTLSName object from an existing one.
	 *
	 * @param aName	An existing CWTLSName object.
	 * @return		The new CWTLSName object.
	 */
	IMPORT_C static CWTLSName* NewL(const CWTLSName& aName);

	/**
	 * Creates a new CWTLSName object from an existing one, 
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aName	An existing CWTLSName object.
	 * @return		The new CWTLSName object.
	 */
	IMPORT_C static CWTLSName* NewLC(const CWTLSName& aName);	
	
	/**
	 * Destructor.
	 *
	 * Frees all resources owned by the object, prior to its destruction.
	 */
	IMPORT_C ~CWTLSName(); 
	
	/**
	 * Performs a simple byte compare between this WTLS name and a specified WTLS name.
	 *
	 * Needed for the constructing/validating of certificate chains.
	 *
	 * @param aName	An existing CWTLSName object.
	 * @return		ETrue, if the WTLS names match; EFalse, otherwise.
	 */	
	IMPORT_C TBool ExactMatchL(const CWTLSName& aName) const;
	
	/** 
	 * Gets the type of the WTLS name.
	 *
	 * @return	Type of WTLS name form.
	 */	
	IMPORT_C TWTLSNameType NameType() const;
	
	/** 
	 * Gets the encoding of the underlying type of WTLS name.
	 *
	 * @return	Pointer descriptor representing the encoding of the WTLS name type.
	 */	
	IMPORT_C TPtrC8 NameData() const;
	
	/**
	 * Gets the decoded value for the common or organisation name.
	 *
	 * Provides the functionality required by the CCertificate::IssuerL() and SubjectL() functions.
	 *
	 * @return A heap descriptor containing the decoded value of the common or organisation name.
	 */
	IMPORT_C HBufC* DisplayNameL() const;
private:
	CWTLSName();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void ConstructL(const CWTLSName& aName);
	void AllocNameDataL(const TDesC8& aBinaryData, TInt& aPos);
	void AllocTextDataL(const TDesC8& aBinaryData, TInt& aPos);
	TWTLSNameType iNameType;
	HBufC8* iNameData;
	};

class CWTLSText : public CBase
	{
public:
	/**
	 * Creates a new CWTLSText object from the specified buffer containing the binary coded representation.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CWTLSText object.
	 */
	IMPORT_C static CWTLSText* NewL(const TDesC8& aBinaryData);

	/**
	 * Creates a new CWTLSText object from the specified buffer containing the binary coded representation,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CWTLSText object.
	 */
	IMPORT_C static CWTLSText* NewLC(const TDesC8& aBinaryData);

	/**
	 * Creates a new CWTLSText object from the specified buffer containing the binary coded representation, 
	 * starting at the specified offset.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding. It specifies an offset into the descriptor, 
	 *						and is updated to the position at the end of the object.
	 * @return				The new CWTLSText object.
	 */
	IMPORT_C static CWTLSText* NewL(const TDesC8& aBinaryData, TInt& aPos);

	/**
	 * Creates a new CWTLSText object from the specified buffer containing the binary coded representation, 
	 * starting at the specified offset, and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding. It specifies an offset into the descriptor, 
	 *						and is updated to the position at the end of the object.
	 * @return				The new CWTLSText object.
	 */
	IMPORT_C static CWTLSText* NewLC(const TDesC8& aBinaryData, TInt& aPos);	
	
	/**
	 * Destructor.
	 *
	 * Frees all resources owned by the object, prior to its destruction.
	 */
	IMPORT_C ~CWTLSText(); 	
	
	/**
	 * Performs a simple byte compare between this CWTLSText object and a specified CWTLSText object.
	 *
	 * There is a subtle difference between this byte-match and CWTLSName::ExactMatchL().
	 * As opposed to the latter, this function should successfully match two names that 
	 * are the same that were encoded using different character sets.
	 *
	 * @param aName	An existing CWTLSText object.
	 * @return		ETrue, if the CWTLSText objects match; EFalse, otherwise.
	 */	
	IMPORT_C TBool ExactMatchL(const CWTLSText& aName) const;	
	
	/**
	 * Gets the name of the CWTLSText object.
	 *
	 * @return A pointer to the name of the CWTLSText object. 
	 */
	IMPORT_C TPtrC Name() const;
	
	/**
	 * Gets the character set of the CWTLSText object.
	 *
	 * @return The character set
	 */
	IMPORT_C TWTLSCharSet CharacterSet() const;
protected:
	/** 
	 * @internalAll
	 */
	CWTLSText();
	/** 
	 * @internalAll
	 */
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	HBufC* iName;
private:
	TInt iCharacterSet;
	};

//this class implements the 'structured' variant of the text type defined in the WTLS spec, section 10.5.2:
//<servicename>; <organization>; <country>[; <commonname>[; <extension>[; <extension>[ …. ]]]]
_LIT(KWTLSCountryName,"C");
_LIT(KWTLSOrganizationName,"O");
_LIT(KWTLSServiceName,"OU");
_LIT(KWTLSTitle,"T");
_LIT(KWTLSCommonName,"CN");

class TWTLSStructuredTextField 
	{
public:
	/** 
     * @internalAll
     */
	TWTLSStructuredTextField(const TDesC& aType, const TDesC& aValue);

	/**
	 *
	 * @return	
	 */
	IMPORT_C TPtrC Type() const;

	/**
	 *
	 * @return
	 */
	IMPORT_C TPtrC Value() const;

private:
	const TPtrC iType;
	const TPtrC iValue; 
	};

class CWTLSStructuredText : public CWTLSText
	{
public:
	/**
	 * Creates a new CWTLSStructuredText object from the specified buffer containing the binary coded representation.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CWTLSStructuredText object.
	 */
	IMPORT_C static CWTLSStructuredText* NewL(const TDesC8& aBinaryData);

	/**
	 * Creates a new CWTLSStructuredText object from the specified buffer containing the binary coded representation,
	 * and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @return				The new CWTLSStructuredText object.
	 */
	IMPORT_C static CWTLSStructuredText* NewLC(const TDesC8& aBinaryData);

	/**
	 * Creates a new CWTLSStructuredText object from the specified buffer containing the binary coded representation, 
	 * starting at the specified offset.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding. It specifies an offset into the descriptor, 
	 *						and is updated to the position at the end of the object.
	 * @return				The new CWTLSStructuredText object.
	 */
	IMPORT_C static CWTLSStructuredText* NewL(const TDesC8& aBinaryData, TInt& aPos);

	/**
	 * Creates a new CWTLSStructuredText object from the specified buffer containing the binary coded representation, 
	 * starting at the specified offset, and puts a pointer to it onto the cleanup stack.
	 *
	 * @param aBinaryData	The encoded binary representation.
	 * @param aPos			The offset position from which to start decoding. It specifies an offset into the descriptor, 
	 *						and is updated to the position at the end of the object.
	 * @return				The new CWTLSStructuredText object.
	 */
	IMPORT_C static CWTLSStructuredText* NewLC(const TDesC8& aBinaryData, TInt& aPos);
	
	/**
	 * Destructor.
	 *
	 * Frees all resources owned by the object, prior to its destruction.
	 */
	IMPORT_C ~CWTLSStructuredText(); 
	
	/**
	 * 
	 *
	 * @return
	 */
	IMPORT_C HBufC* DisplayNameL() const;

	//accessors for defined fields
	
	/**
	 *
	 *
	 * @return
	 */
	IMPORT_C TPtrC ServiceName() const;
	
	/**
	 *
	 *
	 * @return
	 */
	IMPORT_C TPtrC Organization() const;
	
	/**
	 *
	 *
	 * @return
	 */
	IMPORT_C TPtrC Country() const;
	
	/**
	 *
	 *
	 * @return
	 */
	IMPORT_C TInt Count() const;

	
	/**
	 *
	 *
	 * Note 
	 *
	 * @param aType
	 * @return	A pointer to a TWTLSStructuredTextField object; NULL if field not found. 
	 *			The returned object remains the property of the structured text object
	 *			(so don't delete it).
	 */
	IMPORT_C const TWTLSStructuredTextField* FieldByName(const TDesC& aType) const;
	
	/**
	 *
	 *
	 * @return
	 */
	IMPORT_C const TWTLSStructuredTextField& FieldByIndex(TInt aIndex) const;
private:
	CWTLSStructuredText();
	void ConstructL(const TDesC8& aBinaryData, TInt& aPos);
	void AddFieldValueL(const TDesC& aFieldName, TInt& aPos);
	void AddFieldL(TInt& aPos);
	TPtrC GetFieldL(TDesC& aString, TInt& aPos);
	TBool GetSubFieldL(TDesC& aString, TInt& aPos);
	CArrayFixFlat<TWTLSStructuredTextField>* iFields; 
	};

#endif


