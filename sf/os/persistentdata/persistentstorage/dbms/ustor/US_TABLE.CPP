// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "US_STD.H"

//#define __READBIT_CLASS
//#define __WRITEBIT_CLASS

#ifdef __READBIT_CLASS
class TReadBitSequence
	{
public:
	inline TReadBitSequence();
	inline TBool HasBits() const;
	TUint Read(const TUint8*& aPtr);
private:
	TUint iBits;
	};
inline TReadBitSequence::TReadBitSequence()
	:iBits(0) {}
inline TBool TReadBitSequence::HasBits() const
	{return (iBits&0x2000000);}
TUint TReadBitSequence::Read(const TUint8*& aPtr)
	{
	iBits>>=1;
	if ((iBits&0x1000000)==0)
		iBits=*aPtr++ | 0xff000000u;
	return iBits&1;
	}
#define READBITINIT(This) TReadBitSequence This
#define READHASBITS(This) This.HasBits()
#define READBIT(This,ptr) This.Read(ptr)
#define SKIPBIT(This,ptr) READBIT(This,ptr)
#else
#define READBITINIT(This) TUint This##_bits=0
#define READHASBITS(This) (This##_bits&0x2000000)
#define SKIPBIT(This,ptr) (This##_bits>>=1,(This##_bits&0x1000000 ? void(0) : void(This##_bits=*ptr++|0xff000000u)))
#define READBIT(This,ptr) (SKIPBIT(This,ptr),This##_bits&1)
#endif

#ifdef __WRITEBIT_CLASS
class TWriteBitSequence
	{
public:
	inline TWriteBitSequence();
	TUint8* Write(TUint8* aPtr,TUint aBit);
	void Flush();
private:
	TUint iBits;
	TUint8* iPtr;
	};
inline TWriteBitSequence::TWriteBitSequence()
	:iBits(0),iPtr(0) {}
TUint8* TWriteBitSequence::Write(TUint8* aPtr,TUint aBit)
	{
	TUint bits=iBits>>1;
	if ((bits&0x1000000)==0)
		{
		if (iPtr)
			*iPtr=TUint8(bits);
		iPtr=aPtr++;
		bits=0xff000000;
		}
	if (aBit)
		bits|=0x100;
	iBits=bits;
	return aPtr;
	}
void TWriteBitSequence::Flush()
	{
	TUint8* ptr=iPtr;
	if (ptr)
		{
		TUint bits=iBits;
		do bits>>=1; while (bits&0x1000000);
		*ptr=TUint8(bits);
		}
	}
#define WRITEBITINIT(This) TWriteBitSequence This
#define WRITEBIT(This,ptr,bit) ptr=This.Write(ptr,bit)
#define WRITEZEROBIT(This,ptr) ptr=This.Write(ptr,0)
#define FLUSHBITS(This) This.Flush()
#else
#define WRITEBITINIT(This) TUint32 This##_bits=0; TUint8* This##_ptr=0
#define NEXTBIT(This,ptr) This##_bits>>=1;if ((This##_bits&0x1000000)==0){if (This##_ptr) *This##_ptr=TUint8(This##_bits);This##_ptr=ptr++;This##_bits=0xff000000;}
#define WRITEZEROBIT(This,ptr) {NEXTBIT(This,ptr)}
#define WRITEBIT(This,ptr,bit) {NEXTBIT(This,ptr) if (bit) This##_bits|=0x100;}
#define FLUSHBITS(This) {if (This##_ptr){do This##_bits>>=1; while (This##_bits&0x1000000);*This##_ptr=TUint8(This##_bits);}}
#endif

LOCAL_C const TUint8* Read32(const TUint8* aPtr,TUint32* aDest)
// fast read for non-aligned little-endian 32-bit data
	{
	TUint32 x=*aPtr++;
	x|=*aPtr++<<8;
	x|=*aPtr++<<16;
	x|=*aPtr++<<24;
	*aDest=x;
	return aPtr;
	}

