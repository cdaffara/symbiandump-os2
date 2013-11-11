/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "fsmarshaller.h"
#include "fsdatatypes.h"
#include "NullStream.h"
#include <asymmetric.h>
#include <bigint.h>
#include <pbedata.h>
#include <s32mem.h>
#include <certificateapps.h>
#include <ccertattributefilter.h>

// Generic externalization stuff ///////////////////////////////////////////////

/**
 * Determine how many bytes are taked up by the externalized representation of
 * an object.
 */
template <class T>
TInt Size(const T& aObject)
	{
	RNullWriteStream stream;
	TRAPD(err, stream << aObject);
	if(err != KErrNone)
		{
		_LIT(KPanicCategory, "InValid Size");
		User::Panic(KPanicCategory,ESerialisationPanic);
		}
	stream.Close();
	return stream.BytesWritten();
	}

/**
 * Externalize an object to a buffer.  Leaves if an error occurs.
 */
template <class T>
void WriteL(const T& aIn, TDes8& aOut)
	{
	RDesWriteStream stream(aOut);
	stream << aIn;
	stream.Close();
	}

/**
 * Externalize an object to a buffer.  In debug mode, it will panics if an error
 * occurs - eg buffer is too short.  In release mode, errors are ignored.
 */
template <class T>
void Write(const T& aIn, TDes8& aOut)
	{
	TRAPD(err, WriteL(aIn, aOut));
	if(err != KErrNone)
		{
		_LIT(KPanicCategory, "Writing Error");
		User::Panic(KPanicCategory,ESerialisationPanic);
		}
	}

/**
 * Implement Externalization selector for RPointerArrays to use a function.
 */
template <class T>
EXTERNALIZE_FUNCTION(RPointerArray<T>)

/**
 * Function to externalize RPointerArrays.
 *
 * Although this is generic, it's currently only instantiated for CKeyInfo.
 */
template <class T>
void ExternalizeL(const RPointerArray<T>& aIn, RWriteStream& aOut)
	{
	TInt count = aIn.Count();
	aOut.WriteInt32L(count);
	for (TInt i = 0 ; i < count ; ++i)
		{
		T* object = aIn[i];
		if (object == NULL)
			{
			User::Leave(KErrArgument);
			}
		aOut << *object;
		}
	}

/**
 * Implement Externalization selector for RArrays to use a function.
 */
template <class T>
EXTERNALIZE_FUNCTION(RArray<T>)

/**
 * Function to externalize RArrays.
 */
template <class T>
void ExternalizeL(const RArray<T>& aIn, RWriteStream& aOut)
	{
	TInt count = aIn.Count();
	aOut.WriteInt32L(count);
	for (TInt i = 0 ; i < count ; ++i)
		{
		aOut << aIn[i];
		}
	}

// No-so-generic internalization stuff /////////////////////////////////////////

/**
 * Internalize an object from a descriptor.
 */
template <class T>
void ReadL(const TDesC8& aIn, T& aOut)
	{
	RDesReadStream stream(aIn);
	stream >> aOut;
	stream.Close();
	}

/**
 * Implement Internalization selector for RArrays to use a function.
 */
template <class T>
inline Internalize::Function Internalization(const RArray<T>*)
	{
	return Internalize::Function();
	}

/**
 * Function to internalize an RArray.
 */
template <class T>
void InternalizeL(RArray<T>& aOut, RReadStream& aIn)
	{
	ASSERT(aOut.Count() == 0);
	TInt count = aIn.ReadInt32L();
	for (TInt i = 0 ; i < count ; ++i)
		{
		T t;
		aIn >> t;
		User::LeaveIfError(aOut.Append(t));
		}
	}

/**
 * Internalize an object from a stream.  Creates an instance by calling the
 * class' NewL(RReadStream&) method.
 */
template <class T>
inline void CreateL(RReadStream& aIn, T*& aOut)
	{
	aOut = T::NewL(aIn);
	}

/**
 * Internalize an object from a descriptor.  
 */
