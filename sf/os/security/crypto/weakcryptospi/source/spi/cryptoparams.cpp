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
* Generic crypto parameter implementation
* Generic crypto parameter implementation
*
*/


/**
 @file
*/

#include <cryptospi/cryptoparams.h>

using namespace CryptoSpi;

//
//Implementation of the generic crypto parameter
//
CCryptoParam::CCryptoParam(TParamType aType, TUid aUid)
	: iType(aType),
	  iUid(aUid)
	{
	}

EXPORT_C CCryptoParam::~CCryptoParam()
	{
		
	}

EXPORT_C TInt CCryptoParam::Type() const
	{
	return iType;
	}

EXPORT_C TUid CCryptoParam::Uid() const
	{
	return iUid;
	}

//
// Implementation of Descriptor parameters
//
EXPORT_C CCryptoDesC8Param* CCryptoDesC8Param::NewL(const TDesC8& aValue, TUid aUid)
	{
	CCryptoDesC8Param* self=NewLC(aValue, aUid);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CCryptoDesC8Param* CCryptoDesC8Param::NewLC(const TDesC8& aValue, TUid aUid)
	{
	CCryptoDesC8Param* self=new(ELeave) CCryptoDesC8Param(aUid);
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

EXPORT_C const TDesC8& CCryptoDesC8Param::Value() const
	{
	return *iValue;
	}

EXPORT_C CCryptoDesC8Param::~CCryptoDesC8Param()
	{
	if (iValue)
		{
		iValue->Des().FillZ();
		}
	delete iValue;
	}

CCryptoDesC8Param::CCryptoDesC8Param(TUid aUid)
	: CCryptoParam(EDesC8, aUid)
	{
	}
	
void CCryptoDesC8Param::ConstructL(const TDesC8& aValue)
	{
	iValue=aValue.AllocL();
	}


//
// Implementation of Descriptor parameters
//
EXPORT_C CCryptoDesC16Param* CCryptoDesC16Param::NewL(const TDesC16& aValue, TUid aUid)
	{
	CCryptoDesC16Param* self=NewLC(aValue, aUid);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CCryptoDesC16Param* CCryptoDesC16Param::NewLC(const TDesC16& aValue, TUid aUid)
	{
	CCryptoDesC16Param* self=new(ELeave) CCryptoDesC16Param(aUid);
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

EXPORT_C const TDesC16& CCryptoDesC16Param::Value() const
	{
	return *iValue;
	}

EXPORT_C CCryptoDesC16Param::~CCryptoDesC16Param()
	{
	if (iValue)
		{
		iValue->Des().FillZ();
		}
	delete iValue;
	}

CCryptoDesC16Param::CCryptoDesC16Param(TUid aUid)
	: CCryptoParam(EDesC16, aUid)
	{
	}
	
void CCryptoDesC16Param::ConstructL(const TDesC16& aValue)
	{
	iValue=aValue.AllocL();
	}

//
// Implementation of int parameters
//
EXPORT_C CCryptoIntParam* CCryptoIntParam::NewL(TInt aValue, TUid aUid)
	{
	return new(ELeave) CCryptoIntParam(aValue, aUid);
	}

EXPORT_C CCryptoIntParam* CCryptoIntParam::NewLC(TInt aValue, TUid aUid)
	{
	CCryptoIntParam* self=new(ELeave) CCryptoIntParam(aValue, aUid);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C TInt CCryptoIntParam::Value() const
	{
	return iValue;
	}

EXPORT_C CCryptoIntParam::~CCryptoIntParam()
	{
	iValue = 0;
	}

CCryptoIntParam::CCryptoIntParam(TInt aValue, TUid aUid)
	: CCryptoParam(EInt, aUid),
	  iValue(aValue)
	{
		
	}

//	
// Implementation of BigInt parameters
//
EXPORT_C CCryptoBigIntParam* CCryptoBigIntParam::NewL(const TInteger& aValue, TUid aUid)
	{
	CCryptoBigIntParam* self = NewLC(aValue, aUid);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CCryptoBigIntParam* CCryptoBigIntParam::NewLC(const TInteger& aValue, TUid aUid)
	{
	CCryptoBigIntParam* self = new(ELeave) CCryptoBigIntParam(aUid);
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

EXPORT_C const TInteger& CCryptoBigIntParam::Value() const
	{
	return iValue;
	}

EXPORT_C CCryptoBigIntParam::~CCryptoBigIntParam()
	{
	// Secure delete, BigInts used for RSA/DSA modulus and exponent
	iValue.Close();
	}

CCryptoBigIntParam::CCryptoBigIntParam(TUid aUid)
	: CCryptoParam(EBigInt, aUid)
	{
	}
	
void CCryptoBigIntParam::ConstructL(const TInteger& aValue)
	{
	iValue = RInteger::NewL(aValue);
	}

/*
 * CCryptoParams implementation
 */
EXPORT_C CCryptoParams* CCryptoParams::NewL(void)
	{
	CCryptoParams* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CCryptoParams* CCryptoParams::NewLC(void)
	{
	CCryptoParams* self = new(ELeave) CCryptoParams();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CCryptoParams::CCryptoParams()
	{
	}

void CCryptoParams::ConstructL(void)
	{
	}

EXPORT_C CCryptoParams::~CCryptoParams()
	{
	// delete all contained parameters
	iParams.ResetAndDestroy();
	}

EXPORT_C void CCryptoParams::AddL(const TInteger& aParam, TUid aUid)
	{
	CCryptoBigIntParam* param = CCryptoBigIntParam::NewLC(aParam, aUid);
	iParams.AppendL(param);
	CleanupStack::Pop(param);
	}

EXPORT_C void CCryptoParams::AddL(const TInt aParam, TUid aUid)
	{
	CCryptoIntParam* param = CCryptoIntParam::NewLC(aParam, aUid);
	iParams.AppendL(param);
	CleanupStack::Pop(param);
	}

EXPORT_C void CCryptoParams::AddL(const TDesC8& aParam, TUid aUid)
	{
	CCryptoDesC8Param* param = CCryptoDesC8Param::NewLC(aParam, aUid);
	iParams.AppendL(param);
	CleanupStack::Pop(param);
	}

EXPORT_C void CCryptoParams::AddL(const TDesC16& aParam, TUid aUid)
	{
	CCryptoDesC16Param* param = CCryptoDesC16Param::NewLC(aParam, aUid);
	iParams.AppendL(param);
	CleanupStack::Pop(param);
	}

EXPORT_C const TInteger& CCryptoParams::GetBigIntL(TUid aUid) const
	{
	TInteger* paramValue = NULL;
	CCryptoParam* param = GetCryptoParamL(aUid);
	if (param->Type() == CCryptoParam::EBigInt)
		{
		const CCryptoBigIntParam* typedParam = static_cast<const CCryptoBigIntParam*>(param);
		paramValue = const_cast<TInteger*>(&typedParam->Value());
		}
	else
		{
		User::Leave(KErrArgument);
		}
		return *paramValue;
	}

EXPORT_C TInt CCryptoParams::GetTIntL(TUid aUid) const
	{
	TInt paramValue = 0;
	CCryptoParam* param = GetCryptoParamL(aUid);
	if (param->Type() == CCryptoParam::EInt)
		{
		const CCryptoIntParam* typedParam = static_cast<const CCryptoIntParam*>(param);
		paramValue = typedParam->Value();
		}
	else
		{
		User::Leave(KErrArgument);
		}
		return paramValue;
	}

EXPORT_C const TDesC8& CCryptoParams::GetTDesC8L(TUid aUid) const
	{
	TDesC8* paramValue = NULL;
	CCryptoParam* param = GetCryptoParamL(aUid);
	if (param->Type() == CCryptoParam::EDesC8)
		{
		const CCryptoDesC8Param* typedParam = static_cast<const CCryptoDesC8Param*>(param);
		paramValue = const_cast<TDesC8*>(&typedParam->Value());
		}
	else
		{
		User::Leave(KErrArgument);
		}
		return *paramValue;
	}

EXPORT_C const TDesC16& CCryptoParams::GetTDesC16L(TUid aUid) const
	{
	TDesC16* paramValue = NULL;
	CCryptoParam* param = GetCryptoParamL(aUid);
	if (param->Type() == CCryptoParam::EDesC16)
		{
		const CCryptoDesC16Param* typedParam = static_cast<const CCryptoDesC16Param*>(param);
		paramValue = const_cast<TDesC16*>(&typedParam->Value());
		}
	else
		{
		User::Leave(KErrArgument);
		}
		return *paramValue;
	}

EXPORT_C TBool CCryptoParams::IsPresent(TUid aUid) const
	{
	TBool ret = EFalse;
	if (GetCryptoParam(aUid))
		{
		ret = ETrue;
		}
	return ret;
	}

CCryptoParam* CCryptoParams::GetCryptoParam(TUid aUid) const
	{
	CCryptoParam* paramPtr = NULL;
	TInt count = iParams.Count();
	for (TInt i = 0 ;i < count; i++)
		{
		if (iParams[i]->Uid() == aUid)
			{
			paramPtr = iParams[i];
			break;
			}
		}
	return paramPtr;
	}
	
CCryptoParam* CCryptoParams::GetCryptoParamL(TUid aUid) const
	{
	CCryptoParam* paramPtr = GetCryptoParam(aUid);
	// leave if requested uid was not found
	if (!paramPtr)
		{
		User::Leave(KErrArgument);
		}
	return paramPtr;
	}

EXPORT_C CCryptoParams& CCryptoParams::CopyL(const CCryptoParams& aParams)
	{
	iParams.Close();
	TUint count = aParams.iParams.Count();
	for (TUint num = 0; num < count; num++)
		{
		CCryptoParam* item = aParams.iParams[num];

		// Stop armv5 compiler warning about init through switch statement
		CCryptoBigIntParam *b = 0;
		CCryptoDesC8Param *d = 0;
		CCryptoDesC16Param *d16 = 0;
		CCryptoIntParam *i = 0;

		// For each type of cryptoparam, duplicate it, and append to RPtrArray
		switch (item->Type())
			{
			case CCryptoParam::EBigInt:
				b = CCryptoBigIntParam::NewL(((CCryptoBigIntParam*) item)->Value(), ((CCryptoBigIntParam*) item)->Uid());
				CleanupStack::PushL(b);
				iParams.AppendL(b);
				CleanupStack::Pop(b);
				break;
				
			case CCryptoParam::EInt:
				i = CCryptoIntParam::NewL(((CCryptoIntParam*) item)->Value(), ((CCryptoBigIntParam*) item)->Uid());
				CleanupStack::PushL(i);
				iParams.AppendL(i);
				CleanupStack::Pop(i);
				break;
				
			case CCryptoParam::EDesC8:
				d = CCryptoDesC8Param::NewL(((CCryptoDesC8Param*) item)->Value(), ((CCryptoBigIntParam*) item)->Uid());
				CleanupStack::PushL(d);
				iParams.AppendL(d);
				CleanupStack::Pop(d);
				break;
				
			case CCryptoParam::EDesC16:
				d16 = CCryptoDesC16Param::NewL(((CCryptoDesC16Param*) item)->Value(), ((CCryptoBigIntParam*) item)->Uid());
				CleanupStack::PushL(d16);
				iParams.AppendL(d16);
				CleanupStack::Pop(d16);
				break;
				
			default:
				break;
			}
		}
		return *this;
	}

EXPORT_C TInt CCryptoParams::Count(void) const
	{
	return iParams.Count();
	}

EXPORT_C const RPointerArray<CCryptoParam>& CCryptoParams::GetParams() const
	{
	return iParams;
	}

