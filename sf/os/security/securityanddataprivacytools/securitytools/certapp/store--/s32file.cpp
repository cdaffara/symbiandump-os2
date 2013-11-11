/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32file.h>
#include "s32file_private.h"
#include <e32err.h>
#include <fstream>
#include <set>
#include "us_std.h"
#include "filestream.h"
#include "logger.h"
using namespace store_private;

//The offset of the header of a permanent file store.
//Since all permanent file store operations work in their own coordinate system, where physical file offset 32 is
//logical offset 0, KPermanentStoreHeaderOffset is set with -16, which means that the physical file offset is 32 - 16 = 16.
const TInt KPermanentStoreHeaderOffset=-16;

//Permanent file store header length: sizeof(backup TOC ref) + sizeof(handle) + sizeof(TOC ref) + sizeof(crc) = 4 + 4 + 4 + 2 = 14
const TInt KPermanentStoreHeaderLength=14;

//Backup TOC ref length - 4 bytes
const TInt KPermanentStoreBackupLength=4;

const TInt KFileStoreStartOffset=sizeof(TCheckedUid);

const TInt KBaseOffset=KFileStoreStartOffset-KPermanentStoreHeaderOffset;

//
const TInt KMaskHandleHash=0xff000000;
const TInt KHandleInvalid=0x80000000;
const TInt KHandleTocBase=0x40000000;
const TInt KMaskHandleClear=0x30000000;
const TInt KMaskHandleGen=0x0f000000;
const TInt KIncHandleGen=0x01000000;
//
const TInt KMaxHandleIndex=0x00ffffff;
const TInt KMaskHandleIndex=0x00ffffff;
const TInt KSizeHandleIndex=3;
//
const TInt KTocDeltaCap = 64;		//up to 64 entries in a delta TOC
const TInt KTocDeltaMagic = 2;
const TInt KMaxTocDeltaMagic = KMaxTUint16;
const TInt KTocDelta=KHandleInvalid;
//
const TInt KOffsetTocHeader=-12;
const TInt KSizeTocEntry=5;		//base toc entry size is 5 bytes (when stored in the file, 8 bytes when presented in memory)
const TInt KSizeTocDeltaEntry=8;//delta toc entry size is 8 bytes
const TInt KSizeTocDeltaExtra=7;
const TInt KElementsTocBuf=48;
const TInt KBackTocBuf=20*KSizeTocEntry;
const TInt KSizeTocBuf=KElementsTocBuf*KSizeTocEntry;

enum TFrameType16
	{
	EFrameFree16=0x0000,
	EFrameData16=0x4000,
	EFrameDescriptive16=0x8000,
	EFrameContinuation16=0xc000
	};
typedef TUint16 TFrameDes16;
const TInt KSizeFrameDes16=sizeof(TFrameDes16);
const TInt KShiftFrameDes16=1;
//
const TInt KMaskFrame16=0xffff;
const TInt KMaskFrameType16=0xc000;
#if defined(__SMALL_FRAME)
const TInt KMaskFrameLength16=0xf;
const TInt KShiftFrameLength16=4;
#else
const TInt KMaskFrameLength16=0x3fff;
const TInt KShiftFrameLength16=14;
#endif
//
const TInt KFrameOpen16=0;
const TInt KFrameMinLength16=1;
const TInt KFrameMaxLength16=KMaskFrameLength16;
const TInt KFrameFullLength16=KMaskFrameLength16+1;
//
const TInt KFrameNonexistent16=-1;

TUint32 frame2File(TUint32 anOffset)
{
	// The frame offset appears to be to calculated as an offset into
	// a series of maximal length frames.  Each maximal frame can hold
	// 0x4000 bytes of data and has a 2 byte header. Therefore a frame
	// offset of x needs an extra 2 bytes for each maximal frame. The
	// base is the first byte of the payload of the first frame, so we
	// do not need to allow for the header of the first frame.
	//
	// anOffset>>KShiftFrameLength16 gives the number of maximal
	// frames. The left shift multiples by 2 to give minimum space
	// required for frame headers. Adding to the original gives the
	// file offset.
	//
	// This logic is still used even if space for the target frame is
	// really ocupied by a series of smaller frames.
	anOffset += (anOffset>>KShiftFrameLength16<<KShiftFrameDes16);

	// Add 32 to allow for file header
	anOffset += KBaseOffset;
	return anOffset;
}

TUint32 file2Frame(TUint32 aFileOffset)
{
	// subtract 32 to allow for file header
	aFileOffset -= KBaseOffset;

	// File offset is to the first byte of payload of a stream. We
	// need to remove the 2 byte frame header for the (theoretical)
	// maximal length frames before it.

	// Calculate the number of (theoretical) maximal length frames before the file position
	// n = aFileOffset/0x4002
	TInt n=aFileOffset/(KFrameFullLength16+KSizeFrameDes16);
	
	// Subtract the frame header length for each maximal frame
	aFileOffset -= n<<KShiftFrameDes16;

	return aFileOffset;
}



//TPermanentStoreHeader class.
//
//Represents the data kept in the permanent store file header.
//Data members:
// - iBackup - "backup TOC reference", 32-bits integer, which keeps the 31-bit file offset of the backup TOC.
//             Plays important role in the "store commit" procedure.
//             The LSB is a "dirty" bit. If during the store opening phase the dirty bit is found to be set,
//             then it means - the previous "store commit" operation has not been completed successfully and
//             the backup TOC shall be used instead of the TOC;
// - iHandle - 32-bit stream handle (MSB - invalid/deleted, 3 bits - unused, 4 bits - generation counter, 24 bits - stream handle).
//             Plays important role in the "stream relocation" procedure during store compaction.
//             iHandle keeps the handle of the stream being relocated, so if the commit phase fails, the original stream entry
//             can be restored at the moment when the store is reopened;
// - iRef    - Current "TOC reference". Represents a file offset, where the current TOC is;
// - iCrc    - 16-bit CRC, protecting iBackup, iHandle, iRef;
class TPermanentStoreHeader
	{
public:
	TPermanentStoreHeader() : iBackup(0), iHandle(0), iRef(0), iCrc(0) {}
	TUint8* Ptr();
	const TUint8* Ptr() const;
	TBool IsValid() const;
//
	TPermanentStoreHeader(TInt aToc);
	TPermanentStoreHeader(TInt aBackupToc,TInt aHandle,TInt aReference);
//
	TBool IsDirty() const;
	void MarkDirty();
	void SetBackupToc(TInt aBackupToc);
	TInt BackupToc() const;
//
	TInt Handle() const;
	TInt Reference() const;
private:
	void Set(TInt aBackupToc,TInt aHandle,TInt aReference);
private:
	TUint32 iBackup;
	TInt32 iHandle;
	TInt32 iRef;
	TUint16 iCrc;
public:
	// Changes for port
	void InternalizeL(RReadStream &aStream);
	void ExternalizeL(RWriteStream &aStream) const;
	};

