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
* (c) 2003 Symbian Ltd.  All rights reserved.
*
*/


/**
 @file
*/

#include <e32std.h>
#include <hash.h>

#define EXPANDLOOP

//
// 32bit endianness independent MD2 implementation
//
CMD2::CMD2(void)
: CMessageDigest(),iHash(MD2_HASH)
	{
	}
CMD2::CMD2(const CMD2& aMD)
: CMessageDigest(aMD),iHash(aMD.iHash),iNum(aMD.iNum)
	{
	(void)Mem::Copy(iData,aMD.iData,sizeof(iData));
	(void)Mem::Copy(iChecksum,aMD.iChecksum,sizeof(iChecksum));
	(void)Mem::Copy(iState,aMD.iState,sizeof(iState));
	}
EXPORT_C CMD2* CMD2::NewL(void)
	{
	return (new(ELeave) CMD2);
	}
EXPORT_C CMessageDigest* CMD2::ReplicateL(void)
	{
	return NewL();
	}
EXPORT_C CMD2::~CMD2(void)
	{
	}

EXPORT_C TPtrC8 CMD2::Hash(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	DoUpdate(aMessage.Ptr(),aMessage.Size());
	StoreState();	
	DoFinal();
	ptr.Set(iHash);
	RestoreState();	
	return ptr;
	}

EXPORT_C CMessageDigest* CMD2::CopyL(void)
	{
	return new(ELeave) CMD2(*this);
	}
EXPORT_C TInt CMD2::BlockSize(void)
	{
	return sizeof(iData);
	}
EXPORT_C TInt CMD2::HashSize(void)
	{
	return MD2_HASH;
	}
EXPORT_C void CMD2::Reset(void)
	{
	Mem::FillZ(iData,sizeof(iData));
	Mem::FillZ(iChecksum,sizeof(iChecksum));
	Mem::FillZ(iState,sizeof(iState));
	iNum=0;
	}

EXPORT_C void CMD2::Update(const TDesC8& aMessage)
	{
	DoUpdate(aMessage.Ptr(),aMessage.Size());
	}

EXPORT_C TPtrC8 CMD2::Final(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	DoUpdate(aMessage.Ptr(),aMessage.Size());
	DoFinal();
	ptr.Set(iHash);
	Reset();
	return ptr;
	}

EXPORT_C TPtrC8 CMD2::Final()
	{
	TPtrC8 ptr(KNullDesC8());
	DoFinal();
	ptr.Set(iHash);
	Reset();
	return ptr;
	}

void CMD2::DoUpdate(const TUint8* aData,TUint aLength)
	{
	TBool carryOn=ETrue;
	if (iNum)
		{
		if (iNum+aLength>=(TUint)MD2_BLOCK)
			{
			const TUint temp=MD2_BLOCK-iNum;
			(void)Mem::Copy(iData+iNum,aData,temp);
			Block(iData);
			aData+=temp;
			aLength-=temp;
			iNum=0;
			}
		else
			{
			(void)Mem::Copy(iData+iNum,aData,aLength);
			iNum+=aLength;
			carryOn=EFalse;
			}
		}
	// processing by block of MD2_BLOCK
	if (carryOn)
		{
		while (aLength>=(TUint)MD2_BLOCK)
			{
			Block(aData);
			aData+=MD2_BLOCK;
			aLength-=MD2_BLOCK;
			}
		(void)Mem::Copy(iData,aData,aLength);
		iNum=aLength;
		}
	}
static inline TUint CMD2_S(TUint& elt,TUint8 val)
	{
	return elt^=val;
	}
