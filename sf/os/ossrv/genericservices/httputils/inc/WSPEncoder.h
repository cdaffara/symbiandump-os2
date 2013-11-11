// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file WSPEncoder.h
 @publishedAll
 @deprecated
*/

#ifndef __WSPENCODER_H__
#define __WSPENCODER_H__

#include <e32base.h>
#include <f32file.h>		// RFs
#include <badesca.h>		// arrays etc.
#include <stringpool.h>

/**
enum CodecPanic
@publishedAll
@deprecated
*/
enum TWspCodecPanic
	{
	/**  Due to failure to call StartValueLength function */
	EWspCodecPanicStartValueLengthNotCalled=0,
	/** Due to failure to call EndValueLength matching a call to StartValueLength */
	EWspCodecPanicEndValueLengthNotCalled,
	/** Due to failure to call StartHeaderL function */
	EWspCodecPanicStartHeaderLNotCalled,
	/** Due to StartHeaderL function being called twice without EndHeaderL */
	EWspCodecPanicStartHeaderCalledTwice,
	/** Due to parameter Token not having the top bit set */
	EWspCodecPanicInvalidToken
	};
 
/** 
This class can be used to encode one header field at a time,
with all its values and parameters.

It has no knowledge of encoding the BNF of a particular header field, but
the functions provided can be used in combination, producing an 8-bit buffer 
containing the encoded header.

Intended usage would be to call a series of functions. The first one being StartHeader,
The final one being EndHeader, which would return a buffer containing 
the complete encoded header field.
eg:
	encoder->StartHeaderL();
	encoder->AddLongIntL();
	encoder->AddTextStringL();
	HBufC8* output = encoder->EndHeaderL();
@publishedAll
@deprecated
*/
class CWspHeaderEncoder : public CBase
	{
public:
	IMPORT_C static CWspHeaderEncoder* NewL();

	IMPORT_C static CWspHeaderEncoder* NewLC();


    IMPORT_C virtual ~CWspHeaderEncoder(); 

	IMPORT_C void StartHeaderL(TUint8 aToken);	


	IMPORT_C void StartHeaderL(const TDesC8& aString);	

	IMPORT_C void StartHeaderL(const RStringF aString);	


	IMPORT_C HBufC8* EndHeaderL();



	IMPORT_C void AddIntegerL(const TUint aInt);

	
	IMPORT_C void AddShortIntL(const TUint8 aValue);

	IMPORT_C void AddShortLengthL(const TUint8 aValue);


	IMPORT_C void AddLongIntL(const TUint32 aValue);
	
	
	IMPORT_C void AddUintVarL(const TUint aInt);

	
	IMPORT_C void AddTextStringL(const RString& aText);

	IMPORT_C void AddTextStringL(const TDesC8& aText);

	IMPORT_C void AddDateL(const TDateTime aDate);

	IMPORT_C void AddTokenL(const TUint8 aToken);

	IMPORT_C void AddTokenTextL(const TDesC8& aTokenText);

	IMPORT_C void AddDataL(const TDesC8& aData);



	IMPORT_C void StartValueLengthL();

	IMPORT_C void EndValueLengthL();

private:

	CWspHeaderEncoder();

	void Init();


	void ConstructL();

private:
	/**
	Array for storing the partial encoded header.
	Each time StartValueLength is called a new array
	element is used. When EndValueLength is called,
	the array is decremented, data from the last 
	element being added to the one before.
	*/
	RPointerArray<CDesC8Array> iArray;

	/**
	Value incremented as the encoded header increases in size.
	Used to allocate the buffer for storing the final	
	encoded header, output when EndHeader is called.
	*/
	TInt iTotalLength;
	};

/** 
Class encapsulating primitive encoding methods which are defined in the WSP standard.
Input will be encoded and returned in an 8 bit buffer.
@publishedAll
@deprecated
*/
class TWspPrimitiveEncoder
	{
public:
	IMPORT_C static TUint8  ShortInt(const TUint8 aValue);

	IMPORT_C static HBufC8* LongIntL(const TUint32 aValue);

	IMPORT_C static HBufC8* UintVarL(const TUint32 aInt);

	IMPORT_C static HBufC8* TextStringL(const RString aText);

	IMPORT_C static HBufC8* TextStringL(const TDesC8& aText);

	IMPORT_C static HBufC8* DateL(const TDateTime aDate);
	};

#endif	// __WSPENCODER_H__