LOCAL_C TUint8* Write32(TUint8* aPtr,TUint32 aVal)
// fast transfer for non-aligned little-endian 32-bit data
	{
	*aPtr++=TUint8(aVal);
	*aPtr++=TUint8(aVal>>8);
	*aPtr++=TUint8(aVal>>16);
	*aPtr++=TUint8(aVal>>24);
	return aPtr;
	}

inline const TUint8* Read16(const TUint8* aPtr,TUint32* aDest)
// Read unsigned 16-bit value into aDest storage
	{
	TUint x=*aPtr++;
	x|=*aPtr++<<8;
	*aDest=TUint16(x);
	return aPtr;
	}

inline const TUint8* Read16s(const TUint8* aPtr,TUint32* aDest)
// Read signed 16-bit value into aDest storage
	{
	TInt x=*aPtr++<<16;
	x|=*aPtr++<<24;
	*aDest=x>>16;
	return aPtr;
	}

inline TUint8* Write16(TUint8* aPtr,TUint aVal)
// Write little-endian rep of the low 16 bits of aVal
	{
	*aPtr++=TUint8(aVal);
	*aPtr++=TUint8(aVal>>8);
	return aPtr;
	}

LOCAL_C TUint8* WriteCardinality(TUint8* aPtr,TUint aVal)
// compress cardinality into the data stream
	{
	__ASSERT(aVal<(1u<<30));
	if ((aVal>>7)==0)
		{
		*aPtr++=TUint8(aVal<<1);
		return aPtr;
		}
	aVal=(aVal<<2)|1;
	if ((aVal>>16)==0)
		{
		*aPtr++=TUint8(aVal);
		*aPtr++=TUint8(aVal>>8);
		return aPtr;
		}
	return Write32(aPtr,aVal|2);
	}

LOCAL_C TUint ReadCardinality(const TUint8* aPtr)
// extract cardinality from the data stream
	{
	TUint x=aPtr[0];
	if ((x&1)==0)
		return x>>1;
	x|=aPtr[1]<<8;
	if (x&2)
		{
		x|=aPtr[2]<<16;
		x|=aPtr[3]<<24;
		}
	return x>>2;
	}

LOCAL_C const TUint8* ReadCardinality(const TUint8* aPtr,TInt& aVal)
// extract cardinality from the data stream
	{
	TUint x=*aPtr++;
	if ((x&1)==0)
		x>>=1;
	else
		{
		x|=*aPtr++<<8;
		if (x&2)
			{
			x|=*aPtr++<<16;
			x|=*aPtr++<<24;
			}
		x>>=2;
		}
	aVal=x;
	return aPtr;
	}

LOCAL_C TInt SizeOfCardinality(TUint aVal)
// report the externalized size of the Compressed value in bytes
	{
	if ((aVal>>7)==0)
		return 1;
	return (aVal>>14)==0 ? 2 : 4;
	}

LOCAL_C TUint8* WriteBlobId(TUint8* aPtr,TDbBlobId aId)
	{
	return WriteCardinality(aPtr,(aId>>24)|(aId<<8>>4));
	}

LOCAL_C const TUint8* ReadBlobId(const TUint8* aPtr,TDbBlobId& aId)
	{
	aPtr=ReadCardinality(aPtr,*reinterpret_cast<TInt*>(&aId));
	aId=(aId>>4)|(aId<<28>>4);
	return aPtr;
	}

LOCAL_C TInt SizeOfBlobId(TDbBlobId aId)
	{
	return SizeOfCardinality((aId>>24)|(aId<<8>>4));
	}

// Class CDbStoreTable

CDbStoreTable::CDbStoreTable(CDbStoreDatabase& aDatabase,const CDbTableDef& aDef)
	: CDbTable(aDatabase,aDef)
	{}

CDbRecordSpace* CDbStoreTable::RecordSpaceL()
//
// open records handler
//
	{
	return CDbStoreRecords::NewL(Database().ClusterCacheL(),Def());
	}

CDbBlobSpace* CDbStoreTable::BlobSpaceL()
//
// Open a blobs accessor for the table
//
	{
	return new(ELeave) CDbStoreBlobs(Database(),Def().InlineLimit());
	}

