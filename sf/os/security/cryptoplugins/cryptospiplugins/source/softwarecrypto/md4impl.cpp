/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* software md4 implementation
* software md4 implementation
*
*/


/**
 @file
*/

#include "md4impl.h"
#include <cryptospi/hashplugin.h>
#include "pluginconfig.h"


using namespace SoftwareCrypto;

	
CMD4Impl* CMD4Impl::NewL()
	{
	CMD4Impl* self=new (ELeave) CMD4Impl();
	self->Reset();
	return self;						
	}
														
CMD4Impl* CMD4Impl::NewLC()
	{
	CMD4Impl* self=NewL();
	CleanupStack::PushL(self);
	return self;						
	}
														
CMD4Impl::CMD4Impl() : iHash(KMD4HashSize)
	{		
	}
	
CMD4Impl::CMD4Impl(const CMD4Impl& aCMD4Impl)
: iHash(aCMD4Impl.iHash),iA(aCMD4Impl.iA),iB(aCMD4Impl.iB),iC(aCMD4Impl.iC),iD(aCMD4Impl.iD),
  iNl(aCMD4Impl.iNl),iNh(aCMD4Impl.iNh)
	{
	(void)Mem::Copy(iData, aCMD4Impl.iData, sizeof(iData));
	}
	
CMD4Impl::~CMD4Impl()
	{	
	}
	
void CMD4Impl::Reset()
	{
	iA=0x67452301;
	iB=0xefcdab89;
	iC=0x98badcfe;
	iD=0x10325476;
	iNh=0;
	iNl=0;
	}
	
void CMD4Impl::Close()
	{
	delete this;	
	}
	
void CMD4Impl::GetCharacteristicsL(const TCharacteristics*& aPluginCharacteristics)
	{
	aPluginCharacteristics=NULL;
	TInt hashNum=sizeof(KHashCharacteristics)/sizeof(THashCharacteristics*);
	for (TInt i=0;i<hashNum;i++)
		{
		if (KHashCharacteristics[i]->cmn.iImplementationUID == ImplementationUid().iUid)
			{
			aPluginCharacteristics = KHashCharacteristics[i];
			break;
			}
		}	
	}
	
CExtendedCharacteristics* CMD4Impl::CreateExtendedCharacteristicsL()
	{
	// All Symbian software plug-ins have unlimited concurrency, cannot be reserved
	// for exclusive use and are not CERTIFIED to be standards compliant.
	return CExtendedCharacteristics::NewL(KMaxTInt, EFalse);
	}
const CExtendedCharacteristics* CMD4Impl::GetExtendedCharacteristicsL()
	{
	return CMD4Impl::CreateExtendedCharacteristicsL();
	}

TPtrC8 CMD4Impl::Hash(const TDesC8& aMessage)
	{
	DoUpdate(aMessage.Ptr(),aMessage.Size());
	StoreState();
	DoFinal();
	RestoreState();
	return iHash;
	}		
	
void CMD4Impl::Update(const TDesC8& aMessage)
	{
	DoUpdate(aMessage.Ptr(),aMessage.Size());	
	}
	
TPtrC8 CMD4Impl::Final(const TDesC8& aMessage)
	{
	if (aMessage!=KNullDesC8())
		{
		DoUpdate(aMessage.Ptr(),aMessage.Size());			
		}
	DoFinal();
	Reset();
	return iHash;
	}
	
MHash* CMD4Impl::ReplicateL()
	{	 
	return CMD4Impl::NewL();
	}
	
MHash* CMD4Impl::CopyL()
	{
	return new(ELeave) CMD4Impl(*this);	
	}

TUid CMD4Impl::ImplementationUid()
	{
	return KCryptoPluginMd4Uid;
	}

void CMD4Impl::DoUpdate(const TUint8* aData,TUint aLength)
	{
	const TUint8* pend=aData+aLength;	
	for (const TUint8* paData=aData;paData<pend;paData++) 
		{
		const TUint8 byte=*paData;
		switch (iNl&3) 
			{
			case 0:
				iData[iNl>>2]=byte;
				break;
			case 1:
				iData[iNl>>2]|=byte<<8;
				break;
			case 2:
				iData[iNl>>2]|=byte<<16;
				break;
			case 3:
				iData[iNl>>2]|=byte<<24;
				break;
			default:
				break;
			};
		if(++iNl==64) 
			{
			Block();
			iNh+=64;
			iNl=0;
			}
		}
	}
	
