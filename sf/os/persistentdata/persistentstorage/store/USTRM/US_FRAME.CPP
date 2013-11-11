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

TInt RFrame16Buf::Offset(TStreamPos aBase,TStreamPos aPos)
	{
	TInt off=aPos-aBase;
	__ASSERT_DEBUG(off>=0,Panic(EStreamPosInvalid));
	TInt n=off/(KFrameFullLength16+KSizeFrameDes16);
		// number of full frames from base
	off-=n<<KShiftFrameDes16;
		// target offset in frame coordinates
	TInt over=off-(n<<KShiftFrameLength16)-KFrameFullLength16;
		// adjust for overshoot into the frame descriptor
	__ASSERT_DEBUG(over>=-KFrameFullLength16&&over<TInt(KSizeFrameDes16),User::Invariant());
	if (over>0)
		off-=over;
	return off;
	}

TStreamPos RFrame16Buf::Position(TStreamPos aBase,TInt anOffset)
	{
	return aBase+(anOffset+(anOffset>>KShiftFrameLength16<<KShiftFrameDes16));
	}

RFrame16Buf::RFrame16Buf(TStreamPos aBase)
	: iBase(aBase),iHost(NULL),iExt(KFrameNonexistent16)
	{
	__ASSERT_DEBUG(aBase>=KStreamBeginning+KSizeFrameDes16,Panic(EStreamPosInvalid));
	}

TInt RFrame16Buf::ExtendL(TStreamExchange& aHost,TInt anExtent,TInt aMode)
	{
	__ASSERT_DEBUG(anExtent>=0,Panic(EStreamExtentNegative));
	__ASSERT_DEBUG((KMaskFrameType16&aMode)!=EFrameContinuation16,Panic(EStreamTypeInvalid));
	TFrameDes16 des[2]={0,TFrameDes16(KMaskFrameType16&aMode|KFrameOpen16)};
	TUint8* ptr=(TUint8*)&des[1];
	TInt len=KSizeFrameDes16;
	TStreamPos pos=Position(anExtent);
	TInt off=anExtent;
	anExtent&=KMaskFrameLength16;
	if (anExtent==0)
		pos-=KSizeFrameDes16;
	else
		{
		anExtent=KFrameFullLength16-anExtent;
		if (anExtent>KSizeFrameDes16)
			off+=KSizeFrameDes16;
		else
			{
			ptr-=anExtent;
			len+=anExtent;
			off+=anExtent;
			}
		}
//
	TStreamMark mark(pos);
	mark.WriteL(aHost,ptr,len); // platform dependency
	mark.Withdraw(aHost);
	Set(aHost,off,0,aMode);
	return off;
	}

void RFrame16Buf::OpenL(TStreamExchange& aHost,TInt anOffset,TInt aMode)
	{
	__ASSERT_DEBUG(anOffset>=0,Panic(EStreamOffsetNegative));
	__ASSERT_DEBUG((KMaskFrameType16&aMode)!=EFrameContinuation16,Panic(EStreamTypeInvalid));
	TStreamMark mark(Position(anOffset)-KSizeFrameDes16);
	TFrameDes16 des;
	TInt frame=mark.ReadL(aHost,&des,KSizeFrameDes16);
		// platform dependency
	mark.Withdraw(aHost);
	__ASSERT_DEBUG(frame>=0&&frame<=KSizeFrameDes16,Panic(EStreamReadInBreach));
	if (frame<KSizeFrameDes16)
		__LEAVE(KErrEof);
//
	frame=des;
	if ((frame^aMode)&KMaskFrameType16)
		__LEAVE(KErrCorrupt);
//
	TInt in=anOffset&KMaskFrameLength16;
	frame&=KMaskFrameLength16;
	if (frame==KFrameOpen16)
		frame=KFrameFullLength16-in;
	if (in+frame>KFrameFullLength16)
		__LEAVE(KErrCorrupt);
//
	Set(aHost,anOffset,anOffset+frame,aMode);
	}