template <class T>
void CreateL(const TDesC8& aIn, T& aOut)
	{
	RDesReadStream stream(aIn);
	CreateL(stream, aOut);
	stream.Close();
	}

/**
 * Internalize an object from a descriptor leaving the result on the cleanup
 * stack.  This is generic, but is only instantiated for RInteger.
 */
template <class T>
void CreateLC(const TDesC8& aIn, T& aOut)
	{
	RDesReadStream stream(aIn);
	CreateLC(stream, aOut);
	stream.Close();
	}

/**
 * Internalize a cryptotokens object from a descriptor.
 */
template <class T>
void CreateL(const TDesC8& aIn, MCTToken& aToken, T& aOut)
	{
	RDesReadStream stream(aIn);
	CreateL(stream, aToken, aOut);
	stream.Close();
	}

/**
 * Internalize a cryptotoken object from a stream.  Creates an instance by
 * calling the class' NewL(RReadStream&, MCTToken&) method.
 */
template <class T>
inline void CreateL(RReadStream& aIn, MCTToken& aToken, T*& aOut)
	{
	aOut = T::NewL(aIn, aToken);
	}

/**
 * Internalize an array of of key info objects from a stream. 
 */
void CreateL(RReadStream& aIn, MCTToken& aToken, MKeyInfoArray& aOut)
	{
	TInt count = aIn.ReadInt32L();
	for (TInt i = 0 ; i < count ; ++i)
		{
		CCTKeyInfo* t;
		CreateL(aIn, aToken, t);
		CleanupReleasePushL(*t);
		User::LeaveIfError(aOut.Append(t));
		CleanupStack::Pop();
		}
	}

// can we combine this with the one above?

/**
 * Internalize an RMPointerArray of cryptotoken objects from a stream.
 */
template <class T>
void CreateL(RReadStream& aIn, MCTToken& aToken, RMPointerArray<T>& aOut)
	{	
	TInt count = aIn.ReadInt32L();
	for (TInt i = 0 ; i < count ; ++i)
		{
		T* t;
		CreateL(aIn, aToken, t);
		CleanupReleasePushL(*t);
		User::LeaveIfError(aOut.Append(t));
		CleanupStack::Pop();
		}
	}

// Serialization for RIntegers /////////////////////////////////////////////////

// This is exported as it is useful itself in the server

/**
 * Implement a CreateL function for internalizing HBufC8s.
 */
inline void CreateL(RReadStream& aIn, HBufC8*& aOut)
	{
	aOut = HBufC8::NewL(aIn, KMaxIntegerSize);
	}

/**
 * Implement a CreateLC function for internalizing RIntegers. 
 */
EXPORT_C void CreateLC(RReadStream& aIn, RInteger& aOut)
	{	
	HBufC8* rBuf;
	CreateL(aIn, rBuf);
	CleanupStack::PushL(rBuf);
	TPtr8 rPtr(rBuf->Des());	
	aOut = RInteger::NewL(rPtr);
	CleanupStack::PopAndDestroy(rBuf);
	CleanupStack::PushL(aOut);
	}

/**
 * Externalize an RInteger.
 */
EXPORT_C void ExternalizeL(const TInteger& aIn, RWriteStream& aOut)
	{
	HBufC8* sBuf = aIn.BufferLC();
	aOut << *sBuf;
	CleanupStack::PopAndDestroy(sBuf);	
	}

// Externalization for signature objects ///////////////////////////////////////

/**
 * Implement Externalization selector for CDSASignature to use a function.
 */
EXTERNALIZE_FUNCTION(CDSASignature)

/**
 * Externalise a DSA signature object.
 */
void ExternalizeL(const CDSASignature& aIn, RWriteStream& aOut)
	{
	aOut << aIn.R();
	aOut << aIn.S();
	}

/**
 * Implement Externalization selector for CRSASignature to use a function.
 */
EXTERNALIZE_FUNCTION(CRSASignature)

/**
 * Externalize an RSA signature object.
 */
void ExternalizeL(const CRSASignature& aIn, RWriteStream& aOut)
	{
	aOut << aIn.S();
	}