CDbRecordIndex* CDbStoreTable::RecordIndexL(const CDbTableIndexDef& anIndex)
//
// Open an index
//
	{
	return CDbStoreIndex::NewL(Database(),(const CDbStoreIndexDef&)anIndex,Def());
	}

static TUint8* CompressUnicode(TUint8* aRec,const TAny* aData,TInt aSize)
//
// initially assume the compressed data requires 1 byte for the length data
// Copy it if more is necessary. This avoids having to run the compressor twice
//
	{
	TMemoryUnicodeSource source(reinterpret_cast<const TUint16*>(aData));
	TUnicodeCompressor compressor;
	TInt output;
	compressor.CompressL(aRec+1,source,KMaxTInt,aSize>>1,&output);
	TInt lenSize=SizeOfCardinality(output);
	if (lenSize!=1)
		Mem::Copy(aRec+lenSize,aRec+1,output);	// needs more space for length
	return WriteCardinality(aRec,output)+output;
	}

static TInt SizeOfCompressedUnicode(const TAny* aData,TInt aSize)
//
// bytes required to store the unicode data
//
	{
	TMemoryUnicodeSource source(reinterpret_cast<const TUint16*>(aData));
	TInt size=TUnicodeCompressor::CompressedSizeL(source,aSize>>1);
	return SizeOfCardinality(size)+size;
	}

static const TUint8* ExpandUnicodeL(const TUint8* aRec,TAny* aTarget,const TAny* aLimit,TInt& aChars)
//
// Read compressed unicode from the record
//
	{
	TInt bytes;
	aRec=ReadCardinality(aRec,bytes);
	TMemoryUnicodeSink unicode(reinterpret_cast<TUint16*>(aTarget));
	TUnicodeExpander expander;
	TInt used;
	expander.ExpandL(unicode,aRec,(TUint16*)aLimit-(TUint16*)aTarget,bytes,&aChars,&used);
	return bytes==used ? aRec+bytes : 0;	// signal corruption in data, could not fit text in space
	}

static TInt SizeOfExpandedUnicodeL(const TUint8* aData,TInt aSize)
//
// bytes required to store the unicode data
//
	{
	return TUnicodeExpander::ExpandedSizeL(aData,aSize)<<1;
	}

TInt CDbStoreTable::RecordLength(const RDbRow& aRow)
//
// Calculate the size of a record
//
	{
	TInt bits=SizeOfCardinality(OptimizedRowLength(aRow))<<3;		// record buffer size
	HDbColumnSet::TIteratorC col=Def().Columns().Begin();
	const TDbCell* const last=aRow.Last();
	for (const TDbCell* column=aRow.First();column<last;++col,column=column->Next())
		{
		__ASSERT(col<Def().Columns().End());	// columns off end
		TInt size=column->Length();
		if ((col->iAttributes&TDbCol::ENotNull)==0)
			{	//nullable
			++bits;
			if (size==0)
				continue;			// no data
			}
		__ASSERT(size>0);	// must be non-null to reach here
		TDbColType type=col->Type();
		__ASSERT(type>=EDbColBit&&type<=EDbColLongBinary);
		if (type==EDbColBit)
			++bits;
		else if (type<=EDbColDateTime)
			bits+=TRecordSize::FixedFieldSize(type)<<3;
		else if (type==EDbColText16)
			bits+=SizeOfCompressedUnicode(column->Data(),size)<<3;
		else if (type<=EDbColBinary)
			bits+=8+(size<<3);
		else
			{
			__ASSERT(type<=EDbColLongBinary);
			const TDbBlob& blob=*(const TDbBlob*)column->Data();
			if (!blob.IsInline())
				bits+=1+((SizeOfBlobId(blob.Id())+SizeOfCardinality(blob.Size()))<<3);
			else if (type!=EDbColLongText16)
				bits+=9+(blob.Size()<<3);
			else
				bits+=1+(SizeOfCompressedUnicode(blob.Data(),blob.Size())<<3);
			}
		}
	__ASSERT(bits<=(KDbStoreMaxRecordLength<<3));
	return (bits+7)>>3;
	}