// Class TPermanentStoreHeader
TUint8* TPermanentStoreHeader::Ptr()
	{return REINTERPRET_CAST(TUint8*,&iBackup);}
const TUint8* TPermanentStoreHeader::Ptr() const
	{return REINTERPRET_CAST(const TUint8*,&iBackup);}


TPermanentStoreHeader::TPermanentStoreHeader(TInt aToc)
{Set(aToc,0,aToc);}

TPermanentStoreHeader::TPermanentStoreHeader(TInt aBackupToc,TInt aHandle,TInt aReference)
{
	__ASSERT_DEBUG(aHandle!=0,User::Invariant());
	Set(aBackupToc,aHandle,aReference);
}

TBool TPermanentStoreHeader::IsDirty() const
{
	return iBackup&0x1;
}

void TPermanentStoreHeader::MarkDirty()
{
	iBackup|=0x1;
}

void TPermanentStoreHeader::SetBackupToc(TInt aBackupToc)
{
	__ASSERT_DEBUG(aBackupToc>=0,User::Invariant());
	iBackup=TUint32(aBackupToc)<<1;
}

TInt TPermanentStoreHeader::BackupToc() const
{
	return iBackup>>1;
}

TInt TPermanentStoreHeader::Handle() const
{
	return iHandle;
}
TInt TPermanentStoreHeader::Reference() const
{
	return iRef;
}


TBool TPermanentStoreHeader::IsValid() const
{
	if (IsDirty())
		return EFalse;

	TUint16 crc=0;
	Mem::Crc(crc,Ptr(),_FOFF(TPermanentStoreHeader,iCrc));

	return crc==iCrc;
}

//Sets the "backup TOC ref", "handle" and "TOC ref" in current TPermanentStoreHeader object.
//16-bit CRC is calculated, based on the values of the input parameters, and stored together with them in the 
//TPermanentStoreHeader object.
void TPermanentStoreHeader::Set(TInt aBackupToc,TInt aHandle,TInt aReference)
	{
	iBackup=TUint32(aBackupToc)<<1;
	iHandle=aHandle;
	iRef=aReference;
	iCrc=0;
	Mem::Crc(iCrc,Ptr(),_FOFF(TPermanentStoreHeader,iCrc));
	__ASSERT_DEBUG(IsValid(),User::Invariant());
	}


void TPermanentStoreHeader::InternalizeL(RReadStream &aStream)
{
	aStream >> iBackup;
	aStream >> iHandle;
	aStream >> iRef;
	aStream >> iCrc;
}

void TPermanentStoreHeader::ExternalizeL(RWriteStream &aStream) const
{
	aStream << iBackup;
	aStream << iHandle;
	aStream << iRef;
	aStream << iCrc;
}

struct TocHeader
	{
	TInt32 iPrimary;
	TInt32 iAvail;
	TUint32 iCount;
	void ExternalizeL(RWriteStream &aStream) const;
	void InternalizeL(RReadStream &aStream);
	};

void TocHeader::ExternalizeL(RWriteStream &aStream) const
{
	aStream << iPrimary;
	aStream << iAvail;
	aStream << iCount;
}

void TocHeader::InternalizeL(RReadStream &aStream)
{
	aStream >> iPrimary;
	aStream >> iAvail;
	aStream >> iCount;
}

struct STocEntry
	{
	TInt32 handle;
	TInt32 ref;
	void ExternalizeL(RWriteStream &aStream) const;
	void InternalizeL(RReadStream &aStream);
	};


void STocEntry::ExternalizeL(RWriteStream &aStream) const
{
	aStream.WriteL((TUint8*)&handle + KSizeHandleIndex, KSizeTocEntry);
}

void STocEntry::InternalizeL(RReadStream &aStream)
{
	aStream.ReadL((TUint8*)&handle + KSizeHandleIndex, KSizeTocEntry);
}


CStreamStore::~CStreamStore()
{
}


TStreamId CPersistentStore::Root()
{
	return iRoot;
}

void CPersistentStore::SetRootL(TStreamId anId)
{
	iRoot = anId;
}

void CFileStore::SetTypeL(const TUidType& aType)
{
	if(aType != KPermanentFileStoreLayoutUid) FatalError();
}


CFileStore *CPermanentFileStore::ReplaceLC(RFs &aFs, const TDesC &aName, TUint aFileMode)
{
	(void) aFs;
	return new CPermanentFileStore(aName, aFileMode);
}

CPermanentFileStore* CPermanentFileStore::OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode)
{
	(void) aFs;
	CPermanentFileStore *pfs = new CPermanentFileStore(aName, aFileMode);
	pfs->ReadInData();
	return pfs;
}

CPermanentFileStore::CPermanentFileStore(const TDesC& aName,TUint aFileMode)
	: iWritable(false), iFileIn(), iFileOut(), iLastStreamIdCreated(0), 
	  iStreams()
{
	TBuf8<KMaxName> utf8Name;
	utf8Name.Copy(aName);

	std::string name((const char *)utf8Name.Ptr(), utf8Name.Length());
	
	if(aFileMode & EFileWrite)
		{
		iFileOut.reset(new FileWriteStream(name));
		iWritable = true;
		}
	else
		{
		iFileIn.reset(new FileReadStream(name));
		}
}

CPermanentFileStore::~CPermanentFileStore()
{
	while(!iStreams.empty())
		{
		MemStreamBuf *p = (*iStreams.begin()).second;
		iStreams.erase(iStreams.begin());
		delete p;
		}
}