static inline TUint CMD4_F(TUint x,TUint y,TUint z)
	{
	return (x&y) | (~x&z);
	}
static inline TUint CMD4_G(TUint x,TUint y,TUint z)
	{
	return (x&y) | (x&z) | (y&z);
	}
static inline TUint CMD4_H(TUint x,TUint y,TUint z)
	{
	return x^y^z;
	}

	
#ifdef NOREFS
static inline TUint CMD4_FF(TUint a,const TUint b,const TUint c,const TUint d,const TUint x,const TUint s)
	{
	a+=CMD4_F(b,c,d) + x; 
	a=CMD_R(a,s);
	return a;
	}
static inline TUint CMD4_GG(TUint a,const TUint b,const TUint c,const TUint d,const TUint x,const TUint s)
	{
	a+=CMD4_G(b,c,d) + x + (TUint32)0x5a827999; 
	a=CMD_R(a,s);
	return a;
	}
static inline TUint CMD4_HH(TUint a,const TUint b,const TUint c,const TUint d,const TUint x,const TUint s)
	{
	a+=CMD4_H(b,c,d) + x + (TUint32)0x6ed9eba1; 
	a=CMD_R(a,s);
	return a;
	}

void CMD4Impl::Block()
	{
	register TUint tempA=iA;
	register TUint tempB=iB;
	register TUint tempC=iC;
	register TUint tempD=iD;

	tempA = CMD4_FF(tempA,tempB,tempC,tempD,iData[ 0],3);
	tempD = CMD4_FF(tempD,tempA,tempB,tempC,iData[ 1],7);
	tempC = CMD4_FF(tempC,tempD,tempA,tempB,iData[ 2],11);
	tempB = CMD4_FF(tempB,tempC,tempD,tempA,iData[ 3],19);
	tempA = CMD4_FF(tempA,tempB,tempC,tempD,iData[ 4],3);
	tempD = CMD4_FF(tempD,tempA,tempB,tempC,iData[ 5],7);
	tempC = CMD4_FF(tempC,tempD,tempA,tempB,iData[ 6],11);
	tempB = CMD4_FF(tempB,tempC,tempD,tempA,iData[ 7],19);
	tempA = CMD4_FF(tempA,tempB,tempC,tempD,iData[ 8],3);
	tempD = CMD4_FF(tempD,tempA,tempB,tempC,iData[ 9],7);
	tempC = CMD4_FF(tempC,tempD,tempA,tempB,iData[10],11);
	tempB = CMD4_FF(tempB,tempC,tempD,tempA,iData[11],19);
	tempA = CMD4_FF(tempA,tempB,tempC,tempD,iData[12],3);
	tempD = CMD4_FF(tempD,tempA,tempB,tempC,iData[13],7);
	tempC = CMD4_FF(tempC,tempD,tempA,tempB,iData[14],11);
	tempB = CMD4_FF(tempB,tempC,tempD,tempA,iData[15],19);

	tempA = CMD4_GG(tempA,tempB,tempC,tempD,iData[ 0],3);
	tempD = CMD4_GG(tempD,tempA,tempB,tempC,iData[ 4],5);
	tempC = CMD4_GG(tempC,tempD,tempA,tempB,iData[ 8],9);
	tempB = CMD4_GG(tempB,tempC,tempD,tempA,iData[12],13);
	tempA = CMD4_GG(tempA,tempB,tempC,tempD,iData[ 1],3);
	tempD = CMD4_GG(tempD,tempA,tempB,tempC,iData[ 5],5);
	tempC = CMD4_GG(tempC,tempD,tempA,tempB,iData[ 9],9);
	tempB = CMD4_GG(tempB,tempC,tempD,tempA,iData[13],13);
	tempA = CMD4_GG(tempA,tempB,tempC,tempD,iData[ 2],3);
	tempD = CMD4_GG(tempD,tempA,tempB,tempC,iData[ 6],5);
	tempC = CMD4_GG(tempC,tempD,tempA,tempB,iData[10],9);
	tempB = CMD4_GG(tempB,tempC,tempD,tempA,iData[14],13);
	tempA = CMD4_GG(tempA,tempB,tempC,tempD,iData[ 3],3);
	tempD = CMD4_GG(tempD,tempA,tempB,tempC,iData[ 7],5);
	tempC = CMD4_GG(tempC,tempD,tempA,tempB,iData[11],9);
	tempB = CMD4_GG(tempB,tempC,tempD,tempA,iData[15],13);

	tempA = CMD4_HH(tempA,tempB,tempC,tempD,iData[ 0],3);
	tempD = CMD4_HH(tempD,tempA,tempB,tempC,iData[ 8],9);
	tempC = CMD4_HH(tempC,tempD,tempA,tempB,iData[ 4],11);
	tempB = CMD4_HH(tempB,tempC,tempD,tempA,iData[12],15);
	tempA = CMD4_HH(tempA,tempB,tempC,tempD,iData[ 2],3);
	tempD = CMD4_HH(tempD,tempA,tempB,tempC,iData[10],9);
	tempC = CMD4_HH(tempC,tempD,tempA,tempB,iData[ 6],11);
	tempB = CMD4_HH(tempB,tempC,tempD,tempA,iData[14],15);
	tempA = CMD4_HH(tempA,tempB,tempC,tempD,iData[ 1],3);
	tempD = CMD4_HH(tempD,tempA,tempB,tempC,iData[ 9],9);
	tempC = CMD4_HH(tempC,tempD,tempA,tempB,iData[ 5],11);
	tempB = CMD4_HH(tempB,tempC,tempD,tempA,iData[13],15);
	tempA = CMD4_HH(tempA,tempB,tempC,tempD,iData[ 3],3);
	tempD = CMD4_HH(tempD,tempA,tempB,tempC,iData[11],9);
	tempC = CMD4_HH(tempC,tempD,tempA,tempB,iData[ 7],11);
	tempB = CMD4_HH(tempB,tempC,tempD,tempA,iData[15],15);

	iA+=tempA;
	iB+=tempB;
	iC+=tempC;
	iD+=tempD;
	}
