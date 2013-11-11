/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* PRIVATE HEADER FILE CONTAINING IMPLEMENTATION SPECIFIC CLASSES
*
*/


#ifndef __TXTFMSTM_H__
#define __TXTFMSTM_H__

#include <e32std.h>
#include <txtfrmat.h>

// forward declarations
class RReadStream;
class RWriteStream;
class TCharFormatX;
class TCharFormatXMask;

/**
stores format attributes as bytecode, this class is intended for internal use only. 
@publishedAll
*/
class RFormatStream
	{
public:
	RFormatStream();
	void CopyL(const RFormatStream& aSource);
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	void Reset();
	const TUint8* Ptr(TInt& aSize) const;
	void SetParaFormatL(const CParaFormat& aDesiredFormat,const TParaFormatMask& aDesiredMask,const CParaFormat& aCurrentFormat);
	void SenseParaFormatL(CParaFormat& aParaFormat,TParaFormatMask& aMask,CParaFormat::TParaFormatGetMode aMode)const;
	void SetCharFormatL(const TCharFormatX& aCharFormat,const TCharFormatXMask& aMask);
	void SenseCharFormat(TCharFormatX& aCharFormat,TCharFormatXMask& aMask)const;
	void Swap(RFormatStream& aStream);

private:
	void AllocL(TInt aSize);
	TInt Length(TUint8*& aPtr,TTextFormatAttribute aType)const;
	TInt DoCalcSizeParaFormat(const CParaFormat& aDesiredFormat,TParaFormatMask aMask,const CParaFormat& aCurrentFormat);
	void DoSetParaFormat(const CParaFormat& aDesiredFormat,TParaFormatMask aMask,const CParaFormat& aCurrentFormat);
	void RemoveRedundantCharFormat(TCharFormatMask& aMask,const TCharFormatX& aFormat,const TCharFormatX& aEffectiveFormat);
	TInt DoCalcSizeCharFormat(const TCharFormatX& aCharFormat,const TCharFormatXMask& aMask);
	void DoStoreCharFormat(const TCharFormatX& aCharFormat,TCharFormatXMask aMask);
	TUint8* StoreBullet(TUint8* aPtr,const TBullet& aSource);
	TUint8* StoreBorder(TUint8* aPtr,TTextFormatAttribute aType,const TParaBorder& aSource);
	TUint8* StoreTab(TUint8* aPtr,const TTabStop& aSource);
	TInt StoreTabs(TUint8*& aPtr,const CParaFormat& aDesiredFormat,const CParaFormat& aCurrentFormat,TBool aStoreData);
	TInt MergeTabLists(TUint8*& aPtr,const CParaFormat& aDesiredFormat,TInt aDesiredTabCount,
					   const CParaFormat& aCurrentFormat,TInt aCurrentTabCount,TBool aStoreData);
	void StoreAllTabs(TUint8*& aPtr,const CParaFormat& aSource);
	TUint8* Store(TUint8* aPtr,const TLogicalRgb& aRgb);
	TUint8* Store(TUint8* aPtr,const TTypeface& aTypeface);
	TUint8* ReadValue(TUint8* aPtr,TLogicalRgb& aRgb)const;
	TUint8* ReadValue(TUint8* aPtr,TTypeface& aTypeface)const;
	TUint8* ReadValue(TUint8* aPtr,TParaBorder& aBorder)const;
	TUint8* ReadValue(TUint8* aPtr,TBullet& aBullet)const;
	TUint8* ReadTabL(TUint8* aPtr,CParaFormat& aTarget)const;

	TUint8* iBase;	// start of stored bytecode
	TUint8* iEnd;	// end of stored bytecode

	__DECLARE_TEST;
private:
	void TestInvariantL() const;
	TBool DoInvariantCheck() const;
	};

#endif
