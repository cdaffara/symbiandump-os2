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
*
*/


#include "tvectorutils.h"
#include "t_input.h"

//	RSA
_LIT8(KModStart, "<modulus>");
_LIT8(KModEnd, "</modulus>");
_LIT8(KPubExpStart, "<publicExponent>");
_LIT8(KPubExpEnd, "</publicExponent>");
_LIT8(KPrivExpStart, "<privateExponent>");
_LIT8(KPrivExpEnd, "</privateExponent>");
_LIT8(KPStart, "<P>");
_LIT8(KPEnd, "</P>");
_LIT8(KQStart, "<Q>");
_LIT8(KQEnd, "</Q>");
_LIT8(KdPStart, "<dP>");
_LIT8(KdPEnd, "</dP>");
_LIT8(KdQStart, "<dQ>");
_LIT8(KdQEnd, "</dQ>");
_LIT8(KqInvStart, "<qInv>");
_LIT8(KqInvEnd, "</qInv>");

_LIT8(KTrueVal, "ETrue");
_LIT8(KFalseVal, "EFalse");

RInteger VectorUtils::ParseIntegerL(const TDesC8& aDes)
    {
	HBufC8* buf = ParseBinaryL(aDes);
	CleanupStack::PushL(buf);
    RInteger result = RInteger::NewL(*buf);
    CleanupStack::PopAndDestroy(buf);

    return result;
    }

HBufC8* VectorUtils::ParseBinaryL(const TDesC8& aDes)
{
    __ASSERT_ALWAYS(aDes.Length() % 2 == 0, User::Panic(_L("ParseBinaryL"), KErrArgument));
	int length = aDes.Length() / 2;
	HBufC8* buf = HBufC8::NewL(length);
    TPtr8 ptr = buf->Des();
	ptr.SetLength(length);

    for (TInt i = 0 ; i < aDes.Length() ; i += 2)
        {
        TUint8 tmp;
        tmp=(TUint8)(aDes[i]-(aDes[i]>'9'?('A'-10):'0'));
        tmp*=16;
        tmp|=(TUint8)(aDes[i+1]-(aDes[i+1]>'9'?('A'-10):'0'));
		ptr[i / 2] = tmp;
        }

	return buf;
}

// Print an Integer into hex, only works for positive integers
TDesC* VectorUtils::PrintIntegerL(const TInteger& aInt)
{
	HBufC8* binary = aInt.BufferLC();
	TDesC* result = PrintBinaryL(*binary);
	CleanupStack::PopAndDestroy(binary);

	return result;
}

// Print a binary string into hex
TDesC* VectorUtils::PrintBinaryL(const TDesC8& aData)
{
	int length = aData.Length() * 2;
	HBufC* buf = HBufC::NewL(length);
	TPtr ptr = buf->Des();
	ptr.SetLength(length);

	for (int i = 0 ; i < aData.Length() ; ++i)
		{
		TUint8 val = aData[i];
		TUint8 n = (TUint8) ((val & 0xf0) >> 4);
		ptr[i * 2] = (TUint8) (n < 10 ? ('0' + n) : ('A' + n - 10));
		n = (TUint8) (val & 0x0f);
		ptr[i * 2 + 1] = (TUint8) (n < 10 ? ('0' + n) : ('A' + n - 10));
		}

	return buf;
}

TBool VectorUtils::ParseBoolL(const TDesC8& aDes)
    {
    TBool result = EFalse;

    if (aDes == KTrueVal)
        result = ETrue;
    else if (aDes != KFalseVal)
        User::Leave(KErrArgument);

    return result;
    }

CRSAPublicKey* VectorUtils::ReadRSAPublicKeyL(const TDesC8& aData)
    {
	TPtrC8 modIn = Input::ParseElement(aData, KModStart, KModEnd);
    RInteger mod = ParseIntegerL(modIn);
	CleanupStack::PushL(mod);

	TPtrC8 pubExpIn = Input::ParseElement(aData, KPubExpStart, KPubExpEnd);
	RInteger pubExp = ParseIntegerL(pubExpIn);
	CleanupStack::PushL(pubExp);

	CRSAPublicKey* result = CRSAPublicKey::NewL(mod, pubExp);

	CleanupStack::Pop(&pubExp);
	CleanupStack::Pop(&mod);
    return result;
    }

CRSAPrivateKeyStandard* VectorUtils::ReadRSAPrivateKeyL(const TDesC8& aData)
    {
	TPtrC8 modIn = Input::ParseElement(aData, KModStart, KModEnd);
    RInteger mod = ParseIntegerL(modIn);
	CleanupStack::PushL(mod);

	TPtrC8 privExpIn = Input::ParseElement(aData, KPrivExpStart, KPrivExpEnd);
	RInteger privExp = ParseIntegerL(privExpIn);
	CleanupStack::PushL(privExp);

	CRSAPrivateKeyStandard* result  = CRSAPrivateKeyStandard::NewL(mod, privExp);

	CleanupStack::Pop(&privExp);
	CleanupStack::Pop(&mod);
    return result;
    }