#else
#ifdef MACRO
#define CMD4_FF(a, b, c, d, x, s) (CMD_R(a += CMD4_F(b,c,d) + x, s))
#define CMD4_GG(a, b, c, d, x, s) (CMD_R(a += CMD4_G(b,c,d) + x + (TUint32)0x5a827999, s))
#define CMD4_HH(a, b, c, d, x, s) (CMD_R(a += CMD4_H(b,c,d) + x + (TUint32)0x6ed9eba1, s))
void CMD4Impl::Block()
	{
	register TUint tempA=iA;
	register TUint tempB=iB;
	register TUint tempC=iC;
	register TUint tempD=iD;
	
	tempA = CMD4_FF(tempA,tempB,tempC,tempD,iData[ 0],3);
	tempD = CMD4_FF(tempD,tempA,tempB,tempC,iData[ 1],7);
	tempC = CMD4_FF(tempC,tempD,tempA,tempB,iData[ 2],11);
	tempB = CMD4_FF(tempB,tempC,tempD,tempA,iData[ 3],19);
	tempA = CMD4_FF(tempA,tempB,tempC,tempD,iData[ 4],3);
	tempD = CMD4_FF(tempD,tempA,tempB,tempC,iData[ 5],7);
	tempC = CMD4_FF(tempC,tempD,tempA,tempB,iData[ 6],11);
	tempB = CMD4_FF(tempB,tempC,tempD,tempA,iData[ 7],19);
	tempA = CMD4_FF(tempA,tempB,tempC,tempD,iData[ 8],3);
	tempD = CMD4_FF(tempD,tempA,tempB,tempC,iData[ 9],7);
	tempC = CMD4_FF(tempC,tempD,tempA,tempB,iData[10],11);
	tempB = CMD4_FF(tempB,tempC,tempD,tempA,iData[11],19);
	tempA = CMD4_FF(tempA,tempB,tempC,tempD,iData[12],3);
	tempD = CMD4_FF(tempD,tempA,tempB,tempC,iData[13],7);
	tempC = CMD4_FF(tempC,tempD,tempA,tempB,iData[14],11);
	tempB = CMD4_FF(tempB,tempC,tempD,tempA,iData[15],19);

	tempA = CMD4_GG(tempA,tempB,tempC,tempD,iData[ 0],3);
	tempD = CMD4_GG(tempD,tempA,tempB,tempC,iData[ 4],5);
	tempC = CMD4_GG(tempC,tempD,tempA,tempB,iData[ 8],9);
	tempB = CMD4_GG(tempB,tempC,tempD,tempA,iData[12],13);
	tempA = CMD4_GG(tempA,tempB,tempC,tempD,iData[ 1],3);
	tempD = CMD4_GG(tempD,tempA,tempB,tempC,iData[ 5],5);
	tempC = CMD4_GG(tempC,tempD,tempA,tempB,iData[ 9],9);
	tempB = CMD4_GG(tempB,tempC,tempD,tempA,iData[13],13);
	tempA = CMD4_GG(tempA,tempB,tempC,tempD,iData[ 2],3);
	tempD = CMD4_GG(tempD,tempA,tempB,tempC,iData[ 6],5);
	tempC = CMD4_GG(tempC,tempD,tempA,tempB,iData[10],9);
	tempB = CMD4_GG(tempB,tempC,tempD,tempA,iData[14],13);
	tempA = CMD4_GG(tempA,tempB,tempC,tempD,iData[ 3],3);
	tempD = CMD4_GG(tempD,tempA,tempB,tempC,iData[ 7],5);
	tempC = CMD4_GG(tempC,tempD,tempA,tempB,iData[11],9);
	tempB = CMD4_GG(tempB,tempC,tempD,tempA,iData[15],13);

	tempA = CMD4_HH(tempA,tempB,tempC,tempD,iData[ 0],3);
	tempD = CMD4_HH(tempD,tempA,tempB,tempC,iData[ 8],9);
	tempC = CMD4_HH(tempC,tempD,tempA,tempB,iData[ 4],11);
	tempB = CMD4_HH(tempB,tempC,tempD,tempA,iData[12],15);
	tempA = CMD4_HH(tempA,tempB,tempC,tempD,iData[ 2],3);
	tempD = CMD4_HH(tempD,tempA,tempB,tempC,iData[10],9);
	tempC = CMD4_HH(tempC,tempD,tempA,tempB,iData[ 6],11);
	tempB = CMD4_HH(tempB,tempC,tempD,tempA,iData[14],15);
	tempA = CMD4_HH(tempA,tempB,tempC,tempD,iData[ 1],3);
	tempD = CMD4_HH(tempD,tempA,tempB,tempC,iData[ 9],9);
	tempC = CMD4_HH(tempC,tempD,tempA,tempB,iData[ 5],11);
	tempB = CMD4_HH(tempB,tempC,tempD,tempA,iData[13],15);
	tempA = CMD4_HH(tempA,tempB,tempC,tempD,iData[ 3],3);
	tempD = CMD4_HH(tempD,tempA,tempB,tempC,iData[11],9);
	tempC = CMD4_HH(tempC,tempD,tempA,tempB,iData[ 7],11);
	tempB = CMD4_HH(tempB,tempC,tempD,tempA,iData[15],15);

	iA+=tempA;
	iB+=tempB;
	iC+=tempC;
	iD+=tempD;
	}
