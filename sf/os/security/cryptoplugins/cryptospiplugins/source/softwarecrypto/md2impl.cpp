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
* software md2 implementation
* software md2 implementation
*
*/


/**
 @file
*/

#include "md2impl.h"

#include <cryptospi/hashplugin.h>
#include "pluginconfig.h"

using namespace SoftwareCrypto;
	
CMD2Impl* CMD2Impl::NewL()
	{
	CMD2Impl* self=new (ELeave) CMD2Impl();
	return self;						
	}
														
CMD2Impl* CMD2Impl::NewLC()
	{
	CMD2Impl* self=NewL();
	CleanupStack::PushL(self);
	return self;						
	}
														
CMD2Impl::CMD2Impl() : iHash(KMD2HashSize)
	{		
	}
	
CMD2Impl::CMD2Impl(const CMD2Impl& aCMD2Impl)
: iHash(aCMD2Impl.iHash),iNum(aCMD2Impl.iNum)
	{
	(void)Mem::Copy(iData, aCMD2Impl.iData, sizeof(iData));
	(void)Mem::Copy(iChecksum, aCMD2Impl.iChecksum, sizeof(iChecksum));
	(void)Mem::Copy(iState, aCMD2Impl.iState, sizeof(iState));		
	}
		
CMD2Impl::~CMD2Impl()
	{	
	}
	
void CMD2Impl::Reset()
	{
	Mem::FillZ(iData,sizeof(iData));
	Mem::FillZ(iChecksum,sizeof(iChecksum));
	Mem::FillZ(iState,sizeof(iState));
	iNum=0;		
	}
	
void CMD2Impl::Close()
	{
	delete this;	
	}
	
void CMD2Impl::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	aPluginCharacteristics=NULL;
	TInt hashNum=sizeof(KHashCharacteristics)/sizeof(THashCharacteristics*);
	for (TInt i=0;i<hashNum;i++)
		{
		if (KHashCharacteristics[i]->cmn.iImplementationUID==ImplementationUid().iUid)
			{
			aPluginCharacteristics = KHashCharacteristics[i];
			break;
			}
		}	
	}
	
CExtendedCharacteristics* CMD2Impl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}

const CExtendedCharacteristics* CMD2Impl::GetExtendedCharacteristicsL()
	{
	return CMD2Impl::CreateExtendedCharacteristicsL();
	}

TPtrC8 CMD2Impl::Hash(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	DoUpdate(aMessage.Ptr(),aMessage.Size());
	StoreState();	
	DoFinal();
	ptr.Set(iHash);
	RestoreState();	
	return ptr;		
	}		
	
void CMD2Impl::Update(const TDesC8& aMessage)
	{
	DoUpdate(aMessage.Ptr(),aMessage.Size());	
	}
	
TPtrC8 CMD2Impl::Final(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	if (aMessage!=KNullDesC8())
		{
		DoUpdate(aMessage.Ptr(),aMessage.Size());			
		}
	DoFinal();
	ptr.Set(iHash);
	Reset();
	return ptr;
	}
	
MHash* CMD2Impl::ReplicateL()
	{
	return NewL();	
	}
	
MHash* CMD2Impl::CopyL()
	{
	return new(ELeave) CMD2Impl(*this);	
	}

void CMD2Impl::RestoreState()
	{
	Mem::Copy(&iData[0], &iDataTemp[0], KMD2BlockSize);
	Mem::Copy(&iChecksum[0], &iChecksumTemp[0], KMD2BlockSize*sizeof(TUint));
	Mem::Copy(&iState[0], &iStateTemp[0], KMD2BlockSize*sizeof(TUint));		
	}
	
void CMD2Impl::StoreState()
	{
	Mem::Copy(&iDataTemp[0], &iData[0], KMD2BlockSize);
	Mem::Copy(&iChecksumTemp[0], &iChecksum[0], KMD2BlockSize*sizeof(TUint));
	Mem::Copy(&iStateTemp[0], &iState[0], KMD2BlockSize*sizeof(TUint));		
	}


TUid CMD2Impl::ImplementationUid()
	{
	return KCryptoPluginMd2Uid;
	}

static inline TUint CMD2_S(TUint& elt,TUint8 val)
	{
	return elt^=val;
	}

void CMD2Impl::DoUpdate(const TUint8* aData,TUint aLength)
	{
	TBool carryOn=ETrue;
	if (iNum)
		{
		if (iNum+aLength>=(TUint)KMD2BlockSize)
			{
			const TUint temp=KMD2BlockSize-iNum;
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
	// processing by block of KMD2BlockSize
	if (carryOn)
		{
		while (aLength>=(TUint)KMD2BlockSize)
			{
			Block(aData);
			aData+=KMD2BlockSize;
			aLength-=KMD2BlockSize;
			}
		(void)Mem::Copy(iData,aData,aLength);
		iNum=aLength;
		}		
	}
	
void CMD2Impl::DoFinal()
	{
	const TUint pad=KMD2BlockSize-iNum;
	if (pad>0)
		Mem::Fill(iData+iNum,(TUint8)pad,pad);
	Block(iData);

	TUint8* pData=iData;
	const TUint8* pEnd=iData+KMD2BlockSize;
	const TUint* pChecksum=iChecksum;
	while (pData<pEnd)
		{
		*pData=(TUint8)*pChecksum;//checksum is always less than 255 
		pData++;
		pChecksum++;
		}
	Block(iData);

	TUint* pState=iState;
	for (TUint i=0;i<(TUint)KMD2BlockSize;i++,pState++) //safe: iState and iHash have got same size
		iHash[i]=(TUint8)(*pState&0xff);		
	}
	
void CMD2Impl::Block(const TUint8* aData)
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
	TUint state[KMD2BlockSize*3];
	TUint j=iChecksum[KMD2BlockSize-1];
	TUint i=0,temp=0;
	while (i<(TUint)KMD2BlockSize)
		{
		const TUint tempState=iState[i];
		temp=aData[i];
		state[i]=tempState;
		state[i+16]=temp;
		state[i+32]=temp^tempState;
		j=CMD2_S(iChecksum[i++],S[temp^j]);
		}
	temp=0;
	TUint* pstate=state;
	const TUint *pend=state+(KMD2BlockSize*3);
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
	(void)Mem::Copy(iState,state,KMD2BlockSize*sizeof(TUint));		
	}


// Implemented in hmacimpl.cpp or softwarehashbase.cpp
// but required as derived from MHash. No coverage here.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

TAny* CMD2Impl::GetExtension(TUid /*aExtensionId*/)
	{
	return NULL;	
	}

void CMD2Impl::SetOperationModeL(TUid /*aOperationMode*/)
	{
	User::Leave(KErrNotSupported);
	}

void CMD2Impl::SetKeyL(const CKey& /*aKey*/)
	{
	User::Leave(KErrNotSupported);
	}

