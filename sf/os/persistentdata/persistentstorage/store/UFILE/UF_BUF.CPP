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

#include "UF_STD.H"
#include "S32FILEBUFSIZE.H"

//#define MAP_ROM_FILES
#ifdef _DEBUG
//#define IO_TRACING
//#define SIMULATE_PARTIAL_WRITE
#endif

#ifdef IO_TRACING
#include <e32svr.h>
_LIT(KTraceWrite,"RFile::Write [%d,%d)\n");
#define _TRACE_WRITE(p,l) RDebug::Print(KTraceWrite,(p),(p)+(l))
_LIT(KTraceRead,"RFile::Read [%d,%d)\n");
#define _TRACE_READ(p,l) RDebug::Print(KTraceRead,(p),(p)+(l))
_LIT(KTraceSize,"RFile::Size\n");
#define _TRACE_SIZE() RDebug::Print(KTraceSize)
_LIT(KTraceFlush,"RFile::Flush\n");
#define _TRACE_FLUSH() RDebug::Print(KTraceFlush)
_LIT(KTraceSetSize,"RFile::SetSize %d\n");
#define _TRACE_SETSIZE(s) RDebug::Print(KTraceSetSize,(s))
#else
#define _TRACE_WRITE(p,l)
#define _TRACE_READ(p,l)
#define _TRACE_SIZE()
#define _TRACE_FLUSH()
#define _TRACE_SETSIZE(s)
#endif

EXPORT_C RFileBuf::RFileBuf()
	: iBase(NULL),iSize(KDefaultFileBufSize),iWLim(NULL)
/** Constructs the object with a default intermediate buffer size.

The size of the intermediate buffer is the value of the constant KDefaultFileBufSize. */
	{}

EXPORT_C RFileBuf::RFileBuf(TInt aSize)
	: iBase(NULL),iSize(aSize),iWLim(NULL)
/** Constructs the object with the specified intermediate buffer size.

If the intermediate buffer size is zero, then the class provides an MStreamBuf 
interface to unbuffered file I/O.

@param aSize The size of the intermediate buffer. */
	{}

RFileBuf::RFileBuf(TCapture<RFileBuf> aCapture)
//
// Take over from the buffer wrapped inside aCapture.
//
	{
	RFileBuf& buf=aCapture.Object();
	iBase=buf.iBase;
	iSize=buf.iSize;
	SetBuf(ERead,buf.Ptr(ERead),buf.End(ERead));
	SetBuf(EWrite,buf.Ptr(EWrite),buf.End(EWrite));
	SetLimit(EWrite,buf.Limit(EWrite));
	buf.iBase=NULL;
	buf.SetBuf(ERead|EWrite,NULL,NULL);
	iFile=buf.File();
	buf.Detach();
	SetPos(ERead,buf.Pos(ERead));
	SetPos(EWrite,buf.Pos(EWrite));
	iExt=buf.iExt;
	}

EXPORT_C void RFileBuf::Reset()
/** Frees the intermediate buffer.

If there is any read data in the intermediate buffer, then the function reverts 
the read position within the stream.

The intermediate buffer must not contain any outstanding write data, otherwise 
the function raises a STORE-File 6 panic. */
	{
	__ASSERT_ALWAYS(Span(EWrite)==0,Panic(EFileWriteOutstanding));
	MovePos(ERead,Lag(ERead));
	Free();
	}