#else
static inline void CMD4_FF(TUint& a,const TUint b,const TUint c,const TUint d,const TUint x,const TUint s)
	{
	a+=CMD4_F(b,c,d) + x; 
	a=CMD_R(a,s);
	}
static inline void CMD4_GG(TUint& a,const TUint b,const TUint c,const TUint d,const TUint x,const TUint s)
	{
	a+=CMD4_G(b,c,d) + x + (TUint32)0x5a827999; 
	a=CMD_R(a,s);
	}
static inline void CMD4_HH(TUint& a,const TUint b,const TUint c,const TUint d,const TUint x,const TUint s)
	{
	a+=CMD4_H(b,c,d) + x + (TUint32)0x6ed9eba1; 
	a=CMD_R(a,s);
	}

void CMD4Impl::Block()
	{
	register TUint tempA=iA;
	register TUint tempB=iB;
	register TUint tempC=iC;
	register TUint tempD=iD;

	CMD4_FF(tempA,tempB,tempC,tempD,iData[ 0],3);
	CMD4_FF(tempD,tempA,tempB,tempC,iData[ 1],7);
	CMD4_FF(tempC,tempD,tempA,tempB,iData[ 2],11);
	CMD4_FF(tempB,tempC,tempD,tempA,iData[ 3],19);
	CMD4_FF(tempA,tempB,tempC,tempD,iData[ 4],3);
	CMD4_FF(tempD,tempA,tempB,tempC,iData[ 5],7);
	CMD4_FF(tempC,tempD,tempA,tempB,iData[ 6],11);
	CMD4_FF(tempB,tempC,tempD,tempA,iData[ 7],19);
	CMD4_FF(tempA,tempB,tempC,tempD,iData[ 8],3);
	CMD4_FF(tempD,tempA,tempB,tempC,iData[ 9],7);
	CMD4_FF(tempC,tempD,tempA,tempB,iData[10],11);
	CMD4_FF(tempB,tempC,tempD,tempA,iData[11],19);
	CMD4_FF(tempA,tempB,tempC,tempD,iData[12],3);
	CMD4_FF(tempD,tempA,tempB,tempC,iData[13],7);
	CMD4_FF(tempC,tempD,tempA,tempB,iData[14],11);
	CMD4_FF(tempB,tempC,tempD,tempA,iData[15],19);

	CMD4_GG(tempA,tempB,tempC,tempD,iData[ 0],3);
	CMD4_GG(tempD,tempA,tempB,tempC,iData[ 4],5);
	CMD4_GG(tempC,tempD,tempA,tempB,iData[ 8],9);
	CMD4_GG(tempB,tempC,tempD,tempA,iData[12],13);
	CMD4_GG(tempA,tempB,tempC,tempD,iData[ 1],3);
	CMD4_GG(tempD,tempA,tempB,tempC,iData[ 5],5);
	CMD4_GG(tempC,tempD,tempA,tempB,iData[ 9],9);
	CMD4_GG(tempB,tempC,tempD,tempA,iData[13],13);
	CMD4_GG(tempA,tempB,tempC,tempD,iData[ 2],3);
	CMD4_GG(tempD,tempA,tempB,tempC,iData[ 6],5);
	CMD4_GG(tempC,tempD,tempA,tempB,iData[10],9);
	CMD4_GG(tempB,tempC,tempD,tempA,iData[14],13);
	CMD4_GG(tempA,tempB,tempC,tempD,iData[ 3],3);
	CMD4_GG(tempD,tempA,tempB,tempC,iData[ 7],5);
	CMD4_GG(tempC,tempD,tempA,tempB,iData[11],9);
	CMD4_GG(tempB,tempC,tempD,tempA,iData[15],13);

	CMD4_HH(tempA,tempB,tempC,tempD,iData[ 0],3);
	CMD4_HH(tempD,tempA,tempB,tempC,iData[ 8],9);
	CMD4_HH(tempC,tempD,tempA,tempB,iData[ 4],11);
	CMD4_HH(tempB,tempC,tempD,tempA,iData[12],15);
	CMD4_HH(tempA,tempB,tempC,tempD,iData[ 2],3);
	CMD4_HH(tempD,tempA,tempB,tempC,iData[10],9);
	CMD4_HH(tempC,tempD,tempA,tempB,iData[ 6],11);
	CMD4_HH(tempB,tempC,tempD,tempA,iData[14],15);
	CMD4_HH(tempA,tempB,tempC,tempD,iData[ 1],3);
	CMD4_HH(tempD,tempA,tempB,tempC,iData[ 9],9);
	CMD4_HH(tempC,tempD,tempA,tempB,iData[ 5],11);
	CMD4_HH(tempB,tempC,tempD,tempA,iData[13],15);
	CMD4_HH(tempA,tempB,tempC,tempD,iData[ 3],3);
	CMD4_HH(tempD,tempA,tempB,tempC,iData[11],9);
	CMD4_HH(tempC,tempD,tempA,tempB,iData[ 7],11);
	CMD4_HH(tempB,tempC,tempD,tempA,iData[15],15);
		
	iA+=tempA;
	iB+=tempB;
	iC+=tempC;
	iD+=tempD;
	}
