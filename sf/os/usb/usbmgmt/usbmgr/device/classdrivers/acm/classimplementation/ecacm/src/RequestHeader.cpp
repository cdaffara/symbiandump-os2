/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#include "RequestHeader.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "RequestHeaderTraces.h"
#endif


const TDesC8& TUsbRequestHdr::Des()
/**
 * This function packs the TUsbRequestHdr class into a descriptor with the
 * correct byte alignment for transmission on the USB bus.
 *
 * @return Correctly-aligned buffer. NB The buffer returned is a member of 
 * this class and has the same lifetime.
 */
	{
	OstTraceFunctionEntry0( TUSBREQUESTHDR_DES_ENTRY );
	iBuffer.SetLength(KUsbRequestHdrSize);

	iBuffer[0] = iRequestType;
	iBuffer[1] = iRequest;
	iBuffer[2] = static_cast<TUint8>(iValue & 0x00ff);
	iBuffer[3] = static_cast<TUint8>((iValue & 0xff00) >> 8);
	iBuffer[4] = static_cast<TUint8>(iIndex & 0x00ff);
	iBuffer[5] = static_cast<TUint8>((iIndex & 0xff00) >> 8);
	iBuffer[6] = static_cast<TUint8>(iLength & 0x00ff);
	iBuffer[7] = static_cast<TUint8>((iLength & 0xff00) >> 8);

	OstTraceFunctionExit0( TUSBREQUESTHDR_DES_EXIT );
	return iBuffer;
	}

TInt TUsbRequestHdr::Decode(const TDesC8& aBuffer, TUsbRequestHdr& aTarget)
/**
 * This function unpacks into the TUsbRequestHdr class from a descriptor with 
 * the alignment that would be introduced on the USB bus.
 *
 * @param aBuffer Input buffer
 * @param aTarget Unpacked header.
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( TUSBREQUESTHDR_DECODE_ENTRY );
	
	if (aBuffer.Length() < static_cast<TInt>(KUsbRequestHdrSize))
		{
		OstTraceFunctionExit0( TUSBREQUESTHDR_DECODE_EXIT );
		return KErrGeneral;
		}

	aTarget.iRequestType = aBuffer[0];
	aTarget.iRequest = aBuffer[1];
	aTarget.iValue	 = static_cast<TUint16>(aBuffer[2] + (aBuffer[3] << 8));
	aTarget.iIndex	 = static_cast<TUint16>(aBuffer[4] + (aBuffer[5] << 8));
	aTarget.iLength  = static_cast<TUint16>(aBuffer[6] + (aBuffer[7] << 8));
	OstTraceFunctionExit0( TUSBREQUESTHDR_DECODE_EXIT_DUP1 );
	return KErrNone;
	}

TBool TUsbRequestHdr::IsDataResponseRequired() const
/**
 * This function determines whether data is required by the host in response 
 * to a message header.
 * @return TBool	Flag indicating whether a data response required.
 */
	{
	OstTraceFunctionEntry0( TUSBREQUESTHDR_ISDATARESPONSEREQUIRED_ENTRY );
	return (iRequestType & 0x80) ? ETrue : EFalse;
	}

//
// End of file
