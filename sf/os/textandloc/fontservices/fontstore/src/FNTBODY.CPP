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
#include <fntstore.h>
#include "FNTBODY.H"
#include <graphics/openfontconstants.h>

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "FNTBODYTraces.h"
#endif


CFontStoreFile::CFontStoreFile()
 :	iCollectionUid(KNullUid),
	iUsageCount(1),
	iFileStore(NULL),
	iFileAddress(0),
	iDataStreamId(KNullStreamId)
	{
	}

void CFontStoreFile::ConstructL(const TParse& aParse,RFs& aFs)
	{
	TInt drive;
	User::LeaveIfError(RFs::CharToDrive(aParse.Drive()[0], drive));
	TDriveInfo driveinfo;
	User::LeaveIfError(aFs.Drive(driveinfo, drive));
	RFile file;

	// store the filename
	iFullName = aParse.FullName().AllocL();
	User::LeaveIfError(file.Open(aFs, *iFullName, EFileStream | EFileRead | EFileShareReadersOnly));

	// check to see if the fonts are stored on ROM.  Note that NAND != rom so font files in NAND devices
	// must be handled as if they were in RAM.  A NULL pointer returned by IsFileInRom means its RAM

	if (aFs.IsFileInRom(*iFullName)!=NULL)
		{
		// fonts are stored on a XIP (execute in place) device
		TInt ret = file.Seek(ESeekAddress, iFileAddress);

		if (ret != KErrNone)
			{
			file.Close();
			User::Leave(ret);
			}
		}

	// convert RFile into a CDirectFileStore
	CDirectFileStore* fileStore = CDirectFileStore::FromLC(file);

	if (fileStore->Type()[1] != TUid::Uid(KFontStoreFileUidVal))
		User::Leave(KErrNotSupported);
	TStreamId headerid = fileStore->Root();
	RStoreReadStream stream;
	stream.OpenLC(*fileStore, headerid);
	TInt fnttranversion = stream.ReadInt32L();
	// This works for version 42 (with new metrics) and for earlier versions 
	// which will synthesize the required metrics. It may have to be changed 
	// if the version number is incremented again.
	if (fnttranversion < (KFnttranVersion - 1)  && fnttranversion != KFnttran7650Version) 
		User::Leave(KErrNotSupported);
	iFontVersion = fnttranversion;
	stream >> iCollectionUid;
	iKPixelAspectRatio = stream.ReadInt32L();
	stream >> iDataStreamId;
	CleanupStack::PopAndDestroy(&stream);	// close root stream
	// ensure font data stream can be opened
	stream.OpenLC(*fileStore, iDataStreamId);
	CleanupStack::PopAndDestroy(&stream);	// close font stream
	// transfer ownership of fileStore
	CleanupStack::Pop(fileStore);
	iFileStore = fileStore;
	}

CFontStoreFile* CFontStoreFile::NewL(const TParse& aParse, RFs& aFs)
	{
	CFontStoreFile* fontstorefile = new(ELeave) CFontStoreFile;
	CleanupStack::PushL(fontstorefile);
	fontstorefile->ConstructL(aParse, aFs);
	CleanupStack::Pop();
	return fontstorefile;
	}

CFontStoreFile::~CFontStoreFile()
	{
	delete iFullName;
	iFullName = NULL;
	delete iFileStore;
	iFileStore = NULL;
	}

TBitmapCodeSection::TBitmapCodeSection()
 :	TCodeSection(),
	iCharacterData(),
	iBitmapData()
	{
	}

//This method is called  from CFontBitmap::InternalizeL.
//We have to read stream IDs from the stream, not offsets.
//Obviously the method is called once per life time of 
//CFontBitmap instance.
void TBitmapCodeSection::InternalizeL(RReadStream &aStream)
	{
	iStart = aStream.ReadUint16L();
	iEnd = aStream.ReadUint16L();
	aStream >> iCharacterData.iOffsetsId;
	aStream >> iBitmapData.iBitmapId; 
	}

