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
* CorruptionTest.CPP
*
*/


#include "SyntaxResult.h"


CSyntaxResult* CSyntaxResult::NewL(void)
	{
	CSyntaxResult* self = CSyntaxResult::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CSyntaxResult* CSyntaxResult::NewLC(void)
	{
	CSyntaxResult* self = new(ELeave) CSyntaxResult();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSyntaxResult::CSyntaxResult(void)  
	{
	}

CSyntaxResult::~CSyntaxResult()

	{
	iResult = 0;
	};

void CSyntaxResult::ConstructL()  
	{
	}

void CSyntaxResult::SetFilename(const TDesC8 &aFilename)
	{
	iFilename.Copy(aFilename);
	};

void CSyntaxResult::SetResult(const TDesC8 &aResult)
	{
	if(aResult.CompareF(_L8("ErrArgument"))==0)
		iResult = KErrArgument;
	else if(aResult.CompareF(_L8("ErrNotSupported"))==0)
		iResult = KErrNotSupported;
	else 
		iResult = KErrNone;
	}