TInt CDbStoreTable::OptimizedRowLength(const RDbRow& aRow)
//
// Calculate the minimal row buffer size (in words) to store the row data
//
	{
	HDbColumnSet::TIteratorC col=Def().Columns().Begin();
	const TDbCell* const last=aRow.Last();
	TInt cellHead=0;
	TInt words=0;
	for (const TDbCell* column=aRow.First();column<last;++col,column=column->Next())
		{
		++cellHead;
		__ASSERT(col<Def().Columns().End());	// columns off end
		TInt size=column->Length();
		if (size==0)
			continue;
		words+=cellHead;
		cellHead=0;
		TDbColType type=col->Type();
		__ASSERT(type>=EDbColBit&&type<=EDbColLongBinary);
		if (type<=EDbColDateTime)
			__ASSERT(size==(size>>2<<2));
		else if (type<=EDbColBinary)
			;
		else
			size=((const TDbBlob*)column->Data())->CellSize();
		words+=(size+3)>>2;
		}
	return words;
	}

void CDbStoreTable::CopyFromRow(TUint8* aRecord,const RDbRow& aRow)
//
// translate the row buffer into its persistent format in the record buffer
//
	{
	aRecord=WriteCardinality(aRecord,OptimizedRowLength(aRow));	// internal size
//
	WRITEBITINIT(bits);
	HDbColumnSet::TIteratorC iter=Def().Columns().Begin();
	const TDbCell* const last=aRow.Last();
	for (const TDbCell* column=aRow.First();column<last;++iter,column=column->Next())
		{
		__ASSERT(iter<Def().Columns().End());	// columns off end
		TInt size=column->Length();
		if ((iter->iAttributes&TDbCol::ENotNull)==0)
			{	// nullable
			WRITEBIT(bits,aRecord,size!=0);
			if (size==0)
				continue;			// no data
			}
		__ASSERT(size>0);	// must be non-null to reach here
		const TUint32* data=(const TUint32*)column->Data();
		TDbColType type=iter->Type();
		switch (type)
			{
		default:
			__ASSERT(0);
		case EDbColBit:
			WRITEBIT(bits,aRecord,*data);
			break;
		case EDbColInt8:
		case EDbColUint8:
			*aRecord++=TUint8(*data);
			break;
		case EDbColInt16:
		case EDbColUint16:
			aRecord=Write16(aRecord,*data);
			break;
#if defined(__DOUBLE_WORDS_SWAPPED__)
		case EDbColReal64:
			aRecord=Write32(aRecord,data[1]);	// write low word out first
			// drop through to write high word next
#endif
		case EDbColInt32:
		case EDbColUint32:
		case EDbColReal32:
			aRecord=Write32(aRecord,*data);
			break;
#if !defined(__DOUBLE_WORDS_SWAPPED__)
		case EDbColReal64:
#endif
		case EDbColInt64:
		case EDbColDateTime:
			aRecord=Write32(aRecord,data[0]);
			aRecord=Write32(aRecord,data[1]);
			break;
		case EDbColText16:
			aRecord=CompressUnicode(aRecord,data,size);
			break;
		case EDbColText8:
		case EDbColBinary:
			*aRecord++=TUint8(size);
			aRecord=Mem::Copy(aRecord,data,size);
			break;
		case EDbColLongText8:
		case EDbColLongText16:
		case EDbColLongBinary:
			{
			const TDbBlob& blob=*reinterpret_cast<const TDbBlob*>(data);
			size=blob.Size();
			WRITEBIT(bits,aRecord,blob.IsInline());
			if (blob.IsInline())
				{
				if (type==EDbColLongText16)
					aRecord=CompressUnicode(aRecord,blob.Data(),size);
				else
					{
					*aRecord++=TUint8(size);
					aRecord=Mem::Copy(aRecord,blob.Data(),size);
					}
				}
			else
				{
				aRecord=WriteBlobId(aRecord,blob.Id());
				aRecord=WriteCardinality(aRecord,size);
				}
			}
			break;
			}
		}
	FLUSHBITS(bits);
	}