//This method is called  from CFontBitmap::RestoreComponentsL - 
//if the CFontBitmap instance is in RAM and CFontBitmap::iComponentsRestored is EFalse.
//We use here stream IDs, not offsets.
//If the memory allocation for the offsets doesn't fail - aAllocMemCounter is incremented
//After calling of TBitmapCodeSection::InternalizeOffsetsL character metrics streamID is no more valid - 
//we have valid character metrics offset into RAM memory.
void TBitmapCodeSection::InternalizeOffsetsL(const CStreamStore& aStreamStore, RHeap* aHeap, TInt& aAllocMemCounter)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStreamStore, iCharacterData.iOffsetsId);

	TInt size = stream.ReadInt32L();
	TBitmapFontCharacterOffset* characterOffsetsList = (TBitmapFontCharacterOffset*)aHeap->AllocL(sizeof(TBitmapFontCharacterOffset) * size);
	aAllocMemCounter++;
	iCharacterData.iCharacterOffsetsListOffset = TInt(characterOffsetsList) - TInt(this);
	TBitmapFontCharacterOffset* pEnd = characterOffsetsList + size;
	for (TBitmapFontCharacterOffset* p = characterOffsetsList; p < pEnd; p++)
		{
		p->InternalizeL(stream);
		}

	CleanupStack::PopAndDestroy();
	}

//This method is called  from CFontBitmap::RestoreComponentsL - 
//if the CFontBitmap instance is in RAM and CFontBitmap::iComponentsRestored is EFalse.
//We use here stream IDs, not offsets.
//If the memory allocation for the bitmap doesn't fail - aAllocMemCounter is incremented
//After calling of TBitmapCodeSection::InternalizeBitmapL bitmap streamID is no more valid - 
//we have valid bitmap offset into RAM memory.
void TBitmapCodeSection::InternalizeBitmapL(const CStreamStore& aStreamStore, RHeap* aHeap, TInt& aAllocMemCounter)
	{
	RStoreReadStream stream;
	stream.OpenLC(aStreamStore, iBitmapData.iBitmapId);

	TInt size = stream.ReadInt32L();
	TUint8* bitmap = (TUint8*)aHeap->AllocL(size);
	aAllocMemCounter++;
	iBitmapData.iBitmapOffset = TInt(bitmap) - TInt(this);
	stream.ReadL(bitmap, size);

	CleanupStack::PopAndDestroy();
	}

//This method is called from CFontBitmap::InternalizeL if the
//CFontBitmap instance is in ROM.
//We use here stream IDs to calculate offsets.
//After calling of TBitmapCodeSection::FixUpComponents streamIDs are no more valid - 
//we have valid offsets into ROM memory.
//Obviously the method is called once per life time of 
//CFontBitmap instance.
void TBitmapCodeSection::FixUpComponents(TInt aFileAddress)
	{
	TBitmapFontCharacterOffset* characterOffsetsList = (TBitmapFontCharacterOffset*) (aFileAddress + sizeof(TInt) + iCharacterData.iOffsetsId);
	iCharacterData.iCharacterOffsetsListOffset = TInt(characterOffsetsList);
	TUint8* bitmap = (TUint8*) (aFileAddress + sizeof(TInt) + iBitmapData.iBitmapId);
	iBitmapData.iBitmapOffset = TInt(bitmap);
	}

//This method is caled from CFontBitmap::DeleteComponents(),
//only if the CFontBitmap instance is in RAM.
void TBitmapCodeSection::DeleteOffsets(RHeap* aHeap)
	{
	TBitmapFontCharacterOffset*  charactersOffsetsList = CharacterOffsetsList(ETrue);
	if(TUint32(this) != TUint32(charactersOffsetsList))
		{
		aHeap->Free(charactersOffsetsList);
		}
	}

//This method is caled from CFontBitmap::DeleteComponents(),
//only if the CFontBitmap instance is in RAM.
void TBitmapCodeSection::DeleteBitmap(RHeap* aHeap)
	{
	TUint8* bitmap = Bitmap(ETrue);
	if(TUint32(this) != TUint32(bitmap))
		{
		aHeap->Free(bitmap);
		}
	}

TBitmapFontCharacterOffset* TBitmapCodeSection::CharacterOffsetsList(TBool aIsInRAM) const
	{
	return reinterpret_cast <TBitmapFontCharacterOffset*> 
		(iCharacterData.iCharacterOffsetsListOffset + (aIsInRAM ? TInt(this) : 0));
	}

TUint8* TBitmapCodeSection::Bitmap(TBool aIsInRAM) const
	{
	return reinterpret_cast <TUint8*> 
		(iBitmapData.iBitmapOffset + (aIsInRAM ? TInt(this) : 0));
	}