EXPORT_C TInt RFileBuf::Open(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens the specified file and attaches it to this stream buffer.

If the file cannot be opened, then it is not attached to this stream buffer.

@param aFs Handle to a file server session through which the file is opened.
@param aName The name of the file to be opened.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return KErrNone, if successful, otherwise one of the other system wide error 
codes.
@see Attach()
@see TFileMode
@see RFile */
	{
	RFile file;
	TInt r=file.Open(aFs,aName,aFileMode);
	if (r==KErrNone)
		Attach(file);
	return r;
	}

EXPORT_C TInt RFileBuf::Create(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a file with the specified name and attaches it to this stream buffer.

The file must not already exist.

If the file cannot be created and opened, then it is not attached to this 
stream buffer.

@param aFs Handle to a file server session through which the file is created.
@param aName The name of the file to be created.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return KErrNone, if successful, otherwise one of the other system wide error 
codes.
@see Attach()
@see TFileMode
@see RFile */
	{
	RFile file;
	TInt r=file.Create(aFs,aName,aFileMode);
	if (r==KErrNone)
		Attach(file);
	return r;
	}

EXPORT_C TInt RFileBuf::Replace(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Replaces the file with the specified name and attaches it to this stream buffer.

If there is an existing file with the same name, then this function overwrites 
it. If the file does not already exist, it is created.

If the file cannot be replaced, then it is not attached to this stream buffer.

@param aFs Handle to a file server session through which the file is replaced.
@param aName The name of the file to be replaced.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return KErrNone, if successful, otherwise one of the other system wide error 
codes.
@see Attach()
@see TFileMode
@see RFile */
	{
	RFile file;
	TInt r=file.Replace(aFs,aName,aFileMode);
	if (r==KErrNone)
		Attach(file);
	return r;
	}

EXPORT_C TInt RFileBuf::Temp(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode)
/** Creates and opens a temporary file with a unique name and attaches it to this 
stream buffer.

@param aFs Handle to a file server session through which the file is created.
@param aPath The directory in which the file is created.
@param aName On return, contains the full path and file name of the file. The 
filename is guaranteed to be unique within the specified directory.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@see Attach()
@see TFileMode
@see RFile */
	{
	RFile file;
	TInt r=file.Temp(aFs,aPath,aName,aFileMode);
	if (r==KErrNone)
		Attach(file);
	return r;
	}

EXPORT_C void RFileBuf::Attach(RFile& aFile,TInt aPos)
/** Attaches the specified file to this stream buffer.

The function also re-sets the intermediate buffer's read and write marks to 
the beginning of the intermediate buffer and sets the read and write stream 
positions to the specified offset within the file.

@param aFile The file to be attached.
@param aPos The offset within the file to which the read and write stream positions 
are set. By default this is zero.
@see Detach()
@see Reattach() */
	{
	__ASSERT_ALWAYS(Span(EWrite)==0,Panic(EFileWriteOutstanding));
	TUint8* base=iBase;
	SetBuf(ERead|EWrite,base,base);
	iFile=aFile;
	aFile=RFile();
	SetPos(ERead|EWrite,aPos);
	iExt=-1;
	}

EXPORT_C void RFileBuf::Close()
/** Writes any outstanding data from the intermediate buffer before freeing the 
intermediate buffer and closing the attached file. */
	{
	TInt lag=Span(EWrite);
	if (lag>0)
		{
		_TRACE_WRITE(Pos(EWrite),lag);
		File().Write(Pos(EWrite),TPtrC8(iBase,lag));
		}
	Free();
	File().Close();
	}

EXPORT_C void RFileBuf::SetSizeL(TInt aSize)
/** Changes the size of the file attached to this buffer to the specified value.

Writes any outstanding data from the intermediate buffer to the stream hosted 
by the file. Any data in the intermediate buffer that would lie beyond the 
end of the truncated file, is not written.

@param aSize The new size of the file. */
	{
	TUint8* base=iBase;
	TInt pos=Pos(EWrite);
	TInt excess=aSize-pos;
	if (excess>0)
		FileWriteL(base,Min(excess,Span(EWrite)),0);
	MovePos(ERead,Lag(ERead));
	SetBuf(ERead,base,base);
//
	_TRACE_SETSIZE(aSize);
	TInt r=File().SetSize(aSize);
	if (r!=KErrNone)
		{
		SetPos(EWrite,pos);
		iExt=-1;
		__LEAVE(r);
		}
//
	SetPos(EWrite,Min(pos+Lag(EWrite), aSize)); 
	SetBuf(EWrite,base,base);
	iExt=aSize;
	}

EXPORT_C TInt RFileBuf::UnderflowL(TInt aMaxLength)
//
// Fill the buffer's read area.
//
	{
	__ASSERT_DEBUG(Avail(ERead)==0,User::Invariant());
	TUint8* base=iBase;
	if (base==NULL)
		{
#if defined(MAP_ROM_FILES)
		if (Ptr(ERead))
			return 0;		// memory mapped ROM file
		TInt pos=0;
		TInt err = File().Seek(ESeekAddress,pos);
		if (err==KErrNone)
			{				// memory map a ROM file into the read zone
			base=(TUint8*)pos;
			TInt len=EndL();
			SetPos(ERead,len);
			SetBuf(ERead,base,base+len);
			return len;
			}
#endif
		base=AllocL();
		}

	TInt lag=Pos(ERead)-Pos(EWrite);
	TInt span=Span(EWrite);
	if (lag>=0&&lag<span)
		{
		SetBuf(ERead,base+lag,base+span);
		MovePos(ERead,span-lag);
		return span-lag;
		}

	FileWriteL(base,Span(EWrite));
	SetBuf(EWrite,base,base);
	
	// Align file position with file 'blocks' when possible
	TInt align = (Pos(ERead) + aMaxLength + KMaxFileBufReadAhead) & (KFileBufBlockSize-1);
	TInt readahead = KMaxFileBufReadAhead - align;
	if(readahead < 0)
		{
		// if read-ahead doesn't cross block boundary do it all
		readahead = KMaxFileBufReadAhead;	
		}
	TInt len=FileReadL(base, Min(iSize, aMaxLength+readahead));
	
	SetBuf(ERead,base,base+len);
	return len;
	}

EXPORT_C void RFileBuf::OverflowL()
//
// Set up the buffer's write area.
//
	{
	__ASSERT_DEBUG(Avail(EWrite)==0,User::Invariant());
	TUint8* base=iBase;
	if (base==NULL)
		base=AllocL();
	MovePos(ERead,Lag(ERead));
	SetBuf(ERead,base,base);
//
	__ASSERT_DEBUG(Lag(EWrite)==Span(EWrite),User::Invariant());
	FileWriteL(base,Lag(EWrite),0);
	SetBuf(EWrite,base,base+iSize);
	}

EXPORT_C void RFileBuf::DoRelease()
//
// Release all resources, losing any outstanding data.
//
	{
	Free();
	File().Close();
	}

EXPORT_C void RFileBuf::DoSynchL()
//
// Synchronise this buffer with its file, giving up on outstanding writes in case of failure.
//
	{
	TUint8* base=iBase;
#if defined(MAP_ROM_FILES)
	if (base!=NULL)	// do not do this for memory mapped ROM files
#endif
		{
		MovePos(ERead,Lag(ERead));
		TInt span=Span(EWrite);
		TInt rewind=span-Lag(EWrite);
		SetBuf(ERead|EWrite,base,base);
		iExt=-1;
		FileWriteL(base,span,rewind);
		}
	TInt handle = File().SubSessionHandle();
	if (handle!=0)
		{
		_TRACE_FLUSH();
		TInt r=File().Flush();
		if (r!=KErrNone&&r!=KErrAccessDenied)
			{
			__LEAVE(r);
			}
		}
	}

EXPORT_C TInt RFileBuf::DoReadL(TAny* aPtr,TInt aMaxLength)
//
// Read direct from file if asked to transfer more than a bufferful.
//
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EFileReadLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EFileReadNoTransfer));
	TInt avail=Avail(ERead);
	__ASSERT_DEBUG(avail>=0&&Avail(EWrite)>=0,User::Invariant());
	if (avail>0)
		{
		TInt len=Min(aMaxLength,avail);
		TUint8* ptr=Ptr(ERead);
		aPtr=Mem::Copy(aPtr,ptr,len);
		SetPtr(ERead,ptr+len);
		aMaxLength-=len;
		if (aMaxLength==0)
			return len; // that's it
		}
	__ASSERT_DEBUG(Avail(ERead)==0,User::Invariant());
	if (aMaxLength<iSize)
		return avail+TStreamBuf::DoReadL(aPtr,aMaxLength);
//
	TUint8* base=iBase;
	FileWriteL(base,Span(EWrite));
	SetBuf(ERead|EWrite,base,base);
	return avail+FileReadL(aPtr,aMaxLength);
	}