const TUint8* CDbStoreTable::CopyToRowL(TDbCell* aCell,TInt aSize,const TUint8* aRec)
//
// translate persistent record into the row buffer
//
	{
	__ASSERT(aSize>0);
//
	const TDbCell* const end=PtrAdd(aCell,aSize);
	READBITINIT(bits);
	HDbColumnSet::TIteratorC col=Def().Columns().Begin();
	HDbColumnSet::TIteratorC const cend=Def().Columns().End();
	for (;;)
		{
		TInt size=0; //null data
		if (col->iAttributes&TDbCol::ENotNull || READBIT(bits,aRec))	// have data
			{
			if (TInt(end)-TInt(aCell)<=(TInt)sizeof(TUint32))
				return 0;
			size=sizeof(TUint32);	// for most types
			TDbColType type=col->Type();
			switch (type)
				{
			default:
				__ASSERT(0);
			case EDbColBit:
				*(TUint32*)aCell->Data()=READBIT(bits,aRec);
				break;
			case EDbColInt8:
				*(TInt32*)aCell->Data()=*(const TInt8*)aRec;
				aRec+=sizeof(TInt8);
				break;
			case EDbColUint8:
				*(TUint32*)aCell->Data()=*aRec;
				aRec+=sizeof(TUint8);
				break;
			case EDbColInt16:
				aRec=Read16s(aRec,(TUint32*)aCell->Data());
				break;
			case EDbColUint16:
				aRec=Read16(aRec,(TUint32*)aCell->Data());
				break;
	#if defined(__DOUBLE_WORDS_SWAPPED__)
			case EDbColReal64:
				if (TInt(end)-TInt(aCell)<=(TInt)sizeof(TReal64))
					return 0;
				size=sizeof(TReal64);
				aRec=Read32(aRec,(TUint32*)aCell->Data()+1);	// transfer low word first, to high address
				// drop through to transfer high word to low address!
	#endif
			case EDbColInt32:
			case EDbColUint32:
			case EDbColReal32:
				aRec=Read32(aRec,(TUint32*)aCell->Data());
				break;
	#if !defined(__DOUBLE_WORDS_SWAPPED__)
			case EDbColReal64:
	#endif
			case EDbColInt64:
			case EDbColDateTime:
				if (TInt(end)-TInt(aCell)<=(TInt)sizeof(TInt64))
					return 0;
				size=sizeof(TInt64);
				aRec=Read32(aRec,(TUint32*)aCell->Data());
				aRec=Read32(aRec,(TUint32*)aCell->Data()+1);
				break;
			case EDbColText16:
				{
				TInt len;
				aRec=ExpandUnicodeL(aRec,aCell->Data(),end,len);
				if (!aRec)
					return 0;
				size=len<<1;
				}
				break;
			case EDbColText8:
			case EDbColBinary:
				size=*aRec++;
				if (TInt(end)-TInt(aCell)<TInt(size+sizeof(TUint32)))
					return 0;
				Mem::Copy(aCell->Data(),aRec,size);
				aRec+=size;
				break;
			case EDbColLongText8:
			case EDbColLongText16:
			case EDbColLongBinary:
				if (READBIT(bits,aRec)==0)
					{	// out of line Long column
					if (TInt(end)-TInt(aCell)<=TDbBlob::RefSize())
						return 0;
					TDbBlobId id;
					aRec=ReadBlobId(aRec,id);
					TInt sz;
					aRec=ReadCardinality(aRec,sz);
					new(aCell->Data()) TDbBlob(id,sz);
					size=TDbBlob::RefSize();
					}
				else if (type!=EDbColLongText16)
					{	// inline
					size=*aRec++;
					if (TInt(end)-TInt(aCell)<TInt(TDbBlob::InlineSize(size)+sizeof(TUint32)))
						return 0;
					new(aCell->Data()) TDbBlob(aRec,size);
					aRec+=size;
					size=TDbBlob::InlineSize(size);
					}
				else
					{
					TDbBlob* blob=new(aCell->Data()) TDbBlob;
					TInt len;
					aRec=ExpandUnicodeL(aRec,blob->InlineBuffer(),end,len);
					if (!aRec)
						return 0;
					size=len<<1;
					blob->SetSize(size);
					size=TDbBlob::InlineSize(size);
					}
				break;
				}
			}
		aCell->SetLength(size);
		aCell=aCell->Next();
		if (aCell==end)
			return aRec;
		if (++col==cend)
			return 0;
		}
	}