TCharacterMetricsTable::TCharacterMetricsTable(RHeap* aHeap)
 :	iHeap(aHeap),
	iMetricsStartId(KNullStreamId),
	iCharacterMetricsStartPtr(0),
	iNumberOfMetrics(0),
	iMetricsOnHeap(EFalse)
	{}

void TCharacterMetricsTable::InternalizeL(RReadStream& aStream)
	{
	iMetricsStartId = aStream.ReadInt32L();
	iNumberOfMetrics = aStream.ReadInt32L();
	}

void TCharacterMetricsTable::InternalizeMetricsL(RReadStream& aStream)
	{
	aStream.ReadInt32L(); // size
	TBitmapFontCharacterMetrics* charactermetricslist = static_cast<TBitmapFontCharacterMetrics*>(iHeap->AllocL(sizeof(TBitmapFontCharacterMetrics) * iNumberOfMetrics));
	iMetricsOnHeap = ETrue;
	// Offset from this to location on the heap ('cos the file is not in ROM)
	iCharacterMetricsStartPtr = reinterpret_cast<TInt>(charactermetricslist) - reinterpret_cast<TInt>(this);
	TBitmapFontCharacterMetrics* pEnd = charactermetricslist + iNumberOfMetrics;
	for (TBitmapFontCharacterMetrics* p = charactermetricslist; p < pEnd; p++)
		{
		p->InternalizeL(aStream);
		}
	}

void TCharacterMetricsTable::RestoreL(const CStreamStore& aStreamStore)
	{
	if (iCharacterMetricsStartPtr == 0)
		{	// We haven't already read it in from RAM file
		RStoreReadStream stream;
		stream.OpenLC(aStreamStore, iMetricsStartId);
		InternalizeMetricsL(stream);
		CleanupStack::PopAndDestroy();
		}
	}

void TCharacterMetricsTable::FixUp(TInt aFileAddress)
	{
	TBitmapFontCharacterMetrics* charactermetricslist = reinterpret_cast<TBitmapFontCharacterMetrics*>(aFileAddress + sizeof(TInt) + iMetricsStartId.Value());
	iCharacterMetricsStartPtr = TInt(charactermetricslist);	// Ptr to location in a ROM file
	iMetricsStartId = KNullStreamId;
	iMetricsOnHeap = EFalse;
	}

void TCharacterMetricsTable::Delete()
	{	// This routine is only called if the font file is in RAM, not ROM, and therefore the metrics have been stashed on the heap
    if (iMetricsOnHeap && iCharacterMetricsStartPtr)
        {
        iHeap->Free(reinterpret_cast<TAny*>(MetricsFromOffset(0)));
        iCharacterMetricsStartPtr = 0;
        iMetricsOnHeap = EFalse;
        }
	}

const TBitmapFontCharacterMetrics* TCharacterMetricsTable::Metric(TInt aIndex) const
	{
	if ((aIndex < 0) || (aIndex > iNumberOfMetrics))
	    {
	    OstTraceExt2( TRACE_FATAL, TCHARACTERMETRICSTABLE_METRIC, "TCharacterMetricsTable::Metric, aIndex=%d, iNumberOfMetrics=%d, Panic(EFntMetricsIndexOutOfBounds)", aIndex, iNumberOfMetrics);
	    __ASSERT_DEBUG(0, Panic(EFntMetricsIndexOutOfBounds));
	    }
    // Sometimes the start ptr is to a metrics heap item and sometimes it points into a ROM file
    if (iMetricsOnHeap)
        {
        // Start ptr is to metrics heap item
        return MetricsFromOffset(aIndex);
        }
    else
        {
        // Start ptr is to a file in ROM
        return reinterpret_cast<const TBitmapFontCharacterMetrics*> (iCharacterMetricsStartPtr + (aIndex * sizeof(TBitmapFontCharacterMetrics)));
        }
	}

TInt TCharacterMetricsTable::NumberOfMetrics() const
	{
	return iNumberOfMetrics;
	}

TBitmapFontCharacterMetrics* TCharacterMetricsTable::MetricsFromOffset(TInt aIndex) const
    {
    __ASSERT_DEBUG(iMetricsOnHeap,Panic(EFntMetricsNotOnHeap));
    return reinterpret_cast<TBitmapFontCharacterMetrics*>(reinterpret_cast<TInt>(this) + iCharacterMetricsStartPtr+ (aIndex * sizeof(TBitmapFontCharacterMetrics)));
    }