#endif
#endif

void CMD4Impl::DoFinal(void)
	{
	iNh += iNl;
	const TUint ul128=128;
	switch (iNl&3) 
		{
		case 0:
			iData[iNl>>2] = ul128;
			break;
		case 1:
			iData[iNl>>2] += ul128<<8;
			break;
		case 2:
			iData[iNl>>2] += ul128<<16;
			break;
		case 3:
			iData[iNl>>2] += ul128<<24;
			break;
		default:
			break;
		};
	if (iNl>=56) 
		{
		if (iNl<60)
			iData[15]=0;		
		Block();
		Mem::FillZ(iData,14*sizeof(TUint));
		} 
	else
		{
		const TUint offset=(iNl+4)>>2;
		Mem::FillZ(iData+offset,(14-offset)*sizeof(TUint));
		}
   
    iData[14]=iNh<<3;//number in bits
	// this will fail if the total input length is longer than 2^32 in bits
	//(2^31 in bytes) which is roughly half a gig.
	iData[15]=0;

	Block();
	//
	// Generate hash value into iHash
	//
	TUint tmp=iA;
	iHash[0]=(TUint8)(tmp & 255);
	iHash[1]=(TUint8)((tmp >>= 8) & 255);
	iHash[2]=(TUint8)((tmp >>= 8) & 255);
	iHash[3]=(TUint8)((tmp >>= 8) & 255);

	tmp=iB;
	iHash[4]=(TUint8)(tmp & 255);
	iHash[5]=(TUint8)((tmp >>= 8) & 255);
	iHash[6]=(TUint8)((tmp >>= 8) & 255);
	iHash[7]=(TUint8)((tmp >>= 8) & 255);

	tmp=iC;
	iHash[8] =(TUint8)(tmp & 255);
	iHash[9] =(TUint8)((tmp >>= 8) & 255);
	iHash[10]=(TUint8)((tmp >>= 8) & 255);
	iHash[11]=(TUint8)((tmp >>= 8) & 255);

	tmp=iD;
	iHash[12]=(TUint8)(tmp & 255);
	iHash[13]=(TUint8)((tmp >>= 8) & 255);
	iHash[14]=(TUint8)((tmp >>= 8) & 255);
	iHash[15]=(TUint8)((tmp >>= 8) & 255);
	}