void RFrame16Buf::Set(TStreamExchange& aHost,TInt anOffset,TInt anExtent,TInt aMode)
	{
	__ASSERT_DEBUG(anOffset>=0||anOffset==anExtent,Panic(EStreamOffsetNegative));
	__ASSERT_DEBUG(anExtent>=0||anOffset==anExtent,Panic(EStreamExtentNegative));
	iHost=&aHost;
	iOff=anOffset;
	iExt=anExtent;
	TInt lim;
	if (anOffset<0)
		lim=anExtent;
	else
		{
		TStreamPos start=Position(anOffset);
		if (aMode&ERead)
			iRMark=start;
		else
			iRMark.Clear();
		if (aMode&EWrite)
			iWMark=start;
		else
			iWMark.Clear();
		lim=(anOffset&~KMaskFrameLength16)+KFrameFullLength16;
		if (anExtent>0&&lim>anExtent)
			lim=anExtent;
		}
	iRAvail=iWAvail=lim-anOffset;
	iRLim=iWLim=lim;
	iMode=aMode;
	}

void RFrame16Buf::CommitL()
	{
	__ASSERT_DEBUG(!IsCommitted(),Panic(EStreamCommitted));
	TInt ext=Host().SizeL()-Base().Offset();
	if (ext<0)
		__LEAVE(KErrCorrupt);
//
	TInt n=ext/(KFrameFullLength16+KSizeFrameDes16);
		// number of full frames from base
	ext-=n<<KShiftFrameDes16;
		// end in frame coordinates
	TInt off=Offset();
	if (ext==off)
		{ // committing an empty stream
		iOff=iExt=KFrameNonexistent16;
		return;
		}
//
	TInt frame=ext-(n<<KShiftFrameLength16);
		// size of final frame (remainder of the division above)
	__ASSERT_DEBUG(frame>=0&&frame<KFrameFullLength16+KSizeFrameDes16,User::Invariant());
	if (ext<off||frame==0||frame>KFrameFullLength16)
		__LEAVE(KErrCorrupt); // bad offset, empty frame or partial frame descriptor
//
	if (frame<KFrameFullLength16)
		{
		TFrameDes16 des;
		if ((off^ext)&~KMaskFrameLength16)
			{ // in a different frame from the one we started in
			off=ext&~KMaskFrameLength16;
			des=TFrameDes16(EFrameContinuation16|frame);
			}
		else
			{
			frame-=off&KMaskFrameLength16;
			__ASSERT_DEBUG(frame>0&&frame<KFrameFullLength16,User::Invariant());
			des=TFrameDes16(iMode&KMaskFrameType16|frame);
			}
		TStreamMark mark(Position(off)-KSizeFrameDes16);
		mark.WriteL(Host(),&des,KSizeFrameDes16);
		mark.Withdraw(Host());
		}
	iExt=ext;
	}

void RFrame16Buf::DoRelease()
	{
	if (iHost!=NULL)
		{
		iRMark.Withdraw(*iHost);
		iWMark.Withdraw(*iHost);
		iHost=NULL;
		iExt=KFrameNonexistent16;
		}
	}

void RFrame16Buf::DoSynchL()
	{
	if (IsCommitted())
		return;
//
	CommitL();
	}

TInt RFrame16Buf::DoReadL(TAny* aPtr,TInt aMaxLength)
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamReadLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamReadNoTransfer));
	TInt left=aMaxLength;
	TInt avail=iRAvail;
	if (avail==0)
		goto underflow; // we'd used up the whole frame