MemStreamBuf *CPermanentFileStore::CreateStoreWriteStream()
{
	TStreamId id = ++iLastStreamIdCreated;
	MemStreamBuf *handler = new MemStreamBuf(*this, id);
	
	MemStreamBufPtrMap::value_type value(id, handler);
	std::pair<MemStreamBufPtrMap::iterator, bool> ret = iStreams.insert(value);

	if(!ret.second)
		{
		// Insert failed
		FatalError();
		}
	

	return handler;
}

MemStreamBuf *CPermanentFileStore::CreateStoreReadStream(TStreamId aId) const
{
	MemStreamBufPtrMap::const_iterator it = iStreams.find(TUint32(aId));
	if(it == iStreams.end())
		{
		// Lookup failed
		dbg << Log::Indent() << "StreamId 0x" << aId << " does not exist in store!" << Log::Endl();
		FatalError();
		}
	return (*it).second;
}

bool CPermanentFileStore::Writable() const
{
	return iWritable;
}



void CPermanentFileStore::CommitL()
{
	if(!iWritable) FatalError();	

	TCheckedUid id(KPermanentFileStoreLayoutUid);

	*iFileOut << id;

	// Write an empty header, we will correct the fields later
	TPermanentStoreHeader tmpheader(0);
	*iFileOut << tmpheader;

	//
	// Now write out the streams
	//
	std::vector<TUint> streamOffsets;
	for(MemStreamBufPtrMap::const_iterator streamIt = iStreams.begin();
		streamIt != iStreams.end(); 
		++streamIt)
		{
		const MemStreamBuf::StreamData &streamData((*streamIt).second->GetData());

		// Record the offset to the first byte of the payload for use in the TOC
		streamOffsets.push_back(TUint32(iFileOut->iSnk->TellL(MStreamBuf::EWrite))+KSizeFrameDes16);
		
		TUint32 dataAvailable = streamData.size();
		MemStreamBuf::StreamData::const_iterator dataIt = streamData.begin();
		TUint32 currentType = EFrameData16;
		while(dataAvailable)
			{
			TUint32 toWrite;
			TFrameDes16 frame;
			
			TUint32 fileOffset = TUint32(iFileOut->iSnk->TellL(MStreamBuf::EWrite))+KSizeFrameDes16;
			TUint32 frameOffset = file2Frame(fileOffset);
			TUint32 nextAnchor = ((frameOffset + KFrameFullLength16) & ~KMaskFrameLength16);
			if(dataAvailable >= (nextAnchor-frameOffset))
				{
				// We have enough data to write an "Open" frame to
				// take the frame offset up to the next multiple of
				// 0x4000
				toWrite = (nextAnchor-frameOffset);
				frame = currentType | KFrameOpen16;
				}
			else
				{
				// Write a normal frame
				toWrite = (dataAvailable < TUint32(KMaskFrameLength16)) ? (dataAvailable) : (KMaskFrameLength16);
				frame = currentType | toWrite;
				}
			
			// Write frame
			*iFileOut << frame;
			// Write payload
			while(toWrite--)
				{
				*iFileOut << *dataIt;
				++dataIt;
				--dataAvailable;
				}
			// Set type of next frame to continuation
			currentType = EFrameContinuation16;
			}
		}

	//
	// Now write the TOC
	//
	TInt tocLength = sizeof(TocHeader) + (iStreams.size() * KSizeTocEntry);
	// Check toc frame length is not too long
	if(tocLength > KMaskFrameLength16)
		{
		FatalError();
		}
	// Write TOC frame
	TFrameDes16 frame = tocLength | EFrameDescriptive16;
	*iFileOut << frame;

	TStreamPos pos = iFileOut->iSnk->TellL(MStreamBuf::EWrite);	

	// First the header
	TUint32 tocFrameOffset = file2Frame(pos);
	TUint32 tocCookedFrameOffset = tocFrameOffset - KOffsetTocHeader;
	
	TocHeader tocheader;
	tocheader.iPrimary = iRoot;
	tocheader.iAvail = 0;
	tocheader.iCount = iStreams.size();
	*iFileOut << tocheader;

	// Now write the TOC entries
	for(TUint i = 0; i < streamOffsets.size(); ++i)
		{
		STocEntry tocEntry = { 0, file2Frame(streamOffsets[i]) };
		*iFileOut << tocEntry;
		}
	
	
	//
	// Now overwrite the header with one containing the correct master
	// TOC offset
	//
	TPermanentStoreHeader realheader(tocCookedFrameOffset);
	// For some reason the real store code creates a header with a
	// checksum which assumes iBackup == iRef but the iBackup value on
	// disk is set to 0...
	realheader.SetBackupToc(0);
	iFileOut->iSnk->SeekL(MStreamBuf::EWrite, EStreamBeginning, sizeof(TCheckedUid));
	*iFileOut << realheader;


	iFileOut->Close();
}