// 
// data must be MD2_BLOCK bytes long
//
void CMD2::Block(const TUint8* data)
	{
	static const TUint8 S[256]={
		0x29, 0x2E, 0x43, 0xC9, 0xA2, 0xD8, 0x7C, 0x01,
		0x3D, 0x36, 0x54, 0xA1, 0xEC, 0xF0, 0x06, 0x13,
		0x62, 0xA7, 0x05, 0xF3, 0xC0, 0xC7, 0x73, 0x8C,
		0x98, 0x93, 0x2B, 0xD9, 0xBC, 0x4C, 0x82, 0xCA,
		0x1E, 0x9B, 0x57, 0x3C, 0xFD, 0xD4, 0xE0, 0x16,
		0x67, 0x42, 0x6F, 0x18, 0x8A, 0x17, 0xE5, 0x12,
		0xBE, 0x4E, 0xC4, 0xD6, 0xDA, 0x9E, 0xDE, 0x49,
		0xA0, 0xFB, 0xF5, 0x8E, 0xBB, 0x2F, 0xEE, 0x7A,
		0xA9, 0x68, 0x79, 0x91, 0x15, 0xB2, 0x07, 0x3F,
		0x94, 0xC2, 0x10, 0x89, 0x0B, 0x22, 0x5F, 0x21,
		0x80, 0x7F, 0x5D, 0x9A, 0x5A, 0x90, 0x32, 0x27,
		0x35, 0x3E, 0xCC, 0xE7, 0xBF, 0xF7, 0x97, 0x03,
		0xFF, 0x19, 0x30, 0xB3, 0x48, 0xA5, 0xB5, 0xD1,
		0xD7, 0x5E, 0x92, 0x2A, 0xAC, 0x56, 0xAA, 0xC6,
		0x4F, 0xB8, 0x38, 0xD2, 0x96, 0xA4, 0x7D, 0xB6,
		0x76, 0xFC, 0x6B, 0xE2, 0x9C, 0x74, 0x04, 0xF1,
		0x45, 0x9D, 0x70, 0x59, 0x64, 0x71, 0x87, 0x20,
		0x86, 0x5B, 0xCF, 0x65, 0xE6, 0x2D, 0xA8, 0x02,
		0x1B, 0x60, 0x25, 0xAD, 0xAE, 0xB0, 0xB9, 0xF6,
		0x1C, 0x46, 0x61, 0x69, 0x34, 0x40, 0x7E, 0x0F,
		0x55, 0x47, 0xA3, 0x23, 0xDD, 0x51, 0xAF, 0x3A,
		0xC3, 0x5C, 0xF9, 0xCE, 0xBA, 0xC5, 0xEA, 0x26,
		0x2C, 0x53, 0x0D, 0x6E, 0x85, 0x28, 0x84, 0x09,
		0xD3, 0xDF, 0xCD, 0xF4, 0x41, 0x81, 0x4D, 0x52,
		0x6A, 0xDC, 0x37, 0xC8, 0x6C, 0xC1, 0xAB, 0xFA,
		0x24, 0xE1, 0x7B, 0x08, 0x0C, 0xBD, 0xB1, 0x4A,
		0x78, 0x88, 0x95, 0x8B, 0xE3, 0x63, 0xE8, 0x6D,
		0xE9, 0xCB, 0xD5, 0xFE, 0x3B, 0x00, 0x1D, 0x39,
		0xF2, 0xEF, 0xB7, 0x0E, 0x66, 0x58, 0xD0, 0xE4,
		0xA6, 0x77, 0x72, 0xF8, 0xEB, 0x75, 0x4B, 0x0A,
		0x31, 0x44, 0x50, 0xB4, 0x8F, 0xED, 0x1F, 0x1A,
		0xDB, 0x99, 0x8D, 0x33, 0x9F, 0x11, 0x83, 0x14,
		};
	TUint state[MD2_BLOCK*3];
	TUint j=iChecksum[MD2_BLOCK-1];
	TUint i=0,temp=0;
	while (i<(TUint)MD2_BLOCK)
		{
		const TUint tempState=iState[i];
		temp=data[i];
		state[i]=tempState;
		state[i+16]=temp;
		state[i+32]=temp^tempState;
		j=CMD2_S(iChecksum[i++],S[temp^j]);
		}
	temp=0;
	TUint* pstate=state;
	const TUint *pend=state+(MD2_BLOCK*3);
	for (i=0; i<18; i++)
		{
		while (pstate<pend)
			{
			temp=CMD2_S(*pstate,S[temp]);pstate++;
			temp=CMD2_S(*pstate,S[temp]);pstate++;
			temp=CMD2_S(*pstate,S[temp]);pstate++;
			temp=CMD2_S(*pstate,S[temp]);pstate++;
			temp=CMD2_S(*pstate,S[temp]);pstate++;
			temp=CMD2_S(*pstate,S[temp]);pstate++;
			temp=CMD2_S(*pstate,S[temp]);pstate++;
			temp=CMD2_S(*pstate,S[temp]);pstate++;
			}
		pstate=state;
		temp=(temp+i)&0xff;
		}
	(void)Mem::Copy(iState,state,MD2_BLOCK*sizeof(TUint));
	}
void CMD2::DoFinal()
	{
	const TUint pad=MD2_BLOCK-iNum;
	if (pad>0)
		Mem::Fill(iData+iNum,(TUint8)pad,pad);
	Block(iData);

	TUint8* pData=iData;
	const TUint8* pEnd=iData+MD2_BLOCK;
	const TUint* pChecksum=iChecksum;
	while (pData<pEnd)
		{
		*pData=(TUint8)*pChecksum;//checksum is always less than 255 
		pData++;
		pChecksum++;
		}
	Block(iData);

	TUint* pState=iState;
	for (TUint i=0;i<(TUint)MD2_BLOCK;i++,pState++) //safe: iState and iHash have got same size
		iHash[i]=(TUint8)(*pState&0xff);
	}

void CMD2::RestoreState()
{
	Mem::Copy(&iData[0], &iDataTemp[0], MD2_BLOCK);
	Mem::Copy(&iChecksum[0], &iChecksumTemp[0], MD2_BLOCK*sizeof(TUint));
	Mem::Copy(&iState[0], &iStateTemp[0], MD2_BLOCK*sizeof(TUint));
}

void CMD2::StoreState()
{
	Mem::Copy(&iDataTemp[0], &iData[0], MD2_BLOCK);
	Mem::Copy(&iChecksumTemp[0], &iChecksum[0], MD2_BLOCK*sizeof(TUint));
	Mem::Copy(&iStateTemp[0], &iState[0], MD2_BLOCK*sizeof(TUint));
}

