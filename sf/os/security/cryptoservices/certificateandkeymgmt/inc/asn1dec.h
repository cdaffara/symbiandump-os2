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
* ASN1 decoder
*
*/


/**
 @file 
 @publishedAll
 @released
*/
 
#ifndef __ASN1DEC_H__
#define __ASN1DEC_H__

#include <e32std.h>
#include <e32base.h>
#include <asn1cons.h>

class RInteger;


class TASN1DecGeneric
	{
public:
	IMPORT_C explicit TASN1DecGeneric(const TDesC8& aSource);
	IMPORT_C void InitL();
	IMPORT_C TPtrC8 Encoding() const;
	IMPORT_C TInt LengthDER() const;
	IMPORT_C TPtrC8 GetContentDER(void) const;
	IMPORT_C TInt LengthDERContent(void) const;
	IMPORT_C TInt LengthDERHeader() const;
	IMPORT_C TTagType Tag() const;
	IMPORT_C TASN1Class Class() const;
private:
	TPtrC8 iEncoding;
	TInt iStartOfContents;
	TInt iLength;
	TTagType iExplicitTag;
	TASN1Class iClass;
	};

class TASN1DecString
	{
public:
protected:
	IMPORT_C TASN1DecString(const TTagType aTag);
	virtual TInt CheckValid(const TDesC8& aSource)=0;
	};

class TASN1DecBoolean
	{
public:
	IMPORT_C TASN1DecBoolean();
	IMPORT_C TBool DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C TBool DecodeDERL(const TASN1DecGeneric& Source);
private:
	};

class TASN1DecNull
	{
public:
	IMPORT_C TASN1DecNull();
	IMPORT_C void DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C void DecodeDERL(const TASN1DecGeneric& aSource);
	};

class TASN1DecEnumerated
	{
public:
	IMPORT_C TASN1DecEnumerated(void);
	};

class TASN1DecInteger
	{
public:
	IMPORT_C TASN1DecInteger();
	IMPORT_C TInt DecodeDERShortL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C TInt DecodeDERShortL(const TASN1DecGeneric& aSource);
	IMPORT_C RInteger DecodeDERLongL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C RInteger DecodeDERLongL(const TASN1DecGeneric& aSource);
	};

const TInt KNumberOfIDs=15;

class TASN1DecObjectIdentifier
	{
public:
	IMPORT_C TASN1DecObjectIdentifier();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC* DecodeDERL(const TASN1DecGeneric& aSource);
private:
	TInt DecodeContentsL(TFixedArray<TInt, KNumberOfIDs>& aOID, const TDesC8& aSource);
	};

class TASN1DecSet
	{
public:
	IMPORT_C TASN1DecSet();
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* NewDERLC(const TDesC8& aSource);
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* DecodeDERLC(const TDesC8& aSource,TInt& aPos);
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* DecodeDERLC(const TASN1DecGeneric& aSource);
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* DecodeDERLC(const TDesC8& aSource,TInt& aPos, TInt aMin, TInt aMax);
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* DecodeDERLC(const TASN1DecGeneric& aSource, TInt aMin, TInt aMax);
protected:
	/** @internalComponent */
	void DecodeContentsL(const TDesC8& aSource, TInt& aPos, CArrayPtrFlat<TASN1DecGeneric>& aDest);
	/** @internalComponent */
	static void CleanupSet(TAny* aArray);	
	};

