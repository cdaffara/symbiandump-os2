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


#ifndef __FSMARSHALLER_H__
#define __FSMARSHALLER_H__

#include <e32std.h>
#include <s32strm.h>
#include <ct/rmpointerarray.h>

/**
 * @file
 * @internalComponent
 *
 * Marshalling and unmarshalling utils for the filetokens server This is used by
 * both client and server dlls.
 */

class MCTToken;
class CKeyInfoBase;
class CCTKeyInfo;
class CKeyInfo;
class MKeyInfoArray;
class CDSASignature;
class CRSASignature;
class CPBEncryptParms;
struct TDHParams;
class CDHParams;
class CDHPublicKey;
class TInteger;
class RInteger;
class MCertInfo;
class CCTCertInfo;
class CCertInfo;
class TCertificateAppInfo;
class CCertAttributeFilter;

/// A macro that marks a type as externalized by a global function
#define EXTERNALIZE_FUNCTION(TYPE) \
inline Externalize::Function Externalization(const TYPE*) \
	{ \
	return Externalize::Function(); \
	}

// Serialization for bigints, used in server
 
/**
 * Maximum size of integer to decode - keys are limited to 2048 bits, so nothing
 * we pass around should be bigger than this.
 */
const TInt KMaxIntegerSize = 256;

/**
 * Maximum size in bytes of serialised representations, given the limit on key
 * size.
 */
const TInt KMaxDSASignatureSize = 48;	// Two 160 bit integers
const TInt KMaxRSASignatureSize = 516;	// One 4096 bit integer
const TInt KMaxRSAPlaintextSize = 516;	// One 4096 bit integer
const TInt KMaxDHAgreedKeySize = 516;	// One 4096 bit integer

// enum for handling panics

enum KTokenMarshallerPanics
	{ 
	ESerialisationPanic
	};


EXTERNALIZE_FUNCTION(TInteger)
IMPORT_C void ExternalizeL(const TInteger& aIn, RWriteStream& aOut);
IMPORT_C void CreateLC(RReadStream& aIn, RInteger& aOut);

/**	Marshals data over the IPC boundary between filetokens client and server. */
class TokenDataMarshaller
	{
	// Common
public:
	IMPORT_C static void ReadL(const TDesC8& aIn, RArray<TUid>& aOut);

	IMPORT_C static TInt Size(const CKeyInfoBase& aIn);
	IMPORT_C static void Write(const CKeyInfoBase& aIn, TDes8& aOut);

	IMPORT_C static TInt Size(const RArray<TUid>& aIn);
	IMPORT_C static void Write(const RArray<TUid>& aIn, TDes8& aOut);

	IMPORT_C static TInt Size(const MCertInfo& aIn);
	IMPORT_C static void Write(const MCertInfo& aIn, TDes8& aOut);

	// Used by client 
public:	
	IMPORT_C static void ReadL(const TDesC8& aIn, MCTToken& aToken, MKeyInfoArray& aOut);
	IMPORT_C static void ReadL(const TDesC8& aIn, MCTToken& aToken, CCTKeyInfo*& aOut);

	IMPORT_C static void ReadL(const TDesC8& aIn, CDSASignature*& aOut);
	IMPORT_C static void ReadL(const TDesC8& aIn, CRSASignature*& aOut);

	IMPORT_C static TInt Size(const CPBEncryptParms& aIn);	
	IMPORT_C static void Write(const CPBEncryptParms& aIn,  TDes8& aOut);

	IMPORT_C static TInt Size(const CDHParams& aIn);
	IMPORT_C static void WriteL(const CDHParams& aIn, TDes8& aOut);

	IMPORT_C static TInt Size(const CDHPublicKey& aIn);
	IMPORT_C static void WriteL(const CDHPublicKey& aIn, TDes8& aOut);

	IMPORT_C static void ReadL(const TDesC8& aIn, RInteger& aInteger);

	IMPORT_C static void ReadL(const TDesC8& aIn, MCTToken& aToken, RMPointerArray<CCTCertInfo>& aOut);
	IMPORT_C static void ReadL(const TDesC8& aIn, MCTToken& aToken, CCTCertInfo*& aOut);

	IMPORT_C static void ReadL(const TDesC8& aIn, RArray<TCertificateAppInfo>& aOut);

	IMPORT_C static TInt Size(const CCertAttributeFilter& aIn);
	IMPORT_C static void WriteL(const CCertAttributeFilter& aIn, TDes8& aOut);
	
	// Used by server
public:
	IMPORT_C static void ReadL(const TDesC8& aIn, CKeyInfo*& aOut);

	IMPORT_C static void ReadL(const TDesC8& aIn, CPBEncryptParms*& aOut);

	IMPORT_C static TInt Size(const RPointerArray<CKeyInfo>& aIn);
	IMPORT_C static void Write(const RPointerArray<CKeyInfo>& aIn, TDes8& aOut);
	
	IMPORT_C static TInt Size(const CDSASignature& aIn);
	IMPORT_C static void WriteL(const CDSASignature& aIn, TDes8& aOut);

	IMPORT_C static TInt Size(const CRSASignature& aIn);
	IMPORT_C static void WriteL(const CRSASignature& aIn, TDes8& aOut);

	IMPORT_C static void ReadL(const TDesC8& aIn, CDHParams*& aOut);
	IMPORT_C static void ReadL(const TDesC8& aIn, CDHPublicKey*& aOut);
	
	IMPORT_C static TInt Size(const TInteger& aIn);
	IMPORT_C static void WriteL(const TInteger& aIn, TDes8& aOut);

	IMPORT_C static void ReadL(const TDesC8& aIn, CCertInfo*& aOut);

	IMPORT_C static TInt Size(const RPointerArray<CCertInfo>& aIn);
	IMPORT_C static void Write(const RPointerArray<CCertInfo>& aIn, TDes8& aOut);

	IMPORT_C static TInt Size(const RArray<TCertificateAppInfo>& aIn);   
	IMPORT_C static void Write(const RArray<TCertificateAppInfo>& aIn, TDes8& aOut);

	IMPORT_C static void ReadL(const TDesC8& aIn, CCertAttributeFilter*& aOut);
	};

#endif