EXPORT_C TInt RFileBuf::DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
//
// Read up to aMaxLength bytes asynchronously.
//
	{
//#pragma message( __FILE__ " : 'RFileBuf::DoReadL(TDes8&,TInt,TRequestStatus&)' not implemented" )
	__ASSERT_DEBUG(aMaxLength<=aDes.MaxLength(),Panic(EFileReadBeyondEnd));
	aDes.SetLength(DoReadL((TUint8*)aDes.Ptr(),aMaxLength));
	TRequestStatus* stat=&aStatus;
	User::RequestComplete(stat,KErrNone);
	return aMaxLength;
	}

EXPORT_C void RFileBuf::DoWriteL(const TAny* aPtr,TInt aLength)
//
// Write direct to file if asked to transfer more than a bufferful.
//
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EFileWriteLengthNegative));
	__ASSERT_DEBUG(aLength>0,Panic(EFileWriteNoTransfer));
	TInt avail=Avail(EWrite);
	__ASSERT_DEBUG(Avail(ERead)>=0&&avail>=0,User::Invariant());
	if (avail>0)
		{
		TInt len=Min(aLength,avail);
		SetPtr(EWrite,Mem::Copy(Ptr(EWrite),aPtr,len));
		aLength-=len;
		if (aLength==0)
			return; // done
//
		aPtr=(TUint8*)aPtr+len;
		}
	__ASSERT_DEBUG(Avail(EWrite)==0,User::Invariant());
	if (aLength<iSize)
		TStreamBuf::DoWriteL(aPtr,aLength);
	else
		{
		__ASSERT_DEBUG(Lag(EWrite)==Span(EWrite),User::Invariant());
		TUint8* base=iBase;
		FileWriteL(base,Lag(EWrite),0);
		MovePos(ERead,Lag(ERead));
		SetBuf(ERead|EWrite,base,base);
		FileWriteL(aPtr,aLength,0);
		}
	}