void CPermanentFileStore::ReadInData()
{
	prog << Log::Indent() << "Reading store" << Log::Endl();
	AutoIndent ai(prog);
	
	TUint lastStreamIndex = 0;
	TCheckedUid id;

	*iFileIn >> id;

	if(id.UidType() != KPermanentFileStoreLayoutUid)
		{
		dbg << Log::Indent() << "Invalid store" << Log::Endl();
		FatalError();
		}
	
	TPermanentStoreHeader header;
	
	*iFileIn >> header;

	// - If the dirty bit is set, the backup TOC will be used;
	// - If the dirty bit is not set, and the backup TOC ref is not the same as the TOC ref,
	//   then it means the the backup TOC ref has not been written successfully, so the TOC ref will be used;
	TInt toc=header.BackupToc();
	if (!header.IsDirty())
		{
		prog << Log::Indent() << "Store header is clean" << Log::Endl();
		TInt handle=header.Handle();
		TInt ref=header.Reference();
		if (handle==0&&toc!=ref)
			{ // toc pointer not backed up, validate as if it was
			toc=ref;
			//			iState|=EBackup;
			header.SetBackupToc(toc);
			}
		if (!header.IsValid()) // not a permanent store or damaged beyond recognition
			{
			dbg << Log::Indent() << "Store header checksum failed check1" << Log::Endl();
			__LEAVE(KErrNotSupported);
			}
		
		//
		if (toc<0||((handle&~KMaskStreamIdValue)!=0&&handle!=KHandleTocBase)||ref<0||(handle!=0&&ref>=toc+KOffsetTocHeader))
			{
			dbg << Log::Indent() << "Store header fields invalid" << Log::Endl();
			__LEAVE(KErrCorrupt); // integrity compromised
			}
		//
		//		iReloc=handle;
		//		iTarget=ref;
		}
	else
		{
		dbg << Log::Indent() << "Store header dirty" << Log::Endl();
		}
	
	//
	//	if (iToc!=0 && iToc!=toc)		// refresh produced a different toc
	//		__LEAVE(KErrCorrupt);
	//	iToc=toc;


	if(!header.IsValid())
		{
		dbg << Log::Indent() << "Store header checksum failed check2" << Log::Endl();
		FatalError();
		}

	// All offset values are to the payload of frame structures (see RFrame16Buf)
	// Convert to a file offset.
	TInt tocFileOffset = frame2File(toc + KOffsetTocHeader);
	prog << Log::Indent() << "TOC file offset 0x" << tocFileOffset << Log::Endl();

	// nb. We do not validate the length of the "descriptive" stream
	// which contains the TocHeader (aka STocHead) and the table of stream offsets.

	iFileIn->iSrc->SeekL(MStreamBuf::ERead, EStreamBeginning, tocFileOffset);
	TocHeader h;
	*iFileIn >> h;
	if ((h.iPrimary&~(KMaskStreamIdValue|static_cast<TUint>(KTocDelta)))!=0||
		h.iAvail>0||(h.iAvail&KMaskHandleClear)!=0||(h.iCount&~KMaskHandleIndex)!=0)
		{
		// Corrupt
		dbg << Log::Indent() << "Corrupt TOC in input file" << Log::Endl();
		FatalError();
		}
	std::set<TUint32> streamIdsReadExGenField; // IDs of streams already read, EXCLUDING generation field
	if (h.iPrimary & KTocDelta)
		{
		AutoIndent ai(prog);
		prog << Log::Indent() << "Found Delta TOC" << Log::Endl();
		// Process TOC-delta
		TInt oldToc = toc;
		// Find and read in the delta entries and then continue to read in the base entries
		// Read offset to base TOC
		iFileIn->iSrc->ReadL(&toc, sizeof(toc));
		// All offset values are to the payload of frame structures (see RFrame16Buf)
		// Convert to a file offset.
		tocFileOffset = frame2File(toc + KOffsetTocHeader);

		if((tocFileOffset < 0) || (tocFileOffset>oldToc))
			{
			dbg << Log::Indent() << "Invalid TOC in delta TOC" << Log::Endl();
			FatalError(); // Corrupt
			}

		// Read/discard the magic value - Not sure what this should be
		TUint16 magic;
		iFileIn->iSrc->ReadL(&magic, sizeof(magic));
		
		// Read count of delta toc entries
		TInt8 n;
		iFileIn->iSrc->ReadL(&n, sizeof(n));
		
		prog << Log::Indent() << "Delta TOC entries " << int(n) << Log::Endl();
		for(int deltaTocEntry=1; deltaTocEntry <= n; ++deltaTocEntry)
			{
			AutoIndent ai(prog);
			prog << Log::Indent() << "Delta TOC entry " << deltaTocEntry << Log::Endl();

			STocEntry e = {0,0};
			// Read delta toc entry.
			//
			// The main TOC entries are truncated to 5 bytes so only
			// include the flag bits of the handle and not the stream.
			// They gain streamids starting from 1.
			//
			// The delta TOC contains complete stream IDs, and these
			// entries take precedence over the matching (implicitly
			// numbered) entry in the base TOC.
			iFileIn->iSrc->ReadL(&e, sizeof(e));

			if ((e.handle&KMaskHandleClear)!=0 ||
				e.handle>=0 && (e.ref<KFrameNonexistent16) ||
				e.handle<0 && (e.ref>0 || (e.ref&KMaskHandleClear)!=0))
				{
				dbg << Log::Indent() << "Invalid delta TOC entry" << Log::Endl();
				__LEAVE(KErrCorrupt);
				FatalError();
				}

			if(e.handle & KHandleInvalid)
				{
				// Stream marked as deleted, skip it
				prog << Log::Indent() << "Delta TOC entry marked as deleted" << Log::Endl();
				continue;
				}
			
			// We do not deal with any other special cases correctly
			if(((e.handle&(KMaskHandleHash-KMaskHandleGen)) != 0) || (e.ref < 0))
				{
				dbg << Log::Indent() << "store-- does not support handle flags 0x" << e.handle << "in delta toc" << Log::Endl();
				FatalError();
				}

			prog << Log::Indent()
				 << "streamId 0x" << e.handle << " frame offset 0x" << e.ref 
				 << " file offset 0x" << frame2File(e.ref) << Log::Endl();

			if(!streamIdsReadExGenField.insert(e.handle & ~KMaskHandleHash).second)
				{
				dbg << Log::Indent() << "store-- corrupt input? Multiple entries for stream ID " << (e.handle & ~KMaskHandleHash) << "encountered in delta toc!" << Log::Endl();
				FatalError();
				}
			

			// Remember where we are...
			TStreamPos here = iFileIn->iSrc->TellL(MStreamBuf::ERead);
			// Read the stream frame
			ReadStreamData(e.handle, e.ref);
			// restore position to continue reading delta toc
			iFileIn->iSrc->SeekL(MStreamBuf::ERead, EStreamBeginning, here);
			}
		}

	iRoot = TInt(h.iPrimary) & ~KTocDelta;
	prog << Log::Indent() << "Root stream ID is " << iRoot << Log::Endl();

	prog << Log::Indent() << "Processing main TOC at file offset 0x" << tocFileOffset << Log::Endl();
		
	for(TUint tocIndex=0; tocIndex < h.iCount; ++tocIndex)
		{
		// Read in streams
		TStreamId baseStreamId = ++lastStreamIndex;
		prog << Log::Indent() << "Entry " << std::dec << baseStreamId << std::hex << Log::Endl();
		AutoIndent ai(prog);
		
		// Check if we have already read in this stream id from the delta toc
		// Note we only compare the base of the stream ID excluding the generation field.
		if(streamIdsReadExGenField.find(baseStreamId) != streamIdsReadExGenField.end())
			{
			prog << Log::Indent() << "Already read from delta TOC" << Log::Endl();
			continue; // Already read from delta toc
			}
		
		// Seek to the STocEntry for the stream
		iFileIn->iSrc->SeekL(MStreamBuf::ERead, EStreamBeginning, tocFileOffset+sizeof(TocHeader)+KSizeTocEntry*(tocIndex));


		STocEntry e = {0,0};
		// Only the last 5 bytes of the entry are saved to disk.
		// Because we are little endian this is the MSB of the handle and the whole ref.
		*iFileIn >> e;

		if ((e.handle&KMaskHandleClear)!=0 ||
			e.handle>=0 && (e.ref<KFrameNonexistent16) ||
			e.handle<0 && (e.ref>0 || (e.ref&KMaskHandleClear)!=0))
			{
			__LEAVE(KErrCorrupt);
			FatalError();
			}

		if(e.handle & KHandleInvalid)
			{
			// Stream deleted, skip it
			prog << Log::Indent() << "Marked as deleted" << Log::Endl();
			continue;
			}

		// We do not deal with any other special cases correctly 
		if(((e.handle & (KMaskHandleHash-KMaskHandleGen)) != 0) || (e.ref < 0))
			{
			dbg << Log::Indent() << "store-- does not support handle flags 0x" << e.handle << "in main toc" << Log::Endl();
			FatalError();
			}

		TStreamId streamId = baseStreamId | e.handle;
		if(!streamIdsReadExGenField.insert(baseStreamId).second)
			{
			dbg << Log::Indent() << "store-- internal error for stream ID " << baseStreamId << "encountered in delta toc!" << Log::Endl();
				FatalError();
			}
		
		// Read the stream frame
		//		prog << Log::Indent() << "Reading stream 0x" << streamId << " frame offset 0x" << e.ref << Log::Endl();
		ReadStreamData(streamId, e.ref);
		}
}