void CDbStoreTable::CopyToRowL(RDbRow& aRow,const TDesC8& aRecord)
//
// translate persistent record into the row buffer
//
	{
	const TUint8* rec=aRecord.Ptr();
	const TUint8* end=rec+aRecord.Length();
	TInt size;
	rec=ReadCardinality(rec,size);
	size<<=2;
	
	//If such huge allocation is requested(KMaxTInt/2), it is highly likely the file is corrupt
	if((size < 0) || (size >= KMaxTInt/2))
		{
		aRow.SetSize(0);
		__LEAVE(KErrCorrupt);
		}
	
	if (size)
		{
		aRow.GrowL(size);
		rec=CopyToRowL(aRow.First(),size,rec);
		}
	if (rec)
		{
		do
			{
			if (rec==end)
				{
	aRow.SetSize(size);
				return;
				}
			} while (*rec++==0);
		}
	aRow.SetSize(0);
	__LEAVE(KErrCorrupt);
	}

TUint8* CDbStoreTable::AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength,TInt aInlineLimit)
//
// Rewrite the record in the buffer by removing the data from the delete-list
// any changes are recorded in the iAltered member
//
	{
	// initially assume that the length count will be the same size after alteration
	TInt lenSize=SizeOfCardinality(ReadCardinality(aRPtr));
	const TUint8* const rEnd=aRPtr+aLength;
	aRPtr+=lenSize;
	TUint8* wptr=aWPtr+lenSize;
	TInt wRowSize=0;
	TInt headers=0;
//
	READBITINIT(rbits);
	WRITEBITINIT(wbits);
	const HDbColumnSet::TIteratorC cEnd=Def().Columns().End();
	HDbColumnSet::TIteratorC col=Def().Columns().Begin();
	do
		{
		if (aRPtr==rEnd && !READHASBITS(rbits))
			break;	// no more data
		TUint flg=col->iFlags;
		if ((flg&TDbColumnDef::EDropped)==0)
			++headers;
		if ((col->iAttributes&TDbCol::ENotNull)==0)
			{	// nullable
			TUint notnull=READBIT(rbits,aRPtr);
			if ((flg&TDbColumnDef::EDropped)==0)
				WRITEBIT(wbits,wptr,notnull);
			if (notnull==0)	// null data
				continue;
			}
		wRowSize+=headers;
		headers=0;
		TInt size;
		TDbColType type=col->Type();
		switch (type)
			{
		default:
			__ASSERT(0);
		case EDbColBit:
			size=READBIT(rbits,aRPtr);
			if ((flg&TDbColumnDef::EDropped)==0)
				{
				WRITEBIT(wbits,wptr,size);
				++wRowSize;
				}
			continue;
		case EDbColInt8:
		case EDbColUint8:
		case EDbColInt16:
		case EDbColUint16:
		case EDbColInt32:
		case EDbColUint32:
		case EDbColReal32:
		case EDbColReal64:
		case EDbColInt64:
		case EDbColDateTime:
			size=TRecordSize::FixedFieldSize(type);
			if ((flg&TDbColumnDef::EDropped)==0)
				{
				wptr=Mem::Copy(wptr,aRPtr,size);
				wRowSize+=(size+3)>>2;	// # words
				}
			break;
		case EDbColText8:
		case EDbColBinary:
			size=*aRPtr++;
			if ((flg&(TDbColumnDef::EChangedType|TDbColumnDef::EDropped))==0)
				{
				wptr=Mem::Copy(wptr,aRPtr-1,size+1);	// no change, copy the column
				wRowSize+=(size+3)>>2;	// # words
				}
			else if (flg&TDbColumnDef::EChangedType)
				goto alterBlob8;	// type change, into a LongColumn
			else
				__ASSERT(flg&TDbColumnDef::EDropped);	// drop the column
			break;
		case EDbColText16:
			{
			TInt sz;
			aRPtr=ReadCardinality(aRPtr,sz);
			size=sz;
			if ((flg&(TDbColumnDef::EChangedType|TDbColumnDef::EDropped))==0)
				{
				wptr=WriteCardinality(wptr,size);
				wptr=Mem::Copy(wptr,aRPtr,size);	// no change, copy the column
				wRowSize+=(SizeOfExpandedUnicodeL(aRPtr,size)+3)>>2;
				}
			else if (flg&TDbColumnDef::EChangedType)
				goto alterBlob16;	// type change, into a LongColumn
			else
				__ASSERT(flg&TDbColumnDef::EDropped);	// drop the column
			}
			break;
		case EDbColLongText8:
		case EDbColLongBinary:
		case EDbColLongText16:
			if (!READBIT(rbits,aRPtr))
				{	// out-of-line
				TDbBlobId id;
				aRPtr=ReadBlobId(aRPtr,id);
				TInt sz;
				aRPtr=ReadCardinality(aRPtr,sz);
				if (flg&TDbColumnDef::EDropped)
					BlobsL()->DeleteL(id);	// delete the stream
				else
					{
					WRITEZEROBIT(wbits,wptr);			// out-of-line
					wptr=WriteBlobId(wptr,id);
					wptr=WriteCardinality(wptr,sz);
					wRowSize+=TDbBlob::RefSize()>>2;
					}
				size=0;
				}
			else if (type!=EDbColLongText16)
				{	// currently inline
				size=*aRPtr++;
				if (flg&TDbColumnDef::EDropped)
					break;
				// write long-column data, check inline status
alterBlob8:		WRITEBIT(wbits,wptr,size<=aInlineLimit);
				if (size<=aInlineLimit)
					{	// inlined
					*wptr++=TUint8(size);				// blob size
					wptr=Mem::Copy(wptr,aRPtr,size);	// blob data
					wRowSize+=(TDbBlob::InlineSize(size)+3)>>2;
					}
				else
					{
					TDbBlobId id=BlobsL()->CreateL(type,aRPtr,size);
					wptr=WriteBlobId(wptr,id);
					wptr=WriteCardinality(wptr,size);
					wRowSize+=TDbBlob::RefSize()>>2;
					}
				}
			else
				{	// currently inline
				TInt sz;
				aRPtr=ReadCardinality(aRPtr,sz);
				size=sz;
				if (flg&TDbColumnDef::EDropped)
					break;
				// write long-column data, check inline status
alterBlob16:	TInt len=SizeOfExpandedUnicodeL(aRPtr,size);
				WRITEBIT(wbits,wptr,len<=aInlineLimit);
				if (len<=aInlineLimit)
					{	// inlined
					wptr=WriteCardinality(wptr,size);
					wptr=Mem::Copy(wptr,aRPtr,size);	// blob data
					wRowSize+=(TDbBlob::InlineSize(len)+3)>>2;
					}
				else
					{
					TDbBlobId id=BlobsL()->CreateL(EDbColLongText8,aRPtr,size);	// no unicode compressor!
					wptr=WriteBlobId(wptr,id);
					wptr=WriteCardinality(wptr,len);
					wRowSize+=TDbBlob::RefSize()>>2;
					}
				}
			break;
			}
		aRPtr+=size;
		} while (++col<cEnd);
	FLUSHBITS(wbits);
	TInt lsz=SizeOfCardinality(wRowSize);
	if (lenSize!=lsz)
		wptr=Mem::Copy(aWPtr+lsz,aWPtr+lenSize,wptr-(aWPtr+lenSize));
	WriteCardinality(aWPtr,wRowSize);
	return wptr;
	}