EXPORT_C TInt RFileBuf::DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
//
// Write up to aMaxLength bytes asynchronously.
//
	{
//#pragma message( __FILE__ " : 'RFileBuf::DoWriteL(const TDesC8&,TInt,TRequestStatus&)' not implemented" )
	__ASSERT_DEBUG(aMaxLength<=aDes.Length(),Panic(EFileWriteBeyondEnd));
	DoWriteL(aDes.Ptr(),aMaxLength);
	TRequestStatus* stat=&aStatus;
	User::RequestComplete(stat,KErrNone);
	return aMaxLength;
	}

EXPORT_C TStreamPos RFileBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position the mark(s) indicated by aMark at anOffset from aLocation.
//
	{
	TUint8* base=iBase;
	TInt end=EndL();
//
	switch (aLocation)
		{
	case EStreamBeginning:
		break;
	case EStreamMark:
		anOffset+=Mark(aMark);
		break;
	case EStreamEnd:
		anOffset+=end;
		break;
	default:
		Panic(EFileLocationInvalid);
		break;
		}
	TInt r=KErrNone;
	if (anOffset<0)
		{
		anOffset=0;
		r=KErrEof;
		}
	else if (anOffset>end)
		{
		anOffset=end;
		r=KErrEof;
		}
//
	__ASSERT_ALWAYS(!(aMark&~(ERead|EWrite)),Panic(EFileMarkInvalid));
	if (aMark&ERead)
		{
		TInt lag=anOffset-Pos(ERead);
#if defined(MAP_ROM_FILES)
		if (base==NULL&&End(ERead)!=NULL)	// memory mapped
			SetPtr(ERead,End(ERead)+lag);
		else
#endif
		if (lag>=base-End(ERead)&&lag<=0)
			SetPtr(ERead,End(ERead)+lag);
		else
			{
			SetPos(ERead,anOffset);
			SetBuf(ERead,base,base);
			}
		}
	if (aMark&EWrite)
		{
		TInt lag=anOffset-Pos(EWrite);
		TInt span=Span(EWrite);
		if (lag>=0&&lag<=span)
			{
			SetLimit(EWrite,base+span);
			SetPtr(EWrite,base+lag);
			}
		else
			{
			FileWriteL(base,span,0);
			SetPos(EWrite,anOffset);
			SetBuf(EWrite,base,base);
			}
		}
	__LEAVE_IF_ERROR(r);
	return TStreamPos(anOffset);
	}

TUint8* RFileBuf::AllocL()
//
// Allocate space and return a pointer to this buffer's buffer space
//
	{
	__ASSERT_DEBUG(iBase==NULL && iSize>0,User::Invariant());
//
	TUint8* base=(TUint8*)User::AllocL(iSize);
	iBase=base;
	SetBuf(ERead|EWrite,base,base);
	return base;
	}