void CPermanentFileStore::ReadStreamData(TStreamId aStreamId, TInt aFrameOffset)
{
	TInt fileOffset = frame2File(aFrameOffset);
	TFrameDes16 frame;
	iFileIn->iSrc->SeekL(MStreamBuf::ERead, EStreamBeginning, fileOffset - sizeof(frame));

	// Frame runs from fileOffset to fileOffset+2+frame
	std::auto_ptr<MemStreamBuf> apStream(new MemStreamBuf(*this, aStreamId));

	enum {
		EFirst, EContinuation, ELast
	} state = EFirst;
	
	while(state != ELast) 
		{
		iFileIn->iSrc->ReadL(&frame, sizeof(frame));

		// Store uses offsets to the first byte of the payload of a frame, so that is what we display
		TUint32 fileOffset = iFileIn->iSrc->TellL(MStreamBuf::ERead);
		prog << Log::Indent() << "ReadStreamData State " << int(state) << " offset " << file2Frame(fileOffset) << "[" << fileOffset << "]" << Log::Endl();

		TUint32 frameType = (frame & KMaskFrameType16);
		if(frameType == EFrameFree16)
			{
			dbg << Log::Indent() << "Encountered a deleted frame in data stream" << Log::Endl();
			FatalError();
			}

		if((frameType == EFrameData16) && (state != EFirst))
			{
			dbg << Log::Indent() << "Non-continuation frame encountered in middle of data stream" << Log::Endl();
			FatalError();
			}

		if(frameType == EFrameDescriptive16)
			{
			dbg << Log::Indent() << "Descriptive frame encountered in data stream" << Log::Endl();
			FatalError();
			}
		
		if((frameType == EFrameContinuation16) && (state != EContinuation))
			{
			dbg << Log::Indent() << "Continuation frame encountered at start of stream" << Log::Endl();
			FatalError();
			}
	
		TInt len = (frame & KMaskFrameLength16);
		if (len == KFrameOpen16)
			{
			prog << Log::Indent() << "\"Open\" frame length" << Log::Endl();
			len = KFrameFullLength16 - file2Frame(fileOffset);
			state = EContinuation;
			}
		else
			{
			state = ELast;
			}

		prog << Log::Indent() << "Type 0x" << frameType << " length " << std::dec << len << std::hex << Log::Endl();
		apStream->AppendFromFile(iFileIn->iSrc, len);
		};

	MemStreamBufPtrMap::value_type value(aStreamId, apStream.get());
	std::pair<MemStreamBufPtrMap::iterator, bool> ret = iStreams.insert(value);
	if(!ret.second)
		{
		// Insert failed
		FatalError();
		}

	apStream.release();
}

RReadStream::RReadStream()
	: iSrc(0)
{
}

void RReadStream::Close()
{
	iSrc = false;
}

RStoreReadStream::RStoreReadStream()
	: RReadStream()
{
}


void RStoreReadStream::OpenLC(const CStreamStore& aStore, TStreamId aId)
{
	iSrc = aStore.CreateStoreReadStream(aId);
}



RWriteStream::RWriteStream()
	: iSnk(0)
{
}





TStreamId RStoreWriteStream::CreateLC(CStreamStore& aStore)
{
	MemStreamBuf *s = aStore.CreateStoreWriteStream();
	iSnk = s;
	return s->StreamId();
}

EXPORT_C void RReadStream::ReadL(TDes8& aDes)
/** Reads sufficient data from this stream to fill the specified 8 bit descriptor up to its maximum length.
No other information is read from this read stream.

@param aDes A reference to a modifiable descriptor which is to receive the data read from this stream. Passing the build 
independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) at build time.*/
	{
	ReadL(aDes,aDes.MaxLength());
	}

EXPORT_C void RReadStream::ReadL(TDes8& aDes,TInt aLength)
/** Reads data of specified length from this stream into the specified 8 bit descriptor. No other information is read 
from this stream.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. 
Passing the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit 
or the 16 bit) at build time.
@param aLength The length of data to be read from this stream. This value must be non-negative and must not be greater than the maximum length of the descriptor otherwise the function raises a USER 11 panic.*/
	{
	__ASSERT_DEBUG(aLength<=aDes.MaxLength(),Panic(EStreamReadBeyondEnd));
	aDes.SetLength(aLength);
	ReadL((TUint8*)aDes.Ptr(),aLength);
	}