CRSAPrivateKeyCRT* VectorUtils::ReadRSAPrivateKeyCRTL(const TDesC8& aData)
    {
	TPtrC8 modIn = Input::ParseElement(aData, KModStart, KModEnd);
    RInteger mod = ParseIntegerL(modIn);
	CleanupStack::PushL(mod);

	TPtrC8 pIn = Input::ParseElement(aData, KPStart, KPEnd);
	RInteger P = ParseIntegerL(pIn);
	CleanupStack::PushL(P);

	TPtrC8 qIn = Input::ParseElement(aData, KQStart, KQEnd);
	RInteger Q = ParseIntegerL(qIn);
	CleanupStack::PushL(Q);
	
	TPtrC8 dpIn = Input::ParseElement(aData, KdPStart, KdPEnd);
	RInteger dP = ParseIntegerL(dpIn);
	CleanupStack::PushL(dP);
	
	TPtrC8 dqIn = Input::ParseElement(aData, KdQStart, KdQEnd);
	RInteger dQ = ParseIntegerL(dqIn);
	CleanupStack::PushL(dQ);

	TPtrC8 qInvIn = Input::ParseElement(aData, KqInvStart, KqInvEnd);
	RInteger qInv = ParseIntegerL(qInvIn);
	CleanupStack::PushL(qInv);
	
	CRSAPrivateKeyCRT* privKey  = CRSAPrivateKeyCRT::NewL(mod, P, Q, dP, dQ, qInv);

	CleanupStack::Pop(&qInv);
	CleanupStack::Pop(&dQ);
	CleanupStack::Pop(&dP);
	CleanupStack::Pop(&Q);
	CleanupStack::Pop(&P);
	CleanupStack::Pop(&mod);
    return (privKey);
    }

CDSAPublicKey* VectorUtils::ReadDSAPublicKeyL(const TDesC8& aData)
    {
	TPtrC8 pIn = Input::ParseElement(aData, _L8("<p>"));
    RInteger p = ParseIntegerL(pIn);
	CleanupStack::PushL(p);

	TPtrC8 qIn = Input::ParseElement(aData, _L8("<q>"));
    RInteger q = ParseIntegerL(qIn);
	CleanupStack::PushL(q);

	TPtrC8 gIn = Input::ParseElement(aData, _L8("<g>"));
    RInteger g = ParseIntegerL(gIn);
	CleanupStack::PushL(g);

	TPtrC8 yIn = Input::ParseElement(aData, _L8("<y>"));
    RInteger y = ParseIntegerL(yIn);
	CleanupStack::PushL(y);

	CDSAPublicKey* result = CDSAPublicKey::NewL(p, q, g, y);

	CleanupStack::Pop(&y);
	CleanupStack::Pop(&g);
	CleanupStack::Pop(&q);
	CleanupStack::Pop(&p);

    return result;
    }

CDSAPrivateKey* VectorUtils::ReadDSAPrivateKeyL(const TDesC8& aData)
    {
	TPtrC8 pIn = Input::ParseElement(aData, _L8("<p>"));
    RInteger p = ParseIntegerL(pIn);
	CleanupStack::PushL(p);

	TPtrC8 qIn = Input::ParseElement(aData, _L8("<q>"));
    RInteger q = ParseIntegerL(qIn);
	CleanupStack::PushL(q);

	TPtrC8 gIn = Input::ParseElement(aData, _L8("<g>"));
    RInteger g = ParseIntegerL(gIn);
	CleanupStack::PushL(g);

	TPtrC8 xIn = Input::ParseElement(aData, _L8("<x>"));
    RInteger x = ParseIntegerL(xIn);
	CleanupStack::PushL(x);

	CDSAPrivateKey* result = CDSAPrivateKey::NewL(p, q, g, x);

	CleanupStack::Pop(&x);
	CleanupStack::Pop(&g);
	CleanupStack::Pop(&q);
	CleanupStack::Pop(&p);

    return result;
    }

CDSASignature* VectorUtils::ReadDSASignatureL(const TDesC8& aData)
    {
	TPtrC8 rIn = Input::ParseElement(aData, _L8("<r>"));
    RInteger r = ParseIntegerL(rIn);
	CleanupStack::PushL(r);

	TPtrC8 sIn = Input::ParseElement(aData, _L8("<s>"));
    RInteger s = ParseIntegerL(sIn);
	CleanupStack::PushL(s);

	CDSASignature* result = CDSASignature::NewL(r, s);

	CleanupStack::Pop(&s);
	CleanupStack::Pop(&r);

    return result;
    }