TInt CDbStoreTable::IndexSpanL(const CDbTableIndexDef& aIndex,TUint aInclusion,const TDbLookupKey* aLower,const TDbLookupKey* aUpper)
//
// Guess the size of the index set contained in the given restriction
// First check the cached values in the defintion, and only load the index if necessary
//
	{
	const TDbStoreIndexStats& stats=static_cast<const CDbStoreIndexDef&>(aIndex).iStats;
	if (!stats.IsValid())
		IndexL(aIndex);				// ensure that the index is loaded
	return stats.Span(aInclusion,aLower,aUpper,TTextOps::Ops(aIndex.Key().Comparison()));
	}

// Class CDbStoreTable::CDiscarder

CDbStoreTable::CDiscarder::CDiscarder()
	{}

CDbStoreTable::CDiscarder::~CDiscarder()
	{
	if (iTable)
		iTable->Close();
	iRow.Close();
	}

TInt CDbStoreTable::CDiscarder::OpenL(CDbStoreTable* aTable)
	{
	__ASSERT(!iTable);
	iTable=aTable;
	iRecords=&aTable->StoreRecordsL();
	iCluster=iRecords->Head();
	return iRecords->Count()+1;
	}

TInt CDbStoreTable::CDiscarder::StepL(TInt aStep)
	{
	__ASSERT(iTable);
	TInt limit=iTable->Def().Columns().HasLongColumns() ? EBlobDiscardClusters : EDiscardClusters;
	for (TInt inc=0;inc<limit;++inc)
		{
		if (iCluster==KNullClusterId)
			{
			iRecords->DestroyL();
			return 0;
			}
		if (limit==EBlobDiscardClusters)
			iRecords->AlterL(iCluster,*this);
		aStep-=iRecords->DiscardL(iCluster);
		}
	return Max(aStep,1);
	}