CFontBitmap::CFontBitmap(RHeap* aHeap, CFontStoreFile* aFontStoreFile)
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
	iCharacterMetricsTable(aHeap),
	iComponentsRestored(EFalse),
	iAllocMemCounter_Offsets(0),
	iAllocMemCounter_Bitmaps(0),
	iFontCapitalAscent(0),
	iFontMaxAscent(0),
	iFontStandardDescent(0),
	iFontMaxDescent(0),
	iFontLineGap(0)
	{
	iFontStoreFileOffset = TInt(aFontStoreFile) - TInt(this);
	}

void CFontBitmap::InternalizeL(RReadStream &aStream, TInt aFontVersion)
	{
	aStream >> iUid;
	iPosture = aStream.ReadInt8L();
	iStrokeWeight = aStream.ReadInt8L();
	iIsProportional = aStream.ReadInt8L();
	iCellHeightInPixels = aStream.ReadInt8L();
	iAscentInPixels = aStream.ReadInt8L();
	iMaxCharWidthInPixels = aStream.ReadInt8L();
	iMaxNormalCharWidthInPixels = aStream.ReadInt8L();
	if ( aFontVersion  >= KFnttranVersion )
		{ // read the new metrics in
		iFontCapitalAscent = aStream.ReadInt8L();
		iFontMaxAscent = aStream.ReadInt8L();
		iFontStandardDescent = aStream.ReadInt8L();
		iFontMaxDescent = aStream.ReadInt8L();
		iFontLineGap = aStream.ReadInt8L();
		}
	else // synthesize the extra metrics (data compatibility with third party bitmap fonts for old phones)
		{
		iFontMaxAscent = iFontCapitalAscent = iAscentInPixels;
		iFontMaxDescent = iFontStandardDescent = iCellHeightInPixels - iAscentInPixels;
		iFontLineGap = ( ( iCellHeightInPixels * 12 ) + 5) / 10;  // 1.2 times design height
		}	
	iBitmapEncoding = aStream.ReadInt32L();
	iCharacterMetricsTable.InternalizeL(aStream);
	const TBool fixup = FontStoreFile()->iFileAddress;
	if (fixup)
		{
		iCharacterMetricsTable.FixUp(FontStoreFile()->iFileAddress);
		}
	iNumCodeSections = aStream.ReadInt32L();
	TBitmapCodeSection* codesectionlist = (TBitmapCodeSection*)User::LeaveIfNull(iHeap->AllocL(iNumCodeSections * sizeof(TBitmapCodeSection)));
	iCodeSectionListOffset = TInt(codesectionlist) - TInt(this);
	for (TInt i = 0; i < iNumCodeSections; i++)
		{
		new(codesectionlist + i) TBitmapCodeSection;
		codesectionlist[i].InternalizeL(aStream);
		if (fixup)
			codesectionlist[i].FixUpComponents(FontStoreFile()->iFileAddress);
		}
	}

void CFontBitmap::UseL()
	{
	// Note object is created with a Usage Count of 1.
	// So incrementing to 2 normally indicates the first external reference.
	iUsageCount++;
	if (iUsageCount == 2)
		{
		RestoreComponentsL();
		}
	}

void CFontBitmap::Release()
	{
	iUsageCount--;
	if (!iUsageCount)
		{ // object and all its users have closed
		delete this;
		}
	}

