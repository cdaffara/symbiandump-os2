/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef BDFHARN_H
#define BDFHARN_H

#include <e32std.h>
#include <e32test.h>
#include <bitstd.h>
#include <bitdev.h>

class CBDFHarness : public CBase
/**
@internalComponent
*/
	{
	public:
	static CBDFHarness* NewL(const TDesC &aTitle, const TDesC &aHeading);
	~CBDFHarness();
	void MakeBDFFontL();

	void DecodeBitmap(TInt aWidth, TInt aPaddedWidth, TInt aHeight, const TUint8* aCodedBitmap, TUint8* aBitmap);
	TInt ReadBit();
	TInt ReadBitBack();
	void OutHex(TInt aOneHexDigit);

	private:
	CBDFHarness(const TDesC &aTitle, const TDesC &aHeading);
	void ConstructL();

	RTest iTest;
	RFbsSession* iFbs;
	CFbsBitGc* iGc;
	CFbsScreenDevice* iDev;
	CFbsFont* iStandardFont;
	TInt iPPEM;
	static TPtrC iFacename;
	static TInt  iPixelSize;
	static TPtrC iFilename;
	static TPtrC iUid;
	static TFontPosture iPosture;
	static TFontStrokeWeight iStrokeWeight;
	static TFontPrintPosition iPrintPosition;

	public:
	const TUint8* iBitMap;
	TInt iBitNum;
	};

#endif // !BDFHARN_H