//
	do
		{
		__ASSERT_DEBUG(left>0&&avail>0,User::Invariant());
		{
		TInt len=iRMark.ReadL(Host(),aPtr,Min(left,avail));
		__ASSERT_DEBUG(len>=0&&len<=left&&len<=avail,Panic(EStreamReadInBreach));
		iRAvail=avail-=len;
		left-=len;
		if (left==0)
			return aMaxLength; // that's it
//
		aPtr=(TUint8*)aPtr+len;
		}
		if (avail>0)
			{ // we've run out of host 
			if ((Extent()&KMaskFrameLength16)==KFrameOpen16)
				break; // all's well, provided we're open-ended
//
			iRMark.Withdraw(Host());
			__LEAVE(KErrCorrupt);
			}
//
	underflow:
		avail=UnderflowL();
		} while (avail>0);
	return aMaxLength-left;
	}

TInt RFrame16Buf::DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
//
// Read up to aMaxLength bytes asynchronously.
//
	{
//#pragma message( __FILE__ " : 'RFrame16Buf::DoReadL(TDes8&,TInt,TRequestStatus&)' not implemented" )
	__ASSERT_DEBUG(aMaxLength<=aDes.MaxLength(),Panic(EStreamReadBeyondEnd));
	aDes.SetLength(DoReadL((TUint8*)aDes.Ptr(),aMaxLength));
	TRequestStatus* stat=&aStatus;
	User::RequestComplete(stat,KErrNone);
	return aMaxLength;
	}

TStreamTransfer RFrame16Buf::DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer)
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamReadNoTransfer));
	if (iRAvail>0)
		{
		TInt len=iRMark.ReadL(Host(),anInput,aTransfer[iRAvail]);
		aTransfer-=len;
		iRAvail-=len;
		}
	if (aTransfer>0)
		return anInput.ReadFromL(*this,aTransfer);
//
	return aTransfer;
	}

void RFrame16Buf::DoWriteL(const TAny* aPtr,TInt aLength)
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	__ASSERT_DEBUG(aLength>0,Panic(EStreamWriteNoTransfer));
	TInt avail=iWAvail;
	if (avail==0)
		goto overflow; // we'd used up the whole frame
//
	for (;;)
		{
		__ASSERT_DEBUG(aLength>0&&avail>0,User::Invariant());
		{
		TInt len=Min(aLength,avail);
		iWMark.WriteL(Host(),aPtr,len);
		iWAvail=avail-len;
		aLength-=len;
		if (aLength==0)
			return; // won't worry about the next frame just yet
//
		aPtr=(TUint8*)aPtr+len;
		}
	overflow:
		OverflowL();
		avail=iWAvail;
		};
	}

TInt RFrame16Buf::DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
//
// Write up to aMaxLength bytes asynchronously.
//
	{
//#pragma message( __FILE__ " : 'RFrame16Buf::DoWriteL(const TDesC8&,TInt,TRequestStatus&)' not implemented" )
	__ASSERT_DEBUG(aMaxLength<=aDes.Length(),Panic(EStreamWriteBeyondEnd));
	DoWriteL(aDes.Ptr(),aMaxLength);
	TRequestStatus* stat=&aStatus;
	User::RequestComplete(stat,KErrNone);
	return aMaxLength;
	}

TStreamTransfer RFrame16Buf::DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer)
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamWriteNoTransfer));
	if (iWAvail>0)
		{
		TInt len=iWMark.WriteL(Host(),anOutput,aTransfer[iWAvail]);
		aTransfer-=len;
		iWAvail-=len;
		}
	if (aTransfer>0)
		return anOutput.WriteToL(*this,aTransfer);
//
	return aTransfer;
	}