void CMD4Impl::RestoreState()
	{
	iA = iACopy;
	iB = iBCopy;
	iC = iCCopy;
	iD = iDCopy;
	iNl = iNlCopy;
	iNh = iNhCopy;	
	Mem::Copy(&iData[0], &iDataCopy[0], KMD4BlockSize*sizeof(TUint)); 
	}

void CMD4Impl::StoreState()
	{
	iACopy = iA;
	iBCopy = iB;
	iCCopy = iC;
	iDCopy = iD;
	iNlCopy = iNl;
	iNhCopy = iNh;	
	Mem::Copy(&iDataCopy[0], &iData[0], KMD4BlockSize*sizeof(TUint));
	}


// Implemented in hmacimpl.cpp or softwarehashbase.cpp
// but required as derived from MHash. No coverage here.
#ifdef _BullseyeCoverage
#pragma suppress_warnings on
#pragma BullseyeCoverage off
#pragma suppress_warnings off
#endif

TAny* CMD4Impl::GetExtension(TUid /*aExtensionId*/)
	{
	return NULL;	
	}

void CMD4Impl::SetOperationModeL(TUid /*aOperationMode*/)
	{
	User::Leave(KErrNotSupported);
	}

void CMD4Impl::SetKeyL(const CKey& /*aKey*/)
	{
	User::Leave(KErrNotSupported);
	}




