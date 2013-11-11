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


#include <s32file.h>
#include "FNTSTORE.H"
#include "FNTBODY_OLD.H"
#include <graphics/openfontconstants.h>

TBitmapCodeSectionOld::TBitmapCodeSectionOld(RHeap* aHeap)
 :	TCodeSection(),
	iHeap(aHeap),
	iOffsetsId(),
	iCharacterOffsetsListOffset(0),
	iBitmapId(),
	iBitmapOffset(0)
	{
	}

void TBitmapCodeSectionOld::InternalizeL(RReadStream &aStream)
	{
	iStart = aStream.ReadUint16L();
	iEnd = aStream.ReadUint16L();
	aStream >> iOffsetsId;
	aStream >> iBitmapId; 
	}

void TBitmapCodeSectionOld::RestoreComponentsL(const CStreamStore& aStreamStore)
	{
	if (iCharacterOffsetsListOffset == 0)
		{
		RStoreReadStream stream;
		stream.OpenLC(aStreamStore, iOffsetsId);
		InternalizeOffsetsL(stream);
		CleanupStack::PopAndDestroy();
		}
	if (iBitmapOffset == 0)
		{
		RStoreReadStream stream;
		stream.OpenLC(aStreamStore, iBitmapId);
		InternalizeBitmapL(stream);
		CleanupStack::PopAndDestroy();
		}
	}

void TBitmapCodeSectionOld::FixUpComponents(TInt aFileAddress)
	{
	TBitmapFontCharacterOffset* characterOffsetsList = (TBitmapFontCharacterOffset*) (aFileAddress + sizeof(TInt) + iOffsetsId.Value());
	iCharacterOffsetsListOffset = TInt(characterOffsetsList);
	iOffsetsId = KNullStreamId;
	TUint8* bitmap = (TUint8*) (aFileAddress + sizeof(TInt) + iBitmapId.Value());
	iBitmapOffset = TInt(bitmap);
	}

void TBitmapCodeSectionOld::DeleteComponents()
	{
	if (iCharacterOffsetsListOffset)
		{
		iHeap->Free(CharacterOffsetsList());
		iCharacterOffsetsListOffset = 0;
		}
	if (iBitmapOffset)
		{
		iHeap->Free(Bitmap());
		iBitmapOffset = 0;
		}
	}

void TBitmapCodeSectionOld::operator delete(TAny *aThis)
	{
	if (((TBitmapCodeSectionOld *)aThis)->iHeap)
		{
		((TBitmapCodeSectionOld *)aThis)->iHeap->Free(aThis);
		}
	}

void TBitmapCodeSectionOld::InternalizeOffsetsL(RReadStream &aStream)
	{
	TInt size = aStream.ReadInt32L();
	TBitmapFontCharacterOffset* characterOffsetsList = (TBitmapFontCharacterOffset*)iHeap->AllocL(sizeof(TBitmapFontCharacterOffset) * size);
	iCharacterOffsetsListOffset = TInt(characterOffsetsList) - TInt(this);
	TBitmapFontCharacterOffset* pEnd = characterOffsetsList + size;
	for (TBitmapFontCharacterOffset* p = characterOffsetsList; p < pEnd; p++)
		{
		p->InternalizeL(aStream);
		}
	}

void TBitmapCodeSectionOld::InternalizeBitmapL(RReadStream &aStream)
	{
	TInt size = aStream.ReadInt32L();
	TUint8* bitmap = (TUint8*)iHeap->AllocL(size);
	iBitmapOffset = TInt(bitmap) - TInt(this);
	aStream.ReadL(bitmap, size);
	}

TBitmapFontCharacterOffset* TBitmapCodeSectionOld::CharacterOffsetsList() const
	{
	if (iOffsetsId == KNullStreamId)
		{
		return (TBitmapFontCharacterOffset*)iCharacterOffsetsListOffset;
		}
	TInt charOffsList = TInt(this) + iCharacterOffsetsListOffset;
	return (TBitmapFontCharacterOffset*)charOffsList;
	}

