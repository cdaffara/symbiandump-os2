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


#include <e32std.h>
#include <hash.h>

#define EXPANDLOOP

//
// HMAC implementation
//

CHMAC::CHMAC()
:CMessageDigest(){}

CHMAC::CHMAC(CMessageDigest* aDigest)
	:CMessageDigest(),
	iDigest(aDigest),
	iInnerPad(KMaxBlockSize),
	iOuterPad(KMaxBlockSize)
{}

CHMAC::CHMAC(const CHMAC& aMD)
	:CMessageDigest(aMD), 
	iDigest(NULL),
	iInnerPad(aMD.iInnerPad), 
	iOuterPad(aMD.iOuterPad),
	iBlockSize(aMD.iBlockSize)
	{}

EXPORT_C CHMAC::~CHMAC(void)
	{
	delete iDigest;
	}

EXPORT_C CHMAC* CHMAC::NewL(const TDesC8& aKey,CMessageDigest* aDigest)
	{
	CHMAC* self=new(ELeave) CHMAC(aDigest);
	CleanupStack::PushL(self);
	self->InitialiseL(aKey);
	CleanupStack::Pop(self);
	return self;
	}
void CHMAC::InitialiseL(const TDesC8& aKey)
	{
	InitBlockSizeL();
	// initialisation
	if (iDigest)
		{
		iDigest->Reset();
		if( (TUint32)aKey.Size() > iBlockSize)
			{
			iInnerPad = iDigest->Final(aKey);
			}
		else 
			{
			iInnerPad = aKey;
			}
			
		TUint i;
		for (i=iInnerPad.Size();i<iBlockSize;i++)
			iInnerPad.Append(0);

		iOuterPad=iInnerPad;

		const TUint8 Magic1=0x36, Magic2=0x5c;
		for (i=0;i<iBlockSize;i++)
		{
			iInnerPad[i]^=Magic1;
			iOuterPad[i]^=Magic2;
		}
		//start inner hash
		iDigest->Hash(iInnerPad);
		}
	}

void CHMAC::InitBlockSizeL()
 	{
	
 	 iBlockSize = iDigest->BlockSize();
	 if(iBlockSize > KMaxBlockSize)
		 {
		 User::Leave(KErrNotSupported);
		 }
 	 
 	 iInnerPad.SetLength(iBlockSize);
 	 iOuterPad.SetLength(iBlockSize);
 	 iInnerPadCopy.SetLength(iBlockSize);
 	 iOuterPadCopy.SetLength(iBlockSize);
   	}

EXPORT_C CMessageDigest* CHMAC::CopyL(void)
	{
	CHMAC* that=new(ELeave) CHMAC(*this);
	CleanupStack::PushL(that);
	that->iDigest=iDigest ? iDigest->CopyL() : NULL;
	CleanupStack::Pop();
	return that;
	}
EXPORT_C CMessageDigest*  CHMAC::ReplicateL(void)
	{
	CHMAC* that=new(ELeave) CHMAC(*this);
	CleanupStack::PushL(that);
	that->iDigest=iDigest ? iDigest->ReplicateL() : NULL;
	that->Reset();
	CleanupStack::Pop();
	return that;
	}
EXPORT_C TInt CHMAC::BlockSize(void)
	{
	return iBlockSize;
	}

EXPORT_C TInt CHMAC::HashSize(void)
	{
	return iDigest ? iDigest->HashSize() : 0;
	}
EXPORT_C void CHMAC::Reset(void)
	{
	if (iDigest)
		{
		iDigest->Reset();
		iDigest->Update(iInnerPad);
		}
	}

//	JCS, There may be a more efficient method but I can't find it
//	because using the DoFinal/DoUpdate functions directly calls
//	Store/Restore at inappropriate times and scribbles over stored
//	data
//	This is the only way I've found to both generate a hash value
//	and get this in the correctly updated state
EXPORT_C TPtrC8 CHMAC::Hash(const TDesC8& aMessage)
{
	TPtrC8 ptr(KNullDesC8());
	TPtrC8 finalPtr(KNullDesC8());
	StoreState();
	if (iDigest)
	{
		ptr.Set(iDigest->Final(aMessage));
		iDigest->Update(iOuterPad);
		finalPtr.Set(iDigest->Final(ptr));
	}

	RestoreState();
	iDigest->Update(aMessage);

	return (finalPtr);
}

EXPORT_C void CHMAC::Update(const TDesC8& aMessage)
	{
	if(iDigest)
		{
		iDigest->Update(aMessage);
		}
	}

EXPORT_C TPtrC8 CHMAC::Final(const TDesC8& aMessage)
	{
	TPtrC8 ptr(KNullDesC8());
	if(iDigest)
		{
		ptr.Set(iDigest->Final(aMessage));
		iDigest->Update(iOuterPad);
		iDigest->Final(ptr);
		Reset();
		}
	return (ptr);
	}

EXPORT_C TPtrC8 CHMAC::Final()
	{
	TPtrC8 ptr(KNullDesC8());
	if(iDigest)
		{
		ptr.Set(iDigest->Final());
		iDigest->Update(iOuterPad);
		iDigest->Final(ptr);
		Reset();
		}
	return (ptr);
	}

void CHMAC::RestoreState()
{
	iOuterPad.Copy(iOuterPadCopy);
	iInnerPad.Copy(iInnerPadCopy);
	if (iDigest)
		iDigest->RestoreState();
}

void CHMAC::StoreState()
{
	iOuterPadCopy.Copy(iOuterPad);
	iInnerPadCopy.Copy(iInnerPad);
	if (iDigest)
		iDigest->StoreState();
}