#if 0
EXPORT_C void RReadStream::ReadL(TDes8& aDes,TChar aDelim)
/** Reads data from this stream into the 8 bit descriptor, until either the specified delimiter is encountered or the descriptor is filled to its maximum length.
The resulting data in aDes always includes the delimiter aDelim, if aDes is large enough.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing 
the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) at build time.
@param aDelim The delimiter marking the end of the data in the stream.*/

	{
	__ASSERT_DEBUG(iSrc!=NULL,Panic(EStreamNotOpen));
	TUint8* ptr=(TUint8*)aDes.Ptr();
	TDelimitedInput8 input(ptr,aDes.MaxLength(),aDelim);
	do
		{
		iSrc->ReadL(input);
		} while (!input.Done());
	aDes.SetLength(input.Ptr()-ptr);
	}
#endif

EXPORT_C void RReadStream::ReadL(TUint8* aPtr,TInt aLength)
/** Reads data of specified length from this stream into the location defined by the specified TUint8 pointer.

@param aPtr The target location for the streamed in data.
@param aLength The length of data to be streamed in.*/
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamReadLengthNegative));
	if (aLength==0)
		return;
//	void *p = NULL;
	__ASSERT_DEBUG(iSrc!=NULL,Panic(EStreamNotOpen));
	TInt len=iSrc->ReadL(aPtr,aLength);
	__ASSERT_DEBUG(len>=0&&len<=aLength,Panic(EStreamReadInBreach));
	if (len<aLength)
		__LEAVE(KErrEof);
	}

EXPORT_C void RReadStream::ReadL(TInt aLength)
/** Discards data of specified length read from this stream.

@param aLength The length of data to be discarded from this read stream.*/
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamReadLengthNegative));
	if (aLength==0)
		return;
//
	__ASSERT_DEBUG(iSrc!=NULL,Panic(EStreamNotOpen));

#if 1
	iSrc->SeekL(MStreamBuf::ERead, EStreamMark, aLength);
#else
	TNullInput input;
	TInt len=iSrc->ReadL(input,aLength);
	__ASSERT_DEBUG(len>=0&&len<=aLength,Panic(EStreamReadInBreach));
	if (len<aLength)
		__LEAVE(KErrEof);
#endif
	}

EXPORT_C void RReadStream::ReadL(TDes16& aDes)
/** Reads sufficient data from this stream to fill the specified 16 bit descriptor up to its maximum length. 
No other information is read from this read stream.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing 
the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 
bit) at build time.*/
	{
	ReadL(aDes,aDes.MaxLength());
	}

EXPORT_C void RReadStream::ReadL(TDes16& aDes,TInt aLength)
/** Reads data of specified length from this stream into the specified 16 bit descriptor. No other information is read from this stream.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing the 
build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) 
at build time.
@param aLength The length of data to be read from this stream. This value must be non-negative and must not be greater
than the maximum length of the descriptor otherwise the function raises a USER 11 panic.*/
	{
	__ASSERT_DEBUG(aLength<=aDes.MaxLength(),Panic(EStreamReadBeyondEnd));
	aDes.SetLength(aLength);
	ReadL((TUint16*)aDes.Ptr(),aLength);
	}

#if 0
EXPORT_C void RReadStream::ReadL(TDes16& aDes,TChar aDelim)
/** Reads data from this stream into the 16 bit descriptor, until either the specified delimiter is encountered or 
the descriptor is filled to its maximum length.
The resulting data in aDes always includes the delimiter aDelim, if aDes is large enough.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing 
the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) at build time.
@param aDelim The delimiter marking the end of the data in the stream.*/
	{
	__ASSERT_DEBUG(iSrc!=NULL,Panic(EStreamNotOpen));
	TUint16* ptr=(TUint16*)aDes.Ptr();
	TDelimitedInput16 input(ptr,aDes.MaxLength(),aDelim);
	do
		{
		iSrc->ReadL(input);
		} while (!input.Done());
	aDes.SetLength(input.Ptr()-ptr);
	}
#endif

EXPORT_C void RReadStream::ReadL(TUint16* aPtr,TInt aLength)
/** Reads data of specified length from this stream into the specified 16 bit descriptor.
 No other information is read from this stream.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) at build time.
@param aLength The length of data to be read from this stream. This value must be non-negative and must not be greater than the maximum length of the descriptor otherwise the function raises a USER 11 panic.*/
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamReadLengthNegative));
	ReadL((TUint8*)aPtr,aLength<<1); // platform dependency
	}

EXPORT_C TInt8 RReadStream::ReadInt8L()
/** Internalises a TInt8 value The function reads an 8 bit value from this stream 
and interprets it as a TInt8.

@return The 8�bit value read from this stream. */
	{
	TInt8 val;
	ReadL((TUint8*)&val,1); // platform dependency
	return val;
	}

EXPORT_C TInt16 RReadStream::ReadInt16L()
/** Internalises a TInt16 value. The function reads a 16 bit value from this stream 
and interprets it as a TInt16.

@return The 16�bit value read from this stream. */
	{
	TInt16 val;
	ReadL((TUint8*)&val,2); // platform dependency
	return val;
	}

EXPORT_C TInt32 RReadStream::ReadInt32L()
/** Internalises a TInt32 value. The function reads a 32 bit value from this stream 
and interprets it as a TInt32.

@return The 32 bit value read from this stream. */
	{
	TInt32 val;
	ReadL((TUint8*)&val,4); // platform dependency
	return val;
	}

EXPORT_C TUint8 RReadStream::ReadUint8L()
/** Internalises a TUint8 value. The function reads an 8 bit value from this stream 
and interprets it as a TUint8.

@return The 8 bit value read from this stream. */
	{
	TUint8 val;
	ReadL(&val,1);
	return val;
	}

EXPORT_C TUint16 RReadStream::ReadUint16L()
/** Internalises a TUint16 value. The function reads a 16 bit value from this 
stream and interprets it as a TUint16.

@return The 16 bit value read from this stream. */
	{
	TUint16 val;
	ReadL((TUint8*)&val,2); // platform dependency
	return val;
	}