// Internalization for signature objects ///////////////////////////////////////

/**
 * Specialise CreateL for CRSASignature.
 */
template <>
void CreateL(RReadStream& aIn, CRSASignature*& aOut)
	{
	RInteger r;
	CreateLC(aIn, r);
	aOut = CRSASignature::NewL(r);
	CleanupStack::Pop(); // r
	}

/**
 * Specialise CreateL for CDSASignature.
 */
template <>
void CreateL(RReadStream& aIn, CDSASignature*& aOut)
	{
	RInteger r;
	CreateLC(aIn, r);
	RInteger s;
	CreateLC(aIn, s);
	aOut = CDSASignature::NewL(r, s);
	CleanupStack::Pop(2); // s, r
	}

// Serialization for DH ojects /////////////////////////////////////////////////

/**
 * Implement Externalization selector for CDHParams to use a function.
 */
EXTERNALIZE_FUNCTION(CDHParams)

/**
 * Externalise a TDHParams object.
 */
void ExternalizeL(const CDHParams& aIn, RWriteStream& aOut)
	{
	aOut << aIn.N();
	aOut << aIn.G();
	}

/**
 * Specialise CreateL for CDHParams.
 */
template <>
void CreateL(RReadStream& aIn, CDHParams*& aOut)
	{
	RInteger n;
	CreateLC(aIn, n);
	RInteger g;
	CreateLC(aIn, g);
	aOut = CDHParams::NewL(n, g);
	CleanupStack::PopAndDestroy(2, &n); // g, n
	}

/**
 * Implement Externalization selector for CDHPublicKey to use a function.
 */
EXTERNALIZE_FUNCTION(CDHPublicKey)

/**
 * Externalise a CDHPublicKey object.
 */
void ExternalizeL(const CDHPublicKey& aIn, RWriteStream& aOut)
	{
	aOut << aIn.N();
	aOut << aIn.G();
	aOut << aIn.X();
	}

/**
 * Specialise CreateL for CDHPublicKey.
 */
template <>
void CreateL(RReadStream& aIn, CDHPublicKey*& aOut)
	{
	RInteger n;
	CreateLC(aIn, n);
	RInteger g;
	CreateLC(aIn, g);
	RInteger X;
	CreateLC(aIn, X);
	aOut = CDHPublicKey::NewL(n, g, X);
	CleanupStack::Pop(3); // X, g, n
	}

