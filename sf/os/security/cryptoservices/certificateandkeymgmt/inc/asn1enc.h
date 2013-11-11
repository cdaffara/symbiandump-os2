/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declares all classes used to decode ASN.1 data, including the 
* base interface.
*
*/


/**
 @file
 @publishedAll
 @released
*/

#ifndef __ASN1ENC_H__
#define __ASN1ENC_H__

#include <e32base.h>
#include <asn1cons.h>

class CASN1EncBase;
class TASN1EncBase128DER;

class TInteger;

typedef void (CASN1EncBase::* WriteFunc)(TDes8& aBuf) const;

/**
 * Base class for all ASN.1 types that we can encode.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncBase : public CBase
	{
public:
	/**
	 * Gives total number of octets in the DER encoding of this 
	 * object.
	 * @return Number of octets in DER encoding of this object.
	 */
	IMPORT_C TUint LengthDER() const;
	
	/**
	 * Writes entire DER encoding of this object into the given 
	 * buffer.
	 * @param aBuf Buffer receiving the encoding.
	 * @param aPos Position to start writing at.
	 */
	IMPORT_C void WriteDERL(TDes8& aBuf, TUint& aPos) const;

	/**
	 * Sets tag type/class of the encoding object
	 * @param aType Tag type to set
	 * @param aClass Tag class to set.
	 */
	IMPORT_C void SetTag(const TTagType aType,
		const TASN1Class aClass = EContextSpecific);

	IMPORT_C ~CASN1EncBase();

	/**
	 * @internalComponent
	 * 
	 * Sets parent for the object
	 * @param aParent Pointer to an ASN.1 object that becomes this 
	 *     object's parent.
	 */
	void SetParent(CASN1EncBase* aParent);

protected:
	/**
	 * Protected constructor
	 * @param aType Tag type of the new object
	 * @param aClass Tag class of the new object.
	 */
	IMPORT_C CASN1EncBase(const TTagType aType, const TASN1Class aClass);

	/** 
	 * Must call this version from derived classes in their 
	 * ConstructL, but only once they're ready to have 
	 * CalculateContentsLengthDER called on them.
	 */
	IMPORT_C virtual void ConstructL();

	/**
	 * @internalComponent
	 *
	 * Derived classes must call this if the length of their 
	 * contents changes after construction.
	 */
	void ContentsLengthChanged();

protected:
	/** 
	 * @internalComponent
	 * 
	 * Calculates number of octets in DER length encoding. Must set 
	 * value of the appropriate data member. Made protected because it is 
	 * needed by CASN1EncEncoding class.
	 */
	void CalculateLengthLengthDER();

private:
	/** 
	 * Calculates number of octets in DER tag encoding. Must set 
	 * value of the appropriate data member.
	 */
	void CalculateTagLengthDER();

	/** 
	 * Calculates number of octets in DER content encoding. Must set 
	 * value of the appropriate data member.
	 */
	virtual void CalculateContentsLengthDER() = 0;

	virtual TBool IsConstructed() const = 0;
	
	// Write the octet data in each section
	// Note that buffer *will* be big enough: these are called only 
	// after checking.

	/** 
	 * Writes DER tag encoding into supplied buffer, which is 
	 * already verified to be big enough.
	 * @param aBuf Buffer to write to.
	 */
	void WriteTagDERL(TDes8& aBuf) const;

	/** 
	 * Writes DER length encoding into supplied buffer, which is 
	 * already verified to be big enough.
	 * @param aBuf Buffer to write to.
	 */
	virtual void WriteLengthDER(TDes8& aBuf) const;

	/** 
	 * Writes DER content encoding into supplied buffer, which is 
	 * already verified to be big enough. Must be implemented by 
	 * derived classes.
	 * @param aBuf Buffer to write to.
	 */
	virtual void WriteContentsDERL(TDes8& aBuf) const = 0;

	/** 
	 * Helper function, used for efficiency
	 * @param aBuf Buffer to write to
	 * @param aPos Position in the buffer to start writing at 
	 *     (updated on exit)
	 * @param aLength Length of data to write
	 * @param aWriteFunc Points to the function used to perform 
	 *     the actual write operation.
	 */
	void WriteHelperL(TDes8& aBuf, TUint& aPos, const TUint aLength,
		WriteFunc aWriteFunc) const;