TUint8* TBitmapCodeSectionOld::Bitmap() const
	{
	if (iOffsetsId == KNullStreamId)
		{
		return (TUint8*)iBitmapOffset;
		}
	TInt bitmap = TInt(this)+iBitmapOffset;
	return (TUint8*)bitmap;
	}

CFontBitmapOld::CFontBitmapOld(RHeap* aHeap, CFontStoreFile* aFontStoreFile)
 :	iHeap(aHeap),
	iFontStoreFileOffset(0),
	iUid(KNullUid),
	iPosture(0),
	iStrokeWeight(0),
	iIsProportional(0),
	iIsInRAM(!aFontStoreFile->iFileAddress),
	iUsageCount(1),
	iCellHeightInPixels(0),
	iAscentInPixels(0),
	iMaxCharWidthInPixels(0),
	iMaxNormalCharWidthInPixels(0),
	iBitmapEncoding(0),
	iNumCodeSections(0),
	iCodeSectionListOffset(0),
	iCharacterMetricsTable(aHeap)
	{
	iFontStoreFileOffset = TInt(aFontStoreFile) - TInt(this);
	}

void CFontBitmapOld::InternalizeL(RReadStream &aStream)
	{
	aStream >> iUid;
	iPosture = aStream.ReadInt8L();
	iStrokeWeight = aStream.ReadInt8L();
	iIsProportional = aStream.ReadInt8L();
	iCellHeightInPixels = aStream.ReadInt8L();
	iAscentInPixels = aStream.ReadInt8L();
	iMaxCharWidthInPixels = aStream.ReadInt8L();
	iMaxNormalCharWidthInPixels = aStream.ReadInt8L();
	iBitmapEncoding = aStream.ReadInt32L();
	iCharacterMetricsTable.InternalizeL(aStream);
	const TBool fixup = FontStoreFile()->iFileAddress;
	if (fixup)
		{
		iCharacterMetricsTable.FixUp(FontStoreFile()->iFileAddress);
		}
	iNumCodeSections = aStream.ReadInt32L();
	TBitmapCodeSectionOld* codesectionlist = (TBitmapCodeSectionOld*)User::LeaveIfNull(iHeap->AllocL(iNumCodeSections * sizeof(TBitmapCodeSectionOld)));
	iCodeSectionListOffset = TInt(codesectionlist) - TInt(this);
	for (TInt i = 0; i < iNumCodeSections; i++)
		{
		new(codesectionlist + i) TBitmapCodeSectionOld(iHeap);
		codesectionlist[i].InternalizeL(aStream);
		if (fixup)
			codesectionlist[i].FixUpComponents(FontStoreFile()->iFileAddress);
		}
	}

void CFontBitmapOld::UseL()
	{
	iUsageCount++;
	if (iUsageCount == 2)
		RestoreComponentsL();
	}

void CFontBitmapOld::Release()
	{
	iUsageCount--;
	if (!iUsageCount)
		{
		delete this;
		}
	}