TStreamPos RFrame16Buf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position the mark(s) indicated by aMark at anOffset from aLocation.
//
	{
	switch (aLocation)
		{
	case EStreamBeginning:
		anOffset+=Offset();
		break;
	case EStreamMark:
		if (aMark==ERead)
			anOffset+=iRLim-iRAvail;
		else
			{
			__ASSERT_ALWAYS(aMark==EWrite,Panic(EStreamMarkInvalid));
			anOffset+=iWLim-iWAvail;
			}
		break;
	case EStreamEnd:
		anOffset+=EndL();
		break;
	default:
		Panic(EStreamLocationInvalid);
		break;
		}
	TInt r=KErrNone;
	if (anOffset<Offset())
		{
		anOffset=Offset();
		r=KErrEof;
		}
	else if (anOffset>=0&&!ReachL(anOffset))
		{
		__ASSERT_DEBUG(Extent()!=0,User::Invariant());
		anOffset=Extent();
		r=KErrEof;
		}
//
	if (aMark&&anOffset>=0)
		{
		__ASSERT_ALWAYS(!(aMark&~(ERead|EWrite)),Panic(EStreamMarkInvalid));
		TInt in=anOffset&KMaskFrameLength16;
		TInt off=anOffset-in;
		__ASSERT_DEBUG(Extent()==0||anOffset<=Extent(),User::Invariant());
		if (in==0&&off>Offset()&&(Extent()==0||off==Extent()))
			{ // positioning at the start of a frame that may not exist
			in=KFrameFullLength16;
			off-=in;
			}
		TStreamPos pos=Base()+(off+(off>>KShiftFrameLength16<<KShiftFrameDes16))+in;
		TInt lim=off+KFrameFullLength16;
		if (Extent()>0&&lim>Extent())
			lim=Extent();
		TInt left=lim-anOffset;
		__ASSERT_DEBUG(left>=0&&left<=KFrameFullLength16,User::Invariant());
//
		if (aMark&ERead)
			{
			if (iMode&ERead)
				iRMark.SeekL(Host(),pos);
			iRAvail=left;
			iRLim=lim;
			}
		if (aMark&EWrite)
			{
			if (iMode&EWrite)
				iWMark.SeekL(Host(),pos);
			iWAvail=left;
			iWLim=lim;
			}
		}
	__LEAVE_IF_ERROR(r);
	return TStreamPos(anOffset-Offset());
	}

TInt RFrame16Buf::UnderflowL()
	{
	__ASSERT_DEBUG(iRAvail==0,User::Invariant());
	if ((iRLim&KMaskFrameLength16)!=KFrameOpen16)
		{
		__ASSERT_DEBUG(iExt==iRLim,User::Invariant());
		return 0; // we've used up the closing, partial frame
		}
//
	__ASSERT_DEBUG(iRLim>0&&(iExt>0&&iRLim<=iExt||iExt==0),User::Invariant());
		// let's see if there's a continuation frame
	TFrameDes16 des;
	TInt frame=iRMark.ReadL(Host(),&des,KSizeFrameDes16);
		// platform dependency
	__ASSERT_DEBUG(frame>=0&&frame<=KSizeFrameDes16,Panic(EStreamReadInBreach));
	if (frame==KSizeFrameDes16)
		{
		frame=des;
		if ((frame&KMaskFrameType16)==EFrameContinuation16)
			{ // we have a continuation frame, prepare ourselves for it
			frame&=KMaskFrameLength16;
			if (frame==KFrameOpen16)
				frame=KFrameFullLength16;
			iRAvail=frame;
			TInt lim=iRLim;
			iRLim=lim+frame;
			if (iExt==lim)
				iExt=iRLim;
			return frame;
			}
		iRMark.SeekL(Host(),-KSizeFrameDes16);
		frame=0;
		}
//
	if (frame>0||iRLim<iExt)
		{ // we'd definitely seen a continuation frame here before
		iRMark.Withdraw(Host());
		__LEAVE(KErrCorrupt);
		}
	return 0;
	}