protected:
	// Cached length data - data set from the CalculateXxxx methods above
	TUint iTagLengthDER; ///< Length of DER encoded tag
	TUint iLengthLengthDER; ///< Length of DER encoded length
	TUint iContentsLengthDER; ///< Length of DER encoded content

private:
	/** 
	 * The object owning this one (if we're held in a sequence, 
	 * for example).
	 */
	CASN1EncBase* iParent;

	// Tag data
	TTagType iType; ///< Tag type of this object
	TASN1Class iClass; ///< Tag class of this object
	};


/** 
 * Base class for all ASN1 constructed-type objects.  Most of these 
 * are container classes, but another type is the explicit-tagging 
 * wrapper object.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncConstructed : public CASN1EncBase
	{
protected:
	/** 
	 * @internalComponent
	 * 
	 * Protected constructor
	 * @param aType Tag type for the new object
	 * @param aClass Tag class for the new object.
	 */
	CASN1EncConstructed(const TTagType aType, const TASN1Class aClass);

private:
	virtual const CASN1EncBase& Child(const TUint aIndex) const = 0;
	virtual TUint NumChildren() const = 0;

private:
	// From CASN1EncBase
	void CalculateContentsLengthDER();
	TBool IsConstructed() const;
	void WriteContentsDERL(TDes8& aBuf) const;
	};


/** 
 * Class used to wrap other encoding objects in order to give 
 * them an explicit tag.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncExplicitTag : public CASN1EncConstructed
	{
public:
	/** 
	 * Takes ownership of the encoder, *including* the case when
	 * this method leaves.
	 * @param aEncoder ASN.1 encoding object to wrap
	 * @param aType Tag type to assign
	 * @param aClass Tag class to assign
	 * @return Wrapped encoding object pushed on the cleanup stack.
	 */
	IMPORT_C static CASN1EncExplicitTag* NewLC(CASN1EncBase* aEncoder,
		const TTagType aType, const TASN1Class aClass = EContextSpecific);

	/** 
	 * Takes ownership of the encoder, *including* the case when
	 * this method leaves.
	 * @param aEncoder ASN.1 encoding object to wrap
	 * @param aType Tag type to assign
	 * @param aClass Tag class to assign
	 * @return Wrapped encoding object.
	 */
	IMPORT_C static CASN1EncExplicitTag* NewL(CASN1EncBase* aEncoder,
		const TTagType aType, const TASN1Class aClass = EContextSpecific);

	IMPORT_C ~CASN1EncExplicitTag(); // virtual from base

private:
	CASN1EncExplicitTag(CASN1EncBase* aEncoder, const TTagType aType, 
		const TASN1Class aClass);

	// From CASN1EncConstructed
	TUint NumChildren() const;
	const CASN1EncBase& Child(const TUint aIndex) const;

private:
	CASN1EncBase* iEncoder;  // Inner encoding object
	};


/** 
 * Base class for all ASN1 container types - sequences, 
 * sets, etc.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncContainer : public CASN1EncConstructed
	{
public:
	/** 
	 * Call this to add a child object to the container.
	 * Takes ownership if and only if it doesn't Leave.
	 * Checks for null input, calls AddChildInt, calls 
	 * ContentsLengthChanged().
	 * @param aChild Child ASN1 encoding object to add.
	 */
	IMPORT_C void AddChildL(CASN1EncBase* aChild);

	/** 
	 * Call this to add a child object to the container.
	 * Takes ownership if and only if it doesn't Leave.
	 * Checks for null input, calls AddChildInt, calls 
	 * ContentsLengthChanged(). Pops added child object 
	 * off the cleanup stack.
	 * @param aChild Child ASN1 encoding object to add.
	 */
	IMPORT_C void AddAndPopChildL(CASN1EncBase* aChild);

protected:
	/** @internalComponent */
	CASN1EncContainer(const TTagType aType);

private:
	/** 
	 * Internal method, derived classes implement to add a child.
	 * No need to check for null input or call 
	 * ContentsLengthChanged(). Takes ownership, but only if you 
	 * don't leave.
	 * @param aChild Child encoding object to add.
	 */
	virtual void AddChildIntL(const CASN1EncBase* aChild) = 0;
	};

/** 
 * Class for encoding SEQUENCE and SEQUENCE-OF data types.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncSequence : public CASN1EncContainer
	{
public:
	IMPORT_C static CASN1EncSequence* NewL();
	IMPORT_C static CASN1EncSequence* NewLC();

	IMPORT_C ~CASN1EncSequence(); // virtual from base

protected:
	/** @internalComponent */ 
	CASN1EncSequence();