EXPORT_C TUint32 RReadStream::ReadUint32L()
/** Internalises a TUint32 value. The function reads a 32 bit value from this 
stream and interprets it as a TUint32.

@return The 32 bit value read from this stream. */
	{
	TUint32 val;
	ReadL((TUint8*)&val,4); // platform dependency
	return val;
	}


EXPORT_C void RWriteStream::Close()
/** Commits data to the stream before freeing resources used by the stream. This 
ensures that any buffered data is written to the stream.

Note that the function cannot leave. Any errors arising from the attempt to 
commit data to the stream are ignored. */
	{
	if (iSnk==NULL)
		return;
//
	iSnk->Close();
	iSnk=NULL;
	}

EXPORT_C void RWriteStream::Release()
/** Frees resources before abandoning the stream. The function is called after 
data has been committed to the stream.

Note that if a cleanup item for the stream was placed on the cleanup stack 
when the stream was opened (e.g by a call to RStoreWriteStreamss CreateLC(), 
OpenLC(), ReplaceLC() or RDictionaryStores AssignLC() etc), then this function 
need not be called explicitly; clean up is implicitly done by CleanupStack::PopAndDestroy(). */
	{
	if (iSnk==NULL)
		return;
//
	iSnk->Release();
	iSnk=NULL;
	}

EXPORT_C void RWriteStream::CommitL()
/** Ensures that any buffered data is written to the stream. Once committed, it 
is not possible to roll back the newly written data. */
	{
	if (iSnk==NULL)
		return;
//
	iSnk->SynchL();
	}


EXPORT_C void RWriteStream::WriteL(const TDesC8& aDes)
/** Writes the content of the 8 bit descriptor to the stream. No other information 
is written to this write stream.

@param aDes A reference to a descriptor. Passing the build independent type 
TDesC& allows the compiler to choose the appropriate WriteL() variant (i.e 
the 8 bit or the 16 bit) at build time. */
	{
	WriteL(aDes.Ptr(),aDes.Length());
	}

EXPORT_C void RWriteStream::WriteL(const TDesC8& aDes,TInt aLength)
/** Writes data of the specified length from the 8 bit descriptor to the stream. 
No other information is written to this write stream.

@param aDes A reference to a descriptor. Passing the build independent type 
TDesC& allows the compiler to choose the appropriate WriteL() variant (i.e 
the 8 bit or the 16 bit) at build time.
@param aLength The length of data to be written to this stream. */

	{
	__ASSERT_DEBUG(aLength<=aDes.Length(),Panic(EStreamWriteBeyondEnd));
	WriteL(aDes.Ptr(),aLength);
	}

EXPORT_C void RWriteStream::WriteL(const TUint8* aPtr,TInt aLength)
/** Writes 8 bit data of the specified length from the specified location to this 
write stream.

@param aPtr The location from where data is to be streamed out.
@param aLength The length of data to be streamed out. */

	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	if (aLength==0)
		return;
//
	__ASSERT_DEBUG(iSnk!=NULL,Panic(EStreamNotOpen));
	iSnk->WriteL(aPtr,aLength);
	}

#if 0
EXPORT_C void RWriteStream::WriteL(RReadStream &aStream)
/** Writes the content of the specified read stream to this write stream.

@param aStream A reference to a read stream which is to be written to this 
stream. */

	{
	__ASSERT_DEBUG(iSnk!=NULL,Panic(EStreamNotOpen));
	TSourceOutput output(aStream.iSrc);
	iSnk->WriteL(output);
	}
#endif

#if 0
EXPORT_C void RWriteStream::WriteL(RReadStream& aStream,TInt aLength)
/** Writes data of the specified length from the specified read stream to this 
stream.

@param aStream A reference to a read stream part of whose content is to be 
written to this stream.
@param aLength The length of data from the read stream to be written to this 
write stream. */

	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	if (aLength==0)
		return;
//
	__ASSERT_DEBUG(iSnk!=NULL,Panic(EStreamNotOpen));
	TSourceOutput output(aStream.iSrc);
	TInt len=iSnk->WriteL(output,aLength);
	__ASSERT_DEBUG(len>=0&&len<=aLength,Panic(EStreamReadInBreach));
	if (len<aLength)
		__LEAVE(KErrEof);
	}
#endif

EXPORT_C void RWriteStream::WriteL(const TDesC16& aDes)
/** Writes the content of the 16 bit descriptor to the stream. No other information 
is written to this write stream.

@param aDes A reference to a descriptor. Passing the build independent type 
TDesC& allows the compiler to choose the appropriate WriteL() variant (i.e 
the 8 bit or the 16 bit) at build time. */

	{
	WriteL(aDes.Ptr(),aDes.Length());
	}

EXPORT_C void RWriteStream::WriteL(const TDesC16& aDes,TInt aLength)
/** Writes data of the specified length from the 16 bit descriptor to the stream. 
No other information is written to this write stream.

@param aDes A reference to a descriptor. Passing the build independent type 
TDesC& allows the compiler to choose the appropriate WriteL() variant (i.e 
the 8 bit or the 16 bit) at build time.
@param aLength The length of data to be written to this stream. */

	{
	__ASSERT_DEBUG(aLength<=aDes.Length(),Panic(EStreamWriteBeyondEnd));
	WriteL(aDes.Ptr(),aLength);
	}

EXPORT_C void RWriteStream::WriteL(const TUint16* aPtr,TInt aLength)
/** Writes 16 bit data of the specified length from the specified location to this 
write stream.

@param aPtr The location from where data is to be streamed out.
@param aLength The length of data to be streamed out. */

	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	WriteL((const TUint8*)aPtr,aLength<<1); // platform dependency
	}

EXPORT_C void RWriteStream::WriteInt8L(TInt aValue)
/** Writes a TInt value as an 8 bit value to this stream.

@param aValue The value to be written to this stream. */

	{
	WriteL((const TUint8*)&aValue,1); // platform dependency
	}

EXPORT_C void RWriteStream::WriteInt16L(TInt aValue)
/** Writes a TInt value as a 16 bit value to this stream.

@param aValue The value to be written to this stream. */
	{
	WriteL((const TUint8*)&aValue,2); // platform dependency
	}

EXPORT_C void RWriteStream::WriteInt32L(TInt32 aValue)
/** Writes a TInt32 value as a 32 bit value to this stream.

@param aValue The value to be written to this stream. */

	{
	WriteL((const TUint8*)&aValue,4); // platform dependency
	}