class TASN1DecGraphicString
	{
public:
	IMPORT_C TASN1DecGraphicString();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

class TASN1DecVisibleString
	{
public:
	IMPORT_C TASN1DecVisibleString(void);
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

class TASN1DecGeneralizedTime
	{
public:
	IMPORT_C TASN1DecGeneralizedTime();
	IMPORT_C TTime DecodeDERL(const TDesC8& aSource, TInt& aPos);
	IMPORT_C TTime DecodeDERL(const TASN1DecGeneric& aGen);
private:
	TTime GetTimeL(const TDesC8& aSource);
	};

class TASN1DecUTCTime
	{
public:
	IMPORT_C TASN1DecUTCTime();
	IMPORT_C TTime DecodeDERL(const TASN1DecGeneric& aGen);
	IMPORT_C TTime DecodeDERL(const TDesC8& aSource, TInt& aPos);
private:
	TTime GetTimeL(const TDesC8& aSource);
	};

class TASN1DecBitString
	{
public:
	IMPORT_C TASN1DecBitString();
	IMPORT_C HBufC8* DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC8* DecodeDERL(const TASN1DecGeneric& aSource);
	//we need the next 2 functions because many standards encode things as bit
	//strings
	IMPORT_C HBufC8* ExtractOctetStringL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC8* ExtractOctetStringL(const TASN1DecGeneric& aSource);
	};

class TASN1DecIA5String
	{
public:
	IMPORT_C TASN1DecIA5String();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC* DecodeDERL(const TASN1DecGeneric& aSource);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};


class TASN1DecUTF8String
	{
public:
	IMPORT_C TASN1DecUTF8String();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC* DecodeDERL(const TASN1DecGeneric& aSource);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

class TASN1DecGeneralString
	{
public:
	IMPORT_C TASN1DecGeneralString();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

class TASN1DecOctetString
	{
public:
	IMPORT_C TASN1DecOctetString();
	IMPORT_C HBufC8* DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC8* DecodeDERL(const TASN1DecGeneric& aSource);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
	};

class TASN1DecNumericString
	{
public:
	IMPORT_C TASN1DecNumericString();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
protected:
	/** @internalComponent */
	TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

class TASN1DecVideotexString
	{
public:
	IMPORT_C TASN1DecVideotexString();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

class TASN1DecPrintableString
	{
public:
	IMPORT_C TASN1DecPrintableString();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC* DecodeDERL(const TASN1DecGeneric& aSource);
protected:
	/** @internalComponent */
	TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

/**
 * Class used for decoding Teletex strings.
 * The TeletexString type is not fully supported by this library.
 * Instead the decode methods perform a direct conversion from 8 to 16bits by adding 
 * null characters in the second byte of each character. This will work as expected 
 * for cases where the string contains ASCII data.
 * As this is non-destructive, the original data can be retrieved by performing 
 * a Copy to an 8-bit descriptor.
 */
class TASN1DecTeletexString
	{
public:
	IMPORT_C TASN1DecTeletexString();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC* DecodeDERL(const TASN1DecGeneric& Source);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

/**
Class used for decoding ASN.1 BMP strings.
 */
class TASN1DecBMPString
	{
public:
	IMPORT_C TASN1DecBMPString();
	IMPORT_C HBufC* DecodeDERL(const TDesC8& aSource,TInt& aPos);
	IMPORT_C HBufC* DecodeDERL(const TASN1DecGeneric& aSource);
protected:
	IMPORT_C TInt CheckValid(const TDesC8& aSource);
private:
	HBufC* DecodeContentsL(const TDesC8& aSource);
	};

class TASN1DecSequence
	{
public:
	IMPORT_C TASN1DecSequence();
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* DecodeDERLC(const TDesC8& aSource,TInt& aPos);
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* DecodeDERLC(const TASN1DecGeneric& aSource);
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* DecodeDERLC(const TDesC8& aSource,TInt& aPos, TInt aMin, TInt aMax);
	IMPORT_C CArrayPtrFlat<TASN1DecGeneric>* DecodeDERLC(const TASN1DecGeneric& aSource, TInt aMin, TInt aMax);
protected:
	/** @internalComponent */
	void DecodeContentsL(const TDesC8& aSource, CArrayPtrFlat<TASN1DecGeneric>& aDest);
	/** @internalComponent */
	static void CleanupSequence(TAny* aArray);
	};

#endif