private:
	// From CASN1EncContainer
	const CASN1EncBase& Child(const TUint aIndex) const;
	void AddChildIntL(const CASN1EncBase* aChild);
	TUint NumChildren() const;

private:
	RPointerArray<CASN1EncBase> iChildren;
	};

/** 
 * Class for encoding SET and SET-OF data types.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncSet : public CASN1EncContainer
	{
public:
	/**
	Creates an ASN.1 Set encoder.
	@return The fully constructed object.
	*/
	IMPORT_C static CASN1EncSet* NewL();

	/**
	Creates an ASN.1 Set encoder, and puts it onto the cleanup stack.
	@return The fully constructed object.
	*/
	IMPORT_C static CASN1EncSet* NewLC();

	/**
	Destructor.
	*/
	IMPORT_C ~CASN1EncSet(); // virtual from base

protected:
	/**
	 * @internalComponent
	 * 
	 * Constructor
	 */
	CASN1EncSet();

private:
	// From CASN1EncContainer
	const CASN1EncBase& Child(const TUint aIndex) const;
	void AddChildIntL(const CASN1EncBase* aChild);
	TUint NumChildren() const;

private:
	RPointerArray<CASN1EncBase> iChildren;
	};



/**
 * All ASN1 primitive type encoding classes derive from here.
 *
 * @publishedAll
 * @released
 */
class CASN1EncPrimitive : public CASN1EncBase
	{
protected:
	IMPORT_C CASN1EncPrimitive(const TTagType aType);

private:
	TBool IsConstructed() const; ///< Inherited from CASN1EncBase
	};


/** 
 * Class for encoding NULLs.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncNull : public CASN1EncPrimitive
	{
public:
	IMPORT_C static CASN1EncNull* NewL();
	IMPORT_C static CASN1EncNull* NewLC();

private:
	CASN1EncNull();

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER() ;
	void WriteContentsDERL(TDes8& aBuf) const;
	};


/** 
 * Class for encoding Boolean values.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncBoolean : public CASN1EncPrimitive
	{
public:
	IMPORT_C static CASN1EncBoolean* NewLC(const TBool aBool);
	IMPORT_C static CASN1EncBoolean* NewL(const TBool aBool);

private:
	CASN1EncBoolean(const TBool aBool);

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER() ;
	void WriteContentsDERL(TDes8& aBuf) const;

	const TBool iBool;
	};


/** 
 * Class for encoding TInts only. Use CASN1EncBigInt for encoding 
 * Big Integer objects.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncInt : public CASN1EncPrimitive
	{
public:
	IMPORT_C static CASN1EncInt* NewLC(const TInt aInt);
	IMPORT_C static CASN1EncInt* NewL(const TInt aInt);

private:
	CASN1EncInt(const TInt aInt);

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER() ;
	void WriteContentsDERL(TDes8& aBuf) const;

private:
	const TInt iInt;
	};


/** 
 * Class for encoding Big Integer objects only - use CASN1EncInt 
 * for TInts.
 *
 * @publishedAll
 * @released
 */
class CASN1EncBigInt : public CASN1EncPrimitive
	{
public:
	// Takes a deep copy
	IMPORT_C static CASN1EncBigInt* NewLC(const TInteger& aInteger);
	IMPORT_C static CASN1EncBigInt* NewL(const TInteger& aInteger);

	IMPORT_C ~CASN1EncBigInt(); // virtual from base

private:
	CASN1EncBigInt();
	void ConstructL(const TInteger& aInteger);

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER();
	void WriteContentsDERL(TDes8& aBuf) const;

private:
	HBufC8* iContents;
	TPtrC8 iWriteContents;
	};


