/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


/**
 @file
*/

#include <e32base.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <ct.h>
#include "MTestInterface.h"

_LIT(KToken5Info, "Test Token 5");
_LIT(KToken6Info, "Test Token 6");


class CTestObject : public MTestObject
	{
public:
	CTestObject(MCTToken& aToken) : MTestObject(aToken), iToken(aToken) {};

	virtual const TDesC& Label() const;
	virtual MCTToken& Token() const;
	virtual TUid Type() const;
	virtual TCTTokenObjectHandle Handle() const;

private:
	MCTToken& iToken;
	};

class CTestInterface : public MTestInterface
	{
public:
	CTestInterface(const TDesC& aLabel, MCTToken& aToken)
			: iLabel(aLabel), iToken(aToken) {};

	virtual MCTToken& Token();

	virtual const TDesC& Label();

	MTestObject* ObjectL();

private:
	const TDesC& iLabel;
	MCTToken& iToken;
	};

class CTestToken : public CBase, public MCTToken
	{
public:
	CTestToken(const TDesC& aLabel, MCTTokenType& aTokenType);

	virtual void DoGetInterface(TUid aRequiredInterface,
							  MCTTokenInterface*& aReturnedInterface, 
							  TRequestStatus& aStatus);
	virtual TBool DoCancelGetInterface();

	virtual const TDesC& Label();

	virtual MCTTokenType& TokenType();

	virtual TCTTokenHandle Handle();
	virtual const TDesC& Information(TTokenInformation aRequiredInformation);
protected:
	virtual TInt& ReferenceCount();

private:
	MCTTokenType& iTokenType;
	const TDesC& iLabel;
	TInt iCount;
	TBool iAsyncGetInterfaceRunning;
	};

CTestToken::CTestToken(const TDesC& aLabel, MCTTokenType& aTokenType)
		: iTokenType(aTokenType), iLabel(aLabel)
	{
	}

const TDesC& CTestToken::Label()
	{
	return iLabel;
	}

const TDesC& CTestToken::Information(TTokenInformation aRequiredInformation)
	{
	_LIT(KVersion, "The Ultimate Version");
	_LIT(KSerial, "Serial No. 1");
	_LIT(KManufacturer, "ACME Corporation");
	switch (aRequiredInformation)
		{
	case EVersion:
		return KVersion;
	case ESerialNo:
		return KSerial;
	case EManufacturer:
	default:
		return KManufacturer;
		}
	}

MCTTokenType& CTestToken::TokenType()
	{
	return iTokenType;
	}

MCTToken& CTestInterface::Token()
	{
	return iToken;
	}
const TDesC& CTestInterface::Label()
	{
	return iLabel;
	}

MTestObject* CTestInterface::ObjectL()
	{
	return new (ELeave) CTestObject(Token());
	}

const TDesC& CTestObject::Label() const
	{
	return Token().Label();
	}

MCTToken& CTestObject::Token() const
	{
	return iToken;
	}

TUid CTestObject::Type() const
	{
	TUid uid = {0};
	return uid;
	}

TCTTokenObjectHandle CTestObject::Handle() const
	{
	return TCTTokenObjectHandle(Token().Handle(), 1);
	}

TCTTokenHandle CTestToken::Handle()
	{
	return TCTTokenHandle(TokenType().Type(), 1);
	}

void CTestToken::DoGetInterface(TUid aRequiredInterface,
							  MCTTokenInterface*& aReturnedInterface, 
							  TRequestStatus& aStatus)
	{
	// InterfaceC is returned. InterfaceB (actually anything else) is
	// used to test async behaviour and never returns anything.
	TUid uid = {0x101f4e52};
	if (aRequiredInterface != uid)
		{
		iAsyncGetInterfaceRunning = ETrue;
		return;
		}
	aReturnedInterface = new CTestInterface(iLabel, *this);
	TRequestStatus* r = &aStatus;
	if (aReturnedInterface)
		User::RequestComplete(r, KErrNone);
	else
		User::RequestComplete(r, KErrNoMemory);
	}

TBool CTestToken::DoCancelGetInterface()
	{
	if (iAsyncGetInterfaceRunning)
		{
		iAsyncGetInterfaceRunning = EFalse;
		return ETrue;
		}
	return EFalse;
	}


TInt& CTestToken::ReferenceCount()
	{
	return iCount;
	}

class CTokenTypeImplementation : public CCTTokenType
	{
public:
	static CTokenTypeImplementation* NewL5();
	static CTokenTypeImplementation* NewL6();

	virtual void List(RCPointerArray<HBufC>& aTokens, 
					  TRequestStatus& aStatus);
	virtual void CancelList();
	virtual void OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken, 
						   TRequestStatus& aStatus);
	virtual void OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken, 
						   TRequestStatus& aStatus);
	virtual void CancelOpenToken();

	virtual ~CTokenTypeImplementation();
protected:
	HBufC* iMyInfo;
	};

void CTokenTypeImplementation::List(RCPointerArray<HBufC>& aTokens, 
							   TRequestStatus& aStatus)
	{
	TRequestStatus* r = &aStatus;
	HBufC* name = iMyInfo->Alloc();
	if (name)
		User::RequestComplete(r, aTokens.Append(name));
	else
		User::RequestComplete(r, KErrNoMemory);
	}

void CTokenTypeImplementation::CancelList()
	{
	}

void CTokenTypeImplementation::OpenToken(const TDesC& /*aTokenInfo*/, MCTToken*& aToken,
						   TRequestStatus& aStatus)
	{
	aToken = new CTestToken(*iMyInfo, *this);
	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, KErrNone);
	IncReferenceCount();
	}

void CTokenTypeImplementation::OpenToken(TCTTokenHandle /*aHandle*/,
										 MCTToken*& aToken,
										 TRequestStatus& aStatus)
	{
	aToken = new CTestToken(*iMyInfo, *this);
	TRequestStatus* r = &aStatus;
	User::RequestComplete(r, KErrNone);
	IncReferenceCount();
	}

void CTokenTypeImplementation::CancelOpenToken()
	{
	}


CTokenTypeImplementation::~CTokenTypeImplementation()
	{
	delete iMyInfo;
	}

CTokenTypeImplementation* CTokenTypeImplementation::NewL5()
	{
	CTokenTypeImplementation* that = new (ELeave) CTokenTypeImplementation;
	CleanupStack::PushL(that);
	that->iMyInfo = KToken5Info().AllocL();
	CleanupStack::Pop(that);
	return that;
	}

CTokenTypeImplementation* CTokenTypeImplementation::NewL6()
	{
	CTokenTypeImplementation* that = new (ELeave) CTokenTypeImplementation;
	CleanupStack::PushL(that);
	that->iMyInfo = KToken6Info().AllocL();
	CleanupStack::Pop(that);
	return that;
	}

const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(0x101F4E4D,	CTokenTypeImplementation::NewL5),
		IMPLEMENTATION_PROXY_ENTRY(0x101F4E4C,	CTokenTypeImplementation::NewL6),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