TUint8* CDbStoreTable::CDiscarder::AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength)
//
// Scan for and discard all known BLOBs
//
	{
	iTable->CopyToRowL(iRow,TPtrC8(aRPtr,aLength));
	iTable->DiscardBlobsL(iRow);
	return aWPtr;
	}

// class CDbStoreTable::CAlter

CDbStoreTable::CAlter::CAlter()
	{}

CDbStoreTable::CAlter::~CAlter()
	{
	if (iTable)
		iTable->Close();
	}

void CDbStoreTable::CAlter::OpenL(CDbStoreTable* aTable,const HDbColumnSet& aNewSet)
//
// Prepare for alteration of the table data
//
	{
	__ASSERT(!iTable);
	iTable=aTable;
	iInlineLimit=TRecordSize::InlineLimit(aNewSet);
	iRecords=&iTable->StoreRecordsL();
	iCluster=iRecords->Head();
//
// Calculate the maximum possible expansion, based on the changes to the column set
// Currently the only alloed change which affects this is Text->LongText type changes
// these all add a single bit
//
	TInt expand=0;
	const HDbColumnSet& columns=iTable->Def().Columns();
	const HDbColumnSet::TIteratorC end=columns.End();
	HDbColumnSet::TIteratorC col=columns.Begin();
	do
		{
		if (col->iFlags&TDbColumnDef::EChangedType)
			{
			__ASSERT(col->iType>=EDbColText8&&col->iType<=EDbColBinary);
			++expand;
			}
		} while (++col<end);
	iExpansion=(expand+7)>>3;
	}

TInt CDbStoreTable::CAlter::StepL(TInt aStep)
//
// Do some steps to alter the table data
//
	{
	__ASSERT(iTable);
	iStep=aStep;
	iCluster=iRecords->AlterL(iCluster,*this);
	return iCluster==KNullClusterId ? 0 : Max(iStep,1);
	}

TUint8* CDbStoreTable::CAlter::AlterRecordL(TUint8* aWPtr,const TUint8* aRPtr,TInt aLength)
//
// providing for CClusterCache::MAlter
// re-write the record
//
	{
	--iStep;
	return iTable->AlterRecordL(aWPtr,aRPtr,aLength,iInlineLimit);
	}

TInt CDbStoreTable::CAlter::RecordExpansion(const TUint8*,TInt)
//
// providing for CClusterCache::MAlter
// just return the maximum possible expansion, rather than a record-specific one
//
	{
	return iExpansion;
	}
