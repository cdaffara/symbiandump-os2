/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TestPicture.h test picture.
*
*/


#include <gdi.h>

class CTestPicture : public CPicture
	{
public:
	CTestPicture();
	void Draw(CGraphicsContext&, const TPoint&,
		const TRect&, MGraphicsDeviceMap*) const;
	void ExternalizeL(RWriteStream&) const;
	void GetOriginalSizeInTwips(TSize& aSize) const;
	TBool LineBreakPossible(TUint,
		TBool aBeforePicture, TBool aHaveSpaces) const;
	void SetLineBreaksAllowed(TBool aBeforeWithoutSpaces,
		TBool aBeforeWithSpaces, TBool aAfterWithoutSpaces,
		TBool aAfterWithSpaces);

	void SetSizeInTwips(TSize aSize);
	void SetSizeInPixels(TSize aSize, const MGraphicsDeviceMap& aMap);

private:
	TBool iLineBreaksAllowed[4];
	TSize iSizeInTwips;
	};