/** 
 * Class for encoding octet strings.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncOctetString : public CASN1EncPrimitive
	{
public:
	// Takes a deep copy
	IMPORT_C static CASN1EncOctetString* NewLC(const TDesC8& aStr);
	IMPORT_C static CASN1EncOctetString* NewL(const TDesC8& aStr);

	IMPORT_C ~CASN1EncOctetString(); // virtual from base

private:
	CASN1EncOctetString();
	void ConstructL(const TDesC8& aStr);

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER();
	void WriteContentsDERL(TDes8& aBuf) const;

private:
	HBufC8* iContents;
	};


/** 
 * Class for encoding printable strings.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncPrintableString : public CASN1EncPrimitive
	{
public:
	/**
	Creates an ASN.1 Printable String encoder, and puts it onto the cleanup stack.
	@return The fully constructed object.
	*/
	IMPORT_C static CASN1EncPrintableString* NewLC(const TDesC8& aStr);

	/**
	Creates an ASN.1 Printable String encoder.
	@return The fully constructed object.
	*/
	IMPORT_C static CASN1EncPrintableString* NewL(const TDesC8& aStr);

	/**
	Destructor.
	*/
	IMPORT_C ~CASN1EncPrintableString(); // virtual from base

private:
	CASN1EncPrintableString();
	void ConstructL(const TDesC8& aStr);
	TInt CheckValid(const TDesC8& aStr);

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER();
	void WriteContentsDERL(TDes8& aBuf) const;

private:
	HBufC8* iContents;
	};


/**
 * Class for encoding bit strings (keys, for example).
 *
 * @publishedAll
 * @released
 */
class CASN1EncBitString : public CASN1EncPrimitive
	{
public:
	/** 
	 * Constructs a new DER bit string encoder from a bit string that 
	 * does not have unused bits at the end, i.e. is octet-aligned. 
	 * The passed string must be in big-endian format. 
	 * @param aBitStr	Octet-aligned bit string.
	 * @return			A new DER bit string encoder object, 
	 *					which is left on the cleanup stack.
	 */
	IMPORT_C static CASN1EncBitString* NewLC(const TDesC8& aBitStr);

	/** 
	 * Constructs a new DER bit string encoder from a bit string that 
	 * does not have unused bits at the end, i.e. is octet-aligned. 
	 * The passed string must be in big-endian format. 
	 * @param aBitStr	Octet-aligned bit string.
	 * @return			A new DER bit string encoder object.
	 */
	IMPORT_C static CASN1EncBitString* NewL(const TDesC8& aBitStr);
	
	/** 
	 * Constructs a new DER bit string encoder from a bit string that 
	 * is not octet-aligned, which means it has unused bits in its last 
	 * octet. The passed string must be in big-endian format.
	 * @param aBitStr		Bit string.
	 * @param aLengthBits	Length in bits of the passed bit string. 
	 *     					The function will panic if aLengthBits is greater than 
	 *     					the actual bit length of aBitString, or the difference 
	 *     					is more that 7 bits.
	 * @return				A new DER bit string encoder object which is left on the 
	 *						cleanup stack.
	 */
	IMPORT_C static CASN1EncBitString* NewLC(const TDesC8& aBitStr, TUint aLengthBits);
	
	/** 
	 * Constructs a new DER bit string encoder from a bit string that 
	 * is not octet-aligned, which means it has unused bits in its last 
	 * octet. The passed string must be in big-endian format.
	 * @param aBitStr		Bit string.
	 * @param aLengthBits	Length in bits of the passed bit string. 
	 *     					The function will panic if aLengthBits is greater than 
	 *     					the actual bit length of aBitString, or the difference 
	 *     					is more that 7 bits.
	 * @return				A new DER bit string encoder object.
	 */
	IMPORT_C static CASN1EncBitString* NewL(const TDesC8& aBitStr, TUint aLengthBits);

	/**
	 * Wraps the passed encoding object into a bit string.
	 * @param aAsnObj	Encoding object to wrap.
	 * @return			A new bit string containing the passed encoding object.
	 */
	IMPORT_C static CASN1EncBitString* NewL(const CASN1EncBase& aAsnObj);

	/**
	 * Wraps the passed encoding object into a bit string.
	 * @param aAsnObj	Encoding object to wrap.
	 * @return			A new bit string containing the passed encoding object
	 *     				on the cleanup stack.
	 */
	IMPORT_C static CASN1EncBitString* NewLC(const CASN1EncBase& aAsnObj);
	
	IMPORT_C ~CASN1EncBitString(); // virtual from base

private:
	CASN1EncBitString();
	void ConstructL(const TDesC8& aBitStr);
	void ConstructL(const TDesC8& aBitStr, TUint aLengthBits);
	void ConstructL(const CASN1EncBase& aAsnObj);

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER();
	void WriteContentsDERL(TDes8& aBuf) const;

private:
	HBufC8* iContents;
	TUint8 iPadding;
	};