void RFrame16Buf::OverflowL()
	{
	__ASSERT_DEBUG(iWAvail==0,User::Invariant());
	if ((iWLim&KMaskFrameLength16)!=KFrameOpen16)
		{ // filled the closing, partial frame
		__ASSERT_DEBUG(iExt==iWLim,User::Invariant());
	overflow:
		iWMark.Withdraw(Host());
		__LEAVE(KErrOverflow);
		}
//
	__ASSERT_DEBUG(iWLim>0&&(iExt>0&&iWLim<=iExt||iExt==0),User::Invariant());
	if (iWLim==iExt)
		{ // let's see if this carries on
		TFrameDes16 des;
		TInt frame=iWMark.ReadL(Host(),&des,KSizeFrameDes16);
			// platform dependency
		__ASSERT_DEBUG(frame>=0&&frame<=KSizeFrameDes16,Panic(EStreamReadInBreach));
		if (frame==0)
			iExt=0; // end-of-host, let's start extending
		else
			{
			if (frame==KSizeFrameDes16)
				{
				frame=des;
				if ((frame&KMaskFrameType16)==EFrameContinuation16)
					{ // there's more, set up for the next frame
					frame&=KMaskFrameLength16;
					if (frame==KFrameOpen16)
						frame=KFrameFullLength16;
					iWAvail=frame;
					iExt=iWLim+=frame;
					return;
					}
				goto overflow;
				}
//
			iWMark.Withdraw(Host());
			__LEAVE(KErrCorrupt);
			}
		}
//
	__ASSERT_DEBUG(iWLim<iExt||iExt==0,User::Invariant());
		// calculate the frame descriptor to be written
	TInt frame=iExt&KMaskFrameLength16;
	if (iWLim<iExt-frame)
		frame=KFrameOpen16;
	TFrameDes16 des=TFrameDes16(EFrameContinuation16|frame);
	if (frame==KFrameOpen16)
		frame=KFrameFullLength16;
	iWMark.WriteL(Host(),&des,KSizeFrameDes16);
		// platform dependency
	iWAvail=frame;
	iWLim+=frame;
	}

TBool RFrame16Buf::ReachL(TInt anOffset)
	{
	if (iExt<=0)
		return iExt==0;
//
	TStreamMark mark(Base());
	TInt frame=(iExt+(iExt>>KShiftFrameLength16<<KShiftFrameDes16))-KSizeFrameDes16;
	while (iExt<anOffset)
		{
		if ((iExt&KMaskFrameLength16)!=KFrameOpen16)
			{
			mark.Withdraw(Host());
			return EFalse;
			}
//
		TFrameDes16 des;
		mark.SeekL(Host(),EStreamMark,frame);
		frame=mark.ReadL(Host(),&des,KSizeFrameDes16);
			// platform dependency
		__ASSERT_DEBUG(frame>=0&&frame<=KSizeFrameDes16,Panic(EStreamReadInBreach));
		if (frame==KSizeFrameDes16)
			{
			frame=des;
			if ((frame&KMaskFrameType16)==EFrameContinuation16)
				{ // we have a continuation frame, boost upper
				frame&=KMaskFrameLength16;
				if (frame==KFrameOpen16)
					frame=KFrameFullLength16;
				iExt+=frame;
				continue;
				}
			frame=0;
			}
//
		mark.Withdraw(Host());
		if (frame>0)
			__LEAVE(KErrCorrupt);
//
		return EFalse;
		}
	mark.Withdraw(Host());
	return ETrue;
	}

TInt RFrame16Buf::EndL()
	{
	TInt ext=Host().SizeL()-Base().Offset();
	if (ext<0)
		__LEAVE(KErrCorrupt);
//
	TInt n=ext/(KFrameFullLength16+KSizeFrameDes16);
		// number of full frames from base
	ext-=n<<KShiftFrameDes16;
		// end in frame coordinates
	TInt frame=ext-(n<<KShiftFrameLength16);
		// size of final frame (remainder of the division above)
	__ASSERT_DEBUG(frame>=0&&frame<KFrameFullLength16+KSizeFrameDes16,User::Invariant());
	if (ext<Offset()||frame>KFrameFullLength16)
		__LEAVE(KErrCorrupt); // bad offset or partial frame descriptor
//
	if (!ReachL(ext))
		{
		__ASSERT_DEBUG(Extent()!=0,User::Invariant());
		return Extent();
		}
//
	return ext;
	}

