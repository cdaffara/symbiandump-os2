/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Demonstrate a container class with data objects
* This class object is shared using the CTEFSharedData template object
*
*/



/**
 @file TestSharedData.h
*/

#if !(defined __TEST_SHARED_DATA_H__)
#define __TEST_SHARED_DATA_H__
#include "e32std.h"

// Constant
const TInt KMaxSharedDataLength = 255;

class CTestSharedData : public CBase

/**
 * Demonstrate a container class with data objects
 * This class object is shared using the CTEFSharedData template object

 @internalComponent
 @test
*/
 	{
public:
	CTestSharedData();
	~CTestSharedData();
	// Initializes the member variable
	inline void Construct();
	// Copies the values to member varible
	inline void SetText(TDesC& aVal);
	// Appends the text to member variable
	inline void AppendText(TDesC& aVal);
	// Reads the text from member varibale
	inline void GetText(TDes& aVal);
	// Returns the length of the text
	inline TInt TextLength(){return User::StringLength(iText);}
private:
	// Member Variable
	TText	iText[KMaxSharedDataLength];
	};

#include <test/testshareddata.inl>
#endif