void RFileBuf::Free()
//
// Free this buffer's buffer space.
//
	{
	User::Free(iBase);
	iBase=NULL;
	SetBuf(ERead|EWrite,NULL,NULL);
	}

void RFileBuf::SetPos(TMark aMark,TInt aPos)
//
// Set the file position for the mark(s) indicated by aMark
//
	{
	__ASSERT_ALWAYS(!(aMark&~(ERead|EWrite)),Panic(EFileMarkInvalid));
	if (aMark&ERead)
		SetPos(ERead,aPos);
	if (aMark&EWrite)
		SetPos(EWrite,aPos);
	}

TInt RFileBuf::FileReadL(TAny* aPtr,TInt aMaxLength)
//
// Read from the file at the current read position.
//
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EFileReadLengthNegative));
	if (aMaxLength==0)
		return 0;
//
	TPtr8 des((TUint8*)aPtr,aMaxLength);
	TInt pos=Pos(ERead);
	__LEAVE_IF_ERROR(File().Read(pos,des));
	TInt len=des.Length();
	_TRACE_READ(pos,len);
	pos+=len;
	if (len<aMaxLength)
		iExt=pos; // end-of-file encountered
	SetPos(ERead,pos);
	return len;
	}

void RFileBuf::FileWriteL(const TAny* aPtr,TInt aLength)
//
// Write to the file at the current write position
// Use write buffer status to determine the rewind
//
	{
	FileWriteL(aPtr,aLength,Span(EWrite)-Lag(EWrite));
	}

void RFileBuf::FileWriteL(const TAny* aPtr,TInt aLength,TInt aRewind)
//
// Write to the file at the current write position.
// Rewind write position after write
//
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EFileWriteLengthNegative));
	if (aLength==0)
		return;
//
	TInt ext=iExt;
	iExt=-1;
	TInt pos=Pos(EWrite);
	_TRACE_WRITE(pos,aLength);
#ifdef SIMULATE_PARTIAL_WRITE
	TPtrC8 ptr((TUint8*)aPtr,aLength);
	TInt partial = aLength >> 1;
	if (partial)
		{
		__LEAVE_IF_ERROR(File().Write(pos, ptr.Left(partial)));
		}
	__LEAVE_IF_ERROR(File().Write(pos + partial, ptr.Mid(partial)));
#else

#if defined SYSLIBS_TEST && defined _DEBUG
	const TInt KDefaultMediaBlockSize = 512;
	TInt startSectorAddr = pos & ~(KDefaultMediaBlockSize - 1);
	TInt endSectorAddr = (pos + aLength - 1) & ~(KDefaultMediaBlockSize - 1);
	if(startSectorAddr != endSectorAddr && aLength < KDefaultMediaBlockSize)
		{
		TInt len1 = startSectorAddr + KDefaultMediaBlockSize - 	pos;
		TInt len2 = aLength - len1;
		__LEAVE_IF_ERROR(File().Write(pos, TPtrC8((TUint8*)aPtr, len1)));
		__LEAVE_IF_ERROR(File().Write(pos + len1, TPtrC8((TUint8*)aPtr + len1, len2)));
		}
	else
		{
		__LEAVE_IF_ERROR(File().Write(pos,TPtrC8((TUint8*)aPtr,aLength)));
		}
#else //SYSLIBS_TEST		
	__LEAVE_IF_ERROR(File().Write(pos,TPtrC8((TUint8*)aPtr,aLength)));
#endif//SYSLIBS_TEST

#endif
	pos+=aLength;
	if (ext>=0)
		iExt=Max(pos,ext);
	SetPos(EWrite,pos-aRewind);
	}

TInt RFileBuf::EndL()
//
// Determine the end of the file.
//
	{
	TInt ext=iExt;
	if (ext<0)
		{
		_TRACE_SIZE();
		__LEAVE_IF_ERROR(File().Size(ext));
		iExt=ext;
		}
	return Max(ext,Reach(EWrite));
	}

TInt RFileBuf::Mark(TMark aMark) const
//
// Return the position of the mark indicated by aMark.
//
	{
	if (aMark==ERead)
		return Mark(ERead);
//
	__ASSERT_ALWAYS(aMark==EWrite,Panic(EFileMarkInvalid));
	return Mark(EWrite);
	}

