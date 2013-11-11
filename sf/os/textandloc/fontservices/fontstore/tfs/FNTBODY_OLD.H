/*
* Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __FNTBODY_OLD_H__
#define __FNTBODY_OLD_H__

#include <e32std.h>
#include "FNTSTD.H"
#include "FNTBODY.H"

class TBitmapCodeSectionOld : public TCodeSection
	{
	public:			  
	TBitmapCodeSectionOld(RHeap* aHeap);
	void InternalizeL(RReadStream& aStream);
	void RestoreComponentsL(const CStreamStore& aStreamStore);
	void FixUpComponents(TInt aFileAddress);
	void DeleteComponents();
	void operator delete(TAny*);
	void operator delete(TAny*,TAny*) {} // To prevent compiler warning
	TBitmapFontCharacterOffset* CharacterOffsetsList() const;
	TUint8* Bitmap() const;

	private:
	void InternalizeOffsetsL(RReadStream& aStream);
	void InternalizeBitmapL(RReadStream& aStream);

	RHeap* iHeap;
	TStreamId iOffsetsId;
	TInt iCharacterOffsetsListOffset;
	TStreamId iBitmapId;
	TInt iBitmapOffset;
	};

class CFontBitmapOld : public CBase
	{
public:
	CFontBitmapOld(RHeap* aHeap,CFontStoreFile* aFontStoreFile);
	void InternalizeL(RReadStream &aStream);
	void UseL();
	void Release();
	TBitmapFontCharacterMetrics CharacterMetrics(TInt aCode,const TUint8*& aBytes) const;
	void operator delete(TAny*);
	void operator delete(TAny*,TAny*) {} // To prevent compiler warning
	void SetPosture(TFontPosture aPosture);
	TFontPosture Posture();
	void SetStrokeWeight(TFontStrokeWeight aStrokeWeight);
	TFontStrokeWeight StrokeWeight();
	void SetIsProportional(TBool aIsProportional);
	TBool IsProportional();
	CFontStoreFile* FontStoreFile() const;
protected:
	~CFontBitmapOld();
	void RestoreComponentsL();	 //  Has reference to FontStoreFile
	void DeleteComponents();   
	TBitmapCodeSectionOld* CodeSectionList() const;
	TCharacterMetricsTable* MetricsTable() const;
public:
	RHeap* iHeap;
	TInt iFontStoreFileOffset;
	TUid iUid;
protected:
	TInt8 iPosture;
	TInt8 iStrokeWeight;
	TInt8 iIsProportional;
	TBool iIsInRAM;
	TInt iUsageCount;
public:
	TInt8 iCellHeightInPixels;
	TInt8 iAscentInPixels;
	TInt8 iMaxCharWidthInPixels;
	TInt8 iMaxNormalCharWidthInPixels;
	TInt iBitmapEncoding;
	TInt iNumCodeSections;
	TInt iCodeSectionListOffset;
private:
	TCharacterMetricsTable iCharacterMetricsTable;
	};

#endif // __FNTBODY_OLD_H__