/*
Get the metrics for a given character.
Return aBytes as null if the character aCode doesn't exist in the font.
*/
TBitmapFontCharacterMetrics CFontBitmap::CharacterMetrics(TInt aCode, const TUint8*& aBytes) const
	{
 	const TBitmapCodeSection* matchSection = NULL;
	const TBitmapCodeSection* const lastSection = CodeSectionList() + iNumCodeSections - 1;

	TBitmapFontCharacterOffset offset;
	aBytes = NULL;

	TBitmapFontCharacterMetrics metrics;
	const TBitmapCodeSection* startSearchBand = CodeSectionList();
	TInt numCodeSectionsRemaining = iNumCodeSections;
	while (numCodeSectionsRemaining >= 1)
		{
		TInt halfNumCodeSectionsRemaining = numCodeSectionsRemaining/2;
		const TBitmapCodeSection* centralSearchBand = startSearchBand+halfNumCodeSectionsRemaining;
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
		offset =* ((matchSection->CharacterOffsetsList(iIsInRAM)) + (aCode-matchSection->iStart));

		// Fill characters within code section.
		// Recursive call ensures that a valid metric is always returned.
		if (offset.iBitmapOffset == KFillCharacterOffset)
			{
			return CharacterMetrics(KReplacementCharacter, aBytes);
			}
		
		aBytes = matchSection->Bitmap(iIsInRAM) + offset.iBitmapOffset;
		
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

void CFontBitmap::operator delete(TAny *aThis)
	{
	if (((CFontBitmap *)aThis)->iHeap)
		{
		((CFontBitmap *)aThis)->iHeap->Free(aThis);
		}
	}

void CFontBitmap::SetPosture(TFontPosture aPosture)
	{
	iPosture = (TInt8)aPosture;
	}

TFontPosture CFontBitmap::Posture() const
	{
	return (TFontPosture)iPosture;	 // iPosture is always smaller than TFontPosture
	}

void CFontBitmap::SetStrokeWeight(TFontStrokeWeight aStrokeWeight)
	{
	iStrokeWeight = (TInt8)aStrokeWeight;
	}

TFontStrokeWeight CFontBitmap::StrokeWeight() const
	{
	return (TFontStrokeWeight)iStrokeWeight;
	}

void CFontBitmap::SetIsProportional(TBool aIsProportional)
	{
	iIsProportional = (TInt8)aIsProportional;
	}

TBool CFontBitmap::IsProportional() const
	{
	return iIsProportional;
	}

CFontStoreFile* CFontBitmap::FontStoreFile() const
	{
	TInt fsf = TInt(this) + iFontStoreFileOffset;
	return (CFontStoreFile*)fsf;
	}

CFontBitmap::~CFontBitmap()
	{
	DeleteComponents();
	TBitmapCodeSection* codeSectionList = CodeSectionList();
	if(TUint32(this) != TUint32(codeSectionList))
		{
		iHeap->Free(codeSectionList);
		}
	iCodeSectionListOffset = 0;
	}

//We have to count how many offsets and bitmaps are allocated successfully because if
//some of codesection's Internalize..L fails we have to deallocate the right amount of 
//data.
void CFontBitmap::RestoreComponentsL()
	{
	if (iIsInRAM)
		{
		if(!iComponentsRestored)
			{
			iAllocMemCounter_Offsets = 0;
			iAllocMemCounter_Bitmaps = 0;
			CStreamStore& store =* FontStoreFile()->iFileStore;
			for (TInt i = 0; i < iNumCodeSections; i++)
				{
				CodeSectionList()[i].InternalizeOffsetsL(store, iHeap, iAllocMemCounter_Offsets);
				CodeSectionList()[i].InternalizeBitmapL(store, iHeap, iAllocMemCounter_Bitmaps);
				}
			iCharacterMetricsTable.RestoreL(store);
			}
		iComponentsRestored = ETrue;
		}
	}

void CFontBitmap::DeleteComponents()
	{
	if (iIsInRAM)
		{
		TInt i;
		for (i = 0; i < iAllocMemCounter_Offsets; i++)
			{
			CodeSectionList()[i].DeleteOffsets(iHeap);
			}
		for (i = 0; i < iAllocMemCounter_Bitmaps; i++)
			{
			CodeSectionList()[i].DeleteBitmap(iHeap);
			}
		iCharacterMetricsTable.Delete();
		}
	iAllocMemCounter_Offsets = 0;
	iAllocMemCounter_Bitmaps = 0;
	iComponentsRestored = EFalse;
	}

TBitmapCodeSection* CFontBitmap::CodeSectionList() const
	{
	TInt bcs = TInt(this) + iCodeSectionListOffset;
	return (TBitmapCodeSection*)bcs;
	}

TTypefaceFontBitmap::TTypefaceFontBitmap()
 :	iTypeface(NULL),
	iFontBitmap(NULL),
	iHeightFactor(1),
	iWidthFactor(1)
	{
	}

TTypefaceFontBitmap::TTypefaceFontBitmap(TTypeface* aTypeface,CFontBitmap* aFontBitmap)
 :	iTypeface(aTypeface),
	iFontBitmap(aFontBitmap),
	iHeightFactor(1),
	iWidthFactor(1)
	{
	}

TInt TTypefaceFontBitmap::HeightInPixels() const
	{
	return iFontBitmap->iCellHeightInPixels * iHeightFactor;
	}