EXPORT_C void RWriteStream::WriteUint8L(TUint aValue)
/** Writes a TUint value as an 8 bit value to this stream.

@param aValue The value to be written to this stream. */
	{
	WriteL((const TUint8*)&aValue,1); // platform dependency
	}

EXPORT_C void RWriteStream::WriteUint16L(TUint aValue)
/** Writes a TUint value as a 16 bit value to this stream.

@param aValue The value to be written to this stream. */

	{
	WriteL((const TUint8*)&aValue,2); // platform dependency
	}

EXPORT_C void RWriteStream::WriteUint32L(TUint32 aValue)
/** Writes a TUint32 value as a 32 bit value to this stream.

@param aValue The value to be written to this stream. */
	{
	WriteL((const TUint8*)&aValue,4); // platform dependency
	}


namespace store_private
{

MemStreamBuf::MemStreamBuf(const CStreamStore& aStore, TStreamId aId)
	: iStore(&aStore), iStreamId(aId), iCurrentReadIndex(0), iCurrentWriteIndex(0), iData()
{
}

MemStreamBuf::MemStreamBuf(const CStreamStore& aStore, TStreamId aId, 
					 MStreamBuf *aStreamIn, std::streamoff aOffset, TUint32 aLength)
	: iStore(&aStore), iStreamId(aId), iCurrentReadIndex(0), iCurrentWriteIndex(0), iData()
{
	

	aStreamIn->SeekL(MStreamBuf::ERead, EStreamBeginning, aOffset);
	while(aLength)
		{
		TUint8 data;
		aStreamIn->ReadL(&data, 1);
		iData.push_back(data);
		--aLength;
		}
	
}

void MemStreamBuf::AppendFromFile(MStreamBuf *aStreamIn, TUint32 aLength)
{
	while(aLength)
		{
		TUint8 data;
		aStreamIn->ReadL(&data, 1);
		iData.push_back(data);
		--aLength;
		}
}



TStreamId MemStreamBuf::StreamId() const
{
	return iStreamId;
}

const std::vector<TUint8> &MemStreamBuf::GetData() const
{
	return iData;
}


TInt MemStreamBuf::DoReadL(TAny *aPtr,TInt aMaxLength)
{
	char *p = reinterpret_cast<char *>(aPtr);
	for(TInt i=0; i < aMaxLength; ++i)
		{
		*p++ = iData[iCurrentReadIndex++];
		}
	return aMaxLength;
}

void MemStreamBuf::DoWriteL(const TUint8* aPtr,TInt aLength)
{
	for(TInt i=0; i < aLength; ++i)
		{
		if(iCurrentWriteIndex == iData.size())
			{
			iData.push_back(aPtr[i]);
			}
		else
			{
			iData[iCurrentWriteIndex] = aPtr[i];
			}
		++iCurrentWriteIndex;
		}
}

TStreamPos MemStreamBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
{
	if(aMark != ERead && aMark != EWrite)
		{
		FatalError();
		}
	
	TUint32 *p = (aMark == ERead) ? (&iCurrentReadIndex) : (&iCurrentWriteIndex);

	switch(aLocation)
		{
		case EStreamBeginning:
			*p = anOffset;
			break;
			
		case EStreamMark:
			*p += anOffset;
			break;

		case EStreamEnd:
			*p = iData.size() + anOffset;
			break;

		default:
			FatalError();
		}

	if(*p > iData.size())
		{
		// nb we allow a seek 1 byte off end of data
		FatalError();
		}
	return *p;
}

void MemStreamBuf::DoRelease()
{
}

}; // end of namespace store_private 

const TInt KShiftCardinality8=1;
const TInt KShiftCardinality16=2;
const TInt KShiftCardinality32=3;
//
const TInt KDefaultIoBufSize=0xc00;
const TInt KFilterIoBufSize=0x100;

EXPORT_C void TCardinality::ExternalizeL(RWriteStream& aStream) const
/** Externalises this object to a write stream.

The existence of this function means that the standard templated operator<<() 
can be used to externalise objects of this class.

@param aStream Stream to which the object should be externalised.
@see operator<<() */
	{
	TUint n=iCount;
	if (n<=(KMaxTUint8>>KShiftCardinality8))
		aStream.WriteUint8L((n<<KShiftCardinality8));
	else if (n<=(KMaxTUint16>>KShiftCardinality16))
		aStream.WriteUint16L((n<<KShiftCardinality16)+0x1);
	else
		{
		__ASSERT_DEBUG(n<=(TUint)KMaxCardinality,Panic(EStreamCardinalityOutOfRange));
		aStream.WriteUint32L((n<<KShiftCardinality32)+0x3);
		}
	}

EXPORT_C void TCardinality::InternalizeL(RReadStream& aStream)
/** Internalizes this object from a read stream.

The existence of this function means that the standard templated operator>>() 
can be used to internalise objects of this class.

@param aStream Stream store from which the objet is to be internalised.
@see operator>>() */
	{
	TUint n=aStream.ReadUint8L();
	if ((n&0x1)==0)
		n>>=KShiftCardinality8;
	else if ((n&0x2)==0)
		{
		n+=aStream.ReadUint8L()<<8;
		n>>=KShiftCardinality16;
		}
	else if ((n&0x4)==0)
		{
		aStream.ReadL((TUint8*)&iCount,sizeof(TUint32)-sizeof(TUint8));
		n+=TUint(iCount)<<8; // platform dependency
		n>>=KShiftCardinality32;
		}
	else
		__LEAVE(KErrCorrupt);
//
	__ASSERT_DEBUG(n<=(TUint)KMaxCardinality,User::Invariant());
	iCount=n;
	}

EXPORT_C void TCardinality::__DbgChkRange(TInt aCount)
//
// Check for a negative count.
//
	{
		if(!(aCount<=KMaxCardinality)) FatalError();
	}

void CleanupStack::PopAndDestroy(RWriteStream *aStream)
{
	aStream->Close();
}
void CleanupStack::PopAndDestroy(RReadStream *aStream)
{
	aStream->Close();
}

void CleanupStack::PopAndDestroy(CFileStore *aStore)
{
	delete aStore;
}


// End of file