/*
Get the metrics for a given character.
Return aBytes as null if the character aCode doesn't exist in the font.
*/
TBitmapFontCharacterMetrics CFontBitmapOld::CharacterMetrics(TInt aCode, const TUint8*& aBytes) const
	{
 	const TBitmapCodeSectionOld* matchSection = NULL;
	const TBitmapCodeSectionOld* const lastSection = CodeSectionList() + iNumCodeSections - 1;

	TBitmapFontCharacterOffset offset;
	aBytes = NULL;

	TBitmapFontCharacterMetrics metrics;
	const TBitmapCodeSectionOld* startSearchBand = CodeSectionList();
	TInt numCodeSectionsRemaining = iNumCodeSections;
	while (numCodeSectionsRemaining >= 1)
		{
		TInt halfNumCodeSectionsRemaining = numCodeSectionsRemaining/2;
		const TBitmapCodeSectionOld* centralSearchBand = startSearchBand+halfNumCodeSectionsRemaining;
		if ((aCode >= centralSearchBand->iStart) && (aCode <= centralSearchBand->iEnd))
			{
			matchSection = centralSearchBand;
			break;
			}
		else if ((aCode < centralSearchBand->iStart) || (centralSearchBand == lastSection))
			numCodeSectionsRemaining = halfNumCodeSectionsRemaining;
		else
			{
			startSearchBand = centralSearchBand + 1;
			numCodeSectionsRemaining -= halfNumCodeSectionsRemaining + 1;
			}
		}

	if (matchSection)
		{
		offset =* ((matchSection->CharacterOffsetsList()) + (aCode-matchSection->iStart));

		// Fill characters within code section.
		// Recursive call ensures that a valid metric is always returned.
		if (offset.iBitmapOffset == KFillCharacterOffset)
			{
			return CharacterMetrics(KReplacementCharacter, aBytes);
			}
		
		aBytes = matchSection->Bitmap() + offset.iBitmapOffset;
		
		// retrieve metric index from encoded 1 or 2 bytes
		TInt index = 0;
		TUint8 byte1 = (TUint8)*aBytes;
		const TInt switchMask = 0x1;
		const TBool oneByteIndex =! (byte1 & switchMask);
		byte1 = TUint8(byte1 >> 1);
		if (oneByteIndex)
			{
			index = byte1;
			aBytes += 1;
			}
		else
			{
			const TUint8 byte2 = (TUint8)(*(aBytes + 1));
			index = byte1 + (byte2 * 128);
			aBytes += 2;
			}
		// Copy metric from table
		metrics =* iCharacterMetricsTable.Metric(index);
		}
	return metrics;
	}

void CFontBitmapOld::operator delete(TAny *aThis)
	{
	if (((CFontBitmapOld *)aThis)->iHeap)
		{
		((CFontBitmapOld *)aThis)->iHeap->Free(aThis);
		}
	}

void CFontBitmapOld::SetPosture(TFontPosture aPosture)
	{
	iPosture = (TInt8)aPosture;
	}

TFontPosture CFontBitmapOld::Posture()
	{
	return (TFontPosture)iPosture;	 // iPosture is always smaller than TFontPosture
	}

void CFontBitmapOld::SetStrokeWeight(TFontStrokeWeight aStrokeWeight)
	{
	iStrokeWeight = (TInt8)aStrokeWeight;
	}

TFontStrokeWeight CFontBitmapOld::StrokeWeight()
	{
	return (TFontStrokeWeight)iStrokeWeight;
	}

void CFontBitmapOld::SetIsProportional(TBool aIsProportional)
	{
	iIsProportional = (TInt8)aIsProportional;
	}

TBool CFontBitmapOld::IsProportional()
	{
	return iIsProportional;
	}

CFontStoreFile* CFontBitmapOld::FontStoreFile() const
	{
	TInt fsf = TInt(this) + iFontStoreFileOffset;
	return (CFontStoreFile*)fsf;
	}

CFontBitmapOld::~CFontBitmapOld()
	{
	DeleteComponents();
	iHeap->Free(CodeSectionList());
	iCodeSectionListOffset = 0;
	}

void CFontBitmapOld::RestoreComponentsL()
	{
	if (iIsInRAM)
		{
		CStreamStore& store =* FontStoreFile()->iFileStore;
		for (TInt i = 0; i < iNumCodeSections; i++)
			{
			CodeSectionList()[i].RestoreComponentsL(store);
			}
		iCharacterMetricsTable.RestoreL(store);
		}
	}

void CFontBitmapOld::DeleteComponents()
	{
	if (iIsInRAM)
		{
		for (TInt i = 0; i < iNumCodeSections; i++)
			{
			CodeSectionList()[i].DeleteComponents();
			}
		iCharacterMetricsTable.Delete();
		}
	}

TBitmapCodeSectionOld* CFontBitmapOld::CodeSectionList() const
	{
	TInt bcs = TInt(this) + iCodeSectionListOffset;
	return (TBitmapCodeSectionOld*)bcs;
	}