/**
 * Class for encoding object identifiers.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncObjectIdentifier : public CASN1EncPrimitive
	{
public:
	/** 
	 * Takes ints in a string, delimited by '.' characters in 
	 * between (not at ends). Takes a deep copy of the info.
	 * @param aStr	OID string.
	 * @return		New ASN.1 OID object on the cleanup stack.
	 */
	IMPORT_C static CASN1EncObjectIdentifier* NewLC(const TDesC& aStr);

	/** 
	 * Takes ints in a string, delimited by '.' characters in 
	 * between (not at ends). Takes a deep copy of the info.
	 * @param aStr	OID string.
	 * @return		New ASN.1 OID object.
	 */
	IMPORT_C static CASN1EncObjectIdentifier* NewL(const TDesC& aStr);

	/** Destructor */
	IMPORT_C ~CASN1EncObjectIdentifier(); // virtual from base

private:
	CASN1EncObjectIdentifier();
	void ConstructL(const TDesC& aStr);

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER();
	void WriteContentsDERL(TDes8& aBuf) const;

private:
	// Data to encode
	RArray<TASN1EncBase128DER> iData;
	TUint8 iFirstOctet;
	};


/** 
 * Class for encoding GeneralisedTime objects. 
 *
 * Doesn't support fractions of seconds or regional time zone offsets.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncGeneralizedTime : public CASN1EncPrimitive
	{
public:
	IMPORT_C static CASN1EncGeneralizedTime* NewLC(const TTime& aTime);
	IMPORT_C static CASN1EncGeneralizedTime* NewL(const TTime& aTime);

private:
	CASN1EncGeneralizedTime(const TTime& aTime);

	// Methods from CASN1EncBase
	void CalculateContentsLengthDER();
	void WriteContentsDERL(TDes8& aBuf) const;

private:
	TDateTime iDateTime;
	};

/** 
 * Class for encapsulation of already encoded data. 
 *
 * Wraps it so that the data could be used in the ASN.1 hierarchy. 
 * It reverse-engineers and stores the encoded data, providing whatever 
 * information is needed to override pure virtual methods of the base 
 * class and write out the DER encoding in its initial form.
 * 
 * @publishedAll
 * @released
 */
class CASN1EncEncoding : public CASN1EncBase
	{
public:
	/** 
	 * Creates a new object from raw DER encoding and places it on the 
	 * cleanup stack.
	 * @param aEncoding	Raw DER encoding.
	 * @return			New wrapper object placed on the cleanup stack.
	 */
	IMPORT_C static CASN1EncEncoding* NewLC(const TDesC8& aEncoding);

	/** 
	 * Creates a new object from raw DER encoding.
	 * @param aEncoding	Raw DER encoding.
	 * @return			New wrapper object.
	 */
	IMPORT_C static CASN1EncEncoding* NewL(const TDesC8& aEncoding);

	IMPORT_C static CASN1EncEncoding* NewLC(const TDesC8& aEncoding, TTagType aType, TASN1Class aClass);

	IMPORT_C ~CASN1EncEncoding();

protected:
	/** 
	 * Protected constructor.
	 * <!--
	 * @param aType		Tag type of the new object
	 * @param aClass	Tag class of the new object.
	 * -->
	 */
	IMPORT_C CASN1EncEncoding();

private:
	/** 
	 * Constructs the wrapper around the passed raw DER encoding. 
	 * Calculates element sizes. Decodes it to get type and length.
	 * @param aEncoding Raw DER encoding.
	 */
	void ConstructL(const TDesC8& aEncoding);

	void ConstructL(const TDesC8& aEncoding, TTagType aType, TASN1Class aClass);

	virtual TBool IsConstructed() const;
	
	/** 
	 * Writes DER content encoding into supplied buffer, which is 
	 * already verified to be big enough.
	 * @param aBuf	Buffer to write to.
	 */
	virtual void WriteContentsDERL(TDes8& aBuf) const;

	/** 
	 * Calculates number of octets in DER content encoding. Sets 
	 * value of the appropriate data member.
	 */
	virtual void CalculateContentsLengthDER();

private:
	HBufC8* iContents; ///< Copy of the supplied DER encoded data (contents only).
	TASN1Class iClass; ///< ASN.1 class of the encoded object.
	TTagType iTag; ///< ASN.1 tag of the encoding.
	};

#endif // __ASN1ENC_H__