// Common //////////////////////////////////////////////////////////////////////

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, RArray<TUid>& aOut)
	{
	::ReadL(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const CKeyInfoBase& aIn)
	{
	return ::Size(aIn);
	}

EXPORT_C void TokenDataMarshaller::Write(const CKeyInfoBase& aIn, TDes8& aOut)
	{
	::Write(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const RArray<TUid>& aIn)
	{
	return ::Size(aIn);
	}

EXPORT_C void TokenDataMarshaller::Write(const RArray<TUid>& aIn, TDes8& aOut)
	{
	::Write(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const MCertInfo& aIn)
	{
	return ::Size(aIn);
	}

EXPORT_C void TokenDataMarshaller::Write(const MCertInfo& aIn, TDes8& aOut)
	{
	::Write(aIn, aOut);
	}

// Client //////////////////////////////////////////////////////////////////////

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, MCTToken& aToken, MKeyInfoArray& aOut)
	{
	::CreateL(aIn, aToken, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, MCTToken& aToken, CCTKeyInfo*& aOut)
	{
	::CreateL(aIn, aToken, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, CDSASignature*& aOut)
	{
	::CreateL(aIn, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, CRSASignature*& aOut)
	{
	::CreateL(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const CDHParams& aIn)
	{
    return Size(aIn.N()) + Size(aIn.G());
	}

EXPORT_C void TokenDataMarshaller::WriteL(const CDHParams& aIn, TDes8& aOut)
	{
	::WriteL(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const CDHPublicKey& aIn)
	{
	return Size(aIn.N()) + Size(aIn.G()) + Size(aIn.X());
	}

EXPORT_C void TokenDataMarshaller::WriteL(const CDHPublicKey& aIn, TDes8& aOut)
	{
	::WriteL(aIn, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, RInteger& aInteger)
	{
	::CreateLC(aIn, aInteger);
	CleanupStack::Pop();
	}
	
EXPORT_C TInt TokenDataMarshaller::Size(const CPBEncryptParms& aIn)
	{
	return ::Size(aIn);
	}
	
EXPORT_C void TokenDataMarshaller::Write(const CPBEncryptParms& aIn,  TDes8& aOut)
	{
	::Write(aIn, aOut);	
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, MCTToken& aToken, RMPointerArray<CCTCertInfo>& aOut)
	{
	::CreateL(aIn, aToken, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, MCTToken& aToken, CCTCertInfo*& aOut)
	{
	::CreateL(aIn, aToken, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, RArray<TCertificateAppInfo>& aOut)
	{
	::ReadL(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const CCertAttributeFilter& aIn)
	{
	return ::Size(aIn);
	}

EXPORT_C void TokenDataMarshaller::WriteL(const CCertAttributeFilter& aIn, TDes8& aOut)
	{
	::WriteL(aIn, aOut);
	}

// Server //////////////////////////////////////////////////////////////////////

EXPORT_C TInt TokenDataMarshaller::Size(const RPointerArray<CKeyInfo>& aIn)
	{
	return ::Size(aIn);
	}
	
EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, CPBEncryptParms*& aOut)
	{
	::CreateL(aIn, aOut);	
	}

EXPORT_C void TokenDataMarshaller::Write(const RPointerArray<CKeyInfo>& aIn, TDes8& aOut)
	{
	::Write(aIn, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, CKeyInfo*& aOut)
	{
	::CreateL(aIn, aOut);
	}

/**
 * Determine the size of an externalized big integer.  This assumes that the
 * size of the buffer returned by TInteger::BufferLC() is the same as the size
 * of the integer as returned by TInteger::ByteCount().  This is done to avoid
 * allocating the buffer just so we can tell how big it is.
 */
EXPORT_C TInt TokenDataMarshaller::Size(const TInteger& aIn)
	{
	// This is an overestimate as the length is encoded with a TCardinality
	return sizeof(TInt32) + aIn.ByteCount();
	}

EXPORT_C TInt TokenDataMarshaller::Size(const CDSASignature& aIn)
	{
	return Size(aIn.R()) + Size(aIn.S());
	}

EXPORT_C void TokenDataMarshaller::WriteL(const CDSASignature& aIn, TDes8& aOut)
	{
	::WriteL(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const CRSASignature& aIn)
	{
	return Size(aIn.S());
	}

EXPORT_C void TokenDataMarshaller::WriteL(const CRSASignature& aIn, TDes8& aOut)
	{
	::WriteL(aIn, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, CDHParams*& aOut)
	{
	::CreateL(aIn, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, CDHPublicKey*& aOut)
	{
	::CreateL(aIn, aOut);
	}

EXPORT_C void TokenDataMarshaller::WriteL(const TInteger& aIn, TDes8& aOut)
	{
	::WriteL(aIn, aOut);
	}

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, CCertInfo*& aOut)
	{
	::CreateL(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const RPointerArray<CCertInfo>& aIn)
	{
	return ::Size(aIn);
	}

EXPORT_C void TokenDataMarshaller::Write(const RPointerArray<CCertInfo>& aIn, TDes8& aOut)
	{
	::Write(aIn, aOut);
	}

EXPORT_C TInt TokenDataMarshaller::Size(const RArray<TCertificateAppInfo>& aIn)
	{
	return ::Size(aIn);
	}
   
EXPORT_C void TokenDataMarshaller::Write(const RArray<TCertificateAppInfo>& aIn, TDes8& aOut)
	{
	::Write(aIn, aOut);
	}   

EXPORT_C void TokenDataMarshaller::ReadL(const TDesC8& aIn, CCertAttributeFilter*& aOut)
	{
	::CreateL(aIn, aOut);
	}
