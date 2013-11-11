// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains declaration for QuotedPrintable codec
// 
//

/**
 @file
 @publishedPartner
 @released
*/
#ifndef QPCODEC_H
#define QPCODEC_H

/**
	This class is used for QuotedPrintable based decoding .
	This class provides a method  for decoding
	@publishedPartner
	@released
	*/
NONSHARABLE_CLASS (QuotedPrintableCodec) {
public:
	IMPORT_C static TInt Decode(const TDesC8& aSrcString, TDes8& aDestString);
	static TBool IsDigit( TChar aChar );
};
#endif      // QPCODEC_H
