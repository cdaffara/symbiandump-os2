/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __REQUESTHEADER_H__
#define __REQUESTHEADER_H__

#include <e32std.h>

const TUint KUsbRequestHdrSize = 8; ///< Size of the request header

NONSHARABLE_CLASS(TUsbRequestHdr)		///< Class-specific header structure
	{
public:
	TUint8	iRequestType;	///< Request type
	TUint8	iRequest;		///< Request number
	TUint16 iValue; 		///< Request value
	TUint16 iIndex; 		///< Request index
	TUint16 iLength;		///< Request length

public:
	static TInt Decode(const TDesC8& aBuffer, TUsbRequestHdr& aTarget);
	const TDesC8& Des();
	TBool IsDataResponseRequired() const;

private:
	TBuf8<KUsbRequestHdrSize> iBuffer;
	};

#endif // __REQUESTHEADER_H__
