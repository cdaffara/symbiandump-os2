/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* TestParameter.cpp: implementation of the CTestParameter class.
*
*/


/**
 @file
*/

#include "testparameter.h"


CTestParameter::CTestParameter() : iValid(EFalse) 
{
};


CIntTestParameter* CIntTestParameter::NewL(TDes& aValue)
	{
	CIntTestParameter* self = CIntTestParameter::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

CIntTestParameter* CIntTestParameter::NewLC(TDes& aValue)
	{
	CIntTestParameter* self = new(ELeave) CIntTestParameter();
	CleanupStack::PushL(self);
	self->Construct(aValue);
	return self;
	}

void CIntTestParameter::Construct(TDes& aValue)
	{
	TLex lexValue(aValue);

	if(aValue.Left(2) == _L("0x"))
		{
		// its a hex number 
		TUint hexValue;
		TLex hexLex(aValue.Mid(2));

		if(hexLex.Val(hexValue, EHex)!=KErrNone)
			return;
		// checks if hexLex is at end of string, if not there was garbage in the string
		// so throw it out
		else if(!hexLex.Eos())
			return;

		iValue = STATIC_CAST(TInt,hexValue);
		}
	else if(lexValue.Val(iValue)!=KErrNone)
		return;
		// checks if lexValue is at end of string, if not there was garbage in the string
		// so throw it out
	else if(!lexValue.Eos())
		return;

	iValid = ETrue;
	}

CIntRangeTestParameter* CIntRangeTestParameter::NewL(TDes& aValue)
	{
	CIntRangeTestParameter* self = CIntRangeTestParameter::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

CIntRangeTestParameter* CIntRangeTestParameter::NewLC(TDes& aValue)
	{
	CIntRangeTestParameter* self = new(ELeave) CIntRangeTestParameter();
	CleanupStack::PushL(self);
	self->Construct(aValue);
	return self;
	}

void CIntRangeTestParameter::Construct(TDes& aValue)
	{
	if(aValue.Find(_L("..")) != KErrNotFound)
		{
		TInt pos = aValue.Find(_L(".."));
		TLex startLex(aValue.Left(pos));
		TLex endLex(aValue.Mid(pos+2));

		if(startLex.Val(iStart)!=KErrNone)
			return;
			// checks if startLex is at end of string, if not there was garbage in the string so throw it out
		else if(!startLex.Eos())
			return;

		if(endLex.Val(iFinish)!=KErrNone)
			return;
			// checks if startLex is at end of string, if not there was garbage in the string so throw it out
		else if(!endLex.Eos())
			return;
		// checks if range is doable
		if(iStart > iFinish)
			return;
		}
	else
		return;
	iValid = ETrue;
	}

CRandomTestParameter* CRandomTestParameter::NewL(TDes& aValue)
	{
	CRandomTestParameter* self = CRandomTestParameter::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

CRandomTestParameter* CRandomTestParameter::NewLC(TDes& aValue)
	{
	CRandomTestParameter* self = new(ELeave) CRandomTestParameter();
	CleanupStack::PushL(self);
	self->Construct(aValue);
	return self;
	}


void CRandomTestParameter::Construct(TDes& aValue)
	{
	TLex lexValue(aValue);

	if(lexValue.Val(iInterations)!=KErrNone)
		return;
	else if(!lexValue.Eos())
		// checks if startLex is at end of string, if not there was garbage in the string so throw it out
		return;

	iValid = ETrue;
	}

CStringTestParameter* CStringTestParameter::NewL(TDes& aValue)
	{
	CStringTestParameter* self = CStringTestParameter::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

CStringTestParameter* CStringTestParameter::NewLC(TDes& aValue)
	{
	CStringTestParameter* self = new(ELeave) CStringTestParameter();
	CleanupStack::PushL(self);
	self->Construct(aValue);
	return self;
	}

void CStringTestParameter::Construct(TDes& aValue)
	{
	iValue.Copy(aValue);
	iValid = ETrue;
	}
